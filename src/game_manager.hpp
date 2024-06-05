#include "game_rules.hpp"


namespace ulti_minimax {

class GameManager {
public:
	GameManager() = default;
	void simulate();

private:
	PartyState partyState;

	int minimax(int);

	// Time measuremet
	int sum_time = 0;
	int n_meas = 0;
};

}