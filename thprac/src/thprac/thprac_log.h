#pragma once
#include <cstring>
#include <format>


namespace THPrac {

void log_print(const char* const msg, size_t len);

inline void log_print(const char* const null_terminated) {
    return log_print(null_terminated, strlen(null_terminated));
}

template <typename... Args>
inline void log_printf(const std::format_string<Args...> fmt, Args&&... args) {
    auto str = std::vformat(fmt.get(), std::make_format_args(args...));
    return log_print(str.data(), str.length());
}

void log_init(bool launcher, bool console);

}