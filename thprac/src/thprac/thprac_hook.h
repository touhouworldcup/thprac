#pragma once
#define NOMINMAX
#include <Windows.h>
#include <stdint.h>
#include <limits.h>
#include <limits>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <bit>
#include "utils/utils.h"

namespace THPrac {

// hook IAT
BOOL HookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName, void* hookFunction, void** oldAddress);
BOOL UnhookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName);
BOOL HookVTable(void* pInterface, int index, void* hookFunction, void** oldAddress);
BOOL UnhookVTable(void* pInterface, int index, void* oldAddress);

extern uintptr_t ingame_image_base;
#define RVA(a) ((uintptr_t)a + ingame_image_base)

struct HookCtx;
typedef void __fastcall Callback(PCONTEXT pCtx, HookCtx* self);

inline void PushHelper32(CONTEXT* pCtx, DWORD value)
{
    pCtx->Esp -= 4;
    *(DWORD*)pCtx->Esp = value;
}
inline DWORD PopHelper32(CONTEXT* pCtx)
{
    // The compiler will optimize this to just use eax
    DWORD ret = *(DWORD*)pCtx->Esp;
    pCtx->Esp += 4;
    return ret;
}

// Everything starting from here was written by zero318, with only minor tweaks from 32th
#define bitsof(type) \
    (sizeof(type) * CHAR_BIT)

template <size_t bit_count>
using UBitIntType = std::conditional_t<bit_count <= 8, uint8_t,
std::conditional_t<bit_count <= 16, uint16_t,
std::conditional_t<bit_count <= 32, uint32_t,
std::conditional_t<bit_count <= 64, uint64_t,
void>>>>;

static inline constexpr uint8_t char_to_nibble(char c)
{
    switch (c) {
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':
        return c - '0';
    case 'A': case 'B':
    case 'C': case 'D':
    case 'E': case 'F':
        return (c - 'A') + 10;
    case 'a': case 'b':
    case 'c':case 'd':
    case 'e': case 'f':
        return (c - 'a') + 10;
    }
    return 0;
}

template <size_t N = 0>
struct CodeString {
    static inline constexpr size_t size = N;
    static inline constexpr size_t length = N;

#pragma warning(push)
#pragma warning(disable: 4200)
    uint8_t buf[N];
#pragma warning(pop)

    constexpr CodeString(const uint8_t (&buffer)[N])
    {
        for (size_t i = 0; i < N; ++i)
            this->buf[i] = buffer[i];
    }

    template <size_t L>
        requires((L - 1) / 2 == N)
    constexpr CodeString(const char (&str)[L])
        : buf {}
    {
        for (size_t i = 0; i < N; ++i) {
            this->buf[i] = char_to_nibble(str[i * 2 + 1]) | char_to_nibble(str[i * 2]) << 4;
        }
    }

    template <size_t L>
        requires(L - 1 == N)
    constexpr CodeString(const char8_t (&str)[L])
    {
        for (size_t i = 0; i < N; ++i)
            this->buf[i] = (uint8_t)str[i];
    }

    template <typename T>
        requires(std::is_integral_v<T> && sizeof(T) == N)
    constexpr CodeString(T value)
        : buf {}
    {
        using U = std::make_unsigned_t<T>;
        U as_unsigned = (U)value;
        for (size_t i = 0; i < N; ++i)
            this->buf[i] = (uint8_t)(as_unsigned >> i * CHAR_BIT);
    }
    template <typename T>
        requires(std::is_floating_point_v<T> && sizeof(T) == N)
    constexpr CodeString(T value)
        : buf {}
    {
        using U = UBitIntType<bitsof(T)>;
        U as_unsigned = std::bit_cast<U>(value);
        for (size_t i = 0; i < N; ++i)
            this->buf[i] = (uint8_t)(as_unsigned >> i * CHAR_BIT);
    }

    constexpr operator uint8_t*()
    {
        return this->buf;
    }

    constexpr operator const uint8_t*() const
    {
        return this->buf;
    }

