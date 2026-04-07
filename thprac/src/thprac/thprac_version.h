#pragma once

namespace THPrac {

struct ThpracVersion {
    unsigned char meta;
    unsigned char major;
    unsigned char minor;
    unsigned char patch;

    constexpr bool operator==(const ThpracVersion& other) const {
        return meta == other.meta && major == other.major && minor == other.minor && patch == other.patch;
    }

    constexpr bool operator<(const ThpracVersion& other) const
    {
        if (meta != other.meta) return meta < other.meta;
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }

    constexpr bool operator!=(const ThpracVersion& other) const { return !(*this == other); }
    constexpr bool operator> (const ThpracVersion& other) const { return other < *this; }
    constexpr bool operator<=(const ThpracVersion& other) const { return !(*this > other); }
    constexpr bool operator>=(const ThpracVersion& other) const { return !(*this < other); }
};

// literal macros needed for thprac.rc
#define THPRAC_VERSION_0 2
#define THPRAC_VERSION_1 2
#define THPRAC_VERSION_2 2
#define THPRAC_VERSION_3 8
inline constexpr ThpracVersion currentVersion = { THPRAC_VERSION_0, THPRAC_VERSION_1, THPRAC_VERSION_2, THPRAC_VERSION_3 };

#define STRR(X) #X
#define STR(X) STRR(X) 

const char* GetVersionStr();
const wchar_t* GetVersionWcs();
ThpracVersion ParseVersion(const char* str);
}
