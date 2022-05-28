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
