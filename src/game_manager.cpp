#include "game_manager.hpp"


namespace ulti_minimax {

void GameManager::simulate() {
	PartyState partyState = PartyState();
	partyState.init();


}

int GameManager::minimax(PartyState& partyState, int index) {
	std::vector<Card> playableCards;
	partyState.getNotUsedCards(playableCards, index);
	partyState.getPlayableCards(playableCards, index);

	return 0;
}

}
