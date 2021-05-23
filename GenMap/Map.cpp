#include "Map.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "src/DefaultMap.h"
#include <random>

SRegions* Map::GenerateMap(unsigned int& r, unsigned int& c)
{
    // fonction principal
    
    /*
    // alterner les deux fonctions de generations de map pour plus de diversité
    std::srand(std::time(nullptr));
    unsigned int rand_gen = std::rand() % 2; // 0 ou 1
    Regions regions;

    switch (rand_gen) {
    case 0:
        regions = MakeAllRegions();
        break;
    case 1:
        regions = MakeAllRegionsv2();
        break;
    default:
        throw ("error in random");
    }
    */

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

vector_cell Map::MakeRegion(vector_cell non_used_cells)
{
    // choisir une cell au hasard dans le tableau
    //voisin selon la parité de la ligne
    /*
    std::vector<std::pair<unsigned int, unsigned int>> pair = { {-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 0} };
    std::vector<std::pair<unsigned int, unsigned int>> impair = { {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1} };
    */

    std::srand(std::time(nullptr));
    unsigned int rand_cell = std::rand() % non_used_cells.size();
    auto chosen_cell = non_used_cells.begin() + rand_cell;

    // faire  une region avec cette cell comme départ
    unsigned int nb_cell_region = (std::rand() % 6) + 18; // 18 à 23

    vector_cell region;
    vector_cell neighbors;

    // faire un premier tour pour rajouter des voisins
    region.push_back(*chosen_cell);
    unsigned int row = chosen_cell->first;
    unsigned int col = chosen_cell->second;
    if ((chosen_cell->first % 2) == 0) { // c'est une ligne pair 
        vector_cell pair = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
        // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
        // ajout de la cell dans les voisins
        add_neighbors(neighbors, pair, non_used_cells,region);

        // choisir aléatirement
    }
    else { // c'est une ligne impair
        vector_cell impair = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
        add_neighbors(neighbors, impair, non_used_cells,region);
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
            add_neighbors(neighbors, pair, non_used_cells,region);
        }
        else { // c'est une ligne impair
            vector_cell impair = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
            add_neighbors(neighbors, impair, non_used_cells,region);
        }
        neighbors.erase(neighbors.begin());
        i++;
    }
    return region;
}

Regions Map::MakeAllRegions()
{
    // a partir de toutes les cells
    vector_cell non_used_cells = GenerateAllCell(30,30);
    // supprimer des cells aleatoire pour ne pas remplir toute la map
    // faire attention a ne pas rejoindre les bords

    // appel de la fonction make region a partir de ca
    Regions all_region;
    unsigned int i = 0;
    while (!non_used_cells.empty()) {
        // nouvelle region
        auto region = MakeRegion(non_used_cells);
        // mettre a jour non_used_cells
        for (auto it : region) {
            // on recher les cases de la region et on les supprime de non_used_cells
            auto place = std::find(non_used_cells.begin(), non_used_cells.end(), it);
            if (place != non_used_cells.end()) { // devrait arriver tout le temps 
                non_used_cells.erase(place);
            }
        }
        if (region.size()>10) {
            all_region.push_back(region);
        }
    }
    return all_region;
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
    Map::reg_neigh retour;
    retour.region = region;
    retour.voisins = neighbors;
    return retour;
}

Regions Map::MakeAllRegionsv2()
{
    vector_cell non_used_cells = GenerateAllCell(40,40);

    std::srand(std::time(nullptr)); 
    unsigned int row_rand_cell = (std::rand() % 10) + 16; // 16 25
    unsigned int col_rand_cell = (std::rand() % 10) + 16; // 16 25
    auto chosen_cell = std::find(non_used_cells.begin(),non_used_cells.end(),std::make_pair(row_rand_cell,col_rand_cell)); // première cell choisi au centre

    // nombre de region random
    unsigned int nb_region = (std::rand() % 6) + 30; // 30 à 35


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
