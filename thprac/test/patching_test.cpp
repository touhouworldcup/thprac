// ------------------------------
// Go to line 294 for my own code
// ------------------------------

#include <stdint.h>
#include <limits.h>
#include <limits>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <bit>

#define elementsof(a) (sizeof(a) / sizeof(a[0]))

struct HookCtx;
typedef bool Callback(void* pCtx, HookCtx* self);

#define bitsof(type) \
(sizeof(type) * CHAR_BIT)

template <size_t bit_count>
using UBitIntType = std::conditional_t<bit_count <= 8, uint8_t,
    std::conditional_t<bit_count <= 16, uint16_t,
    std::conditional_t<bit_count <= 32, uint32_t,
    std::conditional_t<bit_count <= 64, uint64_t,
    void>>>>;

static inline constexpr uint8_t char_to_nibble(char c) {
    switch (c) {
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        return c - '0';
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return (c - 'A') + 10;
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return (c - 'a') + 10;
    }
    return 0;
}

template <size_t N = 0>
struct CodeString {
    static inline constexpr size_t size = N;
    static inline constexpr size_t length = N;

    uint8_t buf[N];

    constexpr CodeString(const uint8_t(&buffer)[N]) {
        for (size_t i = 0; i < N; ++i) this->buf[i] = buffer[i];
    }

    template <size_t L> requires((L - 1) / 2 == N)
        constexpr CodeString(const char(&str)[L]) : buf{} {
        for (size_t i = 0; i < N; ++i) {
            this->buf[i] = char_to_nibble(str[i * 2 + 1]) | char_to_nibble(str[i * 2]) << 4;
        }
    }

    template <size_t L> requires(L - 1 == N)
        constexpr CodeString(const char8_t(&str)[L]) {
        for (size_t i = 0; i < N; ++i) this->buf[i] = (uint8_t)str[i];
    }

    template <typename T> requires(std::is_integral_v<T> && sizeof(T) == N)
        constexpr CodeString(T value) : buf{} {
        using U = std::make_unsigned_t<T>;
        U as_unsigned = (U)value;
        for (size_t i = 0; i < N; ++i) this->buf[i] = (uint8_t)(as_unsigned >> i * CHAR_BIT);
    }
    template <typename T> requires(std::is_floating_point_v<T> && sizeof(T) == N)
        constexpr CodeString(T value) : buf{} {
        using U = UBitIntType<bitsof(T)>;
        U as_unsigned = std::bit_cast<U>(value);
        for (size_t i = 0; i < N; ++i) this->buf[i] = (uint8_t)(as_unsigned >> i * CHAR_BIT);
    }

    constexpr operator uint8_t* () {
        return this->buf;
    }

    constexpr operator const uint8_t* () const {
        return this->buf;
    }

    template <size_t L>
    static inline constexpr size_t length_for_value(const char(&str)[L]) {
        return (L - 1) / 2;
    }
    template <size_t L>
    static inline constexpr size_t length_for_value(const char8_t(&str)[L]) {
        size_t length = 0;
        while (str[length]) ++length;
        return length;
    }
    template <typename T> requires(std::is_integral_v<T>)
        static inline constexpr size_t length_for_value(T value) {
        return sizeof(T);
    }
    template <typename T> requires(std::is_floating_point_v<T>)
        static inline constexpr size_t length_for_value(T value) {
        return sizeof(T);
    }
    template <size_t L>
    static inline constexpr size_t length_for_value(const CodeString<L>& value) {
        return L;
    }

    template<typename T, typename ... Args>
    static inline constexpr void make_code_string_impl(uint8_t* buf, T&& value, Args&&... args) {
        size_t length = length_for_value(value);
        for (size_t i = 0; i < length; ++i) *buf++ = value[i];
        if constexpr (sizeof...(Args) != 0) {
            make_code_string_impl(buf, std::forward<Args>(args)...);
        }
    }
};

template<size_t L>
CodeString(const char(&str)[L]) -> CodeString<(L - 1) / 2>;
template<size_t L>
CodeString(const char8_t(&str)[L]) -> CodeString<L - 1>;
template<typename T> requires(std::is_integral_v<T>)
CodeString(const T& value)->CodeString<sizeof(T)>;
template<typename T> requires(std::is_floating_point_v<T>)
CodeString(const T& value)->CodeString<sizeof(T)>;

