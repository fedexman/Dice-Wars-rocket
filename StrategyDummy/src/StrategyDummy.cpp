#include "StrategyDummy.h"
#include <vector>
#include <iostream>

StrategyDummy::StrategyDummy(unsigned int id, unsigned int nbPlayer, const SMap* map) :
	Id(id),
	NbPlayer(nbPlayer)
{
	// faire une copie entiére de la structure map localement dans l'objet Map
	Map.cells = new SCell[map->nbCells];
	Map.nbCells = map->nbCells;
	std::cout << map->nbCells << std::endl;//on actualise les cells de la maps
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = map->cells[i].infos; 
		Map.cells[i].nbNeighbors = map->cells[i].nbNeighbors; 
		Map.cells[i].neighbors = new pSCell[Map.cells[i].nbNeighbors];
		for (unsigned int j = 0; j < Map.cells[i].nbNeighbors; ++j) {
			Map.cells[i].neighbors[j] = &Map.cells[map->cells[i].neighbors[j]->infos.id];
		}
		//&Map.cells[map->cells[i].neighbors[j]->infos.id] permet de trouver l'id du neighbords et de renvoyer le pointeur de cette cellule portant cette id
	}
}

StrategyDummy::~StrategyDummy()
{
	// détruire proprement la structure Map

	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		delete[] Map.cells[i].neighbors;
	}
	delete[] Map.cells;
}

