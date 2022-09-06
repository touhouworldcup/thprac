#pragma once

#include "thprac_hook.h"
#include "thprac_utils.h"
#include <MinHook.h>
#include <cstdint>
#include <vector>

namespace THPrac {
struct EventRecord {
    union OmniData32 {
        uint32_t i;
        float f;
    };

    template <class... Args>
    EventRecord(uint32_t _id, Args... rest)
    {
        id = _id;
        PushBack(rest...);
    }
    EventRecord(uint32_t _id)
    {
        id = _id;
    }
    EventRecord() = delete;

    size_t GetDataSize()
    {
        return (valueData.size() * 4 + 4);
    }
    void GetRawData(std::vector<uint32_t>& output)
    {
        output.push_back(id);
        output.push_back(valueData.size());
        for (auto& value : valueData) {
            output.push_back(value.i);
        }
    }
    int ReadRawData(void* input, int size, void** pOut = nullptr)
    {
        if (size < 2 * 4) {
            return -1;
        }
        uint32_t* input32 = (uint32_t*)input;
        id = input32[0];

        uint32_t* dataStart = &(input32[2]);
        int sizeProc = size - 4;
        for (uint32_t i = 0; i < input32[1]; ++i) {
            if (sizeProc <= 0) {
                valueData.clear();
                return -1;
            }
            OmniData32 d { *dataStart };
            valueData.push_back(d);
            dataStart++;
            sizeProc -= 4;
        }

        if (pOut) {
            *pOut = dataStart;
        }
        return sizeProc;
    }

    template <typename T, class... Args>
    void PushBack(T _data, Args... rest)
    {
        PushBack(_data);
        PushBack(rest...);
    }
    template <typename T>
    void PushBack(T i)
    {
        OmniData32 _data;
        _data.i = i;
        valueData.push_back(_data);
    }
    template <>
    void PushBack(float f)
    {
        OmniData32 _data;
        _data.f = f;
        valueData.push_back(_data);
    }

    uint32_t id;
    std::vector<OmniData32> valueData;
};

struct FrameRecord {
    template <class... Args>
    FrameRecord(uint32_t _frame, Args... rest)
    {
        frame = _frame;
        PushBack(rest...);
    }
    FrameRecord(uint32_t _frame)
    {
        frame = _frame;
    }
    FrameRecord() = delete;

    size_t GetDataSize()
    {
        size_t size = 0;
        for (auto& d : eventData) {
            size += d.GetDataSize();
        }
        return size + 8;
    }
    void GetRawData(std::vector<uint32_t>& output)
    {
        output.push_back(frame);
        output.push_back(eventData.size());
        for (auto& event : eventData) {
            event.GetRawData(output);
        }
    }
    int ReadRawData(void* input, int size, void** pOut = nullptr)
    {
        if (size < 2 * 4) {
            return -1;
        }
        uint32_t* input32 = (uint32_t*)input;
        frame = input32[0];

        void* eventDataStart = &(input32[2]);
        int sizeProc = size - 2 * 4;
        for (uint32_t i = 0; i < input32[1]; ++i) {
            if (sizeProc <= 0) {
                eventData.clear();
                return -1;
            }
            eventData.emplace_back(0);
            sizeProc = eventData.back().ReadRawData(eventDataStart, sizeProc, &eventDataStart);
        }

        if (pOut) {
            *pOut = eventDataStart;
        }
        return sizeProc;
    }

    template <class... Args>
    void PushBack(EventRecord _data, Args... rest)
    {
        PushBack(_data);
        PushBack(rest...);
    }
    void PushBack(EventRecord _data)
    {
        eventData.push_back(_data);
    }
    template <class... Args>
    void EmplaceBack(uint16_t id, Args... rest)
    {
        eventData.emplace_back(id, rest...);
    }

    uint32_t frame;
    std::vector<EventRecord> eventData;
};

struct StageRecord {
    StageRecord(uint32_t stage)
    {
        stageId = stage;
    }
    StageRecord() = delete;

