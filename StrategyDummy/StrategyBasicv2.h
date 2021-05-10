#pragma once
#include "./src/StrategyDummy.h"
#include <vector>
class StrategyBasicv2 :
    public StrategyDummy
{
public:
	StrategyBasicv2(unsigned int id, unsigned int nbPlayer, const SMap* map) : StrategyDummy(id, nbPlayer, map) {};
	bool PlayTurn(unsigned int gameTurn, const SGameState *state, STurn *turn);
	~StrategyBasicv2();
protected:
	class playableAttackable {//foncteur qui permet le delete facile de notre structure, permet de lapeller a chaque tour
	public:
		~playableAttackable() {
			for (auto& it : pA) {
				delete it.second;
			}
		}
		bool init(StrategyBasicv2* T);
		int bestPlay(STurn* turn);
		StrategyBasicv2* t;
		std::vector<std::pair<pSCell, std::vector<pSCell>*>> pA;
	};
	//contient en first toutes les cases playable, et en second un vector de toutes les cases attackables depuis cette case
};

