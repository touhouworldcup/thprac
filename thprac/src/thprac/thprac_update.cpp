const wchar_t* UPDATE_JSON_URL = L"https://raw.githubusercontent.com/touhouworldcup/thprac/refs/heads/master/thprac_version.json";

#include "utils/wininternal.h"
#include <commctrl.h>
#include <wininet.h>

#include <yyjson.h>

#include <stdio.h>

#include "thprac_log.h"
#include "thprac_utils.h"
#include "thprac_update.h"

namespace THPrac {
extern wchar_t old_working_dir[];

#define WININET_FUNCS \
    WININET_FUNC(InternetOpenW) \
    WININET_FUNC(InternetOpenUrlW) \
    WININET_FUNC(InternetSetOptionW) \
    WININET_FUNC(InternetQueryDataAvailable) \
    WININET_FUNC(HttpQueryInfoW) \
    WININET_FUNC(InternetReadFile) \
    WININET_FUNC(InternetCloseHandle) \

#define WININET_FUNC(f) decltype(f)* Imp_##f = nullptr;
WININET_FUNCS
#undef WININET_FUNC

// Global variable because the updater is gonna get used in-game
BackgroundUpdateCheck* background_update_check;

unsigned int BackgroundUpdateCallback(DOWNLOAD_CALLBACK_REASON reason, DownloadParams* dl) {
    BackgroundUpdateCheck* ctx = (BackgroundUpdateCheck*)dl;

    switch (reason) {
    case DLCB_FILESIZE:
        dl->out.clear();
        return 0;
    case DLCB_COMPLETE:
        return !ParseUpdateJson((char*)dl->out.data(), dl->out.size(), &ctx->updateJson);
    default:
        return 0;
    }
}

HINTERNET hInternet = NULL;
unsigned int UpdaterInit() {
    HMODULE hWinInet = LoadLibraryW(L"wininet.dll");
    if (!hWinInet) {
        log_printf("Update: failed to load wininet.dll. Cannot proceed\r\n");
        return GetLastError();
    }

#define WININET_FUNC(f) Imp_##f = (decltype(f)*)GetProcAddress(hWinInet, #f);
    WININET_FUNCS
#undef WININET_FUNC

#define WININET_FUNC(f) !Imp_##f ||
        if (WININET_FUNCS 0) {
#undef WININET_FUNC
            log_print("Update: one or more WinInet functions was not found\r\n");
#define WININET_FUNC(f) log_printf(#f ": 0x%04x\r\n", Imp_##f);
            WININET_FUNCS
#undef WININET_FUNCS
            return ERROR_PROC_NOT_FOUND;
        }

    wchar_t user_agent[128] = {};
    _snwprintf(user_agent, 128, L"thprac %u.%u.%u.%u on %s", VER_PARAMS_CUR, windows_version_str());

    hInternet = Imp_InternetOpenW(user_agent, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) {
        log_printf("Update: failed to initialize WinInet, updates will not be checked.\r\n");
        return GetLastError();
    }

    DWORD ignore = 1;
    if (!Imp_InternetSetOptionW(hInternet, INTERNET_OPTION_IGNORE_OFFLINE, &ignore, sizeof(ignore))) {
        log_printf("Update: failed to configure WinInet options, updates will not be checked.\r\n");

        Imp_InternetCloseHandle(hInternet);
        hInternet = NULL;
        return GetLastError();
    }

    background_update_check = new BackgroundUpdateCheck;
    background_update_check->dl = {
        .url = UPDATE_JSON_URL,
        .callback = BackgroundUpdateCallback,
    };

    return 0;
}

bool ParseUpdateJson(char* buf, size_t len, UpdateJson* out) {
    yyjson_doc* doc = yyjson_read_opts(buf, len, YYJSON_READ_JSON5, nullptr, nullptr);
    if (!doc) {
        return false;
    }
    
    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        return false;
    }

    ThpracVersion ver = {};

