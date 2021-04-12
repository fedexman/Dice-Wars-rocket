#pragma once

#include "../../Commun/IStrategyLib.h"

class StrategyDummy
{
public:
	StrategyDummy(unsigned int id, unsigned int nbPlayer, const SMap* map);
	StrategyDummy(const StrategyDummy &obj) =delete;
	StrategyDummy(StrategyDummy &&obj) =delete;
	~StrategyDummy();
	StrategyDummy& operator=(const StrategyDummy &obj) =delete;
	StrategyDummy& operator=(StrategyDummy &&obj) =delete;

public:
	bool PlayTurn(unsigned int gameTurn, const SGameState *state, STurn *turn);

protected:
	const unsigned int Id;
	const unsigned int NbPlayer;
	SMap Map;
};
