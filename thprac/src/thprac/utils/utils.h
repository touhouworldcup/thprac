#pragma once

namespace THPrac {

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

}

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

void memswap(void* buf1_, void* buf2_, unsigned int len);
void debug_msg(const char* title, const char* format, ...);