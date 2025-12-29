// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "square.hpp"

class Position
{
public:

    using Patterns = PatternType[BOARD_SIZE * BOARD_SIZE][2][4];

    Position();

    INLINE Stone SideToMove() const { return m_sideToMove; }

    std::string ToString() const;
    void PrettyPrint() const;
    void PrintThreats() const;
    void PrintNeighborCounts() const;

    bool FromString(const std::string& str);

    uint64_t GetHash() const { return m_hash ^ (uint8_t)m_sideToMove; }
    const Patterns& GetThreats() const { return m_threats; }

    bool IsMoveLegal(const Move move) const;
    void MakeMove(const Move move, const Stone color);
    void UnmakeMove(const Move move);

    void GenerateCandidateMoves(Move* moves, uint32_t& count) const;
    void GenerateMoves(Move* moves, uint32_t& count) const;

    int32_t ScoreMove(const Move move) const;

    bool IsBoardEmpty() const;
    uint32_t GetOccupiedSquaresCount() const;
    GameResult GetGameResult() const;

    // Build line window of length W (odd) centered at s for direction (dx,dy),
    // mapped to 0/1/2 for a given perspective player 'us'.
    // 0 - empty, 1 - us, 2 - opponent or border.
    template<uint32_t WindowSize>
    void BuildWindow(int32_t x, int32_t y, int dx, int dy, Stone us, Stone(&w)[WindowSize]) const;

    PatternType EvalDirection(int32_t x, int32_t y, int dx, int dy, Stone us) const;

    // Evaluate and cache patterns at square (x,y) for both colors and all directions
    void EvaluatePatternsAtSquare(int32_t x, int32_t y, uint32_t dir);

    // Evaluate and cache patterns in all squares around (x0,y0) within pattern window
    void EvaluatePatternsAroundSquare(int32_t x0, int32_t y0);

    uint64_t Perft(uint32_t depth, bool print);

private:

    friend ScoreType Evaluate(const Position& position);

    uint64_t m_hash;
    Stone m_board[BOARD_SIZE * BOARD_SIZE];
    Patterns m_threats; // cached threats patterns for each square, color and direction
    uint8_t m_neighborCount[BOARD_SIZE * BOARD_SIZE];
    uint16_t m_movesPlayed;
    Move m_lastMove; // invalid after UnmakeMove
    Stone m_sideToMove;
};
