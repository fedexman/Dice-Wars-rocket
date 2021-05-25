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
	// fonction reecrite de IMap
	Map() {}
	virtual ~Map() { for (auto pMap : Maps) DeleteMap(pMap); };
	SRegions* GenerateMap(unsigned int& r, unsigned int& c);
	void DeleteMap(SRegions* regions);

	// fonction a nous
	// algo 1
	vector_cell GenerateAllCell(int row,int col);
	vector_cell MakeRegion(vector_cell non_used_cells, std::pair<unsigned int, unsigned int> chosen_cell);
	Regions MakeAllRegions();
	void add_neighbors(vector_cell& neighbors, vector_cell pair, vector_cell non_used_cells, vector_cell region);
	vector_cell DeleteRandomCells(vector_cell& non_used_cells,unsigned int bord);

	// algo 2
	struct reg_neigh {
		vector_cell region;
		vector_cell voisins;
	};
	Map::reg_neigh MakeRegionv2(vector_cell non_used_cells,std::pair<unsigned int,unsigned int> cellfrom);
	Regions MakeAllRegionsv2();


private:
	SRegions* ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC);
private:
	std::set<SRegions*> Maps;
};