template<CodeString... strs, size_t N = (CodeString<>::length_for_value(strs) + ...)> requires(sizeof...(strs) != 0)
static inline constexpr CodeString<N> make_code_string() {
    uint8_t buffer[N]{};
    CodeString<>::make_code_string_impl(buffer, std::forward<const decltype(strs)>(strs)...);
    return buffer;
}

template<size_t N = 1>
struct StringLiteral {

    static inline constexpr size_t length = N - 1;
    static inline constexpr size_t size = N;

    char buf[N];
    constexpr StringLiteral(const char(&str)[N]) {
        for (size_t i = 0; i < length; ++i) this->buf[i] = str[i];
        this->buf[length] = '\0';
    }
    constexpr StringLiteral(const char8_t(&str)[N]) {
        for (size_t i = 0; i < length; ++i) this->buf[i] = str[i];
        this->buf[length] = '\0';
    }

    constexpr StringLiteral(char c) : buf{ c, '\0' } {}

    template<typename T> requires(std::is_integral_v<T> && !std::is_same_v<T, char>)
        constexpr StringLiteral(T value) : buf{} {
        size_t digit_offset = 0;
        char* str = this->buf;
        if constexpr (std::is_signed_v<T>) {
            if (value < 0) {
                *str++ = '-';
            }
            else {
                value = -value;
            }
            for (T temp = value; temp < -9; temp /= 10) ++digit_offset;
            do {
                str[digit_offset] = '0' - value % 10;
                value /= 10;
            } while (digit_offset--);
        }
        else {
            for (T temp = value; temp > 9; temp /= 10) ++digit_offset;
            do {
                str[digit_offset] = '0' + value % 10;
                value /= 10;
            } while (digit_offset--);
        }
    }

    constexpr operator char* () {
        return this->buf;
    }

    constexpr operator const char* () const {
        return this->buf;
    }

    template<typename T> requires(std::is_integral_v<T> && !std::is_same_v<T, char>)
        static inline constexpr size_t length_for_value(T value) {
        size_t length = 1;
        if constexpr (std::is_signed_v<T>) {
            if (value < 0) {
                ++length;
            }
            else {
                value = -value;
            }
            for (; value < -9; value /= 10) ++length;
        }
        else {
            for (; value > 9; value /= 10) ++length;
        }
        return length;
    }
    template<size_t L>
    static inline constexpr size_t length_for_value(const char(&value)[L]) {
        size_t length = 0;
        while (value[length]) ++length;
        return length;
    }
    static inline constexpr size_t length_for_value(char value) {
        return 1;
    }
    template<size_t L>
    static inline constexpr size_t length_for_value(const StringLiteral<L>& value) {
        size_t length = 0;
        while (value[length]) ++length;
        return length;
    }

    template<typename T, typename ... Args>
    static inline constexpr void make_literal_impl(char* str, T&& value, Args&&... args) {
        size_t length = length_for_value(value);
        for (size_t i = 0; i < length; ++i) *str++ = value[i];
        if constexpr (sizeof...(Args) != 0) {
            make_literal_impl(str, std::forward<Args>(args)...);
        }
    }
};

template<size_t N>
StringLiteral(const char(&str)[N]) -> StringLiteral<N>;
template<size_t N>
StringLiteral(const char8_t(&str)[N]) -> StringLiteral<N>;

template<typename T> requires(std::is_integral_v<T> && !std::is_same_v<T, char>)
StringLiteral(T value)->StringLiteral<std::numeric_limits<T>::digits10 + (std::is_signed_v<T> ? 3 : 2)>;

StringLiteral(char c)->StringLiteral<2>;

template<StringLiteral... strs, size_t N = 1 + (StringLiteral<>::length_for_value(strs) + ...)> requires(sizeof...(strs) != 0)
static inline constexpr StringLiteral<N> make_literal() {
    char chars[N]{};
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

    template<StringLiteral str_arg, auto unique>
    inline constexpr PatchBufferImpl(const UniqueString<str_arg, unique>& str) : ptr(str.str), size(str.size) {}
    template<CodeString code_str, auto unique>
    inline constexpr PatchBufferImpl(const UniqueCodeString<code_str, unique>& str) : ptr(str.str), size(str.size) {}
};

