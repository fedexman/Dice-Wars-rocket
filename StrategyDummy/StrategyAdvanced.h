#pragma once
#include "src/StrategyDummy.h"
#include <vector>
#include <iostream>
class StrategyAdvanced : public StrategyDummy
{
public:
	StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) { std::cout << "ADVANCED" << std::endl; };
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	bool InitTurn();
	unsigned int* Pathfinding(unsigned int IdA, unsigned int IdB);

	bool Startgame(STurn* turn);
	bool Middlegame(STurn* turn);
	bool Endgame(STurn* turn);

	~StrategyAdvanced();
protected:
	std::vector<std::pair<pSCell, std::vector<pSCell>>> playableAttackable;
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
	
	enum Status {
		startgame,
		middlegame,
		endgame
	};
	Status status = startgame; // mode de jeu jeu suivant l'avancement de la partie
};

