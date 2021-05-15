#include "StrategyAdvanced.h"
#include <iostream>
#include <vector>
#include <fstream>

StrategyAdvanced::StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) 
{
	std::cout << "ADVANCED" << std::endl;
	outputLog.open("log.txt");
	if (outputLog.is_open())
	{
		outputLog << "log strategy "<< __TIME__ << "\n";
	}
	else std::cout << "Unable to open file";
}

bool StrategyAdvanced::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];//on acutalise les nouvelles infos du tour
	}
	for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {
		points[i] = state->points[i];	// Points de chaque joueur
		diceStock[i] = state->diceStock[i];	// Réserve de dés de chaque joueur
	}
	std::vector<std::pair<pSCell, std::vector<pSCell>>> playableAttackable;
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
	if (gameTurn != turnCount) {
		innerGameTurn = 0;
		turnCount = gameTurn;
		outputLog << std::endl << "TURN:" << turnCount <<std::endl;
		//determination nouvelle strat
		// stratégie se déclenche lorque le gain de dés est supérieur a tout ceux des adversaires
		// condition mode end game : avoir plus de case que les autres joueurs
		unsigned int index = 0;
		for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {
			if (points[i] > points[index]) {
				index = i;
			}
		}
		if (index == Id) { // strategie endgame
			status = Status::endgame;
		}
		else { //stratégie midgame
			status = Status::middlegame;
		}
	}
	else { innerGameTurn++; }

	if (!InitTurn(playableAttackable)) { // initialise la variable playableAttackable et return fasle si ya pas de case jouable
		return false;
	}
	// determiner le mode
	// appeler la fonction mode correspondante

	switch (status)
	{
	case StrategyAdvanced::Status::startgame:
		//
		break;
	case StrategyAdvanced::Status::middlegame:
		return Middlegame(turn, playableAttackable);
		break;
	case StrategyAdvanced::Status::endgame:
		return Endgame(turn, playableAttackable);
		break;
	default:
		throw("pourquoi on est la");
		break;
	}

}

bool StrategyAdvanced::InitTurn(std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable)//init la variable playableAttackable
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
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
		outputLog << Id << ": end of turn : no more playable cell" << std::endl;
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
		outputLog << Id <<": end of turn : no attackable cell" << std::endl;
		return false;//si aucune case jouable, on finit le tour
	}
	return true;
}

std::pair<unsigned int, std::vector<unsigned int>> StrategyAdvanced::Pathfinding(unsigned int depart, unsigned int arrive)
{
	/*
	std::vector<std::vector<unsigned int>> quickestPath;
	quickestPath.resize(Map.nbCells);//Met la size pour le nombre de cells
	*/

	//recursif
	/*
	arret  id de la case est celui recherché
	return nb_dice,path
	*/
	/*
	sinon nb_dice+= case.dice
	return min(recursive(voisins))
	*/



	return std::pair<unsigned int, std::vector<unsigned int>>();
}


bool StrategyAdvanced::Startgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable)
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

bool StrategyAdvanced::Middlegame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable)
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
		outputLog << Id <<": strat midgame on joue, id attaquant " <<turn->cellFrom<<"id defense : "<<turn->cellTo<< std::endl;
		return true;
	}
	// sinon attendre
	outputLog << Id<< ": strat midgame on ne joue pas, pas d'avantage sur les opposants" << std::endl;
	return false;
}

bool StrategyAdvanced::Endgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable)
{

	// attaquer seulement avec un stock de 7 dés ou plus a la fin du tour

	int future_dice_stock = 0; // nb de dés donné à la fin du tour
	int missing_dices = 0; // nb de dés manquant pour être plein de 8 dés
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (Map.cells[i].infos.owner == Id) {
			future_dice_stock += 1;
			missing_dices += (8 - Map.cells[i].infos.nbDices);
		}
	}
	if (future_dice_stock - missing_dices < 7) {
		return false;
	}
	outputLog << (future_dice_stock - missing_dices) << std::endl;

	// on attaque si l'opposant la ou on a le plus de chance de gagner en priorité sinon on attaque un block de 8 dés
	pSCell bestCellP = playableAttackable.begin()->first; // meilleur cellule depuis laquelle on attaque
	pSCell bestCellA = playableAttackable.begin()->second.operator[](0); // meilleur cellule a attaquer
	int bestDiff = playableAttackable[0].first->infos.nbDices - playableAttackable[0].second.operator[](0)->infos.nbDices;// nb de dés de différence entre attaquant et attaqué
	int diff {};
	for (auto& it : playableAttackable) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : it.second) {
			diff = it.first->infos.nbDices - itAttack->infos.nbDices;
			if (diff > bestDiff) {//trouve la case avec le plus grand ecart de des avec son adversere
				bestDiff = diff;
				bestCellP = it.first;//meilleur case d'ou partir
				bestCellA = itAttack;//meilleur case a attaquer
			}
		}
	}
	turn->cellFrom = bestCellP->infos.id;
	turn->cellTo = bestCellA->infos.id;
	outputLog << Id<<": strat endgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo << std::endl;
	return true;
}

StrategyAdvanced::~StrategyAdvanced()
{
	outputLog.close();
}