    template <size_t L>
    static inline constexpr size_t length_for_value([[maybe_unused]] const char (&str)[L])
    {
        return (L - 1) / 2;
    }
    template <size_t L>
    static inline constexpr size_t length_for_value(const char8_t (&str)[L])
    {
        size_t length = 0;
        while (str[length])
            ++length;
        return length;
    }
    template <typename T>
        requires(std::is_integral_v<T>)
    static inline constexpr size_t length_for_value(T)
    {
        return sizeof(T);
    }
    template <typename T>
        requires(std::is_floating_point_v<T>)
    static inline constexpr size_t length_for_value(T)
    {
        return sizeof(T);
    }
    template <size_t L>
    static inline constexpr size_t length_for_value([[maybe_unused]] const CodeString<L>& value)
    {
        return L;
    }

    template <typename T, typename... Args>
    static inline constexpr void make_code_string_impl(uint8_t* buf, T&& value, Args&&... args)
    {
        size_t length_ = length_for_value(value);
        for (size_t i = 0; i < length_; ++i)
            *buf++ = value[i];
        if constexpr (sizeof...(Args) != 0) {
            make_code_string_impl(buf, std::forward<Args>(args)...);
        }
    }
};

template <size_t L>
CodeString(const char (&str)[L]) -> CodeString<(L - 1) / 2>;
template <size_t L>
CodeString(const char8_t (&str)[L]) -> CodeString<L - 1>;
template <typename T>
    requires(std::is_integral_v<T>)
CodeString(const T& value) -> CodeString<sizeof(T)>;
template <typename T>
    requires(std::is_floating_point_v<T>)
CodeString(const T& value) -> CodeString<sizeof(T)>;

template <CodeString... strs, size_t N = (CodeString<>::length_for_value(strs) + ...)>
    requires(sizeof...(strs) != 0)
static inline constexpr CodeString<N> make_code_string()
{
    uint8_t buffer[N] {};
    CodeString<>::make_code_string_impl(buffer, std::forward<const decltype(strs)>(strs)...);
    return buffer;
}

template <size_t N = 1>
struct StringLiteral {

    static inline constexpr size_t length = N - 1;
    static inline constexpr size_t size = N;

    char buf[N];
    constexpr StringLiteral(const char (&str)[N])
    {
        for (size_t i = 0; i < length; ++i)
            this->buf[i] = str[i];
        this->buf[length] = '\0';
    }
    constexpr StringLiteral(const char8_t (&str)[N])
    {
        for (size_t i = 0; i < length; ++i)
            this->buf[i] = str[i];
        this->buf[length] = '\0';
    }

    constexpr StringLiteral(char c)
        : buf { c, '\0' }
    {
    }

    template <typename T>
        requires(std::is_integral_v<T> && !std::is_same_v<T, char>)
    constexpr StringLiteral(T value)
        : buf {}
    {
        size_t digit_offset = 0;
        char* str = this->buf;
        if constexpr (std::is_signed_v<T>) {
            if (value < 0) {
                *str++ = '-';
            } else {
                value = -value;
            }
            for (T temp = value; temp < -9; temp /= 10)
                ++digit_offset;
            do {
                str[digit_offset] = '0' - value % 10;
                value /= 10;
            } while (digit_offset--);
        } else {
            for (T temp = value; temp > 9; temp /= 10)
                ++digit_offset;
            do {
                str[digit_offset] = '0' + value % 10;
                value /= 10;
            } while (digit_offset--);
        }
    }

    constexpr operator char*()
    {
        return this->buf;
    }

    constexpr operator const char*() const
    {
        return this->buf;
    }

