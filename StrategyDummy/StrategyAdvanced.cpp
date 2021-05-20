#include "StrategyAdvanced.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

StrategyAdvanced::StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map)
{
	std::cout << "ADVANCED" << std::endl;
	outputLog.open("log.txt");
	if (outputLog.is_open())
	{
		outputLog << "log strategy " << __TIME__ << "\n";
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
	if ((*result).size() == 1 && all_cluster.size() != 1) {//si il y a au moins un cluster de taille 1, donc une case toutes seul
		//il faut aussi quil y ait plus dun seul cluster
		status = Status::startgame;
		needNewStatus = false;
	}
	else { needNewStatus = true; }
	if (needNewStatus) {//si on est a un nouveau vrai tour, on change de strat
		//determination nouvelle strat
		// stratégie endgame se déclenche lorque le gain de dés est supérieur a tout ceux des adversaires
		// condition mode end game : avoir plus de case que les autres joueurs
		unsigned int index = 0;

		for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {
			//outputLog << "Id:" << i << " points:" << points[i] <<std::endl;
			if (points[i] > points[index]) {
				index = i;
			}
		}

		if (index == Id || diceStock[Id] > 7) { // strategie endgame si on a le joueur avec le plus de points, ou si on a plus de 7 stock en des
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

bool StrategyAdvanced::InitTurn(std::vector<std::pair<pSCell, std::vector<pSCell>>>& playableAttackable)//init la variable playableAttackable
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
{
	for (unsigned i = 0; i < Map.nbCells; ++i) {//permet de trouver tous les cases jouables
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices > 1) {
			for (unsigned j = 0; j < Map.cells[i].nbNeighbors; ++j) {//au moins une case enemi
				if (Map.cells[i].neighbors[j]->infos.owner != Id) {
					//auto attackable = new std::vector<pSCell>;
					std::vector<pSCell> attackable;
					playableAttackable.push_back({ &Map.cells[i],attackable });//on rajoute ladresse des cells qui sont jouables
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
		outputLog << Id << ": end of turn : no attackable cell" << std::endl;
		return false;//si aucune case jouable, on finit le tour
	}
	return true;
}

StrategyAdvanced::informations StrategyAdvanced::Pathfinding(StrategyAdvanced::informations informations)
{
	// les id des cases de départ et d'arrivée sont les mêmes : arret de la récursion
	unsigned int min = 1000;
	//StrategyAdvanced::informations returninfo=informations;

	if (informations.depart->infos.id == informations.arrive->infos.id) {
		if (Id != informations.depart->infos.owner) {//si cest pas notre case
			informations.nb_dices += informations.depart->infos.nbDices;
		}
		informations.path.push_back(informations.depart->infos.id);
		informations.effective_path = true;
		return informations;
	}
	else if (informations.nb_dices > 10) {
		informations.effective_path = false;
		return informations;
	}
	else {
		// mise a jour des informations
		if (informations.depart->infos.owner != Id) {//si cest pas notre case on rajoute le nb de des dans le compteur de des
			informations.nb_dices += informations.depart->infos.nbDices;
		}
		informations.path.push_back(informations.depart->infos.id);

		// aller plus loin dans le chemin : trouver les voisins non visités
		auto departure = informations.depart;
		auto bestinfo = informations;
		for (unsigned int i = 0; i < departure->nbNeighbors; i++) {
			auto neighbor = departure->neighbors[i];
			if (neighbor->infos.owner != Id || neighbor->infos.id == informations.arrive->infos.id) {
				// la case ne nous appartient pas ou la case est la case d'arrivée
				auto p = std::find(informations.path.begin(), informations.path.end(), neighbor->infos.id);
				if (p == informations.path.end()) {
					// on a jamais visité cette case
					// return nb min de dés avec les appels de fonctions de tous les voisins
					informations.depart = neighbor; // nouvelle case de départ
					auto info = Pathfinding(informations);
					if (info.nb_dices < min && info.effective_path) { // nb de dés plus petit
						bestinfo = info;
						bestinfo.depart = neighbor;
						min = info.nb_dices;
					}
				}
			}
		}
		if (min == 1000) {
			informations.effective_path = false;
		}
		else {
			informations = bestinfo;
		}
		return informations;
	}
}

StrategyAdvanced::informations StrategyAdvanced::Pathfindingprim(unsigned int iddepart, unsigned int idarrive)
{
	std::vector<unsigned int> in_range_cell = {}; // les cells qu'on peut ajouter
	std::vector<unsigned int> added_cells = { iddepart }; // les cells qu'on a ajouté (moins de dés a un moment x)
	std::vector<unsigned int> path = { idarrive }; 
	std::map<unsigned int, unsigned int> pred; // couple idcell , predecesseur cell
	std::map<unsigned int, unsigned int> distance_dice;
	
	// initialisation des variables
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (i != iddepart) {
			distance_dice[i] = 1000; // supposée infini
		}
		else {
			distance_dice[i] = 1001;
		}
	}

	pSCell startcell = &Map.cells[iddepart];
	for (unsigned int i = 0; i < startcell->nbNeighbors; ++i) {
		if (startcell->neighbors[i]->infos.owner != Id) {
			in_range_cell.push_back(startcell->neighbors[i]->infos.id);
			pred[startcell->neighbors[i]->infos.id] = iddepart;
			distance_dice[startcell->neighbors[i]->infos.id] = startcell->neighbors[i]->infos.nbDices;
		}
	}

	bool find_arrival = false;
	while (!find_arrival && !in_range_cell.empty()) {
		// choix du plus petit nb de dés : ajout de cette cellule
		std::pair<unsigned int, unsigned int> min_dice = *std::min_element(distance_dice.begin(), distance_dice.end(), [](const std::pair<unsigned int, unsigned int>& i, const std::pair<unsigned int, unsigned int>& j) {return (i.second < j.second);});
		//ajout de cette cell
		added_cells.push_back(min_dice.first);
		pSCell cell_added = &Map.cells[min_dice.first];
			
		// mise a jour des cell voisine
		for (unsigned int i = 0; i < cell_added->nbNeighbors; ++i) {
			if (cell_added->neighbors[i]->infos.owner != Id || cell_added->neighbors[i]->infos.id == idarrive) { // cell a l'adversaire ou arrivée
				auto neigh = cell_added->neighbors[i];
				auto already_added = std::find(added_cells.begin(), added_cells.end(), neigh->infos.id); 
				if (already_added == added_cells.end()) { // cell pas deja ajouté 
					auto already_in_range = std::find(in_range_cell.begin(), in_range_cell.end(), neigh->infos.id);
					if (already_in_range == in_range_cell.end()) { // nouvelle cell : nouvelle distance, nouveau pred, mettre dans in range cell
						distance_dice[neigh->infos.id] = min_dice.second + neigh->infos.nbDices;
						pred[neigh->infos.id] = cell_added->infos.id;
						in_range_cell.push_back(neigh->infos.id);
					}
					else {//ancienne cell : mise a jour pred et distance
						if (distance_dice[neigh->infos.id] > min_dice.second + neigh->infos.nbDices) { // la nouvelle distance est plus petite
							distance_dice[neigh->infos.id] = min_dice.second + neigh->infos.nbDices;
							pred[neigh->infos.id] = cell_added->infos.id;
						}
					}
				}
			}
		}
		// mise a jour distance cell added
		if (min_dice.first != idarrive) {
			distance_dice[min_dice.first] = 1001; // -1 pour valeur max car unsigned int
		}
		// mise a jour in_range_cell ( delete la cell qu'on vient d'ajouter )
		auto just_added = std::find(in_range_cell.begin(), in_range_cell.end(), min_dice.first);
		if (just_added == in_range_cell.end()) {
			outputLog << " c'est mal codé" << std::endl;
		}
		else {
			in_range_cell.erase(just_added);
		}

		// mise a jour variable find_arrival
		auto find_end = std::find(added_cells.begin(), added_cells.end(), idarrive);
		if (find_end != added_cells.end()) {
			find_arrival = true;
		}
	}

	informations retour = informations(iddepart, idarrive, Map);
	if (!find_arrival) { // on a pas réussi à atteindre la fin
		
		retour.effective_path = false;
		retour.nb_dices = 1000;
		retour.path = {};
	}
	else {
		// faire le path : remonter de id fin jusqu'a la cell initial avec pred
		while (path.back() != iddepart)
		{
			// trouver le predecesseur du dernier ajout du path
			path.push_back(pred[path.back()]);
		}

		std::reverse(path.begin(), path.end());
		retour.path = path;
		retour.effective_path = true;
		retour.nb_dices = distance_dice[idarrive];
	}
	return retour;
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
	std::vector<unsigned int> cluster = { idcell };
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


bool StrategyAdvanced::Startgame(STurn* turn, std::vector<std::pair<pSCell, std::vector<pSCell>>>& playableAttackable)
{
	// rassembler les dés dans un coin de map
	//faire cette stratégie tant que tous les dés ne sont pas réuni

	// trouver la case avec le moins de voisin la plus proche

	// diriger les dés vers cette case

	auto all_cluster = All_cluster(Id);
	auto biggest_cluster = std::max_element(all_cluster.begin(), all_cluster.end(), [](std::vector<unsigned int>& a, std::vector<unsigned int>& b) {return a.size() < b.size(); });

	std::vector<unsigned int> cells_joinable{};//contient toutes les cases que lon doit essayer de rejoindre
	for (auto& itcells : *biggest_cluster) {
		auto find = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](std::pair<pSCell, std::vector<pSCell>>& attackplay) {return attackplay.first->infos.id == itcells; });
		if (find != playableAttackable.end()) {//le itcells est jouable/en bordure de cluster
			cells_joinable.push_back(find->first->infos.id);
		}
	}

	std::vector<informations> best_paths;
	for (std::vector<std::vector<unsigned int>>::iterator itCluster = all_cluster.begin(); itCluster != all_cluster.end(); ++itCluster) {
		if (itCluster != biggest_cluster) {
			for (auto& itcells : (*itCluster)) {//on parcours les ids des cells de tous les cluster sauf le plus gros
				auto find = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](std::pair<pSCell, std::vector<pSCell>>& attackplay) {return attackplay.first->infos.id == itcells; });
				if (find != playableAttackable.end()) {//le itcells est jouable
					informations best_path{ 999 };//best_path contient un chemin demandant 999des (pire scenario)
					for (auto& itJoinable : cells_joinable) {//on parcours toutes les cases que lon a pour objectif
						//auto path = Pathfinding(informations(itcells, itJoinable, Map));
						auto path = Pathfindingprim(itcells, itJoinable);
						if (!path.effective_path) {
							break;
						}
						else if (path.nb_dices < best_path.nb_dices) {
							best_path = path;
						}
					}
					if (best_path.nb_dices != 0 && best_path.effective_path) {//on verifie que lon trouve bien un chemin 
						best_paths.push_back(best_path);
					}
				}
			}
		}
	}

	//determination du meilleur mouv a faire de tous les meilleurs mouvs que lon a trouver pour toutes les cases
	std::vector<informations> path_to_keep{};
	for (auto path = begin(best_paths); path != end(best_paths); ++path) {
		const int idFrom = path->path[0];
		const int idTo = path->path[1];

		const int idtolast = path->path[path->path.size() - 2];
		const int idfromlast = path->path.back();

		const int normal_cost = Map.cells[idFrom].infos.nbDices - Map.cells[idTo].infos.nbDices;
		const int reverse_cost = Map.cells[idfromlast].infos.nbDices - Map.cells[idtolast].infos.nbDices;

		if (normal_cost > 0 || reverse_cost > 0) {//si le mouv est pas trop risqué on le conserve
			if (normal_cost < reverse_cost) {//lequel est le mieux entre chemin normal et chemin en sens inverse
				path_to_keep.push_back(*path);
			}
			else {
				std::reverse(begin(path->path), end(path->path));
				path_to_keep.push_back(*path);
			}
		}
	}

	if (path_to_keep.empty()) {
		outputLog << Id << ": strat startgame on joue pas" << std::endl;
		//return false;
		return Middlegame(turn, playableAttackable);//si on peut pas faire de mouv bien avec startgame on appelle middlegame
	}
	else {
		auto final_path = std::min_element(path_to_keep.begin(), path_to_keep.end(), [](informations& a, informations& b) {return a.nb_dices < b.nb_dices; });//trouve le chemin le plus simple a faire
		turn->cellFrom = final_path->path[0];
		turn->cellTo = final_path->path[1];
		return true;
	}
}

bool StrategyAdvanced::Middlegame(STurn* turn, std::vector<std::pair<pSCell, std::vector<pSCell>>>& playableAttackable)
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
		outputLog << Id << ": strat midgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo << std::endl;
		return true;
	}
	// sinon attendre
	outputLog << Id << ": strat midgame on ne joue pas, pas d'avantage sur les opposants" << std::endl;
	return false;
}

