#include <assert.h>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void GameManager::simulate() {
	PartyState partyState = PartyState();
	partyState.init();
	minimax(partyState, 0);
}

int GameManager::minimax(PartyState& partyState, int index) {
	//if (index < 11) std::cout << index << std::endl;
	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);
	assert(playableCards.size());
	//partyState.print(index);

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
