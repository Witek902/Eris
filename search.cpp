// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "search.hpp"
#include "position.hpp"
#include "transposition.hpp"

#include <chrono>

struct SearchContext
{
    TranspositionTable& tt;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
};

int32_t NegaMax(const Position& position, SearchContext& context, int32_t ply, int32_t depth, int32_t alpha, int32_t beta, Move& outBestMove)
{
    // TODO check only if last move was winning move
    GameResult result = position.GetGameResult();
    if (result == GameResult::BlackWins)
        return (position.SideToMove() == Stone::Black) ? (10000000 - ply) : -(10000000 - ply);
    else if (result == GameResult::WhiteWins)
        return (position.SideToMove() == Stone::White) ? (10000000 - ply) : -(10000000 - ply);
    else if (result == GameResult::Draw)
        return 0;

    // terminal node or depth limit reached
    if (depth == 0)
        return Evaluate(position);

    // check for time limit
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (currentTime >= context.endTime)
    {
        return Evaluate(position);
    }

    Move moves[SQUARE_COUNT];
    int32_t moveScores[SQUARE_COUNT];
    uint32_t movesCount = 0;
    position.GenerateCandidateMoves(moves, movesCount);
    if (movesCount == 0)
    {
        position.GenerateMoves(moves, movesCount);
        if (movesCount == 0)
        {
            return 0; // draw
        }
    }

    // probe transposition table
    TTEntry* ttEntry = context.tt.Probe(position.GetHash());
    const Move ttMove = (ttEntry != nullptr) ? ttEntry->bestMove : Move::Invalid();

    // score and sort moves
    for (uint32_t i = 0; i < movesCount; i++)
    {
        if (moves[i] == ttMove)
            moveScores[i] = INT32_MAX; 
        else
            moveScores[i] = position.ScoreMove(moves[i]);
    }
    // simple bubble sort
    for (uint32_t i = 0; i < movesCount - 1; i++)
    {
        for (uint32_t j = 0; j < movesCount - i - 1; j++)
        {
            if (moveScores[j] < moveScores[j + 1])
            {
                std::swap(moveScores[j], moveScores[j + 1]);
                std::swap(moves[j], moves[j + 1]);
            }
        }
    }

    int32_t bestScore = -100000000;
    for (uint32_t i = 0; i < movesCount; i++)
    {
        Position child = position;
        child.MakeMove(moves[i], child.SideToMove());

        Move dummyMove;
        int32_t eval = -NegaMax(child, context, ply + 1, depth - 1, -beta, -alpha, dummyMove);

        child.UnmakeMove(moves[i]);

        if (eval > bestScore)
        {
            outBestMove = moves[i];
            bestScore = eval;
        }

        if (bestScore > alpha)
            alpha = bestScore;

        if (alpha >= beta)
            break; // beta cutoff
    }

    // store in transposition table
    context.tt.Store(position.GetHash(), bestScore, static_cast<uint8_t>(depth), 0, outBestMove);

    return bestScore;
}

void DoSearch(const SearchParams& params, Move& outBestMove, int32_t& outScore)
{
    // remember start time
    auto startTime = std::chrono::high_resolution_clock::now();

    SearchContext context{ params.tt };
    context.endTime = startTime + params.maxTime;

    int32_t bestScore = 0;
    Move bestMove = Move::Invalid();

    // iterative deepening
    for (uint32_t depth = 1; depth <= params.maxDepth; ++depth)
    {
        Move tmpBestMove = Move::Invalid();
        const int32_t alpha = -100000000;
        const int32_t beta = 100000000;
        int32_t score = NegaMax(params.position, context, 0, depth, alpha, beta, tmpBestMove);

        // check if time limit exceeded
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (currentTime >= context.endTime)
            break;

        if (params.debugOutput)
        {
            std::cout << "info depth " << depth << " move " << tmpBestMove.ToString() << " score " << outScore << std::endl;
        }

        bestMove = tmpBestMove;
        bestScore = score;
    }

    if (params.debugOutput)
    {
        std::cout << "bestmove " << bestMove.ToString() << std::endl;
    }

    outBestMove = bestMove;
    outScore = bestScore;
}
