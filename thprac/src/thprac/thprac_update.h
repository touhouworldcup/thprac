#include <Windows.h>

#include "thprac_cfg.h"
#include "thprac_version.h"

#include <stdint.h>
#include <string>
#include <vector>

#pragma once
namespace THPrac {

enum DOWNLOAD_CALLBACK_REASON {
	DLCB_FILESIZE,
	DLCB_PROGRESS,
    DLCB_COMPLETE,
};

struct DownloadParams {
    const wchar_t* url = nullptr;
    unsigned int (*callback)(DOWNLOAD_CALLBACK_REASON reason, DownloadParams* dl) = nullptr;
    bool abort_signal = false;

    uintptr_t filesize = 0;
    std::vector<unsigned char> out;
};

struct UpdateJson {
    ThpracVersion ver = {};
    char url[256] = {};
    char changelog[256] = {};
};

struct BackgroundUpdateCheck {
    DownloadParams dl;

    // hThread determines state
    //  -> hThread == 0: not checking at all
    //  -> WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT: checking
    //  -> WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0: check succeeded
    // Thread exit code encodes additional state.
    HANDLE hThread = NULL;
    UpdateJson updateJson;
};

extern BackgroundUpdateCheck* background_update_check;

__forceinline bool UpdaterInitialized() {
    return background_update_check;
}

unsigned int UpdaterInit();
unsigned int WINAPI DownloadFile(DownloadParams* params);

bool PreLaunchUpdate(HINSTANCE hInstance, wchar_t* pCmdLine, int nCmdShow, bool ask);
bool ParseUpdateJson(char* buf, size_t len, UpdateJson* out);
void CompleteUpdate(unsigned char* buf, size_t len, wchar_t* pCmdLine, int nCmdShow, UpdateJson* updateJson);

}
