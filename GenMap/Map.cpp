#include "Map.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

SRegions* Map::GenerateMap(unsigned int& r, unsigned int& c)
{
    // fonction principal
    // faire les regions
    Regions regions = MakeAllRegionsv2(30);
    unsigned int nbR, nbC;
    // convertir en map
    SRegions* sregions = ConvertMap(regions, nbR, nbC);
    r = nbR;
    c = nbC;

    Maps.insert(sregions);

    return(sregions);
}

void Map::DeleteMap(SRegions* regions)
{
    // supprimer la map
    for (unsigned int i = 0; i < regions->nbRegions; ++i)
	{
		delete[](regions->region[i].cells);
	}
	delete[] regions->region;
	delete regions;
}

vector_cell Map::GenerateAllCell(int row, int col)
{
    // generer les cells de la grille
    // mettre toutes les cells dans un vecteur
    vector_cell all_cell;

    for (unsigned int r = 0; r < row; ++r) {
        for (unsigned int c = 0; c < col; ++c) {
            all_cell.push_back(std::make_pair(r, c));
        }
    }
    return all_cell;
}

void Map::DeleteRandomCells(vector_cell& non_used_cells,unsigned int bord)
{
    // supprimer des cells aleatoire pour ne pas remplir toute la map
    // faire attention que les bords ne se rejoingne pas
    std::srand(std::time(nullptr));

    // initialisation des bordure
    vector_cell bordure;
    for (unsigned int i = 0; i < bord; ++i) {
        for (unsigned int j = 0; j < bord; ++j) {
            if (i == 0 || i == bord-1 || j == 0 || j == bord-1)
                bordure.push_back(std::make_pair(i, j));
        }
    }
    std::vector<vector_cell> hole_in_map; // enregistre toutes les cells de trou dans la map au cas ou ils rejoigne un bord ou un autre trou
    vector_cell cell_deleted; //cell enlevées du groupement en cours
    vector_cell neigh_cell; // voisin des cells delete en cours
    

    unsigned int cpt = 0; // tour de while

    vector_cell neighbors; // voisin de la cell courante

    while (cpt < bord/2) {
        // choix de la premiere cell a enlever
        unsigned int rand_cell = std::rand() % non_used_cells.size();
        auto chosen_cell = non_used_cells.begin() + rand_cell;

        unsigned int nb_cell_delete = std::rand() % 6 + bord/4; // 8 a 13
        
        //reinitialisation des variables
        cell_deleted = {(*chosen_cell)}; 
        neigh_cell = {}; 
        std::pair<unsigned int, unsigned int> cell_bord = { -1,-1 }; // représente la première cell qui touche le bord dans la suppression

        // verifier que la cell choisie n'est pas sur le bord
        auto find_bord = std::find(bordure.begin(), bordure.end(), (*chosen_cell));
        if (find_bord != bordure.end()) { //la cell fait partie de la bordure
            cell_bord = (*chosen_cell);
        }

        // premier tour
        unsigned int row = chosen_cell->first;
        unsigned int col = chosen_cell->second;
        if ((row % 2) == 0) { // c'est une ligne paire  
            neighbors = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
        }
        else { // c'est une ligne impaire
            neighbors = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
        }
        // mise a jour voisins
        for (auto it : neighbors) {
            auto find_used = std::find(non_used_cells.begin(), non_used_cells.end(), it);
            if (find_used != non_used_cells.end()) { //n'est pas utilisé
                auto find_neigh = std::find(neigh_cell.begin(), neigh_cell.end(), it);
                if (find_neigh == neigh_cell.end()) { // n'est pas déjà dans les voisins
                    neigh_cell.push_back(it); // ajouté au voisins
                }
            }
        }

        while (cell_deleted.size() < nb_cell_delete && !neigh_cell.empty()) {
            
            // choix aleatoire dans les voisins
            rand_cell = std::rand() % neigh_cell.size();
            chosen_cell = neigh_cell.begin() + rand_cell;
            
            // ajout dans cell_deleted
            cell_deleted.push_back((*chosen_cell));
            find_bord = std::find(bordure.begin(), bordure.end(), (*chosen_cell));
            if (find_bord != bordure.end()) { //la cell fait partie de la bordure
                cell_bord = (*chosen_cell);
            }

            row = chosen_cell->first;
            col = chosen_cell->second;
            if ((row % 2) == 0) { // c'est une ligne paire  
                neighbors = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
            }
            else { // c'est une ligne impaire
                neighbors = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
            }
            // mise a jour voisins
            for (auto it : neighbors) {
                auto find_used = std::find(non_used_cells.begin(), non_used_cells.end(), it);
                if (find_used != non_used_cells.end()) { //n'est pas utilisé
                    auto find_neigh = std::find(neigh_cell.begin(), neigh_cell.end(), it);
                    if (find_neigh == neigh_cell.end()) { // n'est pas déjà dans les voisins
                        if (cell_bord.first != -1) { // déjà une cell en bordure
                            // verifier que les voisins ne sont pas en bordure
                            auto find = std::find(bordure.begin(), bordure.end(), it);
                            if (find == bordure.end()) {// le voisin n'est pas sur le bord
                                neigh_cell.push_back(it); // ajouté au voisins
                            }
                        }
                        else {// pas de bordure dans le trou 
                            neigh_cell.push_back(it); // ajouté au voisins
                        }
                    }

                }
            }
            // suppression de la cell choisi
            neigh_cell.erase(neigh_cell.begin() + rand_cell);
        }

        // mise a jour non_used cells
        for (auto it : cell_deleted) {
            auto find_used = std::find(non_used_cells.begin(), non_used_cells.end(), it);
            if (find_used != non_used_cells.end()) {
                non_used_cells.erase(find_used);
            }
        }

        bool cell_to_bord = false;
        // mise a jour hole et bord
        for (auto it : cell_deleted) {
            // si une seule cell est un bord alors ajouter les voisins comme nouveau bord
            find_bord = std::find(bordure.begin(), bordure.end(), it);
            if (find_bord != bordure.end()) {
                cell_to_bord = true;
                for (auto itneigh : neigh_cell) {
                    // parcourir hole in map pour voir si un bout du trou en touche un autre
                    for (auto hole : hole_in_map) {
                        auto find_hole = std::find(hole.begin(), hole.end(), itneigh);
                        if (find_hole != hole.end()) { // mettre les cells du trou dans les bords
                            for (auto hole_cell : hole) {
                                bordure.push_back(hole_cell);
                            }
                        }
                    }
                    auto find_bord2 = std::find(bordure.begin(), bordure.end(), itneigh);
                    if (find_bord2 == bordure.end()) {  // pas deja dans la bordure
                        bordure.push_back(itneigh);
                    }
                }
            }
        }
        if (!cell_to_bord) { // le trou ne touche aucun bord
            bool two_holes = false;
            // verifier que deux trou ne se touche pas 
            for (auto it : cell_deleted) {
                for (auto hole : hole_in_map) {
                    auto next_hole = std::find(hole.begin(), hole.end(), it);
                    if (next_hole != hole.end()) {
                        two_holes = true;
                        for (auto neigh : neigh_cell) {
                            hole.push_back(neigh);
                        }
                    }
                }
            }
            if (!two_holes) { // deux trou ne se touche pas
                hole_in_map.push_back(neigh_cell);
            }
        }
        cpt++;
    }
}


