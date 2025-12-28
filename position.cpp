// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include "zobrist.hpp"
#include "pattern.hpp"

#include <bitset>
#include <chrono>

static bool OnBoard(int32_t x, int32_t y)
{
    return x >= 0 && x < (int32_t)BOARD_SIZE && y >= 0 && y < (int32_t)BOARD_SIZE;
}

Position::Position()
{
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        m_board[i] = Stone::None;
        m_neighborCount[i] = 0;
        for (uint32_t color = 0; color < 2; ++color)
        {
            for (uint32_t dir = 0; dir < 4; ++dir)
            {
                m_patterns[i][color][dir] = PatternType::None;
            }
        }
    }

    m_hash = 0;
    m_sideToMove = Stone::Black;
}

bool Position::IsBoardEmpty() const
{
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (m_board[i] != Stone::None)
            return false;
    }
    return true;
}

uint32_t Position::GetOccupiedSquaresCount() const
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (m_board[i] != Stone::None)
            count++;
    }
    return count;
}

GameResult Position::GetGameResult() const
{
    // check if black has connected five
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
        for (uint32_t dir = 0; dir < 4; ++dir)
            if (m_patterns[i][0][dir] == PatternType::FiveInARow)
                return GameResult::BlackWins;

    // check if white has connected five
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
        for (uint32_t dir = 0; dir < 4; ++dir)
            if (m_patterns[i][1][dir] == PatternType::FiveInARow)
                return GameResult::WhiteWins;

    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
        if (m_board[i] == Stone::None)
            return GameResult::InProgress;

    // all squares are filled - it's a draw
    return GameResult::Draw;
}

std::string Position::ToString() const
{
    std::string result;
    for (uint32_t i = 0; i < BOARD_SIZE; i++)
    {
        for (uint32_t j = 0; j < BOARD_SIZE; j++)
        {
            const Stone s = m_board[i * BOARD_SIZE + j];
            if (s == Stone::Black)
                result += "x";
            else if (s == Stone::White)
                result += "o";
            else
                result += ".";

            result += " ";
        }
        result += '\n';
    }
    return result;
}

bool Position::FromString(const std::string& str)
{
    // TODO
    return true;
}


void Position::PrettyPrint() const
{
    std::cout << ToString();
    std::cout << "Side to move: " << (m_sideToMove == Stone::Black ? "X" : "O") << std::endl;
    std::cout << std::endl;
}

