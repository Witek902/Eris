// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "square.hpp"

void DoSearch(const Position& position, TranspositionTable& tt, Move& outBestMove, int32_t& outScore);

int32_t NegaMax(const Position& position, TranspositionTable& tt, int32_t ply, int32_t depth, int32_t alpha, int32_t beta, Move& outBestMove);