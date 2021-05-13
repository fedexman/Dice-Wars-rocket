#include "StrategyAdvanced.h"
#include <iostream>
#include <vector>

bool StrategyAdvanced::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];//on acutalise les nouvelles infos du tour
	}
	for (unsigned int i = 0; i < 8; i++) {
		points[i] = state->points[i];	// Points de chaque joueur
		diceStock[i] = state->diceStock[i];	// R�serve de d�s de chaque joueur
	}
	nbCells = state->nbCells;
	

	if (!InitTurn()) { // pas de case jouable
		return false;
	}
	// determiner le mode
	// appeler la fonction mode correspondante


	// stratégie se déclenche lorque le gain de dés est supérieur a tout ceux des adversaires
	// condition mode end game : avoir plus de case que les autres joueurs
	unsigned int max = 0;
	unsigned int index;
	for (unsigned int i = 0; i < 8; i++) {
		if (points[i] > max) {
			max = points[i];
			index = i;
		}
	}
	if (index == Id) {
		// appel de la strategie endgame
	}
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

unsigned int* StrategyAdvanced::Pathfinding(unsigned int IdA, unsigned int IdB)
{
	
	return nullptr;//aucun chemin possible
}

bool StrategyAdvanced::Startgame(STurn* turn)
{
	auto LessNeighbors = 0;
	for (unsigned int i = 0; i < Map.nbCells;++i) {//variable avec less neighors est en bord de map, on va tenter de reunir toutes nos cases la bas
		if (Map.cells[i].nbNeighbors < Map.cells[LessNeighbors].nbNeighbors) {
			LessNeighbors = Map.cells->infos.id;
		}
	}
	// rassembler les d�s dans un coin de map
	//faire cette strat�gie tant que tous les d�s ne sont pas r�uni

	// trouver la case avec le moins de voisin la plus proche

	// diriger les d�s vers cette case


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

	// attaquer seulement avec un stock de 7 dés 
	if (diceStock[Id] < 7) {
		return false;
	}

	// attendre les stacks de 8 dés sur toutes les cases
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices < 8 ) {
			return false;
		}
	}

	// on attaque si l'opposant a 7 dés ou moins
	for (auto& it : playableAttackable) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : it.second) {
			if(itAttack->infos.nbDices <= 7){
				turn->cellFrom = it.first->infos.id;
				turn->cellTo = itAttack->infos.id;
			}
		}
	}
	return false;
}

StrategyAdvanced::~StrategyAdvanced()
{
}