    if (yyjson_val* version_val = yyjson_obj_get(root, "version")) {
        if (yyjson_arr_size(version_val) == 4) {
            size_t idx, max;
            yyjson_val* val;
            yyjson_arr_foreach(version_val, idx, max, val) {
                if (yyjson_is_int(val)) {
                    ver[idx] = (uint8_t)unsafe_yyjson_get_int(val);
                } else {
                    return false;
                }
            }
        }
        else if (yyjson_is_str(version_val)) {
            ver = ParseVersion(unsafe_yyjson_get_str(version_val));
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

    yyjson_val* direct_link_json = yyjson_obj_get(root, "direct_link");
    const char* direct_link = yyjson_get_str(direct_link_json);
    if (!direct_link) {
        return false;
    }
    size_t direct_link_len = yyjson_get_len(direct_link_json);

    size_t changelog_link_len = 0;
    yyjson_val* changelog_link_json = yyjson_obj_get(root, "changelog");
    const char* changelog_link = yyjson_get_str(changelog_link_json);
    if (!changelog_link) {
        yyjson_val* line0_json = yyjson_arr_get(yyjson_obj_get(root, "description"), 0);
        const char* line0 = yyjson_get_str(line0_json);

        if (line0) {
            std::string_view begin("Changelog: ");
            if (strncmp(line0, begin.data(), begin.size()) == 0) {
                changelog_link = line0 + begin.size();
                changelog_link_len = yyjson_get_len(line0_json) - begin.size();
            }
        }
    }
    else {
        changelog_link_len = yyjson_get_len(changelog_link_json);
    }
    out->ver = ver;
    memcpy(out->url, direct_link, direct_link_len);
    memcpy(out->changelog, changelog_link, changelog_link_len);

    return true;
}

bool CompleteUpdate(unsigned char* buf, size_t len, const wchar_t* pCmdLine, int nCmdShow, UpdateJson* updateJson) {
    if (!pCmdLine) {
        pCmdLine = L"";
    }

    // Determine the name of the post update exe file.
    wchar_t new_exe_path[MAX_PATH + 1] = {};
    auto new_exe_path_len = GetCurrentDirectoryW(MAX_PATH, new_exe_path);
    new_exe_path[new_exe_path_len++] = L'\\';
    wchar_t* new_fn = new_exe_path + new_exe_path_len;
    
    auto& self_path = CurrentPeb()->ProcessParameters->ImagePathName;
    wchar_t self_fn[MAX_PATH] = {};
    size_t self_len = 0;

    for (const wchar_t *self_begin = self_path.Buffer + self_path.Length / 2, *self_end = self_path.Buffer + self_path.Length / 2;;) {
        if (*--self_begin == L'\\') {
            self_begin++;
            memcpy(self_fn, self_begin, (char*)self_end - (char*)self_begin);
            self_len = self_end - self_begin;
            break;
        }
        if (self_begin == self_path.Buffer) {
            break;
        }
    }
    
    switch (gSettings.filename_after_update) {
    case FN_UPDATE_KEEP_DOWNLOADED:
        new_exe_path_len += _snwprintf(new_fn, MAX_PATH - new_exe_path_len, L"thprac.v%u.%u.%u.%u.exe", VER_PARAMS(updateJson->ver));
        break;
    case FN_UPDATE_USE_PREVIOUS:
        memcpy(new_fn, self_fn, self_len * sizeof(wchar_t));
        new_exe_path_len += self_len;
        break;
    case FN_UPDATE_THPRAC_EXE:
        memcpy(new_fn, SIZED(L"thprac.exe"));
        new_exe_path_len += t_strlen(L"thprac.exe");
        break;
    }
    // ---

    // Rename to <self_name>.exe.old
    wchar_t move_to[MAX_PATH] = {};
    memcpy(move_to, self_fn, self_len * sizeof(wchar_t));
    memcpy(move_to + self_len, SIZED(L".old"));

    DeleteFileW(move_to);
    if (!MoveFileW(self_fn, move_to)) {
        // TODO: revisit
        log_printf("Update error: failed to rename running executable, code %d\r\n", GetLastError());
        return false;
    }
    // ---

    // Write new file
    HANDLE new_file = CreateFileW(new_exe_path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!new_file) {
        // Well this was a bust, we renamed ourselves to <self_name>.exe.old but then we can't even write our new exe file.
        // TODO: what else should happen here?
        DWORD err = GetLastError();
        char buf[MAX_PATH * 2];
        int wrote = WideCharToMultiByte(CP_UTF8, 0, new_exe_path, new_exe_path_len, buf, MAX_PATH * 2, nullptr, nullptr);
        log_printf("Update error: failed to open %.*s for writing, error %d\r\n", err, wrote, buf);
        DeleteFileW(self_fn);
        MoveFileW(move_to, self_fn);
        return false;
    }
    DWORD byteRet;
    if (WriteFile(new_file, buf, len, &byteRet, nullptr) && len == byteRet) {
        CloseHandle(new_file);
    } else {
        DWORD err = GetLastError();
        char buf[MAX_PATH * 2];
        int wrote = WideCharToMultiByte(CP_UTF8, 0, new_exe_path, new_exe_path_len, buf, MAX_PATH * 2, nullptr, nullptr);
        log_printf("Update error: failed to write %.*s, error %d\r\n -> Wrote %d/%d bytes\r\n", wrote, buf, err, len, byteRet);
        CloseHandle(new_file);
        DeleteFileW(self_fn);
        MoveFileW(move_to, self_fn);
        return false;
    }
    // ---
    
    // Run new file.
    wchar_t new_cmd_line[8192];
    memcpy(new_cmd_line + 1, new_exe_path, new_exe_path_len * 2);
    new_cmd_line[0] = L'"';
    new_cmd_line[new_exe_path_len + 1] = L'"';
    new_cmd_line[new_exe_path_len + 2] = L' ';

    auto cmd_line_len = t_strlen(pCmdLine);
    memcpy(new_cmd_line + new_exe_path_len + 3, pCmdLine, cmd_line_len * sizeof(wchar_t));
    new_cmd_line[new_exe_path_len + cmd_line_len + 3] = 0;

    ShellExecuteW(NULL, L"open", new_exe_path, new_cmd_line, old_working_dir, nCmdShow);
    // ---

    return true;
}

unsigned int DummyCallback([[maybe_unused]] DOWNLOAD_CALLBACK_REASON reason, [[maybe_unused]] DownloadParams* dl) {
    return 0;
}
unsigned int WINAPI DownloadFile(DownloadParams* params) {
    auto* callback = params->callback ? params->callback : DummyCallback;
    
    HINTERNET hFile = Imp_InternetOpenUrlW(hInternet, params->url, nullptr, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION, 0);
    if (!hFile) {
        return GetLastError();
    }
    defer(Imp_InternetCloseHandle(hFile));

    DWORD fileSize = 0;
    DWORD byteRet = sizeof(fileSize);
    if (!Imp_HttpQueryInfoW(hFile, HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_CONTENT_LENGTH, &fileSize, &byteRet, 0)) {
        return ERROR_HTTP_INVALID_QUERY_REQUEST;
    }

    auto remSize = fileSize;

    params->filesize = fileSize;
    callback(DLCB_FILESIZE, params);
    while (remSize) {
        if (params->abort_signal) {
            return 0x80000000;
        }

        DWORD readSize = 0;
        if (!Imp_InternetQueryDataAvailable(hFile, &readSize, 0, 0)) {
            readSize = remSize;
        }
        if (readSize == 0) {
            return ERROR_INTERNET_DISCONNECTED;
        }
        auto bufPos = params->out.size();
        params->out.resize(bufPos + readSize);

        if (!Imp_InternetReadFile(hFile, params->out.data() + bufPos, readSize, &byteRet)) {
            return GetLastError();
        }
        remSize -= byteRet;
        callback(DLCB_PROGRESS, params);
    }
    return callback(DLCB_COMPLETE, params);
}

struct DownloadPopupContext {
    DownloadParams dl;
    uintptr_t filesize;
    HWND hwnd;
    HBRUSH hBlack;
    HBRUSH hBlue;
};

static LRESULT CALLBACK DownloadPopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCTW*)lParam)->lpCreateParams);
        break;
    }
    case WM_CLOSE: {
        auto* dl = (DownloadParams*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        dl->abort_signal = true;
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        auto* ctx = (DownloadPopupContext*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

        RECT wndRect = { 0, 0, 353, 55 };
        FillRect(hdc, &wndRect, ctx->hBlack);
        RECT pbRect = { 7, 10, 7, 45 };
        if (ctx->filesize) {
            pbRect.right += (ctx->dl.out.size() * 338) / ctx->filesize;
            FillRect(hdc, &pbRect, ctx->hBlue);
        }
        EndPaint(hwnd, &ps);
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static unsigned int DownloadPopupCallback([[maybe_unused]] DOWNLOAD_CALLBACK_REASON reason, DownloadParams* dl) {
    InvalidateRect(((DownloadPopupContext*)dl)->hwnd, NULL, FALSE);
    return 0;
}

static bool DownloadPopup(HINSTANCE hInstance, const wchar_t* title, const wchar_t* url, std::vector<unsigned char>& out) {
    RECT wndRect = { 0, 0, 353, 55 };
    DWORD style = WS_CAPTION | WS_SYSMENU;

    AdjustWindowRect(&wndRect, style, FALSE);
    int width = wndRect.right - wndRect.left, height = wndRect.bottom - wndRect.top;

    DownloadPopupContext dl = {
        .dl = {
            .url = url,
            .callback = DownloadPopupCallback,
        },
        .hBlack = CreateSolidBrush(RGB(13, 13, 13)),
        .hBlue = CreateSolidBrush(RGB(51, 153, 255)),
    };

    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME,
        L"thprac update", title,
        style, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, &dl
    );

    dl.hwnd = hwnd;

    if (auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)) {
        MONITORINFO mi = { .cbSize = sizeof(mi) };
        if (GetMonitorInfoW(monitor, &mi)) {
            int x = (mi.rcMonitor.right - mi.rcMonitor.left) / 2 - width / 2;
            int y = (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2 - height / 2;
            SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }

    ShowWindow(hwnd, SW_SHOW);
    
    HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DownloadFile, &dl, 0, nullptr);
    DWORD waitStatus;

    MSG msg = {};
    for (;;) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) && msg.message != WM_QUIT) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        waitStatus = MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLEVENTS);
        if (waitStatus == WAIT_OBJECT_0) {
            break;
        }
    }
    DestroyWindow(hwnd);
    DeleteObject(dl.hBlack);
    DeleteObject(dl.hBlue);

    out = std::move(dl.dl.out);

    return !dl.dl.abort_signal;
}

