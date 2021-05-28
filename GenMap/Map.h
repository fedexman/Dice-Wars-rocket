#pragma once

#include <vector>
#include <set>
#include <utility>
#include "../Commun/IMapLib.h"

using Regions = std::vector< std::vector<std::pair<unsigned int, unsigned int>> >;
using vector_cell = std::vector<std::pair<unsigned int, unsigned int>>;

class Map : public IMap
{
public:
	// Redéfinition des méthodes de IMap
	Map() {}
	virtual ~Map() { for (auto pMap : Maps) DeleteMap(pMap); };
	SRegions* GenerateMap(unsigned int& r, unsigned int& c);
	void DeleteMap(SRegions* regions);

	// Méthodes propres à Map
	vector_cell GenerateAllCell(int row, int col);
	void add_neighbors(vector_cell& neighbors, vector_cell pair, vector_cell non_used_cells, vector_cell region);
	void DeleteRandomCells(vector_cell& non_used_cells, unsigned int bord);
	struct reg_neigh {
		vector_cell region;
		vector_cell voisins;
	};
	Map::reg_neigh MakeRegionv2(const vector_cell& non_used_cells, std::pair<unsigned int, unsigned int> cellfrom);
	Regions MakeAllRegionsv2(unsigned int size);

private:
	SRegions* ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC);
private:
	std::set<SRegions*> Maps;
};