    size_t GetDataSize()
    {
        size_t size = 0;
        for (auto& d : frameData) {
            size += d.GetDataSize();
        }
        return size + 8;
    }
    void GetRawData(std::vector<uint32_t>& output)
    {
        output.push_back(stageId);
        output.push_back(frameData.size());
        for (auto& frame : frameData) {
            frame.GetRawData(output);
        }
    }
    int ReadRawData(void* input, int size, void** pOut = nullptr)
    {
        if (size < 2 * 4) {
            return -1;
        }
        uint32_t* input32 = (uint32_t*)input;
        stageId = input32[0];

        void* frameDataStart = &(input32[2]);
        int sizeProc = size - 2 * 4;
        for (uint32_t i = 0; i < input32[1]; ++i) {
            if (sizeProc <= 0) {
                frameData.clear();
                return -1;
            }
            frameData.emplace_back(0);
            sizeProc = frameData.back().ReadRawData(frameDataStart, sizeProc, &frameDataStart);
        }

        if (pOut) {
            *pOut = frameDataStart;
        }
        return sizeProc;
    }

    uint32_t stageId;
    std::vector<FrameRecord> frameData;
};

struct ReplayRecord {
    ReplayRecord() = default;

    void GetRawData(std::vector<uint32_t>& output)
    {
        output.push_back('RPHT');
        output.push_back(0x168504ff);
        output.push_back(*(uint32_t*)(&gameId));
        output.push_back(stageData.size());

        output.push_back(time.dwLowDateTime);
        output.push_back(time.dwHighDateTime);
        output.push_back(0);
        output.push_back(0);

        for (auto& h : hash) {
            output.push_back(h);
        }

        for (auto& stage : stageData) {
            stage.GetRawData(output);
        }
    }
    int ReadRawData(void* input, int size)
    {
        if (size < 12 * 4) {
            return -1;
        }
        uint32_t* input32 = (uint32_t*)input;

        if (input32[0] == 'RPHT' && input32[1] == 0x168504ff) {
            gameId = *((float*)&(input32[2]));
            time.dwLowDateTime = input32[4];
            time.dwHighDateTime = input32[5];

            hash[0] = input32[8];
            hash[1] = input32[9];
            hash[2] = input32[10];
            hash[3] = input32[11];

            void* stageDataStart = &(input32[12]);
            int sizeProc = size - 12 * 4;
            for (uint32_t i = 0; i < input32[3]; ++i) {
                if (sizeProc <= 0) {
                    stageData.clear();
                    gameId = 0.0f;
                    return -1;
                }
                stageData.emplace_back(0);
                sizeProc = stageData.back().ReadRawData(stageDataStart, sizeProc, &stageDataStart);
            }
            return sizeProc;
        }

        return -1;
    }

