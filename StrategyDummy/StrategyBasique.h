#pragma once
#include "src/StrategyDummy.h"
class StrategyBasique :
    public StrategyDummy
{
public:
	StrategyBasique(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) {};
};