bool StrategyAdvanced::Endgame(STurn* turn, std::vector<std::pair<pSCell, std::vector<pSCell>>>& playableAttackable)
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
	if (future_dice_supply - missing_dices < 8) {//7 car meme si on pert le duel on est sur davoir de quoi revenir a 8 partout
		outputLog << Id << ": strat endgame on joue pas" << ", future_dice_supply:" << future_dice_supply << ", =prof:" << diceStock[Id] + points[Id] << " missing dices:" << missing_dices << " diceStock:" << diceStock[Id];
		return false;
	}

	// on attaque si l'opposant la ou on a le plus de chance de gagner en priorité sinon on attaque un block de 8 dés
	pSCell bestCellP = playableAttackable.begin()->first; // meilleur cellule depuis laquelle on attaque
	pSCell bestCellA = playableAttackable.begin()->second.operator[](0); // meilleur cellule a attaquer
	int bestDiff = playableAttackable[0].first->infos.nbDices - playableAttackable[0].second.operator[](0)->infos.nbDices;// nb de dés de différence entre attaquant et attaqué
	int diff{};
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
	outputLog << Id << ": strat endgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo;
	outputLog << ", future_dice_supply:" << future_dice_supply << " missing dices:" << missing_dices << " diceStock:" << diceStock[Id] << std::endl;
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
	effective_path = true;
}

std::ostream& operator<<(std::ostream& o, StrategyAdvanced::informations info)
{
	o << "nb_dices : " << info.nb_dices << "\npath : " << std::endl;
	unsigned int i = 0;
	for (auto it = info.path.begin(); it != info.path.end(); it++) {
		i++;
		o << "cell numero : " << i << " id de la cell : " << *it << std::endl;
	}
	return o;
}
