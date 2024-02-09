// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include <bitset>

/*

bool Bitboard::HasConnectedFive() const
{
    // check rows
    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        uint16_t mask = m_values[rank];
        for (int file = 0; file <= BOARD_SIZE - 5; file++)
        {
            if ((mask & (0b11111 << file)) == (0b11111 << file))
                return true;
        }
    }

    // check columns
    for (int file = 0; file < BOARD_SIZE; file++)
    {
        for (int rank = 0; rank <= BOARD_SIZE - 5; rank++)
        {
            bool connected = true;
            for (int k = 0; k < 5; k++)
            {
                if ((m_values[rank + k] & (1 << file)) == 0)
                {
                    connected = false;
                    break;
                }
            }
            if (connected)
                return true;
        }
    }

    // check diagonals (\)
    for (int rank = 0; rank <= BOARD_SIZE - 5; rank++)
    {
        for (int file = 0; file <= BOARD_SIZE - 5; file++)
        {
            bool connected = true;
            for (int k = 0; k < 5; k++)
            {
                if ((m_values[rank + k] & (1 << (file + k))) == 0)
                {
                    connected = false;
                    break;
                }
            }
            if (connected)
                return true;
        }
    }

    // check diagonals (/)
    for (int rank = 4; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file <= BOARD_SIZE - 5; file++)
        {
            bool connected = true;
            for (int k = 0; k < 5; k++)
            {
                if ((m_values[rank - k] & (1 << (file + k))) == 0)
                {
                    connected = false;
                    break;
                }
            }
            if (connected)
                return true;
        }
    }

    return false;
}

*/
