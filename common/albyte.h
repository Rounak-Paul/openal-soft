#ifndef AL_BYTE_H
#define AL_BYTE_H

#include <cstddef>
#include <limits>
#include <type_traits>

namespace al {

/* The "canonical" way to store raw byte data. Like C++17's std::byte, it's not
 * treated as a character type and does not work with arithmatic ops. Only
 * bitwise ops are allowed.
 */
enum class byte : unsigned char { };

#define REQUIRES(...) typename std::enable_if<(__VA_ARGS__),int>::type = 0

template<typename T, REQUIRES(std::is_integral<T>::value)>
inline constexpr T to_integer(al::byte b) noexcept { return T(b); }


template<typename T, REQUIRES(std::is_integral<T>::value)>
inline constexpr al::byte operator<<(al::byte lhs, T rhs) noexcept
{ return al::byte(to_integer<unsigned int>(lhs) << rhs); }

template<typename T, REQUIRES(std::is_integral<T>::value)>
inline constexpr al::byte operator>>(al::byte lhs, T rhs) noexcept
{ return al::byte(to_integer<unsigned int>(lhs) >> rhs); }

#define AL_DECL_OP(op)                                                        \
template<typename T, REQUIRES(std::is_integral<T>::value)>                    \
inline constexpr al::byte operator op (al::byte lhs, T rhs) noexcept          \
{ return al::byte(to_integer<unsigned int>(lhs) op rhs); }                    \
inline constexpr al::byte operator op (al::byte lhs, al::byte rhs) noexcept   \
{ return al::byte(lhs op to_integer<unsigned int>(rhs)); }

AL_DECL_OP(|)
AL_DECL_OP(&)
AL_DECL_OP(^)

#undef AL_DECL_OP

inline constexpr al::byte operator~(al::byte b) noexcept
{ return al::byte(~to_integer<unsigned int>(b)); }


template<typename T, REQUIRES(std::is_integral<T>::value)>
inline al::byte& operator<<=(al::byte &lhs, T rhs) noexcept
{ lhs = lhs << rhs; return lhs; }

template<typename T, REQUIRES(std::is_integral<T>::value)>
inline al::byte& operator>>=(al::byte &lhs, T rhs) noexcept
{ lhs = lhs >> rhs; return lhs; }

#define AL_DECL_OP(op)                                                        \
template<typename T, REQUIRES(std::is_integral<T>::value)>                    \
inline al::byte& operator op##= (al::byte &lhs, T rhs) noexcept               \
{ lhs = lhs op rhs; return lhs; }                                             \
inline al::byte& operator op##= (al::byte &lhs, al::byte rhs) noexcept        \
{ lhs = lhs op rhs; return lhs; }

AL_DECL_OP(|)
AL_DECL_OP(&)
AL_DECL_OP(^)

#undef AL_DECL_OP

template<size_t N>
class bitfield {
    static constexpr size_t bits_per_byte{std::numeric_limits<unsigned char>::digits};
    static constexpr size_t NumElems{(N+bits_per_byte-1) / bits_per_byte};

    byte vals[NumElems]{};

public:
    void set(size_t b) noexcept { vals[b/bits_per_byte] |= 1 << (b%bits_per_byte); }
    void unset(size_t b) noexcept { vals[b/bits_per_byte] &= ~(1 << (b%bits_per_byte)); }
    bool get(size_t b) const noexcept
    { return (vals[b/bits_per_byte] & (1 << (b%bits_per_byte))) != byte{}; }

    template<typename ...Args, REQUIRES(sizeof...(Args) > 0)>
    void set(size_t b, Args ...args) noexcept
    {
        set(b);
        /* Trick for calling set() on each element of the parameter pack. */
        using CharArray = char[sizeof...(Args)];
        (void)(CharArray{ (set(args),'\0')... });
    }
};

#undef REQUIRES

} // namespace al

#endif /* AL_BYTE_H */
