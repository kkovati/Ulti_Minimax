#include <assert.h>
#include <chrono>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void GameManager::simulate() {
	auto start = std::chrono::high_resolution_clock::now();
	
	partyState = PartyState();
	partyState.init();
	int result = minimax(0);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	int time = duration.count();	

	std::cout << "Result: " << result << std::endl;
	std::cout << "No. calls: " << n_minimax_call << std::endl;
	std::cout << "Duration: " << time << " ms" << std::endl;
}

int GameManager::minimax(int index) {	
	n_minimax_call += 1;

	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);
	partyState.simplifyPlayableCards(playableCards);
	assert(playableCards.size());
	//partyState.print_current_state(index, playableCards);

	if (partyState.isLastIndex(index)) {
		assert(playableCards.size() == 1);
		Card card = playableCards[0];
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		return partyState.evaluateParty(index);
	}

	bool isFirstPlayerToHit = partyState.isFirstPlayerToHit(index);
	for (Card card : playableCards) {
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		int result = partyState.evaluateParty(index);
		if (isFirstPlayerToHit) {
			if (result == 1) return 1;
			if (result == -1) continue;
		}
		else { // !isFirstPlayerToHit
			if (result == 1) continue;
			if (result == -1) return -1;
		}
		result = minimax(index + 1);
		assert(result);
		if (isFirstPlayerToHit && result == 1) return 1;
		if (!isFirstPlayerToHit && result == -1) return -1;
	}
	if (isFirstPlayerToHit) return -1;
	if (!isFirstPlayerToHit) return 1;
}


void TreePathCoder::printCode() const {
	for (int i = 0; i < 32; ++i) {
		std::cout << unsigned(getDigit(i));
	}
	std::cout << std::endl;
}

}
