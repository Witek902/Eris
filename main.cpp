// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include "eval.hpp"
#include "zobrist.hpp"
#include "transposition.hpp"
#include "search.hpp"
#include "pattern.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <random>

#ifndef ERIS_VERSION
#define ERIS_VERSION "0.4"
#endif // ERIS_VERSION

static const char* c_EngineName = "Eris " ERIS_VERSION;
static const char* c_Author = "Michal Witanowski";

static Position g_Position;
static TranspositionTable g_TranspositionTable;

static void ParseCommandString(const std::string& str, std::vector<std::string>& outArgList)
{
    std::string tmp;

    // tokenize command string, keep string in quotes as a single string
    for (size_t i = 0; i < str.length(); i++)
    {
        char c = str[i];
        if (c == ' ')
        {
            outArgList.push_back(std::move(tmp));
        }
        else if (c == '\"')
        {
            i++;
            while (str[i] != '\"')
            {
                tmp += str[i];
                i++;
            }
        }
        else
        {
            tmp += c;
        }
    }

    if (!tmp.empty())
    {
        outArgList.push_back(std::move(tmp));
    }
}

// unit tests
static bool Command_Tests()
{

}

static bool Command_Position(const std::vector<std::string>& args)
{
    size_t extraMovesStart = 0;

    if (args.size() >= 2 && args[1] == "startpos")
    {
        g_Position = Position();

        if (args.size() >= 4 && args[2] == "moves")
        {
            extraMovesStart = 2;

            for (size_t i = extraMovesStart + 1; i < args.size(); i++)
            {
                const Square moveSquare = Square::FromString(args[i].c_str());
                if (!moveSquare.IsValid())
                {
                    std::cout << "Invalid move: " << args[i] << std::endl;
                    return false;
                }
                g_Position.MakeMove(moveSquare, g_Position.SideToMove());
            }
        }
    }
    else if (args.size() > 2 && args[1] == "fen")
    {
        // TODO
    }
    else
    {
        return false;
    }

    return true;
}

static bool Command_SelfPlay()
{
    const uint32_t numRandomMoves = 4;

    uint32_t blackWins = 0;
    uint32_t whiteWins = 0;
    uint32_t draws = 0;

    Move moves[SQUARE_COUNT];

    std::random_device rd;
    std::mt19937 gen(rd());

    TranspositionTable tt;

    // play random games
    //for (;;)
    {
        Position pos;

        for (uint32_t i = 0; i < numRandomMoves; ++i)
        {
            uint32_t movesCount = 0;
            pos.GenerateMoves(moves, movesCount);

            if (movesCount == 0)
                break;

            pos.MakeMove(moves[gen() % movesCount], i % 2 == 0 ? Stone::Black : Stone::White);
        }
        pos.PrettyPrint();

        while (pos.GetGameResult() == GameResult::InProgress)
        {
            int32_t score = 0;
            Move bestMove = Move::Invalid();

            SearchParams params{ g_TranspositionTable, pos };
            params.maxTime = std::chrono::milliseconds(500);
            params.maxDepth = 50;
            DoSearch(params, bestMove, score);

            pos.MakeMove(bestMove, pos.SideToMove());

            pos.PrettyPrint();

            std::cout << "Score: " << score << std::endl;
        }

        switch (pos.GetGameResult())
        {
        case GameResult::BlackWins: blackWins++; break;
        case GameResult::WhiteWins: whiteWins++; break;
        case GameResult::Draw: draws++; break;
        default: DEBUG_BREAK(); break;
        }

        const uint32_t numGames = blackWins + whiteWins + draws;
        //if (numGames % 10000 == 0)
        {
            std::cout
                << "X: " << blackWins << " (" << (blackWins * 100.0f / numGames) << "%)"
                << " O: " << whiteWins << " (" << (whiteWins * 100.0f / numGames) << "%)"
                << " Draws: " << draws << " (" << (draws * 100.0f / numGames) << "%)" << std::endl;
        }
    }

    return true;
}

static bool Command_Go()
{
    int32_t score = 0;
    Move bestMove = Move::Invalid();
    SearchParams params{ g_TranspositionTable, g_Position };
    params.maxTime = std::chrono::seconds(10);
    params.debugOutput = true;
    DoSearch(params, bestMove, score);

    return true;
}

static bool BeginSearch()
{
    int32_t score = 0;
    Move bestMove = Move::Invalid();
    SearchParams params{ g_TranspositionTable, g_Position };
    params.maxTime = std::chrono::milliseconds(500); // 0.5 second per move
    DoSearch(params, bestMove, score);

    std::cout << bestMove.ToString() << std::endl;
    g_Position.MakeMove(bestMove, g_Position.SideToMove());
    return true;
}

