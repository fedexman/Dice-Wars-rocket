#pragma once
// v 1.0

#include "IInfoLib.h"

// Structure d�finissant les caract�ristiques d'une cellule
struct SCellInfo
{
	unsigned int id;	// Id de la cellule
	unsigned int owner;	// Id du joueur qui la poss�de
	unsigned int nbDices;	// Nombre de d�s
};

struct SGameState
{
	SCellInfo *cells;	// Tableau d'informations sur le contenu des cellules
	unsigned int nbCells;
	unsigned int points[8];	// Points de chaque joueur
	unsigned int diceStock[8];	// R�serve de d�s de chaque joueur
};

// Structure d�finissant les caract�ristiques d'une cellule
struct SCell;
using pSCell = SCell*;
struct SCell
{
	SCellInfo infos;	// Informations sur la cellule
	pSCell *neighbors;	// Tableau de pointeurs vers des cellules voisines
	unsigned int nbNeighbors;	// Nombre de cellules voisines
};

// Structure d�finissant la carte globale du jeu
struct SMap
{
	SCell *cells;	// Tableau des cellules
	unsigned int nbCells;	// Nombre de cellules
};

// Structure d�finissant les param�tres d'un coup jou�
struct STurn
{
	unsigned int cellFrom;
	unsigned int cellTo;
};

struct IStrategy
{
	virtual ~IStrategy() {}

	// Fonction � appeler au d�but de chaque partie
	//	* Ne pas oublier pour l'arbitre de dupliquer toute la structure map pour chaque appel !
	virtual void InitGame(unsigned int id, unsigned int nbPlayer, const SMap* map) = 0;

	// Fonction � appeler � chaque tour sur la strat�gie et tant que le retour de fonction est vrai et qu'il n'y a pas d'erreur.
	//	* En cas d'erreur, r�tablir la carte dans l'�tat initial avant le premier tour du joueur.
	// valeur de retour : bool�en - false : coups termin�s, true : structure turn compl�t�e avec un nouveau coup � jouer.
	// gameTurn : tour de jeu (permet � la strat�gie de savoir si elle a �chou�e au tour pr�c�dant)
	virtual bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn) = 0;

	// Fonction � appeler � la fin de chaque partie
	virtual void EndGame(unsigned int idWinner) = 0;
};

// Types pr�d�finis pour simplifier la r�cup�ration des fonctions dans la librairie
using pStrategyFactory = IStrategy * (*)();
using pStrategyDelete = void (*)(IStrategy* obj);
