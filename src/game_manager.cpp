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
	//partyState.print_current_state(index, playableCards); // DEBUG PRINT

	if (partyState.isLastIndex(index)) {
		assert(playableCards.size() == 1);
		Card card = playableCards[0];
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		uint8_t result = partyState.evaluateParty(index);
		return result;
	}

	bool isFirstPlayerToHit = partyState.isFirstPlayerToHit(index);
	int cardIndex = -1;
	for (Card& card : playableCards) {
		cardIndex++;
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		uint8_t result = partyState.evaluateParty(index);
		// End party early if have a result already
		if (isFirstPlayerToHit) {
			if (result == 1) {				
				return result;
			}
			if (result == 2) continue;  
		}
		else { // !isFirstPlayerToHit
			if (result == 1) continue;
			if (result == 2) {				
				return result;
			}
		}
		assert(!result); // result == 0
		result = minimax(index + 1);
		assert(result); // result == 1 || result == 2
		if (isFirstPlayerToHit && result == 1) {
			return result;
		}
		if (!isFirstPlayerToHit && result == 2) {
			return result;
		}
	}
	if (isFirstPlayerToHit) {
		return 2;
	}
	else { // !isFirstPlayerToHit
		return 1;
	}
}


void TreePathCoder::printCode() const {
	for (int i = 0; i < size; ++i) {
		std::cout << unsigned(getValue(i));
	}
	std::cout << std::endl;
}

}
