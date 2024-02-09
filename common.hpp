// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include <iostream>
#include <inttypes.h>

#define USE_POPCNT

#if defined(_WIN32)
    #define PLATFORM_WINDOWS
    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #define PLATFORM_LINUX
#endif


#if defined(PLATFORM_LINUX)
    #include <csignal>
#endif


#if defined(PLATFORM_WINDOWS)
    #define DEBUG_BREAK() __debugbreak()
#elif defined(PLATFORM_LINUX)
    #define DEBUG_BREAK() std::raise(SIGINT)
#endif


//#ifndef NDEBUG
#define ASSERT(x) do { if (!(x)) { std::cout << "Assertion failed: " << #x << std::endl; DEBUG_BREAK(); } } while (0)
#define VERIFY(x) do { if (!(x)) { std::cout << "Assertion failed: " << #x << std::endl; DEBUG_BREAK(); } } while (0)
//#else
//#define ASSERT(x) do { } while (0)
//#define VERIFY(x) do { (x); } while (0)
//#endif


#if defined(_MSC_VER) && !defined(__clang__)
    
#define INLINE __forceinline

INLINE uint32_t PopCount(uint16_t x)
{
#ifdef USE_POPCNT
    return __popcnt16(x);
#else // !USE_POPCNT
    const uint16_t m1 = 0x5555;
    const uint16_t m2 = 0x3333;
    const uint16_t m4 = 0x0f0f;
    const uint16_t m8 = 0x00ff;
    x = (x & m1) + ((x >> 1) & m1);
    x = (x & m2) + ((x >> 2) & m2);
    x = (x & m4) + ((x >> 4) & m4);
    x = (x & m8) + ((x >> 8) & m8);
    return x;
#endif // USE_POPCNT
}

INLINE uint32_t FirstBitSet(uint16_t x)
{
    ASSERT(x != 0);
#ifdef USE_POPCNT
    unsigned long index;
    _BitScanForward(&index, x);
    return (uint32_t)index;
#else // !USE_POPCNT
    uint16_t v = x;   // 32-bit word input to count zero bits on right
    uint32_t c = 16;  // c will be the number of zero bits on the right
    v &= -static_cast<int16_t>(v);
    if (v) c--;
    if (v & static_cast<uint16_t>(0x00FF)) c -= 8;
    if (v & static_cast<uint16_t>(0x0F0F)) c -= 4;
    if (v & static_cast<uint16_t>(0x3333)) c -= 2;
    if (v & static_cast<uint16_t>(0x5555)) c -= 1;
    return c;
#endif // USE_POPCNT
}

#elif defined(__GNUC__) || defined(__clang__)

#define INLINE inline __attribute__((always_inline))

INLINE uint32_t PopCount(uint64_t x)
{
    return (uint32_t)__builtin_popcountll(x);
}

INLINE uint32_t FirstBitSet(uint16_t x)
{
    ASSERT(x != 0);
    return (uint32_t)__builtin_ctz(x);
}

#else //  defined(__GNUC__) || defined(__clang__)

#error "Unsupported compiler"

#endif


#if defined(__GNUC__)
#define UNNAMED_STRUCT __extension__
#else
#define UNNAMED_STRUCT
#endif

