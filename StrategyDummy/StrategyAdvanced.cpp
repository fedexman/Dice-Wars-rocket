#include "StrategyAdvanced.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

StrategyAdvanced::StrategyAdvanced(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map)
{
	// enlever le outputlog quand tout sera fini
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
	//on acutalise les nouvelles infos du tour : map , points , dicestock
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		Map.cells[i].infos = state->cells[i];
	}
	for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {
		points[i] = state->points[i];
		diceStock[i] = state->diceStock[i];
	}

	// choix de la stratégie
	bool needNewStatus = false; //permet de savoir quand il faut calculer si on a besoin d'une nouvelle strat.
	
	if (gameTurn != turnCount) {//si on est a une nouveau tour de jeu, on change de strat
		innerGameTurn = 0;//gameTurn a linterieur dun tour
		turnCount = gameTurn;
		outputLog << std::endl << "TURN:" << turnCount << std::endl;
		needNewStatus = true;
	}

	std::vector<std::pair<pSCell, std::vector<pSCell>>> playableAttackable;
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case

	//determination si faut aller en status startgame, a chaque tour interne
	auto all_cluster = All_cluster(Id);
	
	// si il y a plusieurs cluster on active le startgame
	if (all_cluster.size() != 1) {
		status = Status::startgame;
		needNewStatus = false;
	}
	else { needNewStatus = true; }

	//determination des autres startégies
	if (needNewStatus) {
		status = choose_status();
	}
	else { innerGameTurn++; }//si cest pas un nouveau tour on augmente le compteur de tour a linterieur des tour


	if (!InitTurn(playableAttackable)) { // initialise la variable playableAttackable et return fasle si ya pas de case jouable
		return false;
	}

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
		throw("erreur appel de fonction status");
		break;
	}
}