void Map::add_neighbors(vector_cell& neighbors, vector_cell pair, vector_cell non_used_cells, vector_cell region)
{
    // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
    for (auto it : pair) {
        auto find_used = std::find(non_used_cells.begin(), non_used_cells.end(), it);
        if (find_used != non_used_cells.end()) { // jamais utilisé 
            auto find_neigh = std::find(neighbors.begin(), neighbors.end(), it);
            if (find_neigh == neighbors.end()) { // pas dans les voisins
                auto find_region = std::find(region.begin(), region.end(), it);
                if (find_region == region.end()) {
                    neighbors.push_back(it);
                }
            }
        }
    }

}

Map::reg_neigh Map::MakeRegionv2(const vector_cell& non_used_cells, std::pair<unsigned int, unsigned int> cellfrom)
{
    // faire  une region avec cellfrom comme départ

    std::srand(std::time(nullptr));
    unsigned int nb_cell_region = (std::rand() % 6) + 18; // 18 à 23

    vector_cell region = {cellfrom};
    vector_cell neighbors;

    // faire un premier tour pour rajouter des voisins
    unsigned int row = cellfrom.first;
    unsigned int col = cellfrom.second;
    if ((cellfrom.first % 2) == 0) { // c'est une ligne pair 
        vector_cell pair = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
        // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
        // ajout de la cell dans les voisins
        add_neighbors(neighbors, pair, non_used_cells, region);
    }
    else { // c'est une ligne impair
        vector_cell impair = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
        add_neighbors(neighbors, impair, non_used_cells, region);
    }

    unsigned int i = 1;
    while (i <= nb_cell_region && !neighbors.empty()) { // tant que le nb de cells n'est pas atteint et qu'il reste des voisins
        std::pair<unsigned int, unsigned int> chosen_neigh;
        chosen_neigh = neighbors[0];
        // ajout dans la region
        region.push_back(chosen_neigh);

        row = chosen_neigh.first;
        col = chosen_neigh.second;
        if ((chosen_neigh.first % 2) == 0) { // c'est une ligne pair  
            vector_cell pair = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
            // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
            // ajout de la cell dans les voisins
            add_neighbors(neighbors, pair, non_used_cells, region);
        }
        else { // c'est une ligne impair
            vector_cell impair = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
            add_neighbors(neighbors, impair, non_used_cells, region);
        }
        neighbors.erase(neighbors.begin());
        i++;
    }
    //fabriquer la valeur de retour
    Map::reg_neigh retour;
    retour.region = region;
    retour.voisins = neighbors;
    return retour;
}

