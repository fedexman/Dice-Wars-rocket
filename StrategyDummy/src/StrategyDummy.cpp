#include "StrategyDummy.h"

StrategyDummy::StrategyDummy(unsigned int id, unsigned int nbPlayer, const SMap* map) :
	Id(id),
	NbPlayer(nbPlayer)
{
	// faire une copie enti�re de la structure map localement dans l'objet Map
	Map.cells = nullptr;
	Map.nbCells = 0;
}

StrategyDummy::~StrategyDummy()
{
	// d�truire proprement la structure Map
}

bool StrategyDummy::PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn)
{
	// d�rouler votre algorithme de choix
	return(false);
}






