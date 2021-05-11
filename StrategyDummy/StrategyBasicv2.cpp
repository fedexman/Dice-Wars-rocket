#include "StrategyBasicv2.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
bool StrategyBasicv2::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];//on acutalise les nouvelles infos du tour
	}

	for (unsigned i = 0; i < Map.nbCells; ++i) {//permet de trouver tous les cases jouables
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices > 1) {
			for (unsigned j = 0; j < Map.cells[i].nbNeighbors; ++j) {//au moins une case enemi
				if (Map.cells[i].neighbors[j]->infos.owner != Id) {
					auto attackable = new std::vector<pSCell>;
					playableAttackable.push_back({ &Map.cells[i],attackable });//on rajoute ladresse des cells qui sont jouables
					break;
				}
			}
		}
	}
	if (playableAttackable.empty()) {
		clearPA();
		std::cout << "end of turn : no more playable cell" << std::endl;
		return false;//si aucune case jouable, on finit le tour
	}
	//on a donc toutes les cases jouables, on va maintenant trouver toutes les cases attackable depuis nos cases playable
	bool emptyAttackable = true;//permet de savoir des quon a au moins une case attackables
	for (auto& it : playableAttackable) {//on parcours toutes les cases jouables
		for (unsigned i = 0; i < it.first->nbNeighbors; ++i) {//permet de trouver tous les cases ennemi
			if (it.first->neighbors[i]->infos.owner != Id) {
				it.second->push_back(it.first->neighbors[i]);//on rajoute ladresse des cells qui sont attackables
				emptyAttackable = false;
			}
		}
	}
	if (emptyAttackable) {
		clearPA();
		std::cout << "end of turn : no attackable cell" << std::endl;
		return false;//si aucune case jouable, on finit le tour
	}

	pSCell bestCellP = playableAttackable.begin()->first;
	pSCell bestCellA = playableAttackable.begin()->second->operator[](0);
	int bestDiff = playableAttackable[0].first->infos.nbDices - playableAttackable[0].second->operator[](0)->infos.nbDices;//initialise a la premier valeur
	for (auto& it : playableAttackable) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : *it.second) {
			const int diff = it.first->infos.nbDices - itAttack->infos.nbDices;
			if (diff > bestDiff) {//trouve la case avec le plus grand ecart de des avec son adversere
				bestDiff = diff;
				bestCellP = it.first;//meilleur case d'ou partir
				bestCellA = itAttack;//meilleur case a attaquer
			}
		}
	}
	if (bestDiff > 0) { //si la bestDiff est superieur a 0 on des chance de faire ce coup donc on joue
		turn->cellFrom = bestCellP->infos.id;
		turn->cellTo = bestCellA->infos.id;
		clearPA();
		std::cout << "from: " << turn->cellFrom << "dices: " << bestCellP->infos.nbDices << " to:" << turn->cellTo << "dices: " << bestCellA->infos.nbDices << std::endl;
		return true;
	}
	clearPA();
	return false;
}

void StrategyBasicv2::clearPA()
{
	for (auto& it : playableAttackable) {
		delete it.second;
	}
	playableAttackable.clear();
}

StrategyBasicv2::~StrategyBasicv2()
{
	//for (auto& it : playableAttackable) {
	//	delete it.second;
	//}
	//destroy playableattackable
}
