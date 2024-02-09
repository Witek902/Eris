// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "common.hpp"

static constexpr uint32_t BOARD_SIZE = 15;
static constexpr uint32_t SQUARE_COUNT = BOARD_SIZE * BOARD_SIZE;

using ScoreType = int32_t;

enum class Stone : uint8_t
{
    None    = 0,
    Black   = 1,
    White   = 2,
    Us      = 1,
    Them    = 2,
};

enum class GameResult : uint8_t
{
    BlackWins,
    WhiteWins,
    Draw,
    InProgress,
};

// invert color
INLINE Stone operator~(const Stone color)
{
    if (color == Stone::Black)
        return Stone::White;
    else if (color == Stone::White)
        return Stone::Black;
    else
        return Stone::None;
}