Regions Map::MakeAllRegionsv2(unsigned int size)
{
    std::srand(std::time(nullptr));

    // toutes les cells
    vector_cell non_used_cells = GenerateAllCell(size,size);
    // on en delete certaines
    DeleteRandomCells(non_used_cells,size);

    unsigned int rand_cell = std::rand() % non_used_cells.size();
    auto chosen_cell = non_used_cells.begin() + rand_cell;

    // verifier que la cell choisi est au centre pour générer les régions
    while ((chosen_cell->first < size / 4 || chosen_cell->first > 3*size / 4) && (chosen_cell->second < size / 4 || chosen_cell->second > 3*size / 4)) {
        rand_cell = std::rand() % non_used_cells.size();
        chosen_cell = non_used_cells.begin() + rand_cell;
    }
    
    // nombre de region random
    unsigned int nb_region = (std::rand() % 6) + size-5;
    Regions all_region;
    
    // on choisit la premiere cells de la nouvelle region parmi les voisins de toutes les régions
    vector_cell all_neigh = {(*chosen_cell)};

    while (all_region.size() < nb_region && !all_neigh.empty()) { // tant qu'il reste des voisins et qu'on a pas le nb de regions voulu
        
        // choix de la cell  de depart de la region
        unsigned int index_first_cell = std::rand() % all_neigh.size();
        chosen_cell = all_neigh.begin() + index_first_cell;

        // nouvelle region à partir d'une cell des voisins
        auto regandneigh = MakeRegionv2(non_used_cells,(*chosen_cell));
        auto region = regandneigh.region;

        // mise a jour de tous les voisins
        //  eliminer les voisins mis dans la nouvelle région
        for (auto neigh : regandneigh.region) {
            auto neigh_in_reg = std::find(all_neigh.begin(), all_neigh.end(), neigh);
            if (neigh_in_reg != all_neigh.end()) { // ce voisin a été mis dans la region
                all_neigh.erase(neigh_in_reg);
            }
        }
        if (!regandneigh.voisins.empty()) { // la region a des cellules voisins
            // ajout des nouveaux voisins
            for (auto neigh : regandneigh.voisins) {
                auto neigh_in_neigh = std::find(all_neigh.begin(), all_neigh.end(), neigh);
                if (neigh_in_neigh == all_neigh.end()) { // les voisins n'était pas déjà dans la liste
                    all_neigh.push_back(neigh);
                }
            }
        }
        // mettre a jour non_used_cells
        for (auto it : region) {
            // on recher les cases de la region et on les supprime de non_used_cells
            auto place = std::find(non_used_cells.begin(), non_used_cells.end(), it);
            if (place != non_used_cells.end()) { // devrait arriver tout le temps 
                non_used_cells.erase(place);
            }
        }
        // selection des regions avec une taille superieur a 10
        if (region.size() > 10) {
            all_region.push_back(region);
        }
    }

    // au cas ou on ne peut pas faire 30 region avec les cells retirés, on prend tous les cells sans en enlevés
    if (all_region.size() < nb_region && all_neigh.empty()) {
        non_used_cells = GenerateAllCell(size, size);
        unsigned int row_rand_cell = (std::rand() % 10) + size/2; // 16 25
        unsigned int col_rand_cell = (std::rand() % 10) + size/2; // 16 25
        chosen_cell = std::find(non_used_cells.begin(), non_used_cells.end(), std::make_pair(row_rand_cell, col_rand_cell)); // première cell choisi au centre

        // nombre de region random
        nb_region = (std::rand() % 6) + size-5; // 25 à 30
        all_region = {};
        // on choisit la premiere cells de la nouvelle region parmi les voisins de toutes les régions
        all_neigh = { (*chosen_cell) };

        while (all_region.size() < nb_region) {
            unsigned int index_first_cell = std::rand() % all_neigh.size();
            chosen_cell = all_neigh.begin() + index_first_cell;
            // nouvelle region à partir d'une cell des voisins
            auto regandneigh = MakeRegionv2(non_used_cells, (*chosen_cell));
            auto region = regandneigh.region;

            //  eliminer les voisins mis dans la nouvelles région
            for (auto neigh : regandneigh.region) {
                auto neigh_in_reg = std::find(all_neigh.begin(), all_neigh.end(), neigh);
                if (neigh_in_reg != all_neigh.end()) { // ce voisin a été mis dans la region
                    all_neigh.erase(neigh_in_reg);
                }
            }
            // mise a jour de tous les voisins
            if (!regandneigh.voisins.empty()) { // la region a des cellules voisines

                // ajout des nouveaux voisins
                for (auto neigh : regandneigh.voisins) {
                    auto neigh_in_neigh = std::find(all_neigh.begin(), all_neigh.end(), neigh);
                    if (neigh_in_neigh == all_neigh.end()) { // les voisins n'était pas dans la liste
                        all_neigh.push_back(neigh);
                    }
                }
            }
            // mettre a jour non_used_cells
            for (auto it : region) {
                // on recher les cases de la region et on les supprime de non_used_cells
                auto place = std::find(non_used_cells.begin(), non_used_cells.end(), it);
                if (place != non_used_cells.end()) { // devrait arriver tout le temps 
                    non_used_cells.erase(place);
                }
            }
            if (region.size() > 10) {
                all_region.push_back(region);
            }
        }
    }
    return all_region;
}


SRegions* Map::ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC)
{
    // convertir vecteur de vecteur en map ( tout mettre dans la map ) 
    // fonction du prof, devrait marcher
    SRegions* reg = new SRegions;

    nbR = 0;
    nbC = 0;
    reg->nbRegions = regions.size();
    reg->region = new SRegion[reg->nbRegions];
    for (unsigned int i = 0; i < reg->nbRegions; ++i)
    {
        reg->region[i].nbCells = regions[i].size();
        reg->region[i].cells = new SRegionCell[reg->region[i].nbCells];
        for (unsigned int j = 0; j < reg->region[i].nbCells; ++j)
        {
            if (nbR < regions[i][j].first) nbR = regions[i][j].first;
            if (nbC < regions[i][j].second) nbC = regions[i][j].second;
            reg->region[i].cells[j].y = regions[i][j].first;
            reg->region[i].cells[j].x = regions[i][j].second;
        }
    }

    return(reg);
}
