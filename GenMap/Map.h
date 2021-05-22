#pragma once

#include <vector>
#include <set>
#include <utility>
#include "../Commun/IMapLib.h"

using Regions = std::vector< std::vector<std::pair<unsigned int, unsigned int>> >;

class Map : public IMap
{
public:
	// fonction reecrite de IMap
	Map() {}
	virtual ~Map() = default;
	SRegions* GenerateMap(unsigned int& r, unsigned int& c);
	void DeleteMap(SRegions* regions);

	// fonction a nous
	std::vector<std::pair<unsigned int, unsigned int>> GenerateAllCell();
	std::vector<std::pair<unsigned int, unsigned int>> MakeRegion(std::vector<std::pair<unsigned int, unsigned int>> non_used_cells);
	Regions MakeAllRegions();

	void add_neighbors(std::vector<std::pair<unsigned int, unsigned int>>& neighbors, std::vector<std::pair<unsigned int, unsigned int>> pair, std::vector<std::pair<unsigned int, unsigned int>> non_used_cells);
	// objectif 
	// generer toutes les cellules et des regions de 6 cases (facile)

private:
	SRegions* ConvertMap(Regions& regions, unsigned int& nbR, unsigned int& nbC);
private:
	std::set<SRegions*> Maps;
};

