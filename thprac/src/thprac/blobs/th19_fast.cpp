// Use both of these commands to rebuild th19_fast_msvc.obj and th19_fast_llvm.obj
//
// clang++ -m32 -msse -msse2 -mfpmath=sse -std=c++20 -ffast-math -O3 -c th19_fast.cpp -o th19_fast_llvm.obj
// cl /arch:SSE2 /fp:fast /std:c++20 /O2 /c /Fo"th19_fast_msvc.obj" th19_fast.cpp
// 
// If you are using MSVC or choose to define _NO_REGCALL, find every instance of ret 14 (c2 14 00) in the compiled
// .obj files, and replace it with ret followed by a 2 byte nop (c3 66 90)
//

#define elementsof(a) (sizeof(a) / sizeof(a[0]))

#include <stdint.h>
#include <intrin.h>

#include <algorithm>
#include <bit>

// Use the same Pi as ZUN
constexpr const float M_PI = std::bit_cast<float, uint32_t>(0x40490FDB);

#if __clang__
__m128 __vectorcall sincos(float x) asm("___libm_sse2_sincosf_");
#elif _MSC_VER
extern "C" __m128 __cdecl __libm_sse2_sincosf_(float x);
static __forceinline __m128 __vectorcall sincos(float x) {
    return ((__m128(__vectorcall*)(float))&__libm_sse2_sincosf_)(x);
}
#endif

struct Float2 {
    float x;
    float y;
};

struct Float3 : public Float2 {
    float z;
};

struct CPUCollider {
    Float3 pos; // 0x0
    char gapC[12]; // 0xC
    Float2 size; // 0x18
    float radius; // 0x20
    char gap24[4]; // 0x24
    float angle; // 0x28
    char gap2C[12]; // 0x2C
    uint32_t flags; // 0x38
    // 0x3C
};

struct Player {
    char gap0[8420]; // 0x0
    int32_t unfocused_linear_speed; // 0x20E4
    int32_t focused_linear_speed; // 0x20E8
    int32_t unfocused_diagonal_speed; // 0x20EC
    int32_t focused_diagonal_speed; // 0x20F0
};

struct GameSide {
    char gap0[4]; // 0x0
    Player* player_ptr; // 0x4
};

struct CPUHitInf {
    void* vftable; // 0x0
    CPUCollider colliders1[30000]; // 0x4
    CPUCollider colliders2[30000]; // 0x1B7744
    int32_t collider_count; // 0x36EE84
    Float3 pos; // 0x36EE88
    char gap36EE94[52]; // 0x36EE94
    GameSide* game_side_ptr; // 0x36EEC8
    // 0x36EECC
};

// Inlining this function instead of using what the game has because of calling conventions
inline bool line_segments_intersect_2d(
    float a1, float a2,
    float a3, float a4,
    float a5, float a6,
    float a7, float a8)
{
    float v19 = ((a1 - a3) * (a6 - a2)) + ((a2 - a4) * (a1 - a5));
    float v13 = ((a1 - a3) * (a8 - a2)) + ((a2 - a4) * (a1 - a7));

    if ((v13 * v19) <= 0.0) {
        if (v19 != 0.0 || v13 != 0.0)
            return ((((a4 - a6) * (a5 - a7)) + ((a5 - a3) * (a6 - a8))) * (((a5 - a1) * (a6 - a8)) + ((a2 - a6) * (a5 - a7)))) <= 0.0;
        if (a1 > a3) {
            std::swap(a1, a3);
            std::swap(a2, a4);
        }
        if (a5 > a7) {
            std::swap(a5, a7);
            std::swap(a6, a8);
        }
        if (a7 >= a1 && a8 >= a2 && a3 >= a5 && a4 >= a6)
            return 1;
    }
    return 0;
}
template<typename T>
T __forceinline square(T val) {
    return val * val;
}

#ifdef __clang__
#define A(m) m
#elif defined(_MSC_VER)
#define A(m) m.m128_f32
#endif