struct PatchHookImpl {
    void* codecave;
    uint8_t orig_byte;
    uint8_t instr_len;

    inline constexpr PatchHookImpl(uint8_t _instr_size) : codecave(0), orig_byte(0), instr_len(_instr_size) {}
};

union PatchData {
    PatchBufferImpl buffer;
    PatchHookImpl hook;

    inline constexpr PatchData() {}
    inline constexpr PatchData(const PatchBufferImpl& buffer) : buffer(buffer) {}
    inline constexpr PatchData(const PatchHookImpl& hook) : hook(hook) {}
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

// My code starts here

constexpr unsigned int tmpl_arr(unsigned int len) {
	if (len == 0) {
		return 1;
	}
	else {
		return len;
	}
}

struct EclWriteBase {
	uint32_t off;
	uint32_t size;

	// MSVC does not allow 0 sized arrays in structs that other structs inherit
	// from, like, at all. This is an attempt at a workaround.
	// 
	// When using a pointer though (which is the intended usecase), the syntax
	// is kinda ugly. It's either (*w)[i] or w[0][i]. I guess if you're dealing
	// with an array of pointers of these though, you get w[i][j]. Not too bad.
	inline uint8_t operator[](unsigned int i) const {
		return ((uint8_t*)this)[i + sizeof(*this)];
	}
};

template<size_t N>
struct EclWrite : public EclWriteBase {
	CodeString<N> bytes;
};

struct EclWriteSetBase {
	const char* sub_name;
	size_t num_writes;

	const EclWriteBase* operator[](unsigned int i) const {
		return ((EclWriteBase**)((uintptr_t)this + sizeof(*this)))[i];
	}
};

template<size_t N>
struct EclWriteSet : public EclWriteSetBase {
	const EclWriteBase* writes[N];
};

struct EclWriteSetMultiBase {
	size_t length;

	inline auto operator[](unsigned int i) const {
		return ((const EclWriteSetBase**)((uintptr_t)this + sizeof(*this)))[i];
	}
};

template<size_t N>
struct EclWriteSetMulti : public EclWriteSetMultiBase {
	const EclWriteSetBase* write_sets[N];
};

struct StageWarpBase {
	const char* label;
	enum { TYPE_NONE, TYPE_SLIDER, TYPE_COMBO } type = TYPE_NONE;

	// Pointer to an array of pointers because the things being pointed to
	// could all have varying sizes.
	const EclWriteSetMultiBase* write_sets;
	const char* phases_label;
	size_t phase_count;
};

template<size_t N>
struct StageWarp : public StageWarpBase {
	const StageWarpBase* phases[N];
};

template<uint32_t off_, CodeString... strs>
struct make_ecl_write {
	static constexpr CodeString codestr = make_code_string<strs...>();
	static constexpr EclWrite w = { { off_, codestr.size }, codestr };

	static inline constexpr auto get() {
		return static_cast<const EclWriteBase*>(&w);
	}
};

template<uint32_t off, uint32_t dest, uint32_t at_frame, uint32_t ecl_time>
struct make_ecl_jump {
	static constexpr CodeString codestr = make_code_string<ecl_time, "0c0018000000ff2c00000000", dest, at_frame>();
	static constexpr EclWrite w = { { off, codestr.size }, codestr };

	static inline constexpr auto get() {
		return static_cast<const EclWriteBase*>(&w);
	}
};

template<StringLiteral sub_name_, typename... Args>
struct make_ecl_write_set {
	static constexpr EclWriteSet<tmpl_arr(sizeof...(Args))> w = { { sub_name_, sizeof...(Args) }, { Args::get()... } };
	static inline constexpr auto get() {
		return static_cast<const EclWriteSetBase*>(&w);
	}
};

template<typename... Ts>
struct make_multi_ecl_write_sets {
	static constexpr const EclWriteSetMulti<tmpl_arr(sizeof...(Ts))> w = { sizeof...(Ts), Ts::get()... };

