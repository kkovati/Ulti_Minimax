#include <assert.h>
#include <chrono>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void TreePathCoder::printCode() const {
	for (int i = 0; i < N_ACTION; ++i) {
		std::cout << unsigned(getValue(i)) << " ";
	}
	std::cout << std::endl;
}


void GameManager::simulate() {
	auto start = std::chrono::high_resolution_clock::now();
	
	// Initialize
	partyState = PartyState();
	partyState.init();

	// Start minimaX
	TreePathCoder tpc = minimax(0);
	uint8_t result = tpc.getResult();
	tpc.printCode();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	int time = duration.count();	

	//std::cout << "Result: " << result << std::endl;
	std::cout << "No. calls: " << n_minimax_call << std::endl;
	std::cout << "Duration: " << time << " ms" << std::endl;
}

TreePathCoder GameManager::minimax(int index) {
	n_minimax_call += 1;

	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);
	partyState.simplifyPlayableCards(playableCards);
	assert(playableCards.size() && playableCards.size() <= N_CARD_IN_HAND);
	if (DEBUG) partyState.print_current_state(index, playableCards);

	if (partyState.isLastIndex(index)) {
		assert(playableCards.size() == 1);
		Card card = playableCards[0];
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		uint8_t result = partyState.evaluateParty(index);
		TreePathCoder tpc(result, index, 0);
		return tpc;
	}

	bool isFirstPlayerToHit = partyState.isFirstPlayerToHit(index);
	for (uint8_t cardIndex = 0; cardIndex < playableCards.size(); ++cardIndex) {
		partyState.setHitCard(index, playableCards[cardIndex]);
		partyState.setNextPlayer(index);
		
		// End party early if have a result before the last round
		uint8_t result = partyState.evaluateParty(index);
		if (isFirstPlayerToHit) {
			if (result == PLAYER_WIN) {				
				return TreePathCoder(result, index, cardIndex);
			}
			if (result == OPPONENT_WIN) {
				if (cardIndex == playableCards.size() - 1) {
					return TreePathCoder(result, index, cardIndex);
				}
				else {
					continue;
				}				
			}
		}
		else { // !isFirstPlayerToHit
			if (result == PLAYER_WIN) {
				if (cardIndex == playableCards.size() - 1) {
					return TreePathCoder(result, index, cardIndex);
				}
				else {
					continue;
				}
			}
			if (result == OPPONENT_WIN) {				
				return TreePathCoder(result, index, cardIndex);
			}
		}

		// No early ending found, continue searching optimal card
		assert(result == RESULT_UNDEFINED);
		TreePathCoder tpc = minimax(index + 1);
		result = tpc.getResult();
		assert(result == PLAYER_WIN || result == OPPONENT_WIN);
		if (isFirstPlayerToHit && result == PLAYER_WIN) {
			tpc.setValue(index, cardIndex);
			return tpc;
		}
		if (!isFirstPlayerToHit && result == OPPONENT_WIN) {
			tpc.setValue(index, cardIndex);
			return tpc;
		}

		// No optimal card found
		if (cardIndex == playableCards.size() - 1) {
			if (isFirstPlayerToHit) {
				tpc.setResult(OPPONENT_WIN);
				tpc.setValue(index, cardIndex);
				//TreePathCoder tpc(OPPONENT_WIN, index, cardIndex);
				return tpc;
			}
			else { // !isFirstPlayerToHit
				tpc.setResult(PLAYER_WIN);
				tpc.setValue(index, cardIndex);
				//TreePathCoder tpc(PLAYER_WIN, index, cardIndex);
				return tpc;
			}
		}
	}
	assert(false); // Unreachable code
}

}
