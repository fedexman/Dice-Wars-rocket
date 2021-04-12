#pragma once
// v 1.0

#include <cstddef>
#include "IInfoLib.h"

// Structure d�finissant une cellule hexagonale
struct SRegionCell
{
	unsigned int x;
	unsigned int y;
};

// Structure d�finissant une r�gion : ensemble de cellules hexagonales
struct SRegion
{
	SRegionCell *cells;
	size_t nbCells;
};

// Ensemble de r�gions
struct SRegions
{
	SRegion *region;
	size_t nbRegions;
};

struct IMap
{
	virtual ~IMap() {}

	// Fonction � appeler pour g�n�rer une nouvelle carte
	//	les variables r et c indiquent la taille de la carte � g�n�rer
	// ces valeurs peuvent �tre modifi�es par le g�n�rateur de carte
	virtual SRegions* GenerateMap(unsigned int &r, unsigned int &c) = 0;

	// Fonction � appeler pour d�truire les structures allou�es lors de la g�n�ration de carte.
	virtual void DeleteMap(SRegions *regions) = 0;
};

// Types pr�d�finis pour simplifier la r�cup�ration des fonctions dans la librairie
using pMapFactory = IMap * (*)();
using pMapDelete = void (*)(IMap* obj);
