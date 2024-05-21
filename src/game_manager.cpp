#include <assert.h>

#include "game_manager.hpp"


namespace ulti_minimax {

void GameManager::simulate() {
	PartyState partyState = PartyState();
	partyState.init();
	minimax(partyState, 0);
}

int GameManager::minimax(PartyState& partyState, int index) {
	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);

	if (partyState.isLastIndex(index)) {
		assert(playableCards.size() == 1);
		Card card = playableCards[0];
		partyState.setHitCard(index, card);
		return partyState.evaluateParty();
	}

	for (Card card : playableCards) {
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		minimax(partyState, index + 1);
	}

	return 0;
}

}
