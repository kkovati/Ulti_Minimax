#include "game_manager.h"


namespace ulti_minimax {

GameManager::GameManager() {
	partyState = PartyState();
}

GameManager::GameManager(const PartyState& partyState) : partyState(partyState) {}

void GameManager::simulate() {
	partyState = PartyState();
	partyState.deal();
}

}