static const wchar_t* UPDATE_DIALOG_CHECKING[] = { L"检查更新中", L"Checking for update", L"更新を確認しています" };
static const wchar_t* UPDATE_DIALOG_UPDATING[] = { L"正在更新thprac", L"Updating thprac", L"thpracを更新しています" };
static const wchar_t* UPDATE_DIALOG_TITLE[] = { L"更新thprac？", L"Update thprac?", L"thpracを更新しますか？" };
static const wchar_t* UPDATE_DIALOG_TEXT[] = {
    L"已检测到thprac的新版本。\n是否现在下载更新？",
    L"An update for thprac has been detected.\nDownload update now?",
    L"thpracの更新が検出されました。\n今すぐアップデートをダウンロードしますか？"
};

static bool PreLaunchUpdateConfirm() {
    auto locale = Gui::LocaleGet();

    auto* title = UPDATE_DIALOG_TITLE[locale];
    auto* text = UPDATE_DIALOG_TEXT[locale];

    return MessageBoxW(NULL, text, title, MB_YESNO) == IDYES;
}

bool PreLaunchUpdate(HINSTANCE hInstance, wchar_t* pCmdLine, int nCmdShow, bool update_without_confirmation) {
    auto l = Gui::LocaleGet();

    WNDCLASS wc = { 
        .lpfnWndProc = DownloadPopupWndProc,
        .hInstance = hInstance,
        .lpszClassName = L"thprac update",
    };
    RegisterClassW(&wc);
    defer(UnregisterClassW(L"thprac update", hInstance));

    std::vector<unsigned char> buf;
    if (!DownloadPopup(hInstance, UPDATE_DIALOG_CHECKING[l], UPDATE_JSON_URL, buf)) {
        return false;
    }

    UpdateJson updateJson;
    if (!ParseUpdateJson((char*)buf.data(), buf.size(), &updateJson)) {
        return false;
    }

    buf.clear();
    if (updateJson.ver > gVersion
        && (update_without_confirmation || PreLaunchUpdateConfirm())
        && DownloadPopup(
            hInstance,
            UPDATE_DIALOG_UPDATING[l],
            utf8_to_utf16(updateJson.url).c_str(),
            buf)
        )
    {
        CompleteUpdate(buf.data(), buf.size(), pCmdLine, nCmdShow, &updateJson);
        return true;
    }
    
    return false;
}

