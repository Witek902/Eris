// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "types.hpp"

enum class PatternType : uint8_t
{
    None,
    OpenTwo,        // .XX..
    BrokenTwo,      // .X.X.
    OpenThree,      // .XXX.
    BrokenThree,    // .XX.X. or .X.XX.
    ClosedThree,    // |XXX.. or ..XXX|
    OpenFour,       // .XXXX.
    BrokenFour,     // XX.XX or X.XXX or XXX.X
    ClosedFour,     // |XXXX. or .XXXX|
    FiveInARow      // XXXXX
};

template<uint32_t WindowSize>
INLINE static uint32_t EncodeWindow(const Stone* w)
{
    // base 3 encoding
    uint32_t code = 0;
    for (uint32_t i = 0; i < WindowSize; ++i)
    {
        code = code * 3u + static_cast<uint8_t>(w[i]);
    }
    return code;
}

PatternType CombineThreats(const PatternType dir[4]);


// precomputed pattern table

static constexpr uint32_t PatternTableSize = 19683; // 3^9
extern PatternType gPatternTable[PatternTableSize];

void InitializePatternTable();
