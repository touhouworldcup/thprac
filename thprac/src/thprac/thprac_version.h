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

    unsigned char& operator[](unsigned int idx) const {
        return ((unsigned char*)this)[idx];
    }
};

// literal macros needed for thprac.rc
#define THPRAC_VERSION_0 2
#define THPRAC_VERSION_1 3
#define THPRAC_VERSION_2 0
#define THPRAC_VERSION_3 1
extern const ThpracVersion gVersion;
#define VER_PARAMS(v) (v).meta, (v).major, (v).minor, (v).patch
#define VER_PARAMS_CUR VER_PARAMS(gVersion)

ThpracVersion ParseVersion(const char* str);
}