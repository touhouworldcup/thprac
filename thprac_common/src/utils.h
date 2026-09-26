#pragma once
#include <stdint.h>

#define SINGLETON_DEPENDENCY(...)
#define SINGLETON(className)                        \
public:                                             \
    className(const className&) = delete;           \
    className& operator=(className&) = delete;      \
    className(className&&) = delete;                \
    className& operator=(className&&) = delete;     \
    __declspec(noinline) static auto& singleton()   \
    {                                               \
        static className* s_singleton = nullptr;    \
        if (!s_singleton)                           \
            s_singleton = new className();          \
        return *s_singleton;                        \
    }                                               \
                                                    \
private:

#define NON_COPYABLE(className)                      \
public:                                              \
    className(const className&) = delete;            \
    className& operator=(const className&) = delete; \
                                                     \
private:

#define DEFAULT_COPYABLE(className)                   \
public:                                               \
    className(const className&) = default;            \
    className& operator=(const className&) = default; \
                                                      \
private:

#define NON_MOVABLE(className)                  \
public:                                         \
    className(className&&) = delete;            \
    className& operator=(className&&) = delete; \
                                                \
private:

#define DEFAULT_MOVABLE(className)               \
public:                                          \
    className(className&&) = default;            \
    className& operator=(className&&) = default; \
                                                 \
private:

/// defer implementation for C++
/// http://www.gingerbill.org/article/defer-in-cpp.html
/// ----------------------------

// Fun fact: in the vast majority of cases the compiler
// optimizes this down to normal conditional branching.
template <typename F>
struct privDefer {
    F f;
    explicit privDefer(F f)
        : f(f)
    {
    }
    ~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f)
{
    return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = defer_func([&]() { code; })

#define elementsof(a) (sizeof(a) / sizeof(a[0]))

#define member_size(type, member) sizeof(((type*)0)->member)

#define _MACRO_CAT(arg1, arg2) arg1##arg2
#define MACRO_CAT(arg1, arg2) _MACRO_CAT(arg1, arg2)

void memswap(void* buf1_, void* buf2_, unsigned int len);

template <typename T>
constexpr T t_tolower(T c) {
    if (c >= 'A' && c <= 'Z') {
        return c | 0x20;
    }
    else {
        return c;
    }
}

template<typename T>
constexpr unsigned t_strlen(const T* str) {
    unsigned int i = 0;
    for (; str[i]; i++);
    return i;
}

template<typename T>
bool t_str_compare_nocase(const T* a, size_t a_len, const T* b, size_t b_len) {
    if (a_len != b_len) {
       return false;
    }
    
    for (size_t i = 0; i < a_len; i++) {
        if (t_tolower(a[i]) != t_tolower(b[i])) {
            return false;
        }
    }
    return true;
}

#define SIZED(a) a, sizeof(a)
#define COUNTED(a) a, t_strlen(a)

/** round n down to nearest multiple of m */
inline long RoundDown(long n, long m) {
    return n >= 0 ? (n / m) * m : ((n - m + 1) / m) * m;
}

/** round n up to nearest multiple of m */
inline long RoundUp(long n, long m) {
    return n >= 0 ? ((n + m - 1) / m) * m : (n / m) * m;
}

unsigned rand_range(unsigned max);

// Buffer is expected to be 32 chars big
char* FormatNumberWithCommas(int64_t val, char* buffer);
char* FormatNumberFixedPoint(int value, unsigned int dot_pos, char* buffer);

inline bool CheckBufPos(const void* bufStart, const void* bufPos, size_t bufLen) {
    if (bufLen != 0) {
        return (unsigned)((const unsigned char*)bufPos - (const unsigned char*)bufStart) < bufLen;
    } else {
        // Very unsafe, only here because I don't know of a way to obtain a size for the currently running exe.
        // However, if a file were to cause GetExeInfo to crash, Windows wouldn't be able to load it.
        return true;
    }
}
#define CHKBUF(buf, pos, len, ret) if (!CheckBufPos(buf, pos, len)) return ret

#if !NDEBUG
#define debug_msg(title, format, ...) log_mboxf(NULL, 0, title, format, __VA_ARGS__)
#define debug_msg_raw(bufName, size, format, ...)  \
do {                                               \
    char bufName[size];                            \
    snprintf(bufName, size, format, __VA_ARGS__);  \
    MessageBoxA(nullptr, bufName, "Debug", MB_OK); \
} while (false);
#else
#define debug_msg(title, format, ...)
#define debug_msg_raw(buf, size, format, ...)
#endif

// Packs the bytes [c1], [c2], [c3], and [c4] together as a little endian integer
constexpr uint32_t TextInt(uint8_t c1, uint8_t c2 = 0, uint8_t c3 = 0, uint8_t c4 = 0) {
    return c4 << 24 | c3 << 16 | c2 << 8 | c1;
}
// Packs the bytes [c1], [c2], [c3], [c4], [c5], [c6], [c7], and [c8] together as a little endian integer
constexpr uint64_t TextInt64(uint8_t c1, uint8_t c2 = 0, uint8_t c3 = 0, uint8_t c4 = 0, uint8_t c5 = 0, uint8_t c6 = 0, uint8_t c7 = 0, uint8_t c8 = 0) {
    return (uint64_t)c8 << 56 | (uint64_t)c7 << 48 | (uint64_t)c6 << 40 | (uint64_t)c5 << 32 | c4 << 24 | c3 << 16 | c2 << 8 | c1;
}