static inline bool DeleteFileLoop(const wchar_t* fileName, size_t timeout) {
    auto attr = GetFileAttributesW(fileName);
    if (attr == INVALID_FILE_ATTRIBUTES || attr & FILE_ATTRIBUTE_DIRECTORY || attr & FILE_ATTRIBUTE_READONLY) {
        return false;
    }

    size_t localTimeout = 0;
    while (true) {
        if (DeleteFileW(fileName)) {
            return true;
        }
        Sleep(1000);
        localTimeout += 1000;
        if (localTimeout > timeout) {
            return false;
        }
    }
}

// This function is only needed to support upgrading from thprac versions that are coded with the assumption that a Windows executable cannot MoveFile itself
// TODO: do we want to remove this at some point? Maybe if UPDATE_JSON_URL has to be changed at some point, an old one could stay over on GitHub, pointing
// to the first thprac version that uses the new one, and this change could also be used as an opportunity to get rid of this.
bool LegacyPostUpdate(LPCWSTR lpCmdLine, int nCmdShow) {
    auto* substr = wcsstr(lpCmdLine, L"--update-launcher-1 ");

    if (!substr) {
        return false;
    }

    substr += 20;
    
    wchar_t finalPath[MAX_PATH + 1] = {};
    size_t substr_len = t_strlen(substr);
    memcpy(finalPath, substr, substr_len * sizeof(wchar_t));
    
    auto self = CurrentPeb()->ProcessParameters->ImagePathName;

    if (gSettings.filename_after_update != FN_UPDATE_USE_PREVIOUS) {
        for (size_t i = substr_len; i > 0; i--) {
            if (substr[i] == L'\\') {
                i++;
                substr_len = i;
                break;
            }
        }
        auto self_name = L"thprac.exe"_wZ;
        if (gSettings.filename_after_update == FN_UPDATE_KEEP_DOWNLOADED) {
            self_name = self;
            for (USHORT i = self_name.Length / 2; i > 0; i--) {
                if (self_name.Buffer[i] == L'\\') {
                    i++;
                    self_name.Buffer = self_name.Buffer + i;
                    self_name.Length = self_name.Length - i * 2;
                    self_name.MaximumLength = self_name.Length;
                    break;
                }
            }
        }
        memset(finalPath + substr_len, 0, MAX_PATH - substr_len);
        memcpy(finalPath + substr_len, self_name.Buffer, self_name.Length);
    }

    DeleteFileLoop(finalPath, 20000);
    // No need for the other 2 stages (initiated with --update-launcher-2 and --update-launcher) when you can just do this.
    MoveFileW(self.Buffer, finalPath);
    wchar_t finalDir[MAX_PATH + 1] = {};
    memcpy(finalDir, finalPath, substr_len * sizeof(wchar_t));

    // Rerun because moving your own exe file doesn't update CurrentPeb()->ProcessParameters->ImagePathName
    // Rerunning without passing on any command line parameters because the old updater wasn't really designed with
    // thprac supporting more command line parameters than the --update-launcher flags in mind.
    ShellExecuteW(NULL, L"open", finalPath, nullptr, finalDir, nCmdShow);
    return true;
}
}