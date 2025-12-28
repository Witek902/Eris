// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "transposition.hpp"

TranspositionTable::TranspositionTable()
{
    m_table.resize(1 << 20); // 1M entries
}

void TranspositionTable::Clear()
{
    for (auto& entry : m_table)
    {
        entry.key = 0;
    }
}

TTEntry* TranspositionTable::Probe(uint64_t hash)
{
    const size_t index = hash % m_table.size();
    TTEntry& entry = m_table[index];
    if (entry.key == (hash & 0xFFFF))
    {
        return &entry;
    }
    return nullptr;
}

// Store an entry in the transposition table
void TranspositionTable::Store(uint64_t hash, ScoreType score, uint8_t depth, uint8_t flags, Square bestMove)
{
    const size_t index = hash % m_table.size();
    TTEntry& entry = m_table[index];
    entry.key = static_cast<uint16_t>(hash & 0xFFFF);
    entry.score = score;
    entry.depth = depth;
    entry.flags = flags;
    entry.bestMove = bestMove;
}
