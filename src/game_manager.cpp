#include "game_manager.hpp"


namespace ulti_minimax {

GameManager::GameManager(const PartyState& partyState) : partyState(partyState) {}

void GameManager::simulate() {
	PartyState partyState = PartyState();
	partyState.deal();


}

}
