// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include "eval.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <random>

#ifndef ERIS_VERSION
#define ERIS_VERSION "0.1"
#endif // ALPHAXO_VERSION

static const char* c_EngineName = "Eris " ERIS_VERSION;
static const char* c_Author = "Michal Witanowski";

static Position g_Position;

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
                g_Position.MakeMove(moveSquare);
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
    uint32_t blackWins = 0;
    uint32_t whiteWins = 0;
    uint32_t draws = 0;

    Move moves[SQUARE_COUNT];

    std::random_device rd;
    std::mt19937 gen(rd());

    // play random games
    for (;;)
    {
        Position pos;

        uint32_t movesCount = 0;

        while (pos.GetGameResult() == GameResult::InProgress)
        {
            movesCount = 0;
            pos.GenerateMoves(moves, movesCount);

            if (movesCount == 0)
                break;

            pos.MakeMove(moves[gen() % movesCount]);

            //pos.PrettyPrint();
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

static bool BeginSearch()
{
    // make a random move
    Move moves[SQUARE_COUNT];
    int32_t scores[SQUARE_COUNT];
    uint32_t movesCount = 0;
    g_Position.GenerateCandidateMoves(moves, movesCount);
    if (movesCount == 0)
    {
        g_Position.GenerateMoves(moves, movesCount);
        if (movesCount == 0)
        {
            std::cout << "ERROR" << std::endl;
            return false;
        }
    }

    // select best move
    int32_t bestScore = 0;
    for (uint32_t i = 0; i < movesCount; i++)
    {
        scores[i] = g_Position.ScoreMove(moves[i]);
        bestScore = std::max(bestScore, scores[i]);
    }

    // remove moves that are not best
    for (uint32_t i = 0; i < movesCount; )
    {
        if (scores[i] < bestScore)
        {
            // remove move
            moves[i] = moves[movesCount - 1];
            scores[i] = scores[movesCount - 1];
            movesCount--;
        }
        else
        {
            i++;
        }
    }

    // play random among best moves
    std::random_device rd;
    std::mt19937 gen(rd());
    const Move selectedMove = moves[gen() % movesCount];
    std::cout << selectedMove.ToString() << std::endl;

    g_Position.MakeMove(selectedMove);

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

    if (command == "START")
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
    }
    else if (command == "ABOUT")
    {
        std::cout << "name=\"Eris\", version=\"" ERIS_VERSION "\", author=\"" << c_Author << "\", country=\"Poland\"" << std::endl;
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
            g_Position.MakeMove(moveSquare);
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
        BeginSearch();
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
        std::cout << "Candidate moves (" << movesCount << "): ";
        for (uint32_t i = 0; i < movesCount; i++)
        {
            std::cout << moves[i].ToString() << "   " << g_Position.ScoreMove(moves[i]) << std::endl;
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Invalid command" << std::endl;
    }

    return true;
}

int main(int argc, char* argv[])
{
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