static bool ExecuteCommand(const std::string& commandString)
{
    std::vector<std::string> args;
    ParseCommandString(commandString, args);

    if (args.empty())
    {
        std::cout << "Invalid command" << std::endl;
        return true;
    }

    const std::string& command = args[0];

    static bool boardParsingMode = false;

    if (boardParsingMode)
    {
        if (command == "DONE")
        {
            boardParsingMode = false;
            BeginSearch();
            return true;
        }

        int x = 0;
        int y = 0;
        int color = 0;
        if (sscanf(command.c_str(), "%d,%d,%d", &x, &y, &color) != 3)
        {
            // invalid board line
            boardParsingMode = false;
            std::cout << "ERROR" << std::endl;
            return true;
        }

        if (x < 0 || y < 0 || x >= (int)BOARD_SIZE || y >= (int)BOARD_SIZE)
        {
            // invalid coordinates
            boardParsingMode = false;
            std::cout << "ERROR" << std::endl;
            return true;
        }

        const Square square(static_cast<uint8_t>(y * BOARD_SIZE + x));
        if (!g_Position.IsMoveLegal(square))
        {
            // illegal move
            boardParsingMode = false;
            std::cout << "ERROR" << std::endl;
            return true;
        }

        Stone stone = Stone::None;
        if (color == 1)
            stone = Stone::Black;
        else if (color == 2)
            stone = Stone::White;
        else
        {
            // invalid color
            boardParsingMode = false;
            std::cout << "ERROR" << std::endl;
            return true;
        }

        g_Position.MakeMove(square, stone);
    }
    else if (command == "START")
    {
        uint32_t boardSize = atoi(args[1].c_str());
        if (boardSize >= 5 && boardSize <= 19)
        {
            std::cout << "OK" << std::endl;
            g_Position = Position();
        }
        else
        {
            std::cout << "ERROR" << std::endl;
        }
        g_TranspositionTable.Clear();
    }
    else if (command == "ABOUT")
    {
        std::cout << "name=\"Eris\", version=\"" ERIS_VERSION "\", author=\"" << c_Author << "\", country=\"Poland\"" << std::endl;
    }
    else if (command == "BOARD")
    {
        boardParsingMode = true;
    }
    else if (command == "INFO")
    {
        // TODO implement time manager
    }
    else if (command == "TURN")
    {
        if (args.size() != 2)
        {
            std::cout << "ERROR" << std::endl;
            return true;
        }
        const Square moveSquare = Square::FromString(args[1].c_str());
        if (!moveSquare.IsValid() || !g_Position.IsMoveLegal(moveSquare))
        {
            std::cout << "ERROR" << std::endl;
        }
        else
        {
            g_Position.MakeMove(moveSquare, g_Position.SideToMove());
            BeginSearch();
        }
    }
    else if (command == "BEGIN")
    {
        BeginSearch();
    }
    else if (command == "ugi")
    {
        std::cout << "id name " << c_EngineName << "\n";
        std::cout << "id author " << c_Author << "\n";
        std::cout << "ugiok" << std::endl;
    }
    else if (command == "isready")
    {
        std::cout << "readyok" << std::endl;
    }
    else if (command == "uginewgame")
    {
        g_Position = Position();
    }
    else if (command == "selfplay")
    {
        Command_SelfPlay();
    }
    else if (command == "position")
    {
        Command_Position(args);
    }
    else if (command == "go")
    {
        Command_Go();
    }
    else if (command == "eval")
    {
        std::cout << "Evaluation: " << Evaluate(g_Position) << std::endl;
    }
    else if (command == "quit" || command == "exit")
    {
        return false;
    }
    else if (command == "perft")
    {
        if (args.size() != 2)
        {
            std::cout << "Invalid perft arguments" << std::endl;
            return false;
        }

        uint32_t maxDepth = atoi(args[1].c_str());
        g_Position.Perft(maxDepth, true);
    }
    else if (command == "print")
    {
        g_Position.PrettyPrint();
    }
    else if (command == "threats")
    {
        g_Position.PrintThreats();
    }
    else if (command == "neighbors")
    {
        g_Position.PrintNeighborCounts();
    }
    else if (command == "moves")
    {
        // list all legal moves
        Move moves[SQUARE_COUNT];
        uint32_t movesCount = 0;
        g_Position.GenerateMoves(moves, movesCount);
        std::cout << "Legal moves (" << movesCount << "): ";
        for (uint32_t i = 0; i < movesCount; i++)
        {
            std::cout << moves[i].ToString() << "; ";
        }
        std::cout << std::endl;
    }
    else if (command == "candidatemoves")
    {
        // list all legal moves
        Move moves[SQUARE_COUNT];
        uint32_t movesCount = 0;
        g_Position.GenerateCandidateMoves(moves, movesCount);
        std::cout << "Candidate moves (" << movesCount << "): " << std::endl;
        for (uint32_t i = 0; i < movesCount; i++)
        {
            std::cout << moves[i].ToString() << "   " << g_Position.ScoreMove(moves[i]) << std::endl;
        }
        std::cout << std::endl;
    }
    else if (command == "ttprobe") // probe transposition table
    {
        uint64_t hash = g_Position.GetHash();
        TTEntry* entry = g_TranspositionTable.Probe(hash);
        if (entry)
        {
            std::cout << "TT Entry found: "
                << "score=" << entry->score
                << " depth=" << (int)entry->depth
                << " flags=" << (int)entry->flags
                << " bestmove=" << entry->bestMove.ToString() << std::endl;
        }
        else
        {
            std::cout << "TT Entry not found" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid command" << std::endl;
    }

    return true;
}

int main(int argc, char* argv[])
{
    InitZobristHash();
    InitializePatternTable();

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i])
        {
            std::string cmd = argv[i];
            std::cout << "CommandLine: " << cmd << std::endl;

            if (!ExecuteCommand(cmd))
            {
                return 0;
            }
        }
    }

    std::string str;

    while (std::getline(std::cin, str))
    {
        if (!ExecuteCommand(str))
        {
            break;
        }
    }

    return 0;
}
