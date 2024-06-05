#include <assert.h>
#include <chrono>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void GameManager::simulate() {
	partyState = PartyState();
	partyState.init();
	minimax(0);
}

int GameManager::minimax(int index) {
	// Time measurement
	auto start = std::chrono::high_resolution_clock::now();	

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
		minimax(index + 1);
	}

	// Time measurement
	if (index == 13) {
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		int time = duration.count();
		std::cout << "Duration: " << time << "ms" << std::endl;

		//sum_time += time;
		//n_meas++;		
		//if (n_meas % 10 == 0) std::cout << "Avg time: " << sum_time / n_meas << "ms" << std::endl;
	}

	return 0;
}

}
