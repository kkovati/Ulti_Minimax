#include "game_rules.hpp"


namespace ulti_minimax {

class GameManager {
public:
	GameManager() = default;
	GameManager(const PartyState& partyState);

	void simulate();

private:
	PartyState partyState;

};

}