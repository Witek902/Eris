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

struct Node
{
    Position& position;

    int32_t ply = 0;

    // principal variation moves
    Move pvLine[SQUARE_COUNT];
    uint32_t pvLength = 0;
};

int32_t NegaMax(Node& node, SearchContext& context, int32_t depth, int32_t alpha, int32_t beta)
{
    const bool isRootNode = node.ply == 0;
    const bool isPvNode = alpha < beta;
    const int32_t originalAlpha = alpha;

    // clear PV line
    node.pvLength = 0;

    // TODO check only if last move was winning move
    GameResult result = node.position.GetGameResult();
    if (result == GameResult::BlackWins)
        return (node.position.SideToMove() == Stone::Black) ? (MATE_VALUE - node.ply) : -(MATE_VALUE - node.ply);
    else if (result == GameResult::WhiteWins)
        return (node.position.SideToMove() == Stone::White) ? (MATE_VALUE - node.ply) : -(MATE_VALUE - node.ply);
    else if (result == GameResult::Draw)
        return 0;

    // terminal node or depth limit reached
    if (depth == 0)
        return Evaluate(node.position);

    if (!isRootNode)
    {
        // mate distance pruning
        alpha = std::max<ScoreType>(-MATE_VALUE + node.ply, alpha);
        beta = std::min<ScoreType>(MATE_VALUE - node.ply - 1, beta);
        if (alpha >= beta)
            return alpha;
    }

    // check for time limit
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (currentTime >= context.endTime && !isRootNode)
        return 0;

    // probe transposition table
    TTEntry* ttEntry = context.tt.Probe(node.position.GetHash());
    const Move ttMove = (ttEntry != nullptr && node.position.IsMoveLegal(ttEntry->bestMove)) ? ttEntry->bestMove : Move::Invalid();

    // TT cutoff
    if (ttEntry != nullptr && ttEntry->depth >= depth && !isRootNode)
    {
        if (ttEntry->flags == TTFlags::Exact)
            return ttEntry->score;
        else if (ttEntry->flags == TTFlags::Lower && ttEntry->score > alpha)
            alpha = ttEntry->score;
        else if (ttEntry->flags == TTFlags::Upper && ttEntry->score < beta)
            beta = ttEntry->score;
        if (alpha >= beta)
            return ttEntry->score;
    }

    Move moves[SQUARE_COUNT];
    int32_t moveScores[SQUARE_COUNT];
    uint32_t movesCount = 0;
    node.position.GenerateCandidateMoves(moves, movesCount);
    if (movesCount == 0)
    {
        node.position.GenerateMoves(moves, movesCount);
        if (movesCount == 0)
        {
            return 0; // draw
        }
    }

    // score moves
    for (uint32_t i = 0; i < movesCount; i++)
    {
        if (moves[i] == ttMove)
            moveScores[i] = INT32_MAX; 
        else
            moveScores[i] = node.position.ScoreMove(moves[i]);
    }

    Move bestMove = moves[0]; // default to first move

    int32_t bestScore = INT32_MIN;
    for (uint32_t i = 0; i < movesCount; i++)
    {
        Move move = Move::Invalid();
        {
            // select best scoring move
            int32_t bestMoveScoreIndex = i;
            for (uint32_t j = i + 1; j < movesCount; j++)
            {
                if (moveScores[j] > moveScores[bestMoveScoreIndex])
                    bestMoveScoreIndex = j;
            }
            // swap
            if (bestMoveScoreIndex != i)
            {
                std::swap(moves[i], moves[bestMoveScoreIndex]);
                std::swap(moveScores[i], moveScores[bestMoveScoreIndex]);
            }
            move = moves[i];
        }

        node.position.MakeMove(move, node.position.SideToMove());

        Node childNode{ node.position };
        childNode.ply = node.ply + 1;

        //int32_t score = -NegaMax(childNode, context, depth - 1, -beta, -alpha);

        // principal variation search
        int32_t score;
        if (i == 0)
        {
            score = -NegaMax(childNode, context, depth - 1, -beta, -alpha);
        }
        else
        {
            score = -NegaMax(childNode, context, depth - 1, -(alpha + 1), -alpha);
            if (score > alpha && score < beta)
                score = -NegaMax(childNode, context, depth - 1, -beta, -alpha);
        }


        node.position.UnmakeMove(moves[i]);

        if (score > bestScore)
        {
            bestMove = moves[i];
            bestScore = score;

            if (isPvNode)
            {
                node.pvLength = 1u + childNode.pvLength;
                node.pvLine[0] = move;
                memcpy(node.pvLine + 1, childNode.pvLine, sizeof(Move) * childNode.pvLength);
            }
        }

        if (bestScore > alpha)
            alpha = bestScore;

        if (alpha >= beta)
            break; // beta cutoff
    }


    // store in transposition table
    {
        TTFlags flag = TTFlags::Exact;
        if (bestScore <= originalAlpha) flag = TTFlags::Upper;
        else if (bestScore >= beta) flag = TTFlags::Lower;

        context.tt.Store(node.position.GetHash(), bestScore, static_cast<uint8_t>(depth), flag, bestMove);
    }

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
        Node rootNode{ params.position };

        const int32_t alpha = -INF_VALUE;
        const int32_t beta = INF_VALUE;
        int32_t score = NegaMax(rootNode, context, depth, alpha, beta);

        // check if time limit exceeded
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (currentTime >= context.endTime && depth > 1)
            break;

        ASSERT(rootNode.pvLength > 0);

        if (params.debugOutput)
        {
            std::cout << "info depth " << depth << " score " << score << " pv ";
            for (uint32_t i = 0; i < rootNode.pvLength; ++i)
            {
                std::cout << rootNode.pvLine[i].ToString() << " ";
            }
            std::cout << std::endl;
        }

        bestMove = rootNode.pvLine[0];
        bestScore = score;
    }

    if (params.debugOutput)
    {
        std::cout << "bestmove " << bestMove.ToString() << std::endl;
    }

    outBestMove = bestMove;
    outScore = bestScore;
}
