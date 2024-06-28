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

void TreePathCoder::setDigit(int index, uint64_t value) {
	assert(0 <= index && index <= 31 && value <= 9);
	if (index < 16) {
		// Clear the bits at the specified position
		code0 &= ~((uint64_t)0xF << (index * 4));
		// Set the bits to the new value in code1
		code0 |= (value << (index * 4));
	}
	else {
		index -= 16; // Index offset
		code1 &= ~((uint64_t)0xF << (index * 4));
		code1 |= ((uint64_t)value << (index * 4));
	}
}

uint8_t TreePathCoder::getDigit(int index) const {
	assert(0 <= index && index <= 31);
	if (index < 16) {
		// Extract the bits at the specified index
		return (code0 >> (index * 4)) & 0xF;
	}
	else {
		index -= 16; // Index offset
		return (code1 >> (index * 4)) & 0xF;
	}
}

void TreePathCoder::printCode() const {
	for (int i = 0; i < 32; ++i) {
		std::cout << unsigned(getDigit(i));
	}
	std::cout << std::endl;
}

}
