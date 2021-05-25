#include "Map.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

SRegions* Map::GenerateMap(unsigned int& r, unsigned int& c)
{
    // fonction principal
    
    
    Regions regions = MakeAllRegionsv2();
    unsigned int nbR, nbC;
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
    // les cells vont de 0,0 à 27,31
    // mettre toutes les cells dans un vecteur ( pointeur ) 
    vector_cell all_cell;

    for (unsigned int r = 0; r < row; ++r) {
for (unsigned int c = 0; c < col; ++c) {
    all_cell.push_back(std::make_pair(r, c));
}
    }
    return (all_cell);
}

vector_cell Map::MakeRegion(vector_cell non_used_cells,std::pair<unsigned int,unsigned int> chosen_cell)
{
    // choisir une cell au hasard dans le tableau
    //voisin selon la parité de la ligne
    /*
    std::vector<std::pair<unsigned int, unsigned int>> pair = { {-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 0} };
    std::vector<std::pair<unsigned int, unsigned int>> impair = { {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1} };
    */
    std::srand(std::time(nullptr));

    // faire  une region avec cette cell comme départ
    unsigned int nb_cell_region = (std::rand() % 6) + 18; // 18 à 23

    vector_cell region;
    vector_cell neighbors;
    // faire un premier tour pour rajouter des voisins
    region.push_back(chosen_cell);
    unsigned int row = chosen_cell.first;
    unsigned int col = chosen_cell.second;
    if ((chosen_cell.first % 2) == 0) { // c'est une ligne pair 
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
    while (i <= nb_cell_region && !neighbors.empty()) {
        auto chosen_neigh = neighbors[0];
        // ajout dans la region
        region.push_back(chosen_neigh);

        unsigned int row = chosen_neigh.first;
        unsigned int col = chosen_neigh.second;
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
    return region;
}

Regions Map::MakeAllRegions()
{
    // a partir de toutes les cells
    std::srand(std::time(nullptr));
    unsigned int bord = 30;
    vector_cell non_used_cells = GenerateAllCell(bord, bord);

    auto must_complete = DeleteRandomCells(non_used_cells,bord);
    auto chosen_cell = non_used_cells.begin();
    // appel de la fonction make region a partir de ca
    Regions all_region;
    unsigned int i = 0;
    while (!non_used_cells.empty()) {
        // nouvelle region

        
        if (must_complete.empty()) {
            unsigned int rand_cell = std::rand() % non_used_cells.size();
            chosen_cell = non_used_cells.begin() + rand_cell;
        }
        else {
            chosen_cell = must_complete.begin();
        }

        auto region = MakeRegion(non_used_cells,(*chosen_cell));
        if (!must_complete.empty()) {
            must_complete.erase(must_complete.begin());
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
    return all_region;
}

vector_cell Map::DeleteRandomCells(vector_cell& non_used_cells,unsigned int bord)
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
    vector_cell cell_deleted; //cell enleées du groupement en cours
    vector_cell neigh_cell; // voisin des cells delete en cours
    std::pair<unsigned int, unsigned int> cell_bord = {-1,-1}; // représente la première cell qui touche le bord dans la suppression
    vector_cell must_complete_cell; // cell qui rejoigne deux bord de la map

    unsigned int cpt = 0; // tour de while

    vector_cell neighbors; // voisin de la cell courante
    while (cpt < 15) { // on delete 15 groupement 

        // choix de la premiere cell a enlever

        unsigned int rand_cell = std::rand() % non_used_cells.size();
        auto chosen_cell = non_used_cells.begin() + rand_cell;

        
        unsigned int nb_cell_delete = std::rand() % 6 + 8; // 8 a 13
        cell_deleted = {(*chosen_cell)}; // on remet les cell deleted a 0
        neigh_cell = {}; // les voisins sont remis a 0
        cell_bord = { -1,-1 };// remise a 0

        // verifier qu'elle n'est pas sur le bord
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
            if (find_used != non_used_cells.end()) { // est toujours pas utilisé
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
            auto find_bord = std::find(bordure.begin(), bordure.end(), (*chosen_cell));
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
                if (find_used != non_used_cells.end()) { // est toujours pas utilisé
                    auto find_neigh = std::find(neigh_cell.begin(), neigh_cell.end(), it);
                    if (find_neigh == neigh_cell.end()) { // n'est pas déjà dans les voisins
                        if (cell_bord.first != -1) { // déjà une cell en bordure
                            // verifier que c'est pas une bordure
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
            auto find_bord = std::find(bordure.begin(), bordure.end(), it);
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
                    if (find_bord2 != bordure.end()) { // deja présent dans la bordure
                        // ajouter au cellule que l'on doit completer absolument
                        // cette cellule est le seule rempart entre deux bords pour se compléter
                        must_complete_cell.push_back(itneigh);
                    }
                    else {
                        bordure.push_back(itneigh);
                    }
                }
            }
        }
        if (!cell_to_bord) {
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
            if (!two_holes) {
                hole_in_map.push_back(neigh_cell);
            }
        }
        cpt++;
    }
    return must_complete_cell;
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

Map::reg_neigh Map::MakeRegionv2(vector_cell non_used_cells, std::pair<unsigned int, unsigned int> cellfrom)
{
    std::srand(std::time(nullptr)); // randomise

    // faire  une region avec cette cell comme départ
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
    while (i <= nb_cell_region && !neighbors.empty()) {
        std::pair<unsigned int, unsigned int> chosen_neigh;
        if (neighbors.size() >= 3) {
            chosen_neigh = neighbors[i % 3];
        }
        else {
            chosen_neigh = neighbors[0];
        }
        // ajout dans la region
        region.push_back(chosen_neigh);

        unsigned int row = chosen_neigh.first;
        unsigned int col = chosen_neigh.second;
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
    Map::reg_neigh retour;
    retour.region = region;
    retour.voisins = neighbors;
    return retour;
}

Regions Map::MakeAllRegionsv2()
{
    vector_cell non_used_cells = GenerateAllCell(30,30);
    auto must_complete = DeleteRandomCells(non_used_cells,30);


    std::srand(std::time(nullptr)); 
    unsigned int row_rand_cell = (std::rand() % 10) + 16; // 16 25
    unsigned int col_rand_cell = (std::rand() % 10) + 16; // 16 25
    auto chosen_cell = std::find(non_used_cells.begin(),non_used_cells.end(),std::make_pair(row_rand_cell,col_rand_cell)); // première cell choisi au centre

    // nombre de region random
    unsigned int nb_region = (std::rand() % 6) + 25; // 25 à 30


    Regions all_region;
    
    // on choisit la premiere cells de la nouvelle region parmi les voisins de toutes les régions

    vector_cell all_neigh = {(*chosen_cell)};

    unsigned int i = 0;
    while (all_region.size()<nb_region) {
        unsigned int index_first_cell = std::rand() % all_neigh.size();
        auto chosen_cell = all_neigh.begin() + index_first_cell;
        // nouvelle region à partir d'une cell des voisins
        auto regandneigh = MakeRegionv2(non_used_cells,(*chosen_cell));
        auto region = regandneigh.region;

        // mise a jour de tous les voisins
        if (!regandneigh.voisins.empty()) { // la region a des cellules voisines

            //  eliminer les voisins mis dans la nouvelles région
            for (auto neigh : regandneigh.region) {
                auto neigh_in_reg = std::find(all_neigh.begin(), all_neigh.end(), neigh);
                if (neigh_in_reg != all_neigh.end()) { // ce voisin a été mis dans la region
                    all_neigh.erase(neigh_in_reg);
                }
            }

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
        if (region.size() > 9) {
            all_region.push_back(region);
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
