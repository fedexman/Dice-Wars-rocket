﻿#pragma once
#include "src/StrategyDummy.h"
#include <vector>
#include <iostream>
#include <fstream>

class StrategyAdvanced : public StrategyDummy
{
public:
	StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map);
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	bool InitTurn(std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	std::pair<unsigned int, std::vector<unsigned int>> Pathfinding(unsigned int depart, unsigned int arrive);

	bool Startgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	bool Middlegame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	bool Endgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);

	

	~StrategyAdvanced();
protected:
	std::ofstream outputLog;
	unsigned int points[8] = {};	// Points de chaque joueur
	unsigned int diceStock[8] = {};	// Réserve de dés de chaque joueur
	unsigned int innerGameTurn = {};
	int turnCount = -1;
	
	enum class Status {
		startgame,
		middlegame,
		endgame
	};
	Status status = Status::startgame; // mode de jeu jeu suivant l'avancement de la partie
};

