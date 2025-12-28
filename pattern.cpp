// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "pattern.hpp"


PatternType gPatternTable[PatternTableSize];


template<uint32_t WindowSize>
static uint32_t CountRun(const Stone* w, int32_t start, int32_t dir)
{
    uint32_t c = 0;
    for (int32_t i = start; i >= 0 && i < WindowSize; i += dir)
    {
        if (w[i] == Stone::Us)
            c++;
        else
            break;
    }
    return c;
}

template<uint32_t WindowSize>
static PatternType ClassifyWindow(const Stone* w)
{
    constexpr uint32_t C = WindowSize / 2;

    // center must be US
    //if (w[C] != Stone::Us)
    //    return PatternType::None;

    // detect five in a row - XXXXX
    for (int i = 0; i <= 4; ++i)
    {
        bool five = true;
        for (int k = 0; k < 5; ++k)
            five &= w[i + k] == Stone::Us;
        if (five)
            return PatternType::FiveInARow;
    }

    // Fours:

    // detect open four - .XXXX.
    for (int i = 1; i <= 4; ++i)
        if (w[i - 1] == Stone::None &&
            w[i] == Stone::Us && w[i + 1] == Stone::Us && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us &&
            w[i + 4] == Stone::None)
            return PatternType::OpenFour;

    // detect closed four - |XXXX. or .XXXX|
    for (int i = 1; i <= 4; ++i)
        if ((w[i - 1] == Stone::Them && w[i + 4] == Stone::None) ||
            (w[i - 1] == Stone::None && w[i + 4] == Stone::Them))
        {
            if (w[i] == Stone::Us && w[i + 1] == Stone::Us && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us)
                return PatternType::ClosedFour;
        }

    // detect broken four - XX.XX
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::Us && w[i + 1] == Stone::Us && w[i + 2] == Stone::None && w[i + 3] == Stone::Us && w[i + 4] == Stone::Us)
            return PatternType::BrokenFour;

    // detect broken four - X.XXX
    for (int i = 1; i <= 4; ++i)
        if (w[i] == Stone::Us && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us && w[i + 4] == Stone::Us)
            return PatternType::BrokenFour;

    // detect broken four - XXX.X
    for (int i = 1; i <= 4; ++i)
        if (w[i] == Stone::Us && w[i + 1] == Stone::Us && w[i + 2] == Stone::Us && w[i + 3] == Stone::None && w[i + 4] == Stone::Us)
            return PatternType::BrokenFour;

    // Threes:

    // detect open three - .XXX.
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::Us && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us && w[i + 4] == Stone::None)
            return PatternType::OpenThree;

    // detect closed three - |XXX..
    for (int i = 1; i <= 3; ++i)
        if (w[i] == Stone::Them && w[i + 1] == Stone::Us && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us && w[i + 4] == Stone::None && w[i + 5] == Stone::None)
            return PatternType::ClosedThree;

    // detect closed three - ..XXX|
    for (int i = 1; i <= 3; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us && w[i + 4] == Stone::Us && w[i + 5] == Stone::Them)
            return PatternType::ClosedThree;

    // broken three - .XX.X.
    for (int i = 1; i <= 4; ++i)
        if (w[i - 1] == Stone::None && w[i] == Stone::Us && w[i + 1] == Stone::Us && w[i + 2] == Stone::None && w[i + 3] == Stone::Us && w[i + 4] == Stone::None)
            return PatternType::BrokenThree;

    // broken three - .X.XX.
    for (int i = 1; i <= 4; ++i)
        if (w[i - 1] == Stone::None && w[i] == Stone::Us && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us && w[i + 4] == Stone::None)
            return PatternType::BrokenThree;

    // broken three - X.X.X
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::Us && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::None && w[i + 4] == Stone::Us)
            return PatternType::BrokenThree;

    // Twos:

    // detect open two - ..XX.
    for (int i = 0; i <= 5; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::Us && w[i + 4] == Stone::None)
            return PatternType::OpenTwo;

    // detect open two - .XX..
    for (int i = 0; i <= 5; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::Us && w[i + 2] == Stone::Us && w[i + 3] == Stone::None && w[i + 4] == Stone::None)
            return PatternType::OpenTwo;

    // detect broken two - .X.X.
    for (int i = 1; i <= 4; ++i)
        if (w[i - 1] == Stone::None && w[i] == Stone::Us && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::None)
            return PatternType::BrokenTwo;

    // detect broken two - X.X..
    for (int i = 1; i <= 4; ++i)
        if (w[i - 1] == Stone::Us && w[i] == Stone::None && w[i + 1] == Stone::Us && w[i + 2] == Stone::None && w[i + 3] == Stone::None)
            return PatternType::BrokenTwo;

    // detect broken two - ..X.X
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::None && w[i + 4] == Stone::Us)
            return PatternType::BrokenTwo;

    // detect broken two - X...X
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::Us && w[i + 1] == Stone::None && w[i + 2] == Stone::None && w[i + 3] == Stone::None && w[i + 4] == Stone::Us)
            return PatternType::BrokenTwo;

    // Singles:

    // detect X....
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::Us && w[i + 1] == Stone::None && w[i + 2] == Stone::None && w[i + 3] == Stone::None && w[i + 4] == Stone::None)
            return PatternType::Single;

    // detect .X...
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::Us && w[i + 2] == Stone::None && w[i + 3] == Stone::None && w[i + 4] == Stone::None)
            return PatternType::Single;

    // detect ..X..
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::None && w[i + 2] == Stone::Us && w[i + 3] == Stone::None && w[i + 4] == Stone::None)
            return PatternType::Single;

    // detect ...X.
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::None && w[i + 2] == Stone::None && w[i + 3] == Stone::Us && w[i + 4] == Stone::None)
            return PatternType::Single;

    // detect ....X
    for (int i = 0; i <= 4; ++i)
        if (w[i] == Stone::None && w[i + 1] == Stone::None && w[i + 2] == Stone::None && w[i + 3] == Stone::None && w[i + 4] == Stone::Us)
            return PatternType::Single;

    return PatternType::None;
}

void InitializePatternTable()
{
    for (uint32_t code = 0; code < PatternTableSize; ++code) // 3^9
    {
        Stone window[9];
        uint32_t temp = code;
        for (int32_t i = 8; i >= 0; --i)
        {
            window[i] = static_cast<Stone>(temp % 3);
            temp /= 3;
        }
        gPatternTable[code] = ClassifyWindow<9>(window);
    }
}
