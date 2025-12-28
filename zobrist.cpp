// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "zobrist.hpp"

alignas(64) uint64_t s_ZobristHash[BOARD_SIZE * BOARD_SIZE][2];


static inline uint64_t rotl(const uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}

static uint64_t xoroshiro128(uint64_t s[2])
{
    // https://prng.di.unimi.it/xoroshiro128plus.c
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = s0 + s1;
    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = rotl(s1, 37); // c
    return result;
}

void InitZobristHash()
{
    uint64_t s[2] = { 0x2b2fa1f53b24b9f2, 0x0203c66609c7f249 };

    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
        for (uint32_t j = 0; j < 2; ++j)
            s_ZobristHash[i][j] = xoroshiro128(s);
}
