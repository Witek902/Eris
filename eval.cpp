// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include "eval.hpp"

static int32_t SIDE_TO_MOVE_BONUS = 20;

ScoreType Evaluate(const Position& position)
{
    const uint32_t center = BOARD_SIZE / 2;

    ScoreType score[2] = { 0, 0 }; // [0] - black, [1] - white
    ScoreType scoreWhite = 0;

    // simple evaluation: count number of patterns for both sides
    for (uint32_t y = 0; y < BOARD_SIZE; ++y)
    {
        for (uint32_t x = 0; x < BOARD_SIZE; ++x)
        {
            const uint32_t squareIndex = x + y * BOARD_SIZE;

            /*
            // penalty for distance from center
            if (position.m_board[squareIndex] == Stone::Black)
            {
                score[0] -= std::abs(static_cast<int32_t>(x) - static_cast<int32_t>(center));
                score[0] -= std::abs(static_cast<int32_t>(y) - static_cast<int32_t>(center));
            }
            else if (position.m_board[squareIndex] == Stone::White)
            {
                score[1] -= std::abs(static_cast<int32_t>(x) - static_cast<int32_t>(center));
                score[1] -= std::abs(static_cast<int32_t>(y) - static_cast<int32_t>(center));
            }
            */
            
            for (uint32_t c = 0; c < 2; ++c)
            {
                for (uint32_t dir = 0; dir < 4; ++dir)
                {
                    switch (position.m_patterns[squareIndex][c][dir])
                    {
                    case PatternType::OpenTwo:
                        score[c] += 100;
                        break;
                    case PatternType::BrokenTwo:
                        score[c] += 50;
                        break;
                    case PatternType::OpenThree:
                        score[c] += 1000;
                        break;
                    case PatternType::ClosedThree:
                        score[c] += 500;
                        break;
                    case PatternType::BrokenThree:
                        score[c] += 1000;
                        break;
                    case PatternType::OpenFour:
                        score[c] += 10000;
                        break;
                    case PatternType::ClosedFour:
                        score[c] += 5000;
                        break;
                    case PatternType::BrokenFour:
                        score[c] += 10000;
                        break;
                    case PatternType::FiveInARow:
                        score[c] += 1000000;
                        break;
                    }
                }
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