#pragma once

#include "../../Commun/IStrategyLib.h"
#include "StrategyDummy.h"
#include "../StrategyBasic.h"
#include "../StrategyBasicv2.h"

class Strategy : public IStrategy
{
public:
	Strategy() : Current(nullptr) {}
	virtual ~Strategy() { delete Current; }
	void InitGame(unsigned int id, unsigned int nbPlayer, const SMap* map);
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	void EndGame(unsigned int idWinner);

protected:
	StrategyDummy* Current;
};
