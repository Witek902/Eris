// Eris - Gomoku Engine
// Author: Michal Witanowski (Witek902)

#include "position.hpp"
#include "eval.hpp"


static constexpr uint32_t WINDOW_SIZE = 9;

static constexpr int32_t SCORE_FIVE_IN_A_ROW = 1000000; // XXXXX

static constexpr int32_t SCORE_OPEN_FOUR = 10000;       // .XXXX.
static constexpr int32_t SCORE_BROKEN_FOUR = 7500;      // XX.XX or X.XXX or XXX.X
static constexpr int32_t SCORE_CLOSED_FOUR = 5000;      // |XXXX. or .XXXX|

static constexpr int32_t SCORE_OPEN_THREE = 1000;       // .XXX.
static constexpr int32_t SCORE_BROKEN_THREE = 750;      // .XX.X. or .X.XX.
static constexpr int32_t SCORE_CLOSED_THREE = 50;       // |XXX. or .XXX|

static constexpr int32_t SCORE_OPEN_TWO = 100;          // .XX.
static constexpr int32_t SCORE_BROKEN_TWO = 75;         // .X.X.
static constexpr int32_t SCORE_CLOSED_TWO = 50;         // |XX. or .XX|

static constexpr int32_t SCORE_OPEN_ONE = 10;           // .X.
static constexpr int32_t SCORE_CLOSED_ONE = 5;          // |X. or .X|

static INLINE bool isEmpty(char c) { return c == '.'; }
static INLINE bool isUs(char c) { return c == 'X'; }
static INLINE bool isBlock(char c) { return c == 'O' || c == '|'; } // opponent or border


// TODO optimize!
ScoreType EvaluatePattern(const char* w)
{
    // detect five in a row - XXXXX
    for (int i = 0; i <= 4; ++i)
    {
        bool five = true;
        for (int k = 0; k < 5; ++k)
            five &= isUs(w[i + k]);
        if (five)
            return SCORE_FIVE_IN_A_ROW;
    }

    // detect open four - .XXXX.
    for (int i = 1; i <= 4; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isUs(w[i + 3]) && isEmpty(w[i + 4]))
            return SCORE_OPEN_FOUR;
    }

    // detect broken four - XX.XX
    for (int i = 0; i <= 4; ++i)
    {
        if (isUs(w[i]) && isUs(w[i]) && isEmpty(w[i + 2]) && isUs(w[i + 3]) && isUs(w[i + 4]))
            return SCORE_BROKEN_FOUR;
    }

    // detect broken four - X.XXX
    for (int i = 1; i <= 4; ++i)
    {
        if (isUs(w[i]) && isEmpty(w[i + 1]) && isUs(w[i + 2]) && isUs(w[i + 3]) && isUs(w[i + 4]))
            return SCORE_BROKEN_FOUR;
    }

    // detect broken four - XXX.X
    for (int i = 1; i <= 4; ++i)
    {
        if (isUs(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isEmpty(w[i + 3]) && isUs(w[i + 4]))
            return SCORE_BROKEN_FOUR;
    }

    // detect closed four - |XXXX.
    for (int i = 1; i <= 4; ++i)
    {
        if (isBlock(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isUs(w[i + 3]) && isEmpty(w[i + 4]))
            return SCORE_CLOSED_FOUR;
    }

    // detect closed four - .XXXX|
    for (int i = 1; i <= 4; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isUs(w[i + 3]) && isBlock(w[i + 4]))
            return SCORE_CLOSED_FOUR;
    }

    // detect open three - .XXX.
    for (int i = 0; i <= 4; ++i)
    {
        if (isEmpty(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isUs(w[i + 3]) && isEmpty(w[i + 4]))
        {
            return SCORE_OPEN_THREE;
        }
    }

    // detect broken three - .XX.X.
    for (int i = 1; i <= 4; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isEmpty(w[i + 2]) && isUs(w[i + 3]) && isEmpty(w[i + 4]))
            return SCORE_BROKEN_THREE;
    }

    // detect broken three - .X.XX.
    for (int i = 1; i <= 4; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isEmpty(w[i + 1]) && isUs(w[i + 2]) && isUs(w[i + 3]) && isEmpty(w[i + 4]))
            return SCORE_BROKEN_THREE;
    }

    // detect closed three - |XXX.
    for (int i = 1; i <= 4; ++i)
    {
        if (isBlock(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isEmpty(w[i + 3]))
            return SCORE_CLOSED_THREE;
    }

    // detect closed three - .XXX|
    for (int i = 1; i <= 4; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isBlock(w[i + 3]))
            return SCORE_CLOSED_THREE;
    }

    // detect open two - .XX.
    for (int i = 0; i <= 5; ++i)
    {
        if (isEmpty(w[i]) && isUs(w[i + 1]) && isUs(w[i + 2]) && isEmpty(w[i + 3]))
        {
            return SCORE_OPEN_TWO;
        }
    }

    // detect broken two - .X.X.
    for (int i = 1; i <= 5; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isEmpty(w[i + 1]) && isUs(w[i + 2]) && isEmpty(w[i + 3]))
            return SCORE_BROKEN_TWO;
    }

    // detect closed two - |XX.
    for (int i = 1; i <= 5; ++i)
    {
        if (isBlock(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isEmpty(w[i + 2]))
            return SCORE_CLOSED_TWO;
    }

    // detect closed two - .XX|
    for (int i = 1; i <= 5; ++i)
    {
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isUs(w[i + 1]) && isBlock(w[i + 2]))
            return SCORE_CLOSED_TWO;
    }

    // detect open one - .X.
    for (int i = 0; i <= 6; ++i)
        if (isEmpty(w[i]) && isUs(w[i + 1]) && isEmpty(w[i + 2]))
            return SCORE_OPEN_ONE;

    // detect closed one - |X.
    for (int i = 1; i <= 6; ++i)
        if (isBlock(w[i - 1]) && isUs(w[i]) && isEmpty(w[i + 1]))
            return SCORE_CLOSED_ONE;

    // detect closed one - .X|
    for (int i = 1; i <= 6; ++i)
        if (isEmpty(w[i - 1]) && isUs(w[i]) && isBlock(w[i + 1]))
            return SCORE_CLOSED_ONE;

    return 0;
}

ScoreType Evaluate(const Position& position)
{
    ScoreType score = 0;

    return score;
}