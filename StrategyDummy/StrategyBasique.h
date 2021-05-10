#pragma once
#include "src/StrategyDummy.h"
class StrategyBasic :
    public StrategyDummy
{
public:
	StrategyBasic(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) {};
	bool PlayTurn(unsigned int gameTurn, const SGameState *state, STurn *turn);
};

