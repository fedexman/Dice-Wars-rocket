#pragma once
// v 1.0

#include "../../Commun/IMapLib.h"
#include "../../Commun/library.h"

extern "C"
{
	// Fonction � appeler au chargement de la librairie
	// Le g�n�rateur de carte doit compl�ter la structure SInfo
	API_EXPORT void InitMap(SInfo* info);

	API_EXPORT IMap* MapFactory();
	API_EXPORT void MapDelete(IMap* obj);
}
