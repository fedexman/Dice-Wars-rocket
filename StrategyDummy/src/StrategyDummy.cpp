#include "StrategyDummy.h"
#include <vector>
#include <iostream>

StrategyDummy::StrategyDummy(unsigned int id, unsigned int nbPlayer, const SMap* map) :
	Id(id),
	NbPlayer(nbPlayer)
{
	// faire une copie enti�re de la structure map localement dans l'objet Map
	Map.cells = new SCell[map->nbCells];
	Map.nbCells = map->nbCells;
	std::cout << map->nbCells << std::endl;//on actualise les cells de la maps
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = map->cells[i].infos; 
		Map.cells[i].nbNeighbors = map->cells[i].nbNeighbors; 
		Map.cells[i].neighbors = new pSCell[Map.cells[i].nbNeighbors];
		for (unsigned int j = 0; j < Map.cells[i].nbNeighbors; ++j) Map.cells[i].neighbors[j] = map->cells[i].neighbors[j];
	}
}

StrategyDummy::~StrategyDummy()
{
	// d�truire proprement la structure Map
	delete[] Map.cells;
}

bool StrategyDummy::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
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
	std::vector<pSCell> cellsAttackable;
	for (unsigned i = 0; i < FromCell.nbNeighbors; ++i) {//permet de trouver tous les cases ennemi
		if (FromCell.neighbors[i]->infos.owner != Id ) {
			cellsAttackable.push_back(FromCell.neighbors[i]);//on rajoute ladresse des cells qui sont jouables
			break;
		}
	}
	if (cellsAttackable.empty()) {
		return false;//si aucune case jouable, on finit le tour
		std::cout << "end of turn : no attackable cell" << std::endl;
	}
	auto To = cellsAttackable[0]->infos.id;
	for (auto& it : cellsAttackable) {
		if (it->infos.nbDices > Map.cells[To].infos.nbDices) {//trouve la case ennemie en partant de from avec le moins de des
			To = it->infos.id;
		}
	}
	turn->cellFrom = From;
	turn->cellTo = To;
	return true;
	//	auto& From = Map.cells[turn->cellFrom];
	//	unsigned int i = 0;
	//	while ( i < From.nbNeighbors && From.neighbors[i]->infos.owner == Id ) i++;//trouve la premiere case qui nous appartient pas possibilité qui y ait pas de voisin donc faute
	//	turn->cellTo = i;
	//	std::cout << "From.nb" << From.nbNeighbors << std::endl;
	//	for (i; i < From.nbNeighbors; i++)
	//	{
	//		if (From.neighbors[i]->infos.id != Id && From.neighbors[i]->infos.nbDices < From.infos.nbDices && From.neighbors[i]->infos.nbDices < From.neighbors[turn->cellTo]->infos.nbDices) {//verifie que cette case nous appartient pas et que elle a plus de des que la precedente
	//			turn->cellTo = i;
	//		}
	//
	//	}
	//	std::cout << "fin de choix : from " << From.infos.id << " " << turn->cellFrom << std::endl;
	//	std::cout << "to " << turn->cellTo;
	//	return(true);
}