    template <typename T>
        requires(std::is_integral_v<T> && !std::is_same_v<T, char>)
    static inline constexpr size_t length_for_value(T value)
    {
        size_t length = 1;
        if constexpr (std::is_signed_v<T>) {
            if (value < 0) {
                ++length;
            } else {
                value = -value;
            }
            for (; value < -9; value /= 10)
                ++length;
        } else {
            for (; value > 9; value /= 10)
                ++length;
        }
        return length;
    }
    template <size_t L>
    static inline constexpr size_t length_for_value(const char (&value)[L])
    {
        size_t length = 0;
        while (value[length])
            ++length;
        return length;
    }
    static inline constexpr size_t length_for_value(char)
    {
        return 1;
    }
    template <size_t L>
    static inline constexpr size_t length_for_value(const StringLiteral<L>& value)
    {
        size_t length = 0;
        while (value[length])
            ++length;
        return length;
    }

    template <typename T, typename... Args>
    static inline constexpr void make_literal_impl(char* str, T&& value, Args&&... args)
    {
        size_t length = length_for_value(value);
        for (size_t i = 0; i < length; ++i)
            *str++ = value[i];
        if constexpr (sizeof...(Args) != 0) {
            make_literal_impl(str, std::forward<Args>(args)...);
        }
    }
};

template <size_t N>
StringLiteral(const char (&str)[N]) -> StringLiteral<N>;
template <size_t N>
StringLiteral(const char8_t (&str)[N]) -> StringLiteral<N>;

template <typename T>
    requires(std::is_integral_v<T> && !std::is_same_v<T, char>)
StringLiteral(T value) -> StringLiteral<std::numeric_limits<T>::digits10 + (std::is_signed_v<T> ? 3 : 2)>;

StringLiteral(char c) -> StringLiteral<2>;

template <StringLiteral... strs, size_t N = 1 + (StringLiteral<>::length_for_value(strs) + ...)>
    requires(sizeof...(strs) != 0)
static inline constexpr StringLiteral<N> make_literal()
{
    char chars[N] {};
    StringLiteral<>::make_literal_impl(chars, std::forward<const decltype(strs)>(strs)...);
    return chars;
}

template <CodeString code_str, auto unique>
struct UniqueCodeString {
    static inline constinit CodeString str = code_str;
    static inline constexpr size_t size = code_str.size;
};

template <StringLiteral str_arg, auto unique>
struct UniqueString {
    static inline constinit StringLiteral str = str_arg;
    static inline constexpr size_t size = str_arg.size;
};

struct PatchBufferImpl {
    void* ptr;
    size_t size;

    inline constexpr PatchBufferImpl()
        : ptr(nullptr)
        , size(0)
    {}
    template <StringLiteral str_arg, auto unique>
    inline constexpr PatchBufferImpl(const UniqueString<str_arg, unique>& str)
        : ptr(str.str)
        , size(str.size)
    {}
    template <CodeString code_str, auto unique>
    inline constexpr PatchBufferImpl(const UniqueCodeString<code_str, unique>& str)
        : ptr(str.str)
        , size(str.size)
    {}
};

struct PatchHookImpl {
    void* codecave;
    uint8_t orig_byte;
    uint8_t instr_len;

    inline constexpr PatchHookImpl()
        : codecave(0)
        , orig_byte(0)
        , instr_len(0)
    {
    }

    inline constexpr PatchHookImpl(uint8_t _instr_size)
        : codecave(0)
        , orig_byte(0)
        , instr_len(_instr_size)
    {
    }
};

union PatchData {
    PatchBufferImpl buffer;
    PatchHookImpl hook;

    inline constexpr PatchData()
        : buffer()
    {}
    inline constexpr PatchData(const PatchBufferImpl& buffer)
        : buffer(buffer)
    {}
    inline constexpr PatchData(const PatchHookImpl& hook)
        : hook(hook)
    {}
};

#define UniqueStr(...) UniqueString<make_literal<__VA_ARGS__>(), __COUNTER__>()
#define UniqueCode(...) UniqueCodeString<make_code_string<__VA_ARGS__>(), __COUNTER__>()

// IntelliSense does NOT like what's being done here
#if __INTELLISENSE__
#define PatchString(...) PatchData()
#define PatchCode(...) PatchData()
#else
#define PatchString(...) PatchBufferImpl(UniqueStr(__VA_ARGS__))
#define PatchCode(...) PatchBufferImpl(UniqueCode(__VA_ARGS__))
#endif

