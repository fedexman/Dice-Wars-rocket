#pragma once
#include "src/StrategyDummy.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

class StrategyAdvanced : public StrategyDummy
{
public:

	struct informations {
	public:
		informations(unsigned int iddepart, unsigned int idarrive, SMap& Map);
		informations(unsigned int nb_dices) : nb_dices(nb_dices),effective_path(false) {};
		informations(const informations& v) = default;
		informations(informations&& vec) = default;
		~informations() = default;

		informations& operator=(const informations& info) = default;
		informations& operator=(informations&& info) = default;
		friend std::ostream& operator<<(std::ostream& o, StrategyAdvanced::informations info);

		unsigned int nb_dices;
		pSCell depart;
		pSCell arrive;
		std::vector<unsigned int> path;
		bool effective_path;
	};

	StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map);
	bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn);
	bool InitTurn(std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);

	StrategyAdvanced::informations Pathfinding(StrategyAdvanced::informations informations);
	StrategyAdvanced::informations Pathfindingprim(unsigned int iddepart,unsigned int idarrive);

	std::vector<unsigned int> Cluster(unsigned int idcell);
	std::vector<std::vector<unsigned int>> All_cluster(unsigned int owner);

	bool Startgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	bool Middlegame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);
	bool Endgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable);

	

	~StrategyAdvanced();
protected:
	std::ofstream outputLog;
	unsigned int points[8] = {};	// Points de chaque joueur
	unsigned int diceStock[8] = {};	// Réserve de dés de chaque joueur
	unsigned int innerGameTurn = {};
	int turnCount = -1;
	
	enum class Status {
		startgame,
		middlegame,
		endgame
	};
	Status status = Status::startgame; // mode de jeu jeu suivant l'avancement de la partie
};

