#include "StrategyBasicv2.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
bool StrategyBasicv2::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];//on acutalise les nouvelles infos du tour
	}

	playableAttackable pA;//foncteur
	if (!pA.init(this)) { return false; }//on init le foncteur et si il return false on return false, mais si true on continue
	
	if (pA.bestPlay(turn) > 0) { return true; }//si la bestDiff est superieur a 0 on des chance de faire ce coup donc on joue
    return false;
}

StrategyBasicv2::~StrategyBasicv2()
{
	//for (auto& it : playableAttackable) {
	//	delete it.second;
	//}
	//destroy playableattackable
}

bool StrategyBasicv2::playableAttackable::init(StrategyBasicv2* T)//trouve toutes les cases jouables et toutes les cases attackable depuis les cases jouables
{
	t = T;
	for (unsigned i = 0; i < t->Map.nbCells; ++i) {//permet de trouver tous les cases jouables
		if (t->Map.cells[i].infos.owner == t->Id && t->Map.cells[i].infos.nbDices > 1) {
			for (unsigned j = 0; j < t->Map.cells[i].nbNeighbors; ++j) {//au moins une case enemi
				if (t->Map.cells[i].neighbors[j]->infos.owner != t->Id) {
					auto attackable = new std::vector<pSCell>;
					pA.push_back({ &t->Map.cells[i],attackable });//on rajoute ladresse des cells qui sont jouables
					break;
				}
			}
		}
	}
	if (pA.empty()) {
		return false;//si aucune case jouable, on finit le tour
		std::cout << "end of turn : no more playable cell" << std::endl;
	}
	//on a donc toutes les cases jouables, on va maintenant trouver toutes les cases attackable depuis nos cases playable
	bool emptyAttackable = true;//permet de savoir des quon a au moins une case attackables
	for (auto& it:pA) {//on parcours toutes les cases jouables
		for (unsigned i = 0; i < it.first->nbNeighbors; ++i) {//permet de trouver tous les cases ennemi
			if (it.first->neighbors[i]->infos.owner != t->Id) {
				it.second->push_back(it.first->neighbors[i]);//on rajoute ladresse des cells qui sont attackables
				emptyAttackable = false;
			}
		}
	}
	if (emptyAttackable) {
		return false;//si aucune case jouable, on finit le tour
		std::cout << "end of turn : no attackable cell" << std::endl;
	}
	return true;

}

int StrategyBasicv2::playableAttackable::bestPlay(STurn* turn)
{

	//pSCell bestCellP = pA.begin()->first;
	//pSCell bestCellA = pA.begin()->second->operator[](0);
	double bestDiff = double(pA[0].first->infos.nbDices - pA[0].second->operator[](0)->infos.nbDices);//initialise a la premier valeur
	for (auto& it : pA) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : *it.second) {
			const int diff = itAttack->infos.nbDices - it.first->infos.nbDices;
			if (diff > bestDiff) {//trouve la case avec le plus grand ecart de des avec son adversere
				bestDiff = diff; 
				turn->cellFrom = it.first->infos.id;//meilleur case d'ou partir
				turn->cellTo = itAttack->infos.id;//meilleur case a attaquer
			}
		}
	}
	return bestDiff;
}
