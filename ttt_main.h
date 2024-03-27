#ifndef TTT_MAIN_H
#define TTT_MAIN_H


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "agents/negamax.h"

#include "game.h"
#ifdef USE_RL
#include "agents/reinforcement_learning.h"
#elif defined(USE_MCTS)
#include "agents/mcts.h"
#else
#include "agents/negamax.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ttt_main();

#ifdef __cplusplus
}
#endif

#endif /* TTT_MAIN_H */
