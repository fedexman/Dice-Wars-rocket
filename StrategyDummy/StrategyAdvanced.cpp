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
		diceStock[i] = state->diceStock[i];	// Réserve de dés de chaque joueur
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
	if (index == Id) { // strategie endgame
		return Endgame(turn);
	}
	else { //stratégie midgame
		
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
	// rassembler les dés dans un coin de map
	//faire cette stratégie tant que tous les dés ne sont pas réuni

	// trouver la case avec le moins de voisin la plus proche

	// diriger les dés vers cette case


	return false;
}

bool StrategyAdvanced::Middlegame(STurn* turn)
{
	// prendre les cases facile ( proba 90 % )
	pSCell bestCellP = playableAttackable.begin()->first; // meilleur cellule depuis laquelle on attaque
	pSCell bestCellA = playableAttackable.begin()->second.operator[](0); // meilleur cellule a attaquer
	int bestDiff = playableAttackable[0].first->infos.nbDices - playableAttackable[0].second.operator[](0)->infos.nbDices;// nb de dés de différence entre attaquant et attaqué
	for (auto& it : playableAttackable) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : it.second) {
			const int diff = it.first->infos.nbDices - itAttack->infos.nbDices;
			if (diff > bestDiff) {//trouve la case avec le plus grand ecart de des avec son adversere
				bestDiff = diff;
				bestCellP = it.first;//meilleur case d'ou partir
				bestCellA = itAttack;//meilleur case a attaquer
			}
		}
	}
	if (bestDiff > 0) { //si la bestDiff est superieur a 0 on des chance remporter la case adverse donc on joue
		turn->cellFrom = bestCellP->infos.id;
		turn->cellTo = bestCellA->infos.id;
		std::cout << " strat midgame on joue, id attaquant " <<turn->cellFrom<<"id defense : "<<turn->cellTo<< std::endl;
		return true;
	}
	// sinon attendre
	std::cout << "strat midgame on ne joue pas, pas d'avantage sur les opposants" << std::endl;
	return false;
}

bool StrategyAdvanced::Endgame(STurn* turn)
{

	// attaquer seulement avec un stock de 7 dés 
	if (diceStock[Id] < 7) {
		std::cout << "strat endgame on ne joue pas on ne possède pas 7 dés en stock" << std::endl;
		return false;
	}

	// attendre les stacks de 8 dés sur toutes les cases
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices < 8 ) {
			std::cout << "strat endgame on ne joue pas une des cases n'as pas 8 dés" << std::endl;
			return false;
		}
	}

	// on attaque si l'opposant a 7 dés ou moins
	for (auto& it : playableAttackable) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : it.second) {
			if(itAttack->infos.nbDices <= 7){
				turn->cellFrom = it.first->infos.id;
				turn->cellTo = itAttack->infos.id;
				std::cout << " strat endgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo << std::endl;
				return true;
			}
		}
	}
}

StrategyAdvanced::~StrategyAdvanced()
{
}
