#include <assert.h>
#include <chrono>
#include <iostream>

#include "game_manager.hpp"


namespace ulti_minimax {

void TreePathCoder::printResult() const {
	if (getResult() == PLAYER_WIN) std::cout << "PLAYER_WIN"; // << std::endl;
	else if (getResult() == OPPONENT_WIN) std::cout << "OPPONENT_WIN"; // << std::endl;
	else assert(false);
	//std::cout << std::endl;
}

void TreePathCoder::printCode() const {
	for (int i = 0; i < N_ACTION; ++i) {
		std::cout << unsigned(getValue(i)) << " ";
	}
	//std::cout << std::endl;
}

// Start minimax simulation
//
void GameManager::simulate(const std::string& deal) {
	auto start = std::chrono::high_resolution_clock::now();
	
	// Initialize
	partyState = PartyState();
	partyState.init(deal);

	// Start minimaX
	TreePathCoder tpc = minimax(0);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	int time = duration.count();	

	std::cout << "Result: ";
	tpc.printResult();
	std::cout << std::endl;
	std::cout << "No. calls: " << n_minimax_call << std::endl;
	std::cout << "Duration: " << time << " ms" << std::endl;
	std::cout << "TreePathCoder: "; 
	tpc.printCode();
	std::cout << std::endl << std::endl;

	setPartyState(tpc, 0);
}

// This function implements one iteration of the minimax algorithm.
// It handles a single sitation of the game or a single node in the tree.
//
TreePathCoder GameManager::minimax(int index) {
	n_minimax_call += 1;
	if (index == 0 || index == 3 || index == 6) updateProgressBar();

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
		uint8_t result = partyState.evaluateParty(index, false);
		TreePathCoder tpc(result, index, 0);
		return tpc;
	}

	bool isFirstPlayerToHit = partyState.isFirstPlayerToHit(index);
	//for (uint8_t cardIndex = 0; cardIndex < playableCards.size(); ++cardIndex) {	// Incremental direction 
	for (int i = playableCards.size() - 1; i >= 0 ; --i) {							// Decremental direction 
		assert(i >= 0);
		uint8_t cardIndex = static_cast<uint8_t>(i);
		partyState.setHitCard(index, playableCards[cardIndex]);
		partyState.setNextPlayer(index);
		
		// End party early if have a result before the last round
		uint8_t result = partyState.evaluateParty(index, false);
		if (isFirstPlayerToHit) {
			if (result == PLAYER_WIN) {				
				return TreePathCoder(result, index, cardIndex);
			}
			if (result == OPPONENT_WIN) {
				//if (cardIndex == playableCards.size() - 1) {		// Incremental direction 
				if (cardIndex == 0) {								// Decremental direction 
					return TreePathCoder(result, index, cardIndex);
				}
				else {
					continue;
				}				
			}
		}
		else { // !isFirstPlayerToHit
			if (result == PLAYER_WIN) {
				//if (cardIndex == playableCards.size() - 1) {		// Incremental direction 
				if (cardIndex == 0) {								// Decremental direction 
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
		// if (DEBUG) partyState.print_card(index);
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
		//if (cardIndex == playableCards.size() - 1) {		// Incremental direction 
		if (cardIndex == 0) {								// Decremental direction 
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

// This function is heavily relying on GameManager::minimax()
//
void GameManager::setPartyState(TreePathCoder tpc, int index) {
	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);
	partyState.simplifyPlayableCards(playableCards);
	assert(playableCards.size() && playableCards.size() <= N_CARD_IN_HAND);
	partyState.print_current_state(index, playableCards);

	uint8_t cardIndex = tpc.getValue(index);
	if (cardIndex < N_CARD_IN_HAND) {
		partyState.setHitCard(index, playableCards[cardIndex]);
		partyState.setNextPlayer(index);

		partyState.print_card(index);
		uint8_t result = partyState.evaluateParty(index, true); // For printing
	}

	if (partyState.isLastIndex(index) || cardIndex >= N_CARD_IN_HAND) {
		std::cout << std::endl;
		tpc.printResult();
		std::cout << std::endl;
		return;
	}

	setPartyState(tpc, index + 1);
}

void GameManager::updateProgressBar() {
	if (progressBar != 0) 
		std::cout.flush();
	std::cout << "[";
	for (int i = 0; i < 100; ++i) {
		if (i <= progressBar)
			std::cout << "=";
		else
			std::cout << " ";
	}
	std::cout << "] " << progressBar << " %\r";
	progressBar++;
}

}
