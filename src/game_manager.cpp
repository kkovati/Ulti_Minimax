#include <assert.h>
#include <chrono>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void GameManager::simulate() {
	partyState = PartyState();
	partyState.init();
	auto [result, partyState] = minimax(0);
	std::cout << result << std::endl;
	std::cout << n_minimax_call << std::endl;
	if (partyState)
	partyState->print_game_progression();
}

std::tuple<int, std::optional<PartyState>> GameManager::minimax(int index) {
	// Time measurement
	// auto start = std::chrono::high_resolution_clock::now();	
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
		return std::make_tuple(partyState.evaluateParty(index), partyState); // TODO decide when std::nullopt
	}

	bool firstPlayerToHit = partyState.isFirstPlayerToHit(index);
	for (Card card : playableCards) {
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		int result = partyState.evaluateParty(index);
		if (firstPlayerToHit) {
			if (result == 1) return std::make_tuple(1, partyState);
			if (result == -1) continue;
		}
		else { // !firstPlayerToHit
			if (result == 1) continue;
			if (result == -1) return std::make_tuple(-1, std::nullopt);
		}
		auto [result2, inheritedPartyState] = minimax(index + 1);
		assert(result2);
		if (firstPlayerToHit && result == 1) return std::make_tuple(1, inheritedPartyState);
		if (!firstPlayerToHit && result == -1) return std::make_tuple(-1, std::nullopt);
	}
	if (firstPlayerToHit) return std::make_tuple(-1, std::nullopt);
	if (!firstPlayerToHit) return std::make_tuple(1, std::nullopt);

	// Time measurement
	//if (index == 8) {
	//	auto stop = std::chrono::high_resolution_clock::now();
	//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	//	int time = duration.count();
	//	std::cout << "Duration: " << time << "ms" << std::endl;
	//
	//	//sum_time += time;
	//	//n_meas++;		
	//	//if (n_meas % 10 == 0) std::cout << "Avg time: " << sum_time / n_meas << "ms" << std::endl;
	//}
}

}
