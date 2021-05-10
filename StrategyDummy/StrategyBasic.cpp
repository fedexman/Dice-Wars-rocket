#include "StrategyBasic.h"
#include <vector>
#include <iostream>

bool StrategyBasic::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	// d�rouler votre algorithme de choix
	std::cout << state->nbCells << std::endl;

	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];//on acutalise les nouvelles infos du tour
	}

	std::vector<pSCell> cellsPlayable;
	for (unsigned i = 0; i < Map.nbCells; ++i) {//permet de trouver tous les cases jouables
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices > 1) {
			for (unsigned j = 0; j < Map.cells[i].nbNeighbors; ++j) {//au moins une case enemi
				if (Map.cells[i].neighbors[j]->infos.owner != Id) {
					cellsPlayable.push_back(&Map.cells[i]);//on rajoute ladresse des cells qui sont jouables
					break;
				}
			}
		}
	}
	if (cellsPlayable.empty()) {
		return false;//si aucune case jouable, on finit le tour
		std::cout << "end of turn : no more playable cell" << std::endl;
	}

	//strategie basique, from du cellsPlayable ayant le plus de des
	auto From = cellsPlayable[0]->infos.id;
	for (auto& it : cellsPlayable) {
		if (it->infos.nbDices > Map.cells[From].infos.nbDices) {//verifie que cette case nous appartient et que elle a plus de des que la precedente
			From = it->infos.id;
			std::cout << "id : " << From << it->infos.id << std::endl;
		}
	}
	//on a donc la case jouable avec le plus de des
	std::cout << "From nb dices" << Map.cells[From].infos.nbDices << std::endl;
	auto& FromCell = Map.cells[From];
	std::vector<pSCell> cellsAttackable;//transfromer en matrice pour connaitre 
	for (unsigned i = 0; i < FromCell.nbNeighbors; ++i) {//permet de trouver tous les cases ennemi
		if (FromCell.neighbors[i]->infos.owner != Id ) {
			cellsAttackable.push_back(FromCell.neighbors[i]);//on rajoute ladresse des cells qui sont jouables
			//break;
		}
	}
	if (cellsAttackable.empty()) {
		return false;//si aucune case jouable, on finit le tour
		std::cout << "end of turn : no attackable cell" << std::endl;
	}
	auto To = cellsAttackable[0]->infos.id;
	for (auto& it : cellsAttackable) {
		std::cout << To << ' ' << Map.nbCells << std::endl;
		if (it->infos.nbDices > Map.cells[To].infos.nbDices) {//trouve la case ennemie en partant de from avec le moins de des
			To = it->infos.id;
		}
	}
	turn->cellFrom = From;
	turn->cellTo = To;
	std::cout << "end of turn : no problem" << std::endl;
	return true;
}

