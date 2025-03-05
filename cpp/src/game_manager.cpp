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


// Apply ace-ten order from original ace ace-king order.
// Ace-king order is the default order from the webapp.
// Ace-king order: (real card value(order in game))
// 0(0), 1(1), 2(2), 3(3), 4(4), 5(5), 6(6), 7(7)
// Ace-ten order: (real card value(order in game))
// 0(0), 1(1), 2(2), 4(3), 5(4), 6(5), 3(6), 7(7)
// NOTE: rest cards will be determined correctly after this conversion
//
std::string GameManager::applyAceTenOrder(const std::string& deal) {
	// Game type (same as in PartyState::init())
	char gameTypeChar = deal[0];
	if (!std::isdigit(gameTypeChar)) throw std::invalid_argument("Invalid game type code");
	uint8_t gameType = gameTypeChar - '0'; // Convert char to int
	if (!(0 <= gameType && gameType <= 9)) throw std::invalid_argument("Invalid game type code");

	// Check if ace-king order game type is selected (similar as in PartyState::init())
	assert(card_order == 0);
	if (std::find(std::begin(ACE_KING_ORDER_GAMES), std::end(ACE_KING_ORDER_GAMES), gameType) != std::end(ACE_KING_ORDER_GAMES)) {
		// Ace-king order game type is selected and because this is the default card order no change is required
		card_order = ACE_KING_ORDER;
		return deal;
	}
	card_order = ACE_TEN_ORDER;

	// Apply ace-ten order to deal string
	// Deal code structure:
	// deal[0]		= game type
	// deal[1]		= trump [0-3]
	// deal[2...61]	= cards	[suit, value]
	std::string aceTenDeal = deal; // Copy the deal string
	for (int i = 2; i < deal.length(); i += 2) {  // (similar as in PartyState::init())
		char valueChar = deal[i + 1];
		if (!std::isdigit(valueChar)) throw std::invalid_argument("Invalid card code");
		int value = valueChar - '0';
		if (value < 0 || 7 < value) throw std::invalid_argument("Invalid card code");
		switch (valueChar) {
		case '3':
			aceTenDeal[i + 1] = '6';
			break;
		case '4':
			aceTenDeal[i + 1] = '3';
			break;
		case '5':
			aceTenDeal[i + 1] = '4';
			break;
		case '6':
			aceTenDeal[i + 1] = '5';
			break;
		default:
			break;
		}
	}
	return aceTenDeal;
}

// Apply ace-king order from ace-ten order.
// Ace-king order is the default order from the webapp.
// This is the inverse of function applyAceTenOrder().
//
std::string GameManager::applyAceKingOrder(const std::string& gameProgression) {
	// Check if ace-king card order game type is selected
	if (card_order == ACE_KING_ORDER) {
		// Already (default) ace-king order is applied, no change is required
		return gameProgression;
	}
	assert(card_order == ACE_TEN_ORDER);

	// Apply ace-king order to gameProgression string
	// Game progression string structure:
	// gameProgression[0] = result 
	// gameProgression[1] = round starting player index 
	// gameProgression[2] = card suit 
	// gameProgression[3] = card value
	// gameProgression[4] = next player index in the round
	assert(gameProgression.length() == 1 + 3 * N_ACTION);
	std::string AceKingGameProgression = gameProgression; // Copy the gameProgression string
	for (int i = 1; i < gameProgression.length(); i += 3) {
		char valueChar = gameProgression[i + 2];
		switch (valueChar) {
		case '3':
			AceKingGameProgression[i + 2] = '4';
			break;
		case '4':
			AceKingGameProgression[i + 2] = '5';
			break;
		case '5':
			AceKingGameProgression[i + 2] = '6';
			break;
		case '6':
			AceKingGameProgression[i + 2] = '3';
			break;
		default:
			break;
		}
	}
	return AceKingGameProgression;
}

// Start minimax simulation
//
std::string GameManager::simulate(const std::string& deal) {
	auto start = std::chrono::high_resolution_clock::now();
	
	// Initialize
	partyState = PartyState();
	bool prerequisites = partyState.init(deal);
	
	TreePathCoder tpc;
	if (prerequisites)
		// Start minimax
		tpc = minimax(0);	
	else
		// Prerequisites are not met
		tpc = TreePathCoder(OPPONENT_WIN, 0, 0);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	int time = duration.count();	

	std::cout << "Game type: " << std::to_string(partyState.getGameType()) << std::endl;
	std::cout << "Trump: " << std::to_string(partyState.getTrump()) << std::endl;
	std::cout << "Rest cards: " 
		<< partyState.getRestCards()[0] << " " << partyState.getRestCards()[1] << std::endl;
	std::cout << "Result: ";
	tpc.printResult();
	std::cout << std::endl;
	if (!prerequisites) std::cout << "Prerequisites are not met" << std::endl;
	std::cout << "No. calls: " << n_minimax_call << std::endl;
	std::cout << "Duration: " << time << " ms" << std::endl;
	std::cout << "TreePathCoder: "; 
	tpc.printCode();
	std::cout << std::endl << std::endl;

	// Set up PartyState based on the returned TreePathCoder,
	// and also to print the game progression to console
	int last_index = setPartyState(tpc, 0);

	// Generate game progression string from set up PartyState
	// gameProgression[0] = result 
	// gameProgression[1] = round starting player index 
	// gameProgression[2] = card suit 
	// gameProgression[3] = card value
	// gameProgression[4] = next player index in the round
	std::string gameProgression = std::to_string(tpc.getResult()) + partyState.getGameProgressionStr(last_index);

	std::cout << "Game progression str: " << gameProgression << std::endl;
	return gameProgression;
}

// This function implements one iteration of the minimax algorithm.
// It handles a single situation of the game, or in other words, a single node in the tree.
//
TreePathCoder GameManager::minimax(int index) {
	n_minimax_call += 1;
	if (index == 0 || index == 3 || index == 6) updateProgressBar();

	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);
	partyState.simplifyPlayableCards(playableCards);
	assert(playableCards.size() && playableCards.size() <= N_CARD_IN_HAND);
	if (DEBUG) partyState.printCurrentState(index, playableCards);

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

// Based on an input TreePathCoder this function recreates the party into a Party State class.
// It also prints the game progession to console.
// Returns the last index the game was played.
// This function is heavily relying on GameManager::minimax().
//
int GameManager::setPartyState(TreePathCoder tpc, int index) {
	std::vector<Card> playableCards;
	partyState.getPlayableCards(playableCards, index);
	partyState.simplifyPlayableCards(playableCards);
	assert(playableCards.size() && playableCards.size() <= N_CARD_IN_HAND);
	partyState.printCurrentState(index, playableCards);

	uint8_t cardIndex = tpc.getValue(index);
	if (cardIndex < N_CARD_IN_HAND) { // This condition is needed if the party was not played to the end
		partyState.setHitCard(index, playableCards[cardIndex]);
		partyState.setNextPlayer(index);

		partyState.printCard(index);
		uint8_t result = partyState.evaluateParty(index, true); // For printing points
	}

	if (partyState.isLastIndex(index) || cardIndex >= N_CARD_IN_HAND) {
		std::cout << std::endl;
		tpc.printResult();
		std::cout << std::endl;
		return index;
	}

	return setPartyState(tpc, index + 1);
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
