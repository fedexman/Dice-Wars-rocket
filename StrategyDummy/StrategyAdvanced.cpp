#include "StrategyAdvanced.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

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
	for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {//actualisation des infos du tour 
		points[i] = state->points[i];	// Points de chaque joueur
		diceStock[i] = state->diceStock[i];	// Réserve de dés de chaque joueur
	}
	bool needNewStatus = false;//permet de savoir quand il faut calculer si on a besoin d'une nouvelle strat.
	if (gameTurn != turnCount) {//si on est a une nouveau vrai tour, on change de strat
		innerGameTurn = 0;//gameTurn a linterieur dun tour
		turnCount = gameTurn;
		outputLog << std::endl << "TURN:" << turnCount << std::endl;
		needNewStatus = true;
	}
	std::vector<std::pair<pSCell, std::vector<pSCell>>> playableAttackable;
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
	std::vector<unsigned int> cellsAlone{};

	//determination si faut aller en status startgame, a chaque tour interne
	auto all_cluster = All_cluster(Id);
	auto result = std::min_element(all_cluster.begin(), all_cluster.end(), [](std::vector<unsigned int>& a, std::vector<unsigned int>& b) {return a.size() < b.size(); });
	if ((*result).size() == 1) {//si il y a au moins un cluster de taille, donc une case toutes seul
		status = Status::startgame;
	}
	else { needNewStatus = true; }
	if (needNewStatus) {//si on est a un nouveau vrai tour, on change de strat
		//determination nouvelle strat
		// stratégie endgame se déclenche lorque le gain de dés est supérieur a tout ceux des adversaires
		// condition mode end game : avoir plus de case que les autres joueurs
		unsigned int index = 0;

		//		for (unsigned int i = 0; i < Map.nbCells; ++i) {
		//			if (Map.cells[i].infos.owner == Id) {
		//				bool is_group = false;//la cells fait partie dun cluster
		//				for (unsigned int j = 0; j < Map.cells[i].nbNeighbors; ++j) {
		//					if (Map.cells[i].neighbors[j]->infos.owner == Id) {//elle a au moins un voisin a nous
		//						is_group = true;
		//						break;
		//					}
		//				}
		//				if (!is_group) { 
		//					cellsAlone.push_back(i);
		//					break; 
		//				}
		//			}
		//		}
		for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {
			//outputLog << "Id:" << i << " points:" << points[i] <<std::endl;
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
	else { innerGameTurn++; }//si cest pas un nouveau tour on augmente le compteur de tour a linterieur des tour


	if (!InitTurn(playableAttackable)) { // initialise la variable playableAttackable et return fasle si ya pas de case jouable
		return false;
	}

	//if (playableAttackable.size() > 1) {
	//	StrategyAdvanced::informations info(playableAttackable[0].first->infos.id, playableAttackable[1].first->infos.id, Map);
	//	StrategyAdvanced::informations info2 = Pathfinding(info);
	//	outputLog << info2 << std::endl;
	//	for (unsigned int j = 0; j < Map.cells[info2.path.back()].nbNeighbors; ++j) {
	//		outputLog << Map.cells[info2.path.back()].neighbors[j]->infos.id << "||";
	//	}
	//	outputLog << playableAttackable[0].first->infos.id << "|" << playableAttackable[1].first->infos.id << std::endl;
	//}

	// determiner le mode
	// appeler la fonction mode correspondante
	switch (status)
	{
	case StrategyAdvanced::Status::startgame:
		return Startgame(turn, playableAttackable);
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

StrategyAdvanced::informations StrategyAdvanced::Pathfinding(StrategyAdvanced::informations informations)
{
	// les id des cases de départ et d'arrivée sont les mêmes : arret de la récursion
	unsigned int min = 1000000;
	//StrategyAdvanced::informations returninfo=informations;

	if (informations.depart->infos.id == informations.arrive->infos.id) {
		if (Id != informations.depart->infos.owner) {
			informations.nb_dices += informations.depart->infos.nbDices;
		}
		informations.path.push_back(informations.arrive->infos.id);
		return informations;
	}
	else {
		// mise a jour des informations
		if (informations.depart->infos.owner) {
			informations.nb_dices += informations.depart->infos.nbDices;
		}
		informations.path.push_back(informations.depart->infos.id);

		// aller plus loin dans le chemin : trouver les voisins non visités
		for (unsigned int i = 0; i < informations.depart->nbNeighbors; i++) {
			auto neighbor = informations.depart->neighbors[i];
			if (neighbor->infos.owner != Id || neighbor->infos.id == informations.arrive->infos.id) {
				// la case ne nous appartient pas ou la case est la case d'arrivée
				auto p = std::find(informations.path.begin(), informations.path.end(), neighbor->infos.id);
				if (p == informations.path.end()) {
					 // on a jamais visité cette case
					 // return nb min de dés avec les appels de fonctions de tous les voisins
					informations.depart = neighbor; // nouvelle case de départ
					auto info = Pathfinding(informations);
					if (info.nb_dices < min) { // nb de dés plus petit
						//returninfo = info;
						informations = info;
					}
				}
			}
		}
		return informations;
	}
}

std::vector<std::vector<unsigned int>> StrategyAdvanced::All_cluster(unsigned int owner)
{
	// retourne un vecteur contenant les differents cluster du joueur avec l'Id owner
	// les différents clusteer sont contenus dans des vecteurs
	// les cells sont représentés par leur id

	std::vector<std::vector<unsigned int>> all_cluster = {};

	// on cherche les cells qui appartiennent a owner
	for (unsigned int i = 0; i < Map.nbCells; i++) {
		if (Map.cells[i].infos.owner == owner) {
			
			if (all_cluster.empty()) { // premier tour de boucle
				all_cluster.push_back(Cluster(Map.cells[i].infos.id)); // on met le cluster appartenant a la première cell trouvé
			}
			else {
				// on vérifie que la cell n'est pas déja présente dans un cluster que l'on a déja identifié
				bool already_seen = false;
				for (auto it = all_cluster.begin(); it != all_cluster.end(); it++) { // parcours des différents cluster enregistré
					auto p = std::find(it->begin(), it->end(), Map.cells[i].infos.id);
					if (p != it->end()) {
						already_seen = true;
						break;
					}
				}
				if (!already_seen) {
					all_cluster.push_back(Cluster(Map.cells[i].infos.id));
				}
			}
		}
	}

	return all_cluster;
}

std::vector<unsigned int> StrategyAdvanced::Cluster(unsigned int idcell)
{
	// calcul de toutes les cases présente dans un cluster à partir de l'Id d'une cell à nous
	
	// on trouve la cell dans la map
	pSCell currentcell = &Map.cells[idcell];
	unsigned int ownercell = Id;
	if (Id != currentcell->infos.owner) {
		outputLog << "fct cluster, attention cette cell n'est pas à vous" << std::endl;
		ownercell = currentcell->infos.owner;
	}
	// tant que currentcell a des voisins du meme owner
	unsigned int i = 0;
	std::vector<unsigned int> cluster = {idcell};
	std::vector<pSCell> non_visited_cells = {};

	//premiere itération
	for (unsigned int i = 0; i < currentcell->nbNeighbors; i++) { // parcours des voisins
		if (currentcell->neighbors[i]->infos.owner == ownercell) { // selections des voisins avec le meme owner
			cluster.push_back(currentcell->neighbors[i]->infos.id);
			non_visited_cells.push_back(currentcell->neighbors[i]);
		}
	}
	// tant qu'on a pas  visité toutes les cells du cluster
	while (!non_visited_cells.empty()) {
		currentcell = non_visited_cells[0];
		for (unsigned int i = 0; i < currentcell->nbNeighbors; i++) { // parcours des voisins
			if (currentcell->neighbors[i]->infos.owner == ownercell) { // selections des voisins avec le meme owner
				auto p = std::find(cluster.begin(), cluster.end(), currentcell->neighbors[i]->infos.id); 
				if (p == cluster.end()) { //verif pas deja ajouté
					cluster.push_back(currentcell->neighbors[i]->infos.id);
					non_visited_cells.push_back(currentcell->neighbors[i]);
				}
			}
		}
		non_visited_cells.erase(non_visited_cells.begin());// on enlève la case visité 
	}
	return cluster;
}


bool StrategyAdvanced::Startgame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable)
{
	// rassembler les dés dans un coin de map
	//faire cette stratégie tant que tous les dés ne sont pas réuni

	// trouver la case avec le moins de voisin la plus proche

	// diriger les dés vers cette case

	auto all_cluster = All_cluster(Id);
	auto biggest_cluster = std::max_element(all_cluster.begin(), all_cluster.end(), [](std::vector<unsigned int>& a, std::vector<unsigned int>& b) {return a.size() < b.size(); });

	std::vector<unsigned int> cells_joinable{};//contient toutes les cases que lon doit essayer de rejoindre
	for (auto& itcells : *biggest_cluster) {
		// trouver les cases qui ne sont entouré par des cells d'autre joueurs
		auto find = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](std::pair<pSCell, std::vector<pSCell>>& i) {return i.first->infos.id == itcells; });
		if (find != playableAttackable.end()) {//le itcells est jouable/en bordure de cluster
			cells_joinable.push_back(find->first->infos.id);
		}
	}

	std::vector<informations> best_paths;
	for (auto itcluster = all_cluster.begin(); itcluster != all_cluster.end(); ++itcluster) {
		if (itcluster != biggest_cluster) {//
			for (auto& itcells : (*itcluster)) {//on parcours les ids des cells de tous les cluster sauf le plus gros
				auto find = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](std::pair<pSCell, std::vector<pSCell>>& i) {return i.first->infos.id == itcells; });
				if (find != playableAttackable.end()) {//le itcells est jouable
					informations best_path{ 999 };//best_path contient un chemin demandant 999des (pire scenario)
					for (auto& itjoinable : cells_joinable) {//on parcours toutes les cases que lon a pour objectif
						auto path = Pathfinding(informations(itcells, itjoinable, Map));
						if (path.nb_dices < best_path.nb_dices) {
							best_path = path;
						}
					}
					if (best_path.nb_dices == 0) {
						outputLog << "bug";
					}
					best_paths.push_back(best_path);
				}
			}
		}
	}
	if (best_paths.empty()) {
		outputLog << Id << ": strat startgame on joue pas" << std::endl;
		return false;
	}
	auto final_path = std::min_element(best_paths.begin(), best_paths.end(), [](informations& a, informations& b) {return a.nb_dices < b.nb_dices; });//trouve le chemin le plus simple a faire
	turn->cellFrom = (*final_path).path[0];
	turn->cellTo = (*final_path).path[1];
	outputLog << Id<<": strat startgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo ;
	return true;
}

