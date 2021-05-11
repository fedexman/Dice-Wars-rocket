#pragma once
#include "./src/StrategyDummy.h"
#include <vector>
class StrategyBasicv2 :
	public StrategyDummy
{
public:
	StrategyBasicv2(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) {};
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	void clearPA();
	~StrategyBasicv2();
protected:
	std::vector<std::pair<pSCell, std::vector<pSCell>*>> playableAttackable;
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
};

