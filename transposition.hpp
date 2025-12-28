// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "square.hpp"
#include <vector>

struct TTEntry
{
    uint16_t key;
    ScoreType score;
    uint8_t depth;
    uint8_t flags;
    Square bestMove;
};

class TranspositionTable
{
public:
    TranspositionTable();

    // Clear the transposition table
    void Clear();

    // Probe the transposition table for a given position hash
    TTEntry* Probe(uint64_t hash);

    // Store an entry in the transposition table
    void Store(uint64_t hash, ScoreType score, uint8_t depth, uint8_t flags, Square bestMove);

private:
    std::vector<TTEntry> m_table;
};