bool StrategyAdvanced::Middlegame(STurn* turn,std::vector<std::pair<pSCell, std::vector<pSCell>>> &playableAttackable)
{
	// prendre les cases facile 
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

	int future_dice_supply = diceStock[Id] + points[Id]; // nb de dés donné à la fin du tour 
	//la variable sera diceStock[Id] + points[Id]
	int missing_dices = 0; // nb de dés manquant pour être plein de 8 dés
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (Map.cells[i].infos.owner == Id) {
			//future_dice_supply += 1;
			missing_dices += (8 - Map.cells[i].infos.nbDices);
		}
	}
	if (future_dice_supply - missing_dices < 7) {//7 car meme si on pert le duel on est sur davoir de quoi revenir a 8 partout
		outputLog << Id << ": strat endgame on joue pas" << ", future_dice_supply:" << future_dice_supply << ", =prof:" << diceStock[Id]+points[Id] << " missing dices:" << missing_dices << " diceStock:" << diceStock[Id];
		return false;
	}

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
	outputLog << Id<<": strat endgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo ;
	outputLog << ", future_dice_supply:" << future_dice_supply << " missing dices:" << missing_dices << " diceStock:" << diceStock[Id] <<std::endl;
	return true;
}

StrategyAdvanced::~StrategyAdvanced()
{
	outputLog.close();
}

StrategyAdvanced::informations::informations(unsigned int iddepart, unsigned int idarrive, SMap& Map)
{
	depart = &Map.cells[iddepart];
	arrive = &Map.cells[idarrive];
	nb_dices = 0;
}

std::ostream & operator<<(std::ostream& o, StrategyAdvanced::informations info)
{
	o << "nb_dices : " << info.nb_dices <<"\npath : "<< std::endl;
	unsigned int i = 0;
	for (auto it = info.path.begin(); it != info.path.end(); it++) {
		i++;
		o << "cell numero : " << i << " id de la cell : " << *it << std::endl;
	}
	return o;
}
