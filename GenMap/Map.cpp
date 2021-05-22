#include "Map.h"
#include <iostream>
#include <vector>
#include <algorithm>

SRegions* Map::GenerateMap(unsigned int& r, unsigned int& c)
{
    // fonction principal 
    Regions regions = MakeAllRegions();
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
}

std::vector<std::pair<unsigned int, unsigned int>> Map::GenerateAllCell()
{
    // les cells vont de 0,0 à 31,27
    // mettre toutes les cells dans un vecteur ( pointeur ) 
    std::vector<std::pair<unsigned int, unsigned int>> all_cell;

    for (unsigned int row = 0; row < 28; ++row) {
        for (unsigned int col = 0; col < 32; ++col) {
            all_cell.push_back(std::make_pair(row, col));
        }
    }
    return (all_cell);
}

std::vector<std::pair<unsigned int, unsigned int>> Map::MakeRegion(std::vector<std::pair<unsigned int, unsigned int>> non_used_cells)
{
    // choisir une cell au hasard dans le tableau
    //voisin selon la parité de la ligne
    /*
    std::vector<std::pair<unsigned int, unsigned int>> pair = { {-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 0} };
    std::vector<std::pair<unsigned int, unsigned int>> impair = { {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1} };
    */
    unsigned int rand_cell = rand() % non_used_cells.size();
    auto chosen_cell = non_used_cells.begin() + rand_cell;

    // faire  une region avec cette cell comme départ
    unsigned int nb_cell_region = (rand() % 6) + 18; // 18 à 23

    std::vector<std::pair<unsigned int, unsigned int>> region;
    std::vector<std::pair<unsigned int, unsigned int>> neighbors;

    // faire un premier tour pour rajouter des voisins
    unsigned int row = chosen_cell->first;
    unsigned int col = chosen_cell->second;
    if ((chosen_cell->first % 2) == 0) { // c'est une ligne pair 
        std::vector<std::pair<unsigned int, unsigned int>> pair = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
        // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
        // ajout de la cell dans les voisins
        add_neighbors(neighbors, pair, non_used_cells);

        // choisir aléatirement
    }
    else { // c'est une ligne impair
        std::vector<std::pair<unsigned int, unsigned int>> impair = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
        add_neighbors(neighbors, impair, non_used_cells);
    }


    unsigned int i = 1;
    while (i <= nb_cell_region && !neighbors.empty()) {

        // choisir aleatoirement un voisin 
        unsigned int chosen_neigh = rand() % neighbors.size();
        // ajout dans la region
        region.push_back(neighbors[chosen_neigh]);

        unsigned int row = neighbors[chosen_neigh].first;
        unsigned int col = neighbors[chosen_neigh].second;
        if ((neighbors[chosen_neigh].first % 2) == 0) { // c'est une ligne pair  
            std::vector<std::pair<unsigned int, unsigned int>> pair = { {row - 1, col - 1}, {row - 1, col}, {row, col - 1}, {row, col + 1}, {row + 1, col - 1}, {row + 1, col} };
            // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
            // ajout de la cell dans les voisins
            add_neighbors(neighbors, pair, non_used_cells);
        }
        else { // c'est une ligne impair
            std::vector<std::pair<unsigned int, unsigned int>> impair = { {row - 1, col}, {row - 1, col + 1}, {row, col - 1}, {row, col + 1}, {row + 1, col}, {row + 1, col + 1} };
            add_neighbors(neighbors, impair, non_used_cells);
        }
    }
    return region;
}

Regions Map::MakeAllRegions()
{
    // a partir de toutes les cells
    std::vector<std::pair<unsigned int, unsigned int>> non_used_cells = GenerateAllCell();
    // faire un nombres random de regions entre 25 et 35
    unsigned int nb_regions = (rand() % 11) + 25; // 25 to 35

    // appel de la fonction make region a partir de ca
    Regions all_region;
    unsigned int i = 0;
    while (i < nb_regions) {
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
        all_region.push_back(region);
    }
    return all_region;
}

void Map::add_neighbors(std::vector<std::pair<unsigned int, unsigned int>>& neighbors, std::vector<std::pair<unsigned int, unsigned int>> pair, std::vector<std::pair<unsigned int, unsigned int>> non_used_cells)
{
    // trouver si la cellule est dans non_used et si elle n'est pas déjà dans les voisins
    for (auto it : pair) {
        auto find_used = std::find(non_used_cells.begin(), non_used_cells.end(), it);
        if (find_used != non_used_cells.end()) { // jamais utilisé 
            auto find_neigh = std::find(neighbors.begin(), neighbors.end(), it);
            if (find_neigh == neighbors.end()) { // pas dans les voisins
                neighbors.push_back(it);
            }
        }
    }

}


SRegions* Map::ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC)
{
    // convertir vecteur de vecteur en map ( tout mettre dans la map ) 
    return nullptr;
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
