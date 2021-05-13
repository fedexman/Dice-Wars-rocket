#include "StrategyAdvanced.h"
#include <iostream>
#include <vector>

bool StrategyAdvanced::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];//on acutalise les nouvelles infos du tour
	}

	if (!InitTurn()) { // pas de case jouable
		return false;
	}
	// determiner le mode
	// appeler la fonction mode correspondante

}

bool StrategyAdvanced::InitTurn()
{
	for (unsigned i = 0; i < Map.nbCells; ++i) {//permet de trouver tous les cases jouables
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices > 1) {
			for (unsigned j = 0; j < Map.cells[i].nbNeighbors; ++j) {//au moins une case enemi
				if (Map.cells[i].neighbors[j]->infos.owner != Id) {
					//auto attackable = new std::vector<pSCell>;
					std::vector<pSCell> attackable;
					playableAttackable.push_back({ &Map.cells[i],attackable});//on rajoute ladresse des cells qui sont jouables
					break;
				}
			}
		}
	}
	if (playableAttackable.empty()) {
		std::cout << "end of turn : no more playable cell" << std::endl;
		return false;//si aucune case jouable, on finit le tour
	}
	//on a donc toutes les cases jouables, on va maintenant trouver toutes les cases attackable depuis nos cases playable
	bool emptyAttackable = true;//permet de savoir des quon a au moins une case attackables
	for (auto& it : playableAttackable) {//on parcours toutes les cases jouables
		for (unsigned i = 0; i < it.first->nbNeighbors; ++i) {//permet de trouver tous les cases ennemi
			if (it.first->neighbors[i]->infos.owner != Id) {
				it.second.push_back(it.first->neighbors[i]);//on rajoute ladresse des cells qui sont attackables
				emptyAttackable = false;
			}
		}
	}
	if (emptyAttackable) {
		std::cout << "end of turn : no attackable cell" << std::endl;
		return false;//si aucune case jouable, on finit le tour
	}
	return true;
}

bool StrategyAdvanced::Startgame(STurn* turn)
{
	// rassembler les dés dans un coin de map
	//faire cette stratégie tant que tous les dés ne sont pas réuni

	// trouver la case avec le moins de voisin la plus proche

	// diriger les dés vers cette case


	return false;
}

bool StrategyAdvanced::Middlegame(STurn* turn)
{
	// prendre les cases facile ( proba 90 % )

	// sinon attendre 
	return false;
}

bool StrategyAdvanced::Endgame(STurn* turn)
{
	// stratégie se déclenche lorque le gain de dés est supérieur a tout ceux des adversaires

	// attendre les stacks de 8 dés sur toutes les cases

	// attaquer seulement avec un stock de 7 dés 
	return false;
}

StrategyAdvanced::~StrategyAdvanced()
{
}