    float gameId = 0.0;
    FILETIME time;
    uint32_t hash[4] {};
    std::vector<StageRecord> stageData;
};


void AnlyDataInit();
void AnlyEventRec(EventRecord& data);
template <class... Args>
void AnlyEventRec(uint32_t id, Args... rest)
{
    EventRecord value(id, rest...);
    AnlyEventRec(value);
}
ReplayRecord& AnlyDataGet();
void AnlyDataReset();
void AnlyDataStageStart();
void AnlyDataCollect();
void AnlyAutoRecTest();
void AnlyWriteTest();
void AnlyLoadTest();


#define FLT_ARG(...) -1, [](int retAddr) -> uint32_t { auto addr = GetMemAddr(__VA_ARGS__); return retAddr ? addr : (*(uint32_t*)addr); }
#define FLT_REF(id) (*(float*)DataRef<id>(-1))
#define FLT_PTR(id) ((float*)DataRef<id>(-1))
#define INT_ARG(type, ...) -1, [](int retAddr) -> uint32_t { auto addr = GetMemAddr(__VA_ARGS__); return retAddr ? addr : (uint32_t)(*(type*)addr); }
#define INT_REF(id, type) (*(type*)DataRef<id>(-1))
#define INT_PTR(id, type) ((type*)DataRef<id>(-1))
#define I32_ARG(...) INT_ARG(int32_t, __VA_ARGS__)
#define I32_REF(id) INT_REF(id, int32_t)
#define I32_PTR(id) INT_PTR(id, int32_t)
#define I16_ARG(...) INT_ARG(int16_t, __VA_ARGS__)
#define I16_REF(id) INT_REF(id, int16_t)
#define I16_PTR(id) INT_PTR(id, int16_t)
#define I8_ARG(...) INT_ARG(int8_t, __VA_ARGS__)
#define I8_REF(id) INT_REF(id, int8_t)
#define I8_PTR(id) INT_PTR(id, int8_t)
#define U32_ARG(...) INT_ARG(uint32_t, __VA_ARGS__)
#define U32_REF(id) INT_REF(id, uint32_t)
#define U32_PTR(id) INT_PTR(id, uint32_t)
#define U16_ARG(...) INT_ARG(uint16_t, __VA_ARGS__)
#define U16_REF(id) INT_REF(id, uint16_t)
#define U16_PTR(id) INT_PTR(id, uint16_t)
#define U8_ARG(...) INT_ARG(uint8_t, __VA_ARGS__)
#define U8_REF(id) INT_REF(id, uint8_t)
#define U8_PTR(id) INT_PTR(id, uint8_t)


static uint8_t g_counterHookCallbackTemplate[] { 0x83, 0x05, 0xCC, 0xCC, 0xCC, 0xCC, 0x01, 0x50, 0x50, 0xa1, 0xFF, 0xFF, 0xFF, 0xFF, 0x89, 0x44, 0x24, 0x04, 0x58, 0xc3 };
template <uint32_t id>
uint32_t& DataRef(uint32_t addr = 0, uint32_t(* callback)(int) = nullptr)
{
    static uint32_t m_counter = 0;
    static void* m_target = nullptr;
    static void* m_counterTrampoline = nullptr;
    static uint8_t* m_counterHookCallback = nullptr;
    static decltype(callback) m_addrCallback = nullptr;
    static uint32_t m_addrReg = 0;

    if (addr) {
        if (addr != 0xFFFFFFFF) {
            DWORD oldProtect;
            MH_Initialize();
            if (m_target) {
                MH_DisableHook(m_target);
                MH_RemoveHook(m_target);
                m_target = nullptr;
            }

            if (!m_counterHookCallback) {
                m_counterHookCallback = (uint8_t*)malloc(32);
            }
            for (unsigned int i = 0; i < sizeof(g_counterHookCallbackTemplate); ++i) {
                m_counterHookCallback[i] = g_counterHookCallbackTemplate[i];
            }
            VirtualProtect(m_counterHookCallback, 32, PAGE_EXECUTE_READWRITE, &oldProtect);
            FlushInstructionCache(GetCurrentProcess(), m_counterHookCallback, 32);

            *(uint32_t**)(&(m_counterHookCallback[2])) = &m_counter;
            *(void**)(&(m_counterHookCallback[10])) = &m_counterTrampoline;
            MH_CreateHook((void*)addr, m_counterHookCallback, &m_counterTrampoline);
            MH_EnableHook((void*)addr);

            m_counter = 0;
        } else {
            if (callback) {
                m_addrCallback = callback;
                return m_counter;
            } else {
                if (m_addrCallback) {
                    m_addrReg = m_addrCallback(1);
                }
                return m_addrReg;
            }
        }
    }

    if (m_addrCallback) {
        m_counter = m_addrCallback(0);
    }
    return m_counter;
}
template <uint64_t id>
HookCtx* ImHook(void* addr = nullptr, void(__stdcall* callback)(PCONTEXT) = nullptr)
{
    static HookCtx* m_hook = nullptr;

    if (addr) {
        if (m_hook) {
            m_hook->Reset();
        } else {
            m_hook = new HookCtx();
            m_hook->Setup(addr, callback);
            m_hook->Enable();
        }
    }

    return m_hook;
}
template <void* end = nullptr>
void AnlyDataRec(EventRecord& rec)
{
}
template <uint32_t id, uint32_t... rest, void* end = nullptr>
void AnlyDataRec(EventRecord& rec)
{
    rec.PushBack(DataRef<id>());
    AnlyDataRec<rest...>(rec);
}
template <uint32_t... rest>
void AnlyDataRec()
{
    EventRecord rec(0);
    AnlyDataRec<rest...>(rec);
    AnlyEventRec(rec);
}
template <uint32_t id>
int& DataIdx(int idx = -1)
{
    static int m_idx = 0;
    return m_idx;
}
template <void* end = nullptr>
void DataBatchReset()
{
}
template <uint32_t id, uint32_t... rest, void* end = nullptr>
void DataBatchReset()
{
    DataRef<id>() = 0;
    DataBatchReset<rest...>();
}
void DataBatchRem(EventRecord& rec, void(* callback)(EventRecord&) = nullptr);
void DataBatchResetRem(void(* callback)() = nullptr);
template <void* end = nullptr>
void DataBatch(int idx = 0)
{
}
template <uint32_t id, uint32_t... rest, void* end = nullptr>
void DataBatch(int idx = 0)
{
    if (idx == 0) {
        auto record = EventRecord(EVENT_DATA_COLLECT);
        DataBatchRem(record,
            [](EventRecord& rec) { AnlyDataRec<id, rest...>(rec); });
        DataBatchResetRem([]() {
            return DataBatchReset<id, rest...>();
        });
    }
    DataIdx<id>() = idx;
    DataBatch<rest...>(++idx);
}
template <uint32_t id>
EventRecord& DataBatchCache(bool snapshot)
{
    static EventRecord* m_rec = nullptr;
    if (!m_rec) {
        m_rec = new EventRecord(DATA_FRAME);
    }
    if (snapshot) {
        m_rec->id = DATA_FRAME;
        m_rec->valueData.clear();
        DataBatchRem(*m_rec);
    }
    return *m_rec;
}
#define STAGE_OFFSET_INIT(rec, useStageOffset) \
    EventRecord& __so_rec = rec;               \
    bool __so_enable = useStageOffset;         \
    ImVec4 __so_white { 1.0f, 1.0f, 1.0f, 1.0f }; \
    ImVec4 __so_highlight { 1.0f, 1.0f, 0.0f, 1.0f };
#define STAGE_OFFSET(id) (DataRef<id>() - (__so_enable ? __so_rec.valueData[DataIdx<id>()].i : 0))
#define PRT_WITH_SO(...) ImGui::TextColored(__so_enable ? __so_highlight : __so_white, __VA_ARGS__)


typedef void __stdcall SwitchHackCallback(PCONTEXT);
template <typename cmpInsType, typename indirectTabletype = uint8_t, size_t switchSize = 0x400>
bool SwitchHackSet(SwitchHackCallback* callback,
    void* cmpIns, size_t sizeExpand,
    void* jmpTableIns, size_t jmpTableSize,
    void* indirectTableIns, size_t indirectTableSize)
{
    static uint32_t m_switchHackJmpTable[switchSize];
    static indirectTabletype m_switchHackIndirectTable[switchSize];
    static uint8_t* m_switchHackInt3 = nullptr;
    static HookCtx* m_switchHackHook = nullptr;
    DWORD oldProtect;

    if (!m_switchHackHook) {
        m_switchHackHook = new HookCtx();
    }
    if (!m_switchHackInt3) {
        m_switchHackInt3 = (uint8_t*)malloc(32);
    }

    VirtualProtect(cmpIns, sizeof(cmpInsType), PAGE_EXECUTE_READWRITE, &oldProtect);
    *(cmpInsType*)cmpIns += (cmpInsType)sizeExpand;
    FlushInstructionCache(GetCurrentProcess(), cmpIns, sizeof(cmpInsType));
    VirtualProtect(cmpIns, sizeof(cmpInsType), oldProtect, &oldProtect);

    uint32_t* jmpTableAddr = *(uint32_t**)jmpTableIns;
    for (size_t i = 0; i < (jmpTableSize / 4); ++i) {
        m_switchHackJmpTable[i] = jmpTableAddr[i];
    }
    for (size_t i = 0; i < (switchSize - jmpTableSize / 4); ++i) {
        m_switchHackJmpTable[(jmpTableSize / 4) + i] = (uint32_t)m_switchHackInt3;
    }
    VirtualProtect(jmpTableIns, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
    *(void**)jmpTableIns = m_switchHackJmpTable;
    FlushInstructionCache(GetCurrentProcess(), jmpTableIns, 4);
    VirtualProtect(jmpTableIns, 4, oldProtect, &oldProtect);

    if (indirectTableIns) {
        indirectTabletype* indirectTableAddr = *(indirectTabletype**)indirectTableIns;
        for (size_t i = 0; i < indirectTableSize; ++i) {
            m_switchHackIndirectTable[i] = indirectTableAddr[i];
        }
        for (size_t i = 0; i < (switchSize - indirectTableSize); ++i) {
            m_switchHackIndirectTable[indirectTableSize + i] = (indirectTabletype)((jmpTableSize / 4));
        }
        VirtualProtect(indirectTableIns, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        *(void**)indirectTableIns = m_switchHackIndirectTable;
        FlushInstructionCache(GetCurrentProcess(), indirectTableIns, 4);
        VirtualProtect(indirectTableIns, 4, oldProtect, &oldProtect);
    }

    for (int i = 0; i < 32; ++i) {
        m_switchHackInt3[i] = 0xcc;
    }
    VirtualProtect(m_switchHackInt3, 32, PAGE_EXECUTE_READWRITE, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), m_switchHackInt3, 32);
    m_switchHackHook->Setup(m_switchHackInt3, callback);
    m_switchHackHook->Enable();

    return true;
}
static void SwitchHackRevert()
{
}
void InsHookInit(uint16_t insHookId, void* insSwitchEip, std::function<uint32_t(PCONTEXT)>&& switchIdGetter);
void InsHookRegister(uint32_t stage, uint32_t fileId, uint32_t addr, uint16_t id);
void InsHookApply();
void __stdcall InsHookCallback(PCONTEXT pCtx);


void InputDispWidget(int shot, int bomb, int slow, int up, int down, int left, int right, int skip, float buttonSize = 30.0f);
static const char* FormatScore(uint32_t score)
{
    static char outBuf[64];
    uint64_t n = (uint64_t)score * (uint64_t)10;

    auto out = outBuf;
    int c;
    char buf[20];
    char* p;

    sprintf(buf, "%llu", n);
    c = 2 - strlen(buf) % 3;
    for (p = buf; *p != 0; p++) {
        *out++ = *p;
        if (c == 1) {
            *out++ = ',';
        }
        c = (c + 1) % 3;
    }
    *--out = 0;

    return outBuf;
}


#pragma warning(disable : 4307)
template <typename Str>
constexpr uint64_t constexpr_hash_64(const Str& toHash)
{
    static_assert(sizeof(uint64_t) == 8);
    uint64_t result = 0xcbf29ce484222325; // FNV offset basis

    for (char c : toHash) {
        result ^= c;
        result *= 1099511628211; // FNV prime
    }

    return result;
}
template <typename Str>
constexpr uint32_t constexpr_hash_32(const Str& toHash)
{
    static_assert(sizeof(uint32_t) == 4);
    uint32_t result = 2166136261u; // FNV offset basis

    for (char c : toHash) {
        result ^= c;
        result *= 16777619u; // FNV prime
    }

    return result;
}
#define ENUM_DEF(id) id = constexpr_hash_32(#id)
//#define ImHook_(strId, ...) ImHook<constexpr_hash(strId)>(__VA_ARGS__)
enum AnlyEnum : uint32_t {
    ENUM_DEF(EVENT_DATA_COLLECT),
    ENUM_DEF(EVENT_SECTION),
    ENUM_DEF(EVENT_CHAPTER),
    ENUM_DEF(EVENT_MISS),
    ENUM_DEF(EVENT_BOMB),
    ENUM_DEF(EVENT_DEATH_BOMB),
    ENUM_DEF(EVENT_TRANCE_PASSIVE),
    ENUM_DEF(EVENT_TRANCE_ACTIVE),
    ENUM_DEF(EVENT_TRANCE_EXIT),
    ENUM_DEF(EVENT_SPELLCARD_ENTER),
    ENUM_DEF(EVENT_SPELLCARD_GET),
    ENUM_DEF(EVENT_CONTINUE),
    ENUM_DEF(DATA_SCENE_ID),
    ENUM_DEF(DATA_FRAME),
    ENUM_DEF(DATA_RND_SEED),
    ENUM_DEF(DATA_DIFFCULTY),
    ENUM_DEF(DATA_SHOT_TYPE),
    ENUM_DEF(DATA_SHOT_GOAST),
    ENUM_DEF(DATA_SHOT_SEASON),
    ENUM_DEF(DATA_SUB_SHOT_TYPE),
    ENUM_DEF(DATA_STAGE),
    ENUM_DEF(DATA_STARTING_STAGE),
    ENUM_DEF(DATA_GAME_INPUT),
    ENUM_DEF(DATA_HIGH_SCORE),
    ENUM_DEF(DATA_SCORE),
    ENUM_DEF(DATA_ECL_BASE),
    ENUM_DEF(DATA_VALUE),
    ENUM_DEF(DATA_LIFE),
    ENUM_DEF(DATA_LIFE_FRAG),
    ENUM_DEF(DATA_BOMB),
    ENUM_DEF(DATA_BOMB_FRAG),
    ENUM_DEF(DATA_POWER),
    ENUM_DEF(DATA_TRANCE_METER),
    ENUM_DEF(DATA_CHAPTER_GRAZE),
    ENUM_DEF(DATA_GRAZE),
    ENUM_DEF(DATA_EXTEND),
    ENUM_DEF(DATA_SPIRIT_BLUE),
    ENUM_DEF(DATA_SPIRIT_GREY),
    ENUM_DEF(DATA_SPIRIT_LIFE),
    ENUM_DEF(DATA_SPIRIT_BOMB),
    ENUM_DEF(DATA_SPIRIT_BLUE_TRANCE),
    ENUM_DEF(DATA_SPIRIT_GREY_TRANCE),
    ENUM_DEF(DATA_SPIRIT_LIFE_TRANCE),
    ENUM_DEF(DATA_SPIRIT_BOMB_TRANCE),
    ENUM_DEF(DATA_ITEM_POWER),
    ENUM_DEF(DATA_ITEM_POINT),
    ENUM_DEF(DATA_ITEM_CANCEL),
    ENUM_DEF(SYSFLAG_INGAME),
    ENUM_DEF(SYSFLAG_INREPLAY),
    ENUM_DEF(SYSFLAG_BOSSRUSH),
    ENUM_DEF(SYSFLAG_LAST_SECTION),
    ENUM_DEF(MARKETEER_FRAME_REC_1),
    ENUM_DEF(MARKETEER_LIMIT_LOCK),
    ENUM_DEF(MARKETEER_LOADING_LOCK),
    ENUM_DEF(MARKETEER_OLD_TH_LOCK),
    ENUM_DEF(MARKETEER_OLD_TH_SEED),
    ENUM_DEF(MARKETEER_OLD_TH_STARTING),
};
#pragma warning(default : 4307)



}