// Code written by Khangaroo (https://github.com/khang06)
extern "C" uint32_t* __vectorcall CPUHitInf_CheckColliders(CPUHitInf* self, int, uint32_t* hit_flags_ptr, float, float, float radius) {
#ifdef __clang__
    __m128 temp = _mm_cvtepi32_ps(_mm_loadu_si128((__m128i*)&self->game_side_ptr->player_ptr->unfocused_linear_speed)) * (1.0f / 128.0f);
    float unfocused_linear_px = temp[0];
    float focused_linear_px = temp[1];
    float unfocused_diagonal_px = temp[2];
    float focused_diagonal_px = temp[3];
#elif defined(_MSC_VER)
    // MSVC doesn't overload operator * on __m128, needs explicit _mm_mul_ps
    __m128 mult = { .m128_f32 {
        (1.0f / 128.0f),
        (1.0f / 128.0f),
        (1.0f / 128.0f),
        (1.0f / 128.0f) } };

    __m128 temp = _mm_mul_ps(_mm_cvtepi32_ps(_mm_loadu_si128((__m128i*)&self->game_side_ptr->player_ptr->unfocused_linear_speed)), mult);

    float unfocused_linear_px = temp.m128_f32[0];
    float focused_linear_px = temp.m128_f32[1];
    float unfocused_diagonal_px = temp.m128_f32[2];
    float focused_diagonal_px = temp.m128_f32[3];
#endif

    Float2 check_pos_list[18];
    for (size_t i = 0; i < elementsof(check_pos_list); i++)
         check_pos_list[i] = Float2 { self->pos.x, self->pos.y };
    // I don't feel like trying to make this more readable
    check_pos_list[1].y = check_pos_list[1].y - unfocused_linear_px;
    check_pos_list[2].y = check_pos_list[2].y + unfocused_linear_px;
    check_pos_list[3].x = check_pos_list[3].x - unfocused_linear_px;
    check_pos_list[4].x = check_pos_list[4].x + unfocused_linear_px;
    check_pos_list[5].x = check_pos_list[5].x - unfocused_diagonal_px;
    check_pos_list[5].y = check_pos_list[5].y - unfocused_diagonal_px;
    check_pos_list[6].x = check_pos_list[6].x + unfocused_diagonal_px;
    check_pos_list[6].y = check_pos_list[6].y - unfocused_diagonal_px;
    check_pos_list[7].x = check_pos_list[7].x - unfocused_diagonal_px;
    check_pos_list[7].y = check_pos_list[7].y + unfocused_diagonal_px;
    check_pos_list[8].x = check_pos_list[8].x + unfocused_diagonal_px;
    check_pos_list[8].y = check_pos_list[8].y + unfocused_diagonal_px;
    check_pos_list[10].y = check_pos_list[10].y - focused_linear_px;
    check_pos_list[11].y = check_pos_list[11].y + focused_linear_px;
    check_pos_list[12].x = check_pos_list[12].x - focused_linear_px;
    check_pos_list[13].x = check_pos_list[13].x + focused_linear_px;
    check_pos_list[14].x = check_pos_list[14].x - focused_diagonal_px;
    check_pos_list[14].y = check_pos_list[14].y - focused_diagonal_px;
    check_pos_list[15].x = check_pos_list[15].x + focused_diagonal_px;
    check_pos_list[15].y = check_pos_list[15].y - focused_diagonal_px;
    check_pos_list[16].x = check_pos_list[16].x - focused_diagonal_px;
    check_pos_list[16].y = check_pos_list[16].y + focused_diagonal_px;
    check_pos_list[17].x = check_pos_list[17].x + focused_diagonal_px;
    check_pos_list[17].y = check_pos_list[17].y + focused_diagonal_px;
    // Idk if it's faster to have this clamping as a separate loop
    for (size_t i = 0; i < elementsof(check_pos_list); i++) {
        check_pos_list[i].x = std::clamp(check_pos_list[i].x, -140.0f, 140.0f);
        check_pos_list[i].y = std::clamp(check_pos_list[i].y, 32.0f, 448.0f - 16.0f);
    }
    uint32_t hit_flags = 0;
    float radius_sq = radius * radius;
    for (int32_t i = 0; i < self->collider_count; i++) {
        CPUCollider* collider = &self->colliders1[i];
        if (collider->flags & 1) {
            float total_dist = (collider->radius * collider->radius) + radius_sq;
            for (size_t j = 0; j < _countof(check_pos_list); j++) {
                if ((hit_flags >> j) & 1)
                    continue;
                float x_diff = check_pos_list[j].x - collider->pos.x;
                float y_diff = check_pos_list[j].y - collider->pos.y;
                if (total_dist > (y_diff * y_diff) + (x_diff * x_diff))
                    hit_flags |= 1 << j;
              }
         } else {
            auto angle_sincos = sincos(-collider->angle);
            float half_size_x = collider->size.x * 0.5f;
            float half_size_y = collider->size.y * 0.5f;
            for (size_t j = 0; j < _countof(check_pos_list); j++) {
                if ((hit_flags >> j) & 1)
                    continue;
                float x_diff = check_pos_list[j].x - collider->pos.x;
                float y_diff = check_pos_list[j].y - collider->pos.y;
                float rot_x = (A(angle_sincos)[1] * x_diff) - (A(angle_sincos)[0] * y_diff);
                float rot_y = (A(angle_sincos)[1] * y_diff) + (A(angle_sincos)[0] * x_diff);
                float x_sq1 = square(rot_x - half_size_x);
                float x_sq2 = square(half_size_x + rot_x);
                float y_sq1 = square(rot_y - half_size_y);
                float y_sq2 = square(half_size_y + rot_y);
                if ((half_size_x + radius) >= fabs(rot_x) && half_size_y >= fabs(rot_y)
                    || half_size_x >= fabs(rot_x) && (half_size_y + radius) >= fabs(rot_y)
                    || radius_sq > x_sq1 + y_sq1
                    || radius_sq > x_sq2 + y_sq1
                    || radius_sq > x_sq1 + y_sq2
                    || radius_sq > x_sq2 + y_sq2) {
                        hit_flags |= 1 << j;
                    }
            }
        }
    }
    *hit_flags_ptr = hit_flags;
    return hit_flags_ptr;
}
// ---

