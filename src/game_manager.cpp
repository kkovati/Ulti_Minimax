#include <assert.h>
#include <chrono>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void TreePathCoder::printCode() const {
	for (int i = 0; i < N_ACTION; ++i) {
		std::cout << unsigned(getValue(i));
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
	Result result = tpc.getResult();

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
	assert(playableCards.size());
	//partyState.print_current_state(index, playableCards); // DEBUG PRINT

	if (partyState.isLastIndex(index)) {
		assert(playableCards.size() == 1);
		Card card = playableCards[0];
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		Result result = partyState.evaluateParty(index);
		TreePathCoder tpc(result, index, 0);
		return tpc;
	}

	bool isFirstPlayerToHit = partyState.isFirstPlayerToHit(index);
	int cardIndex = -1;
	for (Card& card : playableCards) {
		cardIndex++;
		partyState.setHitCard(index, card);
		partyState.setNextPlayer(index);
		Result result = partyState.evaluateParty(index);
		// End party early if have a result before the last round
		if (isFirstPlayerToHit) {
			if (result == Result::PLAYER_WIN) {				
				return TreePathCoder(result, index, cardIndex);
			}
			if (result == Result::OPPONENT_WIN) continue;  
		}
		else { // !isFirstPlayerToHit
			if (result == Result::PLAYER_WIN) continue;
			if (result == Result::OPPONENT_WIN) {				
				return TreePathCoder(result, index, cardIndex);
			}
		}
		assert(result == Result::UNDEFINED);
		TreePathCoder tpc = minimax(index + 1);
		result = tpc.getResult();
		assert(result == Result::PLAYER_WIN && result == Result::OPPONENT_WIN);
		if (isFirstPlayerToHit && result == Result::PLAYER_WIN) {
			tpc.setValue(index, cardIndex);
			return tpc;
		}
		if (!isFirstPlayerToHit && result == Result::OPPONENT_WIN) {
			tpc.setValue(index, cardIndex);
			return tpc;
		}
	}
	if (isFirstPlayerToHit) {
		TreePathCoder tpc(Result::OPPONENT_WIN, index, cardIndex);
		return tpc;
	}
	else { // !isFirstPlayerToHit
		TreePathCoder tpc(Result::PLAYER_WIN, index, cardIndex);
		return tpc;
	}
}

}
