// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "square.hpp"

#include <chrono>

struct SearchParams
{
    TranspositionTable& tt;
    Position& position; 

    bool debugOutput = false;

    // search limits:
    uint32_t maxDepth = 20; // maximum search depth
    std::chrono::milliseconds maxTime = std::chrono::milliseconds(100); // maximum search time
};

void DoSearch(const SearchParams& params, Move& outBestMove, int32_t& outScore);