struct HookCtx {
    uintptr_t addr = 0;
    const char* name = nullptr;
    bool setup = false;
    bool enabled = false;
    Callback* callback = nullptr;

    PatchData data;

    void PatchSwap();
    void Enable();
    void Disable();
    void Setup();

    void Toggle(bool status) {
        if (status) {
            this->Enable();
        } else {
            this->Disable();
        }
    }
};

struct HookSlice {
    HookCtx* ptr;
    size_t len;
};

template <HookCtx... args>
HookSlice make_hook_array()
{
    static constinit HookCtx arr[sizeof...(args)] = {
        args...
    };

    return { arr, elementsof(arr) };
}

#define HOOKSET_DEFINE(name) static constinit HookCtx name[] = {

#define EHOOK_DY(name_, addr_, instr_size_, ...) { .addr = addr_, .name = #name_, .callback = []([[maybe_unused]] PCONTEXT pCtx, [[maybe_unused]] HookCtx * self) __VA_ARGS__, .data = PatchHookImpl(instr_size_) },
#define PATCH_DY(name_, addr_, ...) { .addr = addr_, .name = #name_, .data = PatchCode(__VA_ARGS__) },

#define HOOKSET_ENDDEF() };

#define EHOOK_ST(name_, addr_, instr_size_, ...) constinit HookCtx name_ { .addr = addr_, .name = #name_, .callback = []([[maybe_unused]] PCONTEXT pCtx, [[maybe_unused]] HookCtx* self) __VA_ARGS__, .data = PatchHookImpl(instr_size_) }
#define PATCH_ST(name_, addr_, ...) constinit HookCtx name_ { .addr = addr_, .name = #name_, .data = PatchCode(__VA_ARGS__)}


void EnableAllHooksImpl(HookCtx* hooks, size_t num);
void DisableAllHooksImpl(HookCtx* hooks, size_t num);

#define EnableAllHooks(hooks) EnableAllHooksImpl(hooks, elementsof(hooks))
#define DisableAllHooks(hooks) DisableAllHooksImpl(hooks, elementsof(hooks))

void VEHHookInit();

enum CallType {
    Cdecl, // cdecl and CDECL are both predefined. :P
    Stdcall,
    Fastcall,
    Thiscall,
    Vectorcall
};
template <uintptr_t addr, CallType type, typename R = void, typename... Args>
static inline R asm_call(Args... args) {
    if constexpr (type == Cdecl) {
        auto* func = (R(__cdecl*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Stdcall) {
        auto* func = (R(__stdcall*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Fastcall) {
        auto* func = (R(__fastcall*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Vectorcall) {
        auto* func = (R(__vectorcall*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Thiscall) {
        auto* func = (R(__thiscall*)(Args...))addr;
        return func(args...);
    }
}

template <uintptr_t addr, CallType type, typename R = void, typename... Args>
static inline R asm_call_rel(Args... args)
{
    uintptr_t _addr = RVA(addr);
    if constexpr (type == Cdecl) {
        auto* func = (R(__cdecl*)(Args...))_addr;
        return func(args...);
    } else if constexpr (type == Stdcall) {
        auto* func = (R(__stdcall*)(Args...))_addr;
        return func(args...);
    } else if constexpr (type == Fastcall) {
        auto* func = (R(__fastcall*)(Args...))_addr;
        return func(args...);
    } else if constexpr (type == Vectorcall) {
        auto* func = (R(__vectorcall*)(Args...))_addr;
        return func(args...);
    } else if constexpr (type == Thiscall) {
        auto* func = (R(__thiscall*)(Args...))_addr;
        return func(args...);
    }
}

namespace EFLAGS {
    enum : uint32_t {
        CF = 0x1,
        PF = 0x4,
        AF = 0x10,
        ZF = 0x40,
        SF = 0x80,
        TF = 0x100,
        IF = 0x200,
        DF = 0x400,
        OF = 0x800,
    };
}

}