void Position::PrintThreats() const
{
    for (uint32_t color = 0; color < 2; ++color)
    {
        std::cout << (color == 0 ? "Black threats:\n" : "White threats:\n");
        for (uint32_t y = 0; y < BOARD_SIZE; ++y)
        {
            for (uint32_t x = 0; x < BOARD_SIZE; ++x)
            {
                const uint32_t squareIndex = x + y * BOARD_SIZE;
                for (uint32_t dir = 0; dir < 4; ++dir)
                {
                    const PatternType pt = m_patterns[squareIndex][color][dir];
                    char c = ' ';
                    switch (pt)
                    {
                    case PatternType::None:          c = '.'; break;
                    case PatternType::OpenTwo:       c = '2'; break;
                    case PatternType::BrokenTwo:     c = 'b'; break;
                    case PatternType::OpenThree:     c = '3'; break;
                    case PatternType::BrokenThree:   c = 't'; break;
                    case PatternType::ClosedThree:   c = 'c'; break;
                    case PatternType::OpenFour:      c = '4'; break;
                    case PatternType::BrokenFour:    c = 'f'; break;
                    case PatternType::ClosedFour:    c = 'd'; break;
                    case PatternType::FiveInARow:    c = 'X'; break;
                    case PatternType::Single:        c = 's'; break;
                    default: DEBUG_BREAK(); break;
                    }
                    std::cout << c;
                }
                std::cout << "|";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void Position::PrintNeighborCounts() const
{
    std::cout << "Neighbor counts:\n";
    for (uint32_t y = 0; y < BOARD_SIZE; ++y)
    {
        for (uint32_t x = 0; x < BOARD_SIZE; ++x)
        {
            const uint32_t squareIndex = x + y * BOARD_SIZE;
            std::cout << static_cast<uint32_t>(m_neighborCount[squareIndex]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool Position::IsMoveLegal(const Move move) const
{
    return m_board[move.m_index] == Stone::None;
}

void Position::MakeMove(const Move move, const Stone color)
{
    // ensure the move is legal
    ASSERT(m_board[move.m_index] == Stone::None);
    ASSERT(color == Stone::Black || color == Stone::White);

    // make the move
    m_board[move.m_index] = color;

    // update hash
    m_hash ^= s_ZobristHash[move.m_index][static_cast<uint8_t>(color) - static_cast<uint8_t>(Stone::Black)];

    // switch side to move
    m_sideToMove = ~color;

    // update patterns cache in neighboring squares
    const int32_t x0 = (move.m_index % BOARD_SIZE);
    const int32_t y0 = (move.m_index / BOARD_SIZE);
    EvaluatePatternsAroundSquare(x0, y0);

    // increment neighbor counts
    for (int32_t dy = -NEIGHBOR_SIZE; dy <= NEIGHBOR_SIZE; ++dy)
    {
        for (int32_t dx = -NEIGHBOR_SIZE; dx <= NEIGHBOR_SIZE; ++dx)
        {
            // skip self
            if (dx == 0 && dy == 0)
                continue;

            const int32_t nx = x0 + dx;
            const int32_t ny = y0 + dy;
            if (OnBoard(nx, ny))
            {
                const uint32_t neighborIndex = nx + ny * BOARD_SIZE;
                m_neighborCount[neighborIndex]++;
            }
        }
    }

}

void Position::UnmakeMove(const Move move)
{
    // switch side to move
    m_sideToMove = ~m_sideToMove;

    // update hash
    const Stone color = m_board[move.m_index];
    m_hash ^= s_ZobristHash[move.m_index][static_cast<uint8_t>(color) - static_cast<uint8_t>(Stone::Black)];

    // unmake the move
    m_board[move.m_index] = Stone::None;

    // update patterns cache in neighboring squares
    const int32_t x0 = (move.m_index % BOARD_SIZE);
    const int32_t y0 = (move.m_index / BOARD_SIZE);
    EvaluatePatternsAroundSquare(x0, y0);

    // decrement neighbor counts
    for (int32_t dy = -NEIGHBOR_SIZE; dy <= NEIGHBOR_SIZE; ++dy)
    {
        for (int32_t dx = -NEIGHBOR_SIZE; dx <= NEIGHBOR_SIZE; ++dx)
        {
            // skip self
            if (dx == 0 && dy == 0)
                continue;

            const int32_t nx = x0 + dx;
            const int32_t ny = y0 + dy;
            if (OnBoard(nx, ny))
            {
                const uint32_t neighborIndex = nx + ny * BOARD_SIZE;
                ASSERT(m_neighborCount[neighborIndex] > 0);
                m_neighborCount[neighborIndex]--;
            }
        }
    }
}

void Position::GenerateCandidateMoves(Move* moves, uint32_t& count) const
{
    count = 0;
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (m_board[i] == Stone::None && m_neighborCount[i] > 0)
        {
            moves[count++] = Move(static_cast<uint8_t>(i));
        }
    }
}

void Position::GenerateMoves(Move* moves, uint32_t& count) const
{
    count = 0;
    for (uint32_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (m_board[i] == Stone::None)
        {
            moves[count++] = Move(static_cast<uint8_t>(i));
        }
    }
}

int32_t Position::ScoreMove(const Move move) const
{
    const int32_t x0 = move.m_index % BOARD_SIZE;
    const int32_t y0 = move.m_index / BOARD_SIZE;

    const int32_t dxs[4] = { 1, 0, 1, 1 };
    const int32_t dys[4] = { 0, 1, 1, -1 };

    PatternType bestPattern = PatternType::None;
    for (uint32_t dir = 0; dir < 4; ++dir)
    {
        Stone window[9];
        BuildWindow<9>(x0, y0, dxs[dir], dys[dir], m_sideToMove, window);
        window[4] = Stone::Us; // simulate the move

        const uint32_t patternCode = EncodeWindow<9>(window);
        ASSERT(patternCode < PatternTableSize);

        PatternType pattern = gPatternTable[patternCode];
        if (pattern > bestPattern)
            bestPattern = pattern;
    }

    switch (bestPattern)
    {
    case PatternType::FiveInARow:    return 100000;
    case PatternType::OpenFour:      return 10000;
    case PatternType::ClosedFour:    return 5000;
    case PatternType::BrokenFour:    return 2000;
    case PatternType::OpenThree:     return 500;
    case PatternType::ClosedThree:   return 200;
    case PatternType::BrokenThree:   return 100;
    case PatternType::OpenTwo:       return 50;
    case PatternType::BrokenTwo:     return 10;
    default:                        return 0;
    }
}

template<uint32_t WindowSize>
void Position::BuildWindow(int32_t x, int32_t y, int32_t dx, int32_t dy, Stone us, Stone(&w)[WindowSize]) const
{
    static_assert(WindowSize % 2 == 1, "Invalid window size");
    constexpr int32_t C = WindowSize / 2;
    const Stone them = ~us;

    for (int32_t i = -C; i <= C; ++i)
    {
        int32_t nx = x + i * dx;
        int32_t ny = y + i * dy;
        Stone v = Stone::Them; // border -> blocked
        if (OnBoard(nx, ny))
        {
            const Stone s = m_board[nx + ny * BOARD_SIZE];

            if (s == Stone::None)   v = Stone::None;
            else if (s == us)       v = Stone::Us;
            else if (s == them)     v = Stone::Them;
        }
        w[i + C] = v;
    }
    // w[C] should be 0 for candidate move evaluation (since s is empty)
}

PatternType Position::EvalDirection(int32_t x, int32_t y, int dx, int dy, Stone us) const
{
    Stone window[9];
    BuildWindow<9>(x, y, dx, dy, us, window);

    const uint32_t patternCode = EncodeWindow<9>(window);
    ASSERT(patternCode < PatternTableSize);

    return gPatternTable[patternCode];
}

void Position::EvaluatePatternsAtSquare(int32_t x, int32_t y)
{
    ASSERT(x >= 0 && x < (int32_t)BOARD_SIZE);
    ASSERT(y >= 0 && y < (int32_t)BOARD_SIZE);
    const uint32_t squareIndex = x + y * BOARD_SIZE;
    for (uint32_t color = 0; color < 2; ++color)
    {
        const Stone us = (color == 0) ? Stone::Black : Stone::White;
        m_patterns[squareIndex][color][0] = EvalDirection(x, y, 1, 0, us);  // horizontal
        m_patterns[squareIndex][color][1] = EvalDirection(x, y, 0, 1, us);  // vertical
        m_patterns[squareIndex][color][2] = EvalDirection(x, y, 1, -1, us); // diagonal
        m_patterns[squareIndex][color][3] = EvalDirection(x, y, 1, 1, us);  // diagonal
    }
}

void Position::EvaluatePatternsAroundSquare(int32_t x0, int32_t y0)
{
    for (int32_t i = -4; i <= 4; ++i)
    {
        if (OnBoard(x0 + i, y0))
            EvaluatePatternsAtSquare(x0 + i, y0); // horizontal
        if (OnBoard(x0, y0 + i))
            EvaluatePatternsAtSquare(x0, y0 + i); // vertical
        if (OnBoard(x0 + i, y0 + i))
            EvaluatePatternsAtSquare(x0 + i, y0 + i); // diagonal
        if (OnBoard(x0 + i, y0 - i))
            EvaluatePatternsAtSquare(x0 + i, y0 - i); // diagonal
    }
}

uint64_t Position::Perft(uint32_t depth, bool print)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    if (print)
        startTime = std::chrono::high_resolution_clock::now();

    uint32_t numMoves = 0;
    Move moveList[SQUARE_COUNT];
    GenerateMoves(moveList, numMoves);

    uint64_t nodes = 0;
    for (uint32_t i = 0; i < numMoves; i++)
    {
        const Move move = moveList[i];

        Position child = *this;
        child.MakeMove(move, child.SideToMove());
        uint64_t numChildNodes = depth == 1 ? 1 : child.Perft(depth - 1, false);

        if (print)
        {
            //std::cout << move.ToString() << " : " << numChildNodes << std::endl;
        }

        nodes += numChildNodes;
    }

    const auto endTime = std::chrono::high_resolution_clock::now();

    if (print)
    {
        const float t = (endTime - startTime).count() * 1.0e-9f;

        std::cout << "Total nodes:      " << nodes << std::endl;
        std::cout << "Time:             " << t << " seconds" << std::endl;
        std::cout << "Nodes per second: " << 1.0e-6f * (nodes / t) << "M" << std::endl;
    }

    return nodes;
}
