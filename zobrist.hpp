// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "types.hpp"

extern uint64_t s_ZobristHash[BOARD_SIZE * BOARD_SIZE][2];

void InitZobristHash();
