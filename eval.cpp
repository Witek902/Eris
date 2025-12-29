// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include "eval.hpp"
#include "pattern.hpp"

static int32_t SIDE_TO_MOVE_BONUS = 25;

static ScoreType ScorePattern(PatternType pattern)
{
    switch (pattern)
    {
    case PatternType::FiveInARow:   return 100'000'000;
    case PatternType::OpenFour:     return 10'000'000;
    case PatternType::ClosedFour:   return 50000;
    case PatternType::BrokenFour:   return 80000;
    case PatternType::OpenThree:    return 1000;
    case PatternType::ClosedThree:  return 500;
    case PatternType::BrokenThree:  return 800;
    case PatternType::OpenTwo:      return 100;
    case PatternType::BrokenTwo:    return 50;
    default:                        return 0;
    }
}

ScoreType Evaluate(const Position & position)
{
    const uint32_t center = BOARD_SIZE / 2;

    ScoreType score[2] = { 0, 0 }; // [0] - black, [1] - white

    // simple evaluation: count number of patterns for both sides
    for (uint32_t y = 0; y < BOARD_SIZE; ++y)
    {
        for (uint32_t x = 0; x < BOARD_SIZE; ++x)
        {
            const uint32_t squareIndex = x + y * BOARD_SIZE;

            // skip occupied squares
            if (position.m_board[squareIndex] != Stone::None)
                continue;

            // skip non-candidate squares
            if (position.m_neighborCount[squareIndex] == 0)
                continue;

            for (uint32_t c = 0; c < 2; ++c)
            {
                score[c] += ScorePattern(CombineThreats(position.m_threats[squareIndex][c]));
                score[c] -= std::abs(static_cast<int32_t>(x) - static_cast<int32_t>(center));
                score[c] -= std::abs(static_cast<int32_t>(y) - static_cast<int32_t>(center));
            }
        }
    }

    int32_t finalScore = SIDE_TO_MOVE_BONUS;

    if (position.m_sideToMove == Stone::Black)
        finalScore += score[0] - score[1];
    else
        finalScore += score[1] - score[0];

    return finalScore;
}