bool StrategyAdvanced::InitTurn(std::vector<std::pair<pSCell, std::vector<pSCell>>>& playableAttackable)
{
	//init la variable playableAttackable
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case

	//trouver tous les cases jouables
	for (unsigned i = 0; i < Map.nbCells; ++i) {
		if (Map.cells[i].infos.owner == Id && Map.cells[i].infos.nbDices > 1) { // cell a nous et + de 1 dé
			for (unsigned j = 0; j < Map.cells[i].nbNeighbors; ++j) { //au moins 1 case ennemi dans les voisins
				if (Map.cells[i].neighbors[j]->infos.owner != Id) {
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
	//trouver toutes les cases attackable depuis nos cases playable
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

StrategyAdvanced::informations StrategyAdvanced::Pathfindingprim(unsigned int iddepart, unsigned int idarrive)
{
	// pathfinding mis en place a partir de l'algorithme de prim
	// a chaque passage on ajoute la cellule avec le moins de dés sur le chemin depuis la case de depart
	// on reitere jusqu'a trouver l'arrivée

	std::vector<unsigned int> in_range_cell = {}; // les cells qu'on peut ajouter
	std::vector<unsigned int> added_cells = { iddepart }; // les cells qu'on a ajouté ( elles avaient moins de dés a un moment x)
	std::vector<unsigned int> path = { idarrive };
	std::map<unsigned int, unsigned int> pred; // couple idcell , predecesseur cell , le predecesseur est la cell depuis laquel on ajoute la prochaine
	std::map<unsigned int, unsigned int> distance_dice; // couple idcell , nb de dés pour arriver a la cell

	// initialisation des variables
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (i != iddepart) {
			distance_dice[i] = 1000; // supposée infini
		}
		else {
			distance_dice[i] = 1001; // case de départ ne doit pas être ajoutée
		}
	}

	pSCell startcell = &Map.cells[iddepart]; //cell de départ
	for (unsigned int i = 0; i < startcell->nbNeighbors; ++i) {
		if (startcell->neighbors[i]->infos.owner != Id) {
			in_range_cell.push_back(startcell->neighbors[i]->infos.id);
			pred[startcell->neighbors[i]->infos.id] = iddepart;
			distance_dice[startcell->neighbors[i]->infos.id] = startcell->neighbors[i]->infos.nbDices;
		}
	}

	// commencement du chemin
	bool find_arrival = false;
	while (!find_arrival && !in_range_cell.empty()) { // tant qu'on a pas trouver l'arrivé et qu'il reste des cellules que l'on peut ajouter
		// choix du plus petit nb de dés : ajout de cette cellule
		std::pair<unsigned int, unsigned int> min_dice = *std::min_element(distance_dice.begin(), distance_dice.end(), [](const std::pair<unsigned int, unsigned int>& i, const std::pair<unsigned int, unsigned int>& j) {return (i.second < j.second); });
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
			distance_dice[min_dice.first] = 1001; // on ne veut plus que cette cellule soit selectionnable
		}
		else {
			if (cell_added->infos.owner == Id) {
				distance_dice[min_dice.first] -= cell_added->infos.nbDices; // on retire les dés de l'arrivée si ils nous appartiennent
			}
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

	informations retour = informations(iddepart, idarrive, Map); // valeur de retour

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
	// les différents cluster sont contenus dans des vecteurs
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
				for (auto cluster = all_cluster.begin(); cluster != all_cluster.end(); cluster++) { // parcours des différents cluster enregistré
					auto p = std::find(cluster->begin(), cluster->end(), Map.cells[i].infos.id);
					if (p != cluster->end()) { // on a deja vu cette cell pas la peine d'aller plus loin
						already_seen = true;
						break;
					}
				}
				if (!already_seen) { // on a jamais vu cette cell, on ajoute son cluster
					all_cluster.push_back(Cluster(Map.cells[i].infos.id));
				}
			}
		}
	}
	return all_cluster;
}

std::vector<unsigned int> StrategyAdvanced::Cluster(unsigned int idcell)
{
	// calcul de toutes les cases présente dans un cluster à partir de l'Id d'une cell

	// on trouve la cell dans la map
	pSCell currentcell = &Map.cells[idcell];
	unsigned int ownercell = Id;
	if (Id != currentcell->infos.owner) {
		outputLog << "fct cluster, attention cette cell n'est pas à vous" << std::endl;
		ownercell = currentcell->infos.owner;
	}
	// tant que currentcell a des voisins du meme owner
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
	//faire cette stratégie tant que tous les dés ne sont pas réunis
	//trouver le plus gros cluster
	//diriger les dés vers se cluster

	auto all_cluster = All_cluster(Id);//renvoie tous les cluster
	auto biggest_cluster = all_cluster.begin();//on initialize le max au premier 
	for (auto itCluster = all_cluster.begin(); itCluster != all_cluster.end(); ++itCluster) {// on cherche le plus gros cluster
		if (itCluster->size() > biggest_cluster->size()) { // cluster plus gros
			biggest_cluster = itCluster; 
		}
		else if (itCluster->size() == biggest_cluster->size()) {// cluster de meme taille
			unsigned int nbNeighA = 0;//compte les neighs de chaqun
			unsigned int nbNeighB = 0;
			for (unsigned int id = 0; id < biggest_cluster->size(); ++id) {
				//on regarde donc lequel a le moins de voisin pour essayer daller en bord de map
				//bord de map toujours avantageux
				nbNeighA += Map.cells[itCluster->operator[](id)].nbNeighbors;
				nbNeighB += Map.cells[biggest_cluster->operator[](id)].nbNeighbors;
			}
			if (nbNeighA < nbNeighB) { //si il a le moins de voisin cest le nouveau biggest_cluster
				biggest_cluster = itCluster; 
			}
		}
	}
	//ancienne version pour trouver le biggest_cluster
	//auto biggest_cluster = std::max_element(all_cluster.begin(), all_cluster.end(), [](std::vector<unsigned int>& a, std::vector<unsigned int>& b) {return a.size() < b.size(); });

	std::vector<unsigned int> cells_joinable{};//contient toutes les cases que lon doit essayer de rejoindre du plus gros cluster
	for (auto& itcells : *biggest_cluster) {
		auto find = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](const std::pair<pSCell, std::vector<pSCell>>& attackplay) {return attackplay.first->infos.id == itcells; });
		if (find != playableAttackable.end()) {//le itcells est jouable/en bordure de cluster
			cells_joinable.push_back(find->first->infos.id);
		}
	}

	// trouver tous les meilleurs chemins entre les autres cluster et les cells joinable
	std::vector<informations> best_paths;
	for (auto itCluster = all_cluster.begin(); itCluster != all_cluster.end(); ++itCluster) {
		if (itCluster != biggest_cluster) {
			for (auto& itcells : (*itCluster)) {//on parcours les ids des cells de tous les cluster sauf le plus gros
				auto find = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](const std::pair<pSCell, std::vector<pSCell>>& attackplay) {return attackplay.first->infos.id == itcells; });
				if (find != playableAttackable.end()) {//le itcells est jouable
					informations best_path{ 999 };//best_path contient un chemin demandant 999 des (pire scenario), supposé infini
					for (auto& itJoinable : cells_joinable) {//on parcours toutes les cases que l on a pour objectif
						//auto path = Pathfinding(informations(itcells, itJoinable, Map)); //recursif
						auto path = Pathfindingprim(itcells, itJoinable); // iteratif
						if (!path.effective_path) { // le path relie le départ et l'arrivée
							break;
						}
						else if (path.nb_dices < best_path.nb_dices) { // la path est meilleur que l'ancien
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

	//chemin optimal parmi les meilleurs chemins
	std::vector<informations> path_to_keep{};
	for (auto path = begin(best_paths); path != end(best_paths); ++path) {
		const int idFrom = path->path[0];
		const int idTo = path->path[1];

		const int idtolast = path->path[path->path.size() - 2];
		const int idfromlast = path->path.back();

		const int normal_advantage = Map.cells[idFrom].infos.nbDices - Map.cells[idTo].infos.nbDices;
		const int reverse_advantage = Map.cells[idfromlast].infos.nbDices - Map.cells[idtolast].infos.nbDices;

		if (normal_advantage > 0 || reverse_advantage > 0) {//si le mouv est avantageux on le conserve
			if (normal_advantage > reverse_advantage && normal_advantage > 0) {//lequel est le mieux entre chemin normal et chemin en sens inverse
				path_to_keep.push_back(*path);
			}
			else {
				std::reverse(begin(path->path), end(path->path));
				path_to_keep.push_back(*path);
			}
		}
	}

	if (path_to_keep.empty()) { 
		// on ne peux pas rejoindre les cluster
		// donc on étend le cluster le plus gros

		// suppression des cases jouables par les petits cluster
		for (std::vector<std::vector<unsigned int>>::iterator itCluster = all_cluster.begin(); itCluster != all_cluster.end(); ++itCluster) {
			if (itCluster != biggest_cluster) {
				for (auto& itcells : (*itCluster)) {//on parcours les ids des cells de tous les cluster sauf le plus gros
					auto p = std::find_if(playableAttackable.begin(), playableAttackable.end(), [itcells](const std::pair<pSCell, std::vector<pSCell>>& attackplay) {return attackplay.first->infos.id == itcells; });
					if (p != playableAttackable.end()) {
						playableAttackable.erase(p);
					}
				}
			}
		}
		// si il nous reste des coups a jouer
		if (!playableAttackable.empty()) {
			// on passe le relais a une autre stratégie
			outputLog << Id << ": strat startgame on joue pas, appel middlegame" << std::endl;
			//appel fonction selection strat
			status = choose_status();
			switch (status)
			{
			case StrategyAdvanced::Status::startgame://cas impossible normalement
				return Middlegame(turn, playableAttackable);
				break;
			case StrategyAdvanced::Status::middlegame:
				return Middlegame(turn, playableAttackable);
				break;
			case StrategyAdvanced::Status::endgame:
				return Endgame(turn, playableAttackable);
				break;
			default:
				return false;
				break;
			}
		}
		else { // aucun coup
			outputLog << Id << ": strat startgame on joue pas" << std::endl;
			return false;
		}
	}
	else { // un chemin pour rejoindre les cluster existe
		// selection du chemin le moins couteux
		auto final_path = std::min_element(path_to_keep.begin(), path_to_keep.end(), [](const informations& a, const informations& b) {return a.nb_dices < b.nb_dices; });
		turn->cellFrom = final_path->path[0];
		turn->cellTo = final_path->path[1];
		outputLog << Id << ": strat stratgame on joue, id attaquant " << turn->cellFrom << "id defense : " << turn->cellTo << std::endl;
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
				bestCellP = it.first;
				bestCellA = itAttack;
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

	// attaquer seulement avec un stock de 8 dés ou plus a la fin du tour

	int future_dice_supply = diceStock[Id] + points[Id]; // nb de dés donné à la fin du tour 
	int missing_dices = 0; // nb de dés manquant pour être plein de 8 dés
	for (unsigned int i = 0; i < Map.nbCells; ++i) {
		if (Map.cells[i].infos.owner == Id) {
			missing_dices += (8 - Map.cells[i].infos.nbDices);
		}
	}
	if (future_dice_supply - missing_dices < 8) {//8 car meme si on pert le duel on est sur davoir de quoi revenir a 8 partout
		outputLog << Id << ": strat endgame on joue pas" << ", future_dice_supply:" << future_dice_supply << " missing dices : " << missing_dices << " diceStock:" << diceStock[Id];
		return false;
	}

	// on attaque si l'opposant la ou on a le plus de chance de gagner en priorité sinon on attaque un block de 8 dés
	pSCell bestCellP = playableAttackable.begin()->first; // meilleur cellule depuis laquelle on attaque
	pSCell bestCellA = playableAttackable.begin()->second.operator[](0); // meilleur cellule a attaquer
	int bestDiff = playableAttackable[0].first->infos.nbDices - playableAttackable[0].second.operator[](0)->infos.nbDices;// nb de dés de différence entre attaquant et attaqué
	for (auto& it : playableAttackable) {//parcours tous les cases attackables pour chaque cases playable
		for (auto& itAttack : it.second) {
			int diff = it.first->infos.nbDices - itAttack->infos.nbDices;
			if (diff > bestDiff) {//trouve la case avec le plus grand ecart de des avec son adversere
				bestDiff = diff;
				bestCellP = it.first;
				bestCellA = itAttack;
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
	// pas de new toutes les variables sont détruites automatiquement
	outputLog.close();
}

StrategyAdvanced::Status StrategyAdvanced::choose_status()
{
	//determination nouvelle strat
	// stratégie endgame se déclenche lorque le gain de dés est supérieur a tout ceux des adversaires ou dicestock >8

	unsigned int index = 0;

	for (unsigned int i = 0; i < 8 && i < NbPlayer; i++) {
		//outputLog << "Id:" << i << " points:" << points[i] <<std::endl;
		if (points[i] > points[index]) {
			index = i;
		}
	}

	if (index == Id || diceStock[Id] > 8) { // strategie endgame si on a le joueur avec le plus de points, ou si on a plus de 8 stock en des
		status = Status::endgame;
	}
	else { //stratégie midgame
		status = Status::middlegame;
	}

	return status;
}

StrategyAdvanced::informations::informations(unsigned int iddepart, unsigned int idarrive, SMap& Map)
{
	// constructeur informations
	depart = &Map.cells[iddepart];
	arrive = &Map.cells[idarrive];
	nb_dices = 0;
	effective_path = true;
}

std::ostream& operator<<(std::ostream& o, StrategyAdvanced::informations info)
{
	//affichage information
	o << "nb_dices : " << info.nb_dices << "\npath : " << std::endl;
	unsigned int i = 0;
	for (auto it = info.path.begin(); it != info.path.end(); it++) {
		i++;
		o << "cell numero : " << i << " id de la cell : " << *it << std::endl;
	}
	return o;
}
