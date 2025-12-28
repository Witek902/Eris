// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "types.hpp"

#include <string>
#include <limits>

class Square
{
public:
    using IndexType = uint16_t;

    INLINE Square() = default;

    INLINE explicit Square(uint8_t x, uint8_t y) : m_index(y * BOARD_SIZE + x) { }

    INLINE explicit constexpr Square(IndexType index) : m_index(index) {}

    // invalid square
    static constexpr Square Invalid() { return Square{ std::numeric_limits<IndexType>::max() }; }

    // check if square is valid
    constexpr bool IsValid() const
    {
        return m_index < BOARD_SIZE * BOARD_SIZE;
    }

    INLINE bool operator == (const Square other) const
    {
        return m_index == other.m_index;
    }

    INLINE bool operator != (const Square other) const
    {
        return m_index != other.m_index;
    }

    constexpr uint16_t x() const
    {
        return m_index % BOARD_SIZE;
    }

    constexpr uint16_t y() const
    {
        return m_index / BOARD_SIZE;
    }

    std::string ToString() const
    {
        std::string result;
        result += std::to_string(x());
        result += ',';
        result += std::to_string(y());
        return result;
    }

    static Square FromString(const char* str)
    {
        int x = 0;
        int y = 0;
        if (sscanf(str, "%d,%d", &x, &y) != 2)
            return Square::Invalid();

        if (x < 0 || y < 0 || x >= (int)BOARD_SIZE || y >= (int)BOARD_SIZE)
            return Square::Invalid();

        return Square(static_cast<uint8_t>(y * BOARD_SIZE + x));
    }

    IndexType m_index;
};

// move is just equal to target square
using Move = Square;
