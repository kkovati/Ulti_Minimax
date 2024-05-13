#include "game_rules.h"


namespace ulti_minimax {

class GameManager {
public:
	GameManager();

	GameManager(const PartyState& partyState);

	void simulate();

private:
	PartyState partyState;
};

}