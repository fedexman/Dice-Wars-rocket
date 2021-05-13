﻿#pragma once
#include "src/StrategyDummy.h"
#include <vector>
class StrategyAdvanced : public StrategyDummy
{
public:
	StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) {};
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	bool InitTurn();

	bool Startgame(STurn* turn);
	bool Middlegame(STurn* turn);
	bool Endgame(STurn* turn);

	~StrategyAdvanced();
protected:
	std::vector<std::pair<pSCell, std::vector<pSCell>>> playableAttackable;
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case

	unsigned int nbCells;
	unsigned int points[8];	// Points de chaque joueur
	unsigned int diceStock[8];	// R�serve de d�s de chaque joueur
	
	enum Status {
		startgame,
		middlegame,
		endgame
	};
	Status status = startgame; // mode de jeu jeu suivant l'avancement de la partie
};