inline float angle_normalize(float angle)
{
    while (angle < -(M_PI)) {
        angle += (2 * M_PI);
    }
    while (angle > (M_PI)) {
        angle -= (2 * M_PI);
    }
    return angle;
}

__forceinline bool _RxD1E00_fast_impl (
    int a1,
    float a2, float a3, float a4, float a5,
    float a6, float a7, float a8, float a9,
    float a10)
{
    __m128 sincos_a10_2 = sincos(a10);

    for (int i = 0; i < a1 * 2; ++i) {
        float v13 = a8;
        if (i & 1)
            v13 = a9;

        __m128 sincos_a10_1 = sincos_a10_2;

        a10 = angle_normalize(a10 + M_PI / a1);

        sincos_a10_2 = sincos(a10);

        if (line_segments_intersect_2d(
            (A(sincos_a10_1)[1] * v13) + a6,
            (A(sincos_a10_1)[0] * v13) + a7,

            (A(sincos_a10_2)[1] * v13) + a6,
            (A(sincos_a10_2)[0] * v13) + a7,
            a2, a3, a4, a5)) {
                return true;
            }
    }
    return false;
}

#if defined(__clang__) && !defined(_NO_REGCALL)
extern "C" bool __regcall _RxD1E00_fast (
    int eax_, int ecx_, int edx_, int edi_, int esi_,
    float xmm0, float xmm1, float xmm2, float xmm3, float xmm4, float xmm5, float xmm6, float xmm7,

    uint32_t stack0, uint32_t stack1, uint32_t stack2, uint32_t stack3, uint32_t stack4
) asm("_RxD1E00_fast@@52");
// Name mangling inside an asm statement to match with the name that the MSVC version will have
extern "C" bool __regcall _RxD1E00_fast (
    int eax_, int ecx_, int edx_, int edi_, int esi_,
    float xmm0, float xmm1, float xmm2, float xmm3, float xmm4, float xmm5, float xmm6, float xmm7,

    uint32_t stack0, uint32_t stack1, uint32_t stack2, uint32_t stack3, uint32_t stack4
) {
    bool res = _RxD1E00_fast_impl(
        ecx_, xmm0, xmm1, xmm2, xmm3,
        std::bit_cast<float>(stack0),
        std::bit_cast<float>(stack1),
        std::bit_cast<float>(stack2),
        std::bit_cast<float>(stack3),
        std::bit_cast<float>(stack4)
    );
    asm {
        mov edi, edi_
        mov esi, esi_
    }
    return res;
}
#else
extern "C" bool __vectorcall _RxD1E00_fast(
    uint32_t ecx, uint32_t edx,
    float xmm0, float xmm1, float xmm2, float xmm3, float xmm4, float xmm5,
    uint32_t stack0, uint32_t stack1, uint32_t stack2, uint32_t stack3, uint32_t stack4
) {
    // Declaring stack params as int to ensure Clang compatibility
    // Clang, when it runs out of xmm registers to put float parameters in
    // will use the next available slots for int parameters (ecx, edx, stack)
    // to pass pointers to floats.
    return _RxD1E00_fast_impl(
        ecx, xmm0, xmm1, xmm2, xmm3,
        std::bit_cast<float>(stack0),
        std::bit_cast<float>(stack1),
        std::bit_cast<float>(stack2),
        std::bit_cast<float>(stack3),
        std::bit_cast<float>(stack4)
    );
}
#endif