	static inline constexpr auto get() {
		return static_cast<const EclWriteSetMultiBase*>(&w);
	}
};

template<
	StringLiteral label,
	decltype(StageWarpBase::type) type,
	StringLiteral phases_label,
	typename write_sets_type,
	typename... phases_types
>
struct make_stage_warps {
	// MSVC won't compile this if there's an assignment to a 0 sized array in any
	// capacity, even if the thing being assigned is a fold expression from a pack
	// with 0 parameters in it. Thus, the std::max
	// Won't resolve to Windows.h max macro because of the angle brackets.
	static constexpr const StageWarp<tmpl_arr(sizeof...(phases_types))> w = {label, type, write_sets_type::get(), phases_label, sizeof...(phases_types), phases_types::get()... };

	static constexpr const auto get() {
		return static_cast<const StageWarpBase*>(&w);
	}
};

constinit const StageWarpBase* w =
make_stage_warps<"", StageWarpBase::TYPE_SLIDER, "Progress", make_multi_ecl_write_sets<>,
	make_stage_warps<"Wave 1", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<>>,
	make_stage_warps<"Wave 2", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<make_ecl_write_set<"main", make_ecl_jump<0x258, 0x129, 0, 0>, make_ecl_write<0x254, (uint32_t)2>>>>,
	make_stage_warps<"Wave 3", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<make_ecl_write_set<"main", make_ecl_jump<0x258, 0x1F4, 0, 0>, make_ecl_write<0x254, (uint32_t)3>>>>,

	make_stage_warps<"Boss", StageWarpBase::TYPE_COMBO, "Boss", make_multi_ecl_write_sets<make_ecl_write_set<"main", make_ecl_jump<0x258, 0x2D8, 0, 0>>>, 
		make_stage_warps<"None (Random)", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<>>,
		make_stage_warps<"Goutokuzi Mike", StageWarpBase::TYPE_COMBO, "Attack", make_multi_ecl_write_sets<make_ecl_write_set<"WorldWaveB00", make_ecl_write<0x34, "00">>>,
			make_stage_warps<"Non Spell", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<>>,
			make_stage_warps<"Money Sign \"Casting Koban Coins Before a Calico Cat\"", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<make_ecl_write_set<"Boss01Boss", make_ecl_write<0x1b0, "14050000">>, make_ecl_write_set<"Boss01Boss1", make_ecl_write<0x10, "00">>>>
		>,
		make_stage_warps<"Aki Minoriko", StageWarpBase::TYPE_COMBO, "Attack", make_multi_ecl_write_sets<make_ecl_write_set<"WorldWaveB00", make_ecl_write<0x34, "00">>>,
			make_stage_warps<"Non Spell", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<>>,
			make_stage_warps<"Autumn Sign \"Bumper Crop Waves\"", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<make_ecl_write_set<"Boss01Boss", make_ecl_write<0x1b0, "14050000">>, make_ecl_write_set<"Boss01Boss1", make_ecl_write<0x10, "00">>>>
		>,
		make_stage_warps<"Eternity Larva", StageWarpBase::TYPE_COMBO, "Attack", make_multi_ecl_write_sets<make_ecl_write_set<"WorldWaveB00", make_ecl_write<0x34, "00">>>,
			make_stage_warps<"Non Spell", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<>>,
			make_stage_warps<"Butterfly Sign \"Deadly Butterfly\"", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<make_ecl_write_set<"Boss01Boss", make_ecl_write<0x1b0, "14050000">>, make_ecl_write_set<"Boss01Boss1", make_ecl_write<0x10, "00">>>>
		>,
		make_stage_warps<"Sakata Nemuno", StageWarpBase::TYPE_COMBO, "Attack", make_multi_ecl_write_sets<make_ecl_write_set<"WorldWaveB00", make_ecl_write<0x34, "00">>>,
			make_stage_warps<"Non Spell", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<>>,
			make_stage_warps< "Kitchen Knife \"Mince Up Till Your Blade Chips\"", StageWarpBase::TYPE_NONE, "", make_multi_ecl_write_sets<make_ecl_write_set<"Boss01Boss", make_ecl_write<0x1b0, "14050000">>, make_ecl_write_set<"Boss01Boss1", make_ecl_write<0x10, "00">>>>
		>
	>
>::get();

const StageWarpBase* test() {
	return w;
}

// -----------------------------------------------
// Go to line 296 to find the start of my own code
// -----------------------------------------------