// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#pragma once

#include "square.hpp"

class Square;

/*

// 15x15 bitboard
class Bitboard
{
public:

    static constexpr uint16_t ROW_MASK = (1u << BOARD_SIZE) - 1;

    Bitboard() = default;
    INLINE constexpr Bitboard(const Bitboard& other) = default;
    INLINE constexpr Bitboard& operator = (const Bitboard& other) = default;

    INLINE constexpr Bitboard operator & (const Bitboard rhs) const
    {
        Bitboard result{};
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = m_values[i] & rhs.m_values[i];
        }
        return result;
    }

    INLINE constexpr Bitboard operator | (const Bitboard rhs) const
    {
        Bitboard result{};
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = m_values[i] | rhs.m_values[i];
        }
        return result;
    }

    INLINE constexpr Bitboard operator ^ (const Bitboard rhs) const
    {
        Bitboard result{};
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = m_values[i] ^ rhs.m_values[i];
        }
        return result;
    }

    INLINE Bitboard& operator &= (const Bitboard rhs)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            m_values[i] &= rhs.m_values[i];
        }
        return *this;
    }

    INLINE Bitboard& operator |= (const Bitboard rhs)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            m_values[i] |= rhs.m_values[i];
        }
        return *this;
    }

    INLINE Bitboard& operator ^= (const Bitboard rhs)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            m_values[i] ^= rhs.m_values[i];
        }
        return *this;
    }

    constexpr Bitboard operator ~() const
    {
        Bitboard result{};
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = ~m_values[i] & ROW_MASK;
        }
        return result;
    }

    constexpr bool operator == (const Bitboard rhs) const
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (m_values[i] != rhs.m_values[i])
                return false;
        }
        return true;
    }

    constexpr bool operator != (const Bitboard rhs) const
    {
        return !(*this == rhs);
    }

    template<typename Func>
    INLINE void Iterate(const Func func) const
    {
        for (uint8_t rank = 0; rank < BOARD_SIZE; rank++)
        {
            uint16_t mask = m_values[rank];
            while (mask)
            {
                const uint32_t index = FirstBitSet(mask);
                mask &= ~(1ull << index);
                func(static_cast<uint8_t>(index), rank);
            };
        }
    }

    // debug print
    std::string Print() const
    {
        std::string str;
        for (int32_t rank = BOARD_SIZE - 1; rank >= 0; rank--)
        {
            for (int32_t file = 0; file < BOARD_SIZE; file++)
            {
                str += (m_values[rank] & (1 << file)) ? '1' : '0';
                str += ' ';
            }
            str += '\n';
        }
        return str;
    }

    INLINE static constexpr Bitboard Empty()
    {
        Bitboard result{};
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = 0;
        }
        return result;
    }

    INLINE static constexpr Bitboard Full()
    {
        Bitboard result{};
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = ROW_MASK;
        }
        return result;
    }

    INLINE static constexpr Bitboard FromSquare(const Square square)
    {
        Bitboard result = Bitboard::Empty();
        result.m_values[square.y()] = (1u << square.x());
        return result;
    }

    // predefined rank mask
    template<uint16_t rank>
    INLINE static constexpr Bitboard Rank()
    {
        static_assert(rank < BOARD_SIZE, "Invalid rank");
        Bitboard result = Bitboard::Empty();
        result.m_values[rank] = ROW_MASK;
        return result;
    }

    // predefined file mask
    template<uint16_t file>
    INLINE static constexpr Bitboard File()
    {
        static_assert(file < BOARD_SIZE, "Invalid file");
        Bitboard result = Bitboard::Empty();
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result.m_values[i] = (1u << file);
        }
        return result;
    }

    uint16_t GetRank(uint8_t rank) const
    {
        return m_values[rank];
    }

    uint16_t GetFile(uint8_t file) const
    {
        uint16_t result = 0;
        for (uint8_t rank = 0; rank < BOARD_SIZE; rank++)
        {
            if ((m_values[rank] & (1u << file)) != 0)
            {
                result |= (1u << rank);
            }
        }
        return result;
    }

    bool HasConnectedFive() const;

private:
    // TODO use AVX2
    uint16_t m_values[BOARD_SIZE];
};
*/
