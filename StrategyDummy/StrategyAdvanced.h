#pragma once
#include "src/StrategyDummy.h"
#include <vector>
#include <iostream>
class StrategyAdvanced : public StrategyDummy
{
public:
	StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) { std::cout << "ADVANCED" << std::endl; };
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	bool InitTurn(std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	unsigned int* Pathfinding(unsigned int IdA, unsigned int IdB);

	bool Startgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	bool Middlegame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	bool Endgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);

	~StrategyAdvanced();
protected:

	unsigned int points[8];	// Points de chaque joueur
	unsigned int diceStock[8];	// Réserve de dés de chaque joueur
	
	enum Status {
		startgame,
		middlegame,
		endgame
	};
	Status status = startgame; // mode de jeu jeu suivant l'avancement de la partie
};

