#include "thprac_data_anly.h"
#include <LzmaEnc.h>
#include <LzmaDec.h>

namespace THPrac {

void* SzAlloc(ISzAllocPtr p, size_t size)
{
    return malloc(size);
}
void SzFree(ISzAllocPtr p, void* address)
{
    free(address);
}
ISzAlloc g_Alloc = { SzAlloc, SzFree };

ReplayRecord* g_anlyData = nullptr;
uint32_t g_anlyRecFreq = 60;
uint32_t g_prevFrame = 0;
void AnlyDataInit()
{
    if (!g_anlyData) {
        g_anlyData = new ReplayRecord();
    }
}
void AnlyEventRec(EventRecord& data)
{
    uint32_t stage = U8_REF(DATA_STAGE);
    uint32_t frame = U32_REF(DATA_FRAME);

    auto& stageData = g_anlyData->stageData;
    if (!stageData.size() || stageData.back().stageId != stage) {
        stageData.emplace_back(stage);
    }

    auto& frameData = stageData.back().frameData;
    if (!frameData.size() || frameData.back().frame != frame) {
        frameData.emplace_back(frame);
    }

    frameData.back().eventData.push_back(data);
}
ReplayRecord& AnlyDataGet()
{
    return *g_anlyData;
}
void AnlyDataPack(ReplayRecord& record, void** outBuffer, size_t* size)
{
    std::vector<uint32_t> recordData;
    record.GetRawData(recordData);
    auto recordDataSize = recordData.size() * 4;

    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    props.level = 9;
    props.writeEndMark = 0;

    auto dataBuf = malloc(recordDataSize + 64);
    memset(dataBuf, 0, recordDataSize + 64);
    SizeT dataLen = recordDataSize + 16;
    SizeT propLen = 16;

    LzmaEncode(((Byte*)dataBuf) + 32, &dataLen, (Byte*)recordData.data(), recordDataSize, &props, ((Byte*)dataBuf) + 16, &propLen, 0, NULL, &g_Alloc, &g_Alloc);
    ((uint32_t*)dataBuf)[0] = 'RPHT';
    ((uint32_t*)dataBuf)[1] = 0x160004ff;
    ((uint32_t*)dataBuf)[3] = recordDataSize;

    *outBuffer = dataBuf;
    *size = dataLen + 32;
}
void AnlyDataRead(ReplayRecord& record, void* inputBuffer, size_t size)
{
    uint32_t* dataBuf = (uint32_t*)inputBuffer;
    if (dataBuf[0] == 'RPHT' && dataBuf[1] == 0x160004ff) {
        auto outputBuf = malloc(dataBuf[3] + 32);
        SizeT outputLen = dataBuf[3];
        SizeT srcLen = size - 32;
        ELzmaStatus status;

        LzmaDecode((Byte*)outputBuf, &outputLen, ((Byte*)inputBuffer) + 32, &srcLen, ((Byte*)inputBuffer) + 16, 5, LZMA_FINISH_ANY, &status, &g_Alloc);

        record.ReadRawData(outputBuf, outputLen);
        free(outputBuf);
    }
}
void AnlyDataReset()
{
    g_anlyData->stageData.clear();
}
void AnlyDataStageStart()
{
    if (U8_REF(DATA_STAGE) == U8_REF(DATA_STARTING_STAGE)) {
        AnlyDataReset();
    }
}
void AnlyDataCollect()
{
    EventRecord rec(EVENT_DATA_COLLECT);
    DataBatchRem(rec);
    AnlyEventRec(rec);
}
void AnlyAutoRecTest()
{
    if (DataRef<SYSFLAG_INGAME>()) {
        uint32_t stage = U8_REF(DATA_STAGE);
        uint32_t frame = U32_REF(DATA_FRAME);
        auto& stageData = g_anlyData->stageData;
        if (stageData.size() && stageData.back().stageId == stage) {
            auto& frameData = stageData.back().frameData;
            if (frameData.size() && frameData.back().frame == frame) {
                auto& eventData = frameData.back().eventData;
                for (auto& event : eventData) {
                    if (event.id == EVENT_DATA_COLLECT) {
                        return;
                    }
                }
                EventRecord rec(EVENT_DATA_COLLECT);
                DataBatchRem(rec);
                eventData.push_back(rec);
                g_prevFrame = DataRef<DATA_FRAME>();
                return;
            }
        }

        if (g_prevFrame != frame && !(frame % g_anlyRecFreq)) {
            EventRecord rec(EVENT_DATA_COLLECT);
            DataBatchRem(rec);
            AnlyEventRec(rec);
            g_prevFrame = DataRef<DATA_FRAME>();
        }
    }
}
void AnlyWriteTest()
{
    OPENFILENAME ofn;
    // a another memory buffer to contain the file name
    wchar_t szFile[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = 0;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = nullptr;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    //ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);

    if (szFile[0]) {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        DWORD fileSize = 0;
        HANDLE hFileMap = NULL;
        void* pFileMapView = nullptr;
        DWORD bytesProcessed;
        hFile = CreateFileW(ofn.lpstrFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        SetEndOfFile(hFile);
        //fileSize = GetFileSize(hFile, NULL);
        //hFileMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
        //pFileMapView = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, fileSize);
        ;
        void* buf;
        size_t size;
        AnlyDataPack(AnlyDataGet(), &buf, &size);
        WriteFile(hFile, buf, size, &bytesProcessed, NULL);
        free(buf);
        //AnlyDBTest();

        if (pFileMapView)
            UnmapViewOfFile(pFileMapView);
        if (hFileMap)
            CloseHandle(hFileMap);
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
    }
}
void AnlyLoadTest()
{
    OPENFILENAME ofn;
    // a another memory buffer to contain the file name
    wchar_t szFile[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = 0;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = nullptr;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    //ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);

    if (szFile[0]) {
        MappedFile file(szFile);
        AnlyDataRead(AnlyDataGet(), file.fileMapView, file.fileSize);
    }
}

void(* g_dataBatchRem)(EventRecord&) = nullptr;
void(* g_dataBatchResetRem)() = nullptr;
void DataBatchRem(EventRecord& rec, void(* callback)(EventRecord&))
{
    if (callback) {
        g_dataBatchRem = callback;
        return;
    }
    if (g_dataBatchRem) {
        g_dataBatchRem(rec);
    }
}
void DataBatchResetRem(void(* callback)())
{
    if (callback) {
        g_dataBatchResetRem = callback;
        return;
    }
    if (g_dataBatchResetRem) {
        g_dataBatchResetRem();
    }
}

struct InsHookCtx {
    InsHookCtx() = default;
    uint32_t fileId = 0;
    uint32_t addr = 0;
    uint32_t backup = 0;
    uint16_t id = 0;
};
uint16_t g_insHooksId = 0;
uint32_t g_insHookStage = 0;
void* g_insSwitchEip = nullptr;
std::vector<std::vector<InsHookCtx>>* g_insHooks = nullptr;
std::function<uint32_t(PCONTEXT)> g_insHookSwitchIdGetter;
void InsHookInit(uint16_t insHookId, void* insSwitchEip, std::function<uint32_t(PCONTEXT)>&& switchIdGetter)
{
    g_insHooksId = insHookId;
    g_insHookSwitchIdGetter = switchIdGetter;
    g_insSwitchEip = insSwitchEip;
    if (!g_insHooks) {
        g_insHooks = new std::vector<std::vector<InsHookCtx>>();
        g_insHooks->resize(24);
    }
}
void InsHookRegister(uint32_t stage, uint32_t fileId, uint32_t addr, uint16_t id)
{
    if (g_insHooks->size() < stage + 1) {
        g_insHooks->resize(stage + 1);
    }
    (*g_insHooks)[stage].emplace_back();
    (*g_insHooks)[stage].back().fileId = fileId;
    (*g_insHooks)[stage].back().addr = addr;
    (*g_insHooks)[stage].back().id = id;
}
void InsHookApply()
{
    uint32_t stage = U8_REF(DATA_STAGE);
    g_insHookStage = stage;
    auto filePtr = U32_PTR(DATA_ECL_BASE);
    for (auto& ctx : (*g_insHooks)[stage]) {
        if (!ctx.backup) {
            ctx.backup = *(uint32_t*)(filePtr[ctx.fileId] + ctx.addr);
        }
        *(uint16_t*)(filePtr[ctx.fileId] + ctx.addr) = g_insHooksId;
        *(uint16_t*)(filePtr[ctx.fileId] + ctx.addr + 2) = ctx.id;
    }
}
void __stdcall InsHookCallback(PCONTEXT pCtx)
{
    auto currentInsPtr = g_insHookSwitchIdGetter(pCtx);
    bool triggered = false;
    auto filePtr = U32_PTR(DATA_ECL_BASE);
    auto switchId = *(uint16_t*)(currentInsPtr + 2); // Compiler bug

    for (auto& ctx : (*g_insHooks)[g_insHookStage]) {
        if (switchId == ctx.id) {
            if (!triggered) {
                triggered = true;
                AnlyEventRec(EVENT_SECTION, ctx.id);
                DataRef<SYSFLAG_LAST_SECTION>() = ctx.id;
            }
            *(uint32_t*)(filePtr[ctx.fileId] + ctx.addr) = ctx.backup;
        }
    }
    pCtx->Eip = (DWORD)g_insSwitchEip;
}


enum InputType {
    INPUT_NONE,
    INPUT_SHOT,
    INPUT_BOMB,
    INPUT_SLOW,
    INPUT_SKIP,
    INPUT_SPECIAL1,
    INPUT_SPECIAL2,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT
};
void InputButton(InputType type, int bitmask, ImVec2& buttonSize)
{
    if (type == INPUT_NONE) {
        ImGui::InvisibleButton("##__not_visible", buttonSize);
        return;
    }

    bool bitTest = (U16_REF(DATA_GAME_INPUT) & (1 << bitmask));
    if (bitTest) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.10f, 0.10f, 1.0f)); // 0.12 0.30 0.55 1.0 /0.55 0.10 0.10
    }

    switch (type) {
    case INPUT_SHOT:
        ImGui::Button("##Shot", buttonSize);
        break;
    case INPUT_BOMB:
        ImGui::Button("##Bomb", buttonSize);
        break;
    case INPUT_SLOW:
        ImGui::Button("##Slow", buttonSize);
        break;
    case INPUT_SKIP:
        ImGui::Button("##Skip", buttonSize);
        break;
    case INPUT_UP:
        ImGui::ArrowButtonEx("##UP", ImGuiDir_Up, buttonSize, 0);
        break;
    case INPUT_LEFT:
        ImGui::ArrowButtonEx("#LEFT", ImGuiDir_Left, buttonSize, 0);
        break;
    case INPUT_DOWN:
        ImGui::ArrowButtonEx("##DOWN", ImGuiDir_Down, buttonSize, 0);
        break;
    case INPUT_RIGHT:
        ImGui::ArrowButtonEx("##RIGHT", ImGuiDir_Right, buttonSize, 0);
        break;
    default:
        ImGui::InvisibleButton("##__not_visible", buttonSize);
        break;
    }

    if (bitTest) {
        ImGui::PopStyleColor();
    }
}
void InputDispWidget(int shot, int bomb, int slow, int up, int down, int left, int right, int skip, float buttonSize)
{
    ImVec2 buttonSizeVec(buttonSize, buttonSize);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.30f, 0.55f, 1.0f));

    ImGui::BeginDisabled();
    ImGui::PopStyleVar();
    InputButton(INPUT_SLOW, slow, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_SHOT, shot, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_BOMB, bomb, buttonSizeVec);
    ImGui::SameLine(0.0f, 4.0f);
    InputButton(INPUT_NONE, 0, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_UP, up, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_NONE, 0, buttonSizeVec);

    InputButton(INPUT_SKIP, skip, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_NONE, 0, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_NONE, 0, buttonSizeVec);
    ImGui::SameLine(0.0f, 4.0f);
    InputButton(INPUT_LEFT, left, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_DOWN, down, buttonSizeVec);
    ImGui::SameLine(0.0f, 0.0f);
    InputButton(INPUT_RIGHT, right, buttonSizeVec);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    ImGui::EndDisabled();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}
}
