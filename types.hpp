// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "common.hpp"

static constexpr uint32_t BOARD_SIZE = 15;
static constexpr uint32_t SQUARE_COUNT = BOARD_SIZE * BOARD_SIZE;

static constexpr int32_t WINDOW_SIZE = 9; // must be odd
static constexpr int32_t NEIGHBOR_SIZE = 2;

static constexpr int32_t MATE_VALUE = 1'000'000'000;
static constexpr int32_t INF_VALUE = 2'000'000'000;

enum class PatternType : uint8_t;
class Position;
class TranspositionTable;

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
