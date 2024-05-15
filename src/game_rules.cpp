#include <algorithm>
#include <assert.h>
#include <random>

#include "game_rules.hpp"

namespace ulti_minimax {

bool Card::operator==(const Card& other) const {
	return suit == other.suit && value == other.value;
}

bool Card::operator>(const Card& other) const {
	return suit == other.suit ? value > other.value : true;
}

bool Card::operator<(const Card& other) const {
	return suit == other.suit ? value > other.value : false;
}


Deck::Deck() {
    for (int i = 0; i < N_SUIT; ++i) {
        for (int j = 0; j < N_VALUE; ++j) {
            cards[i * j + j] = Card(i, j);
        }
    }
}

void Deck::shuffle() {
    // Fix seed
    std::mt19937 g(SEED);
    // Shuffle cards
    std::shuffle(cards.begin(), cards.end(), g);
}

std::vector<std::vector<Card>> Deck::deal(int nPlayer, int nCardInHand) {
    shuffle();
    // Create random sets each containing cards
    std::vector<std::vector<Card>> randomHands(nPlayer, std::vector<Card>(nCardInHand));
    for (int i = 0; i < nPlayer; ++i) {
        for (int j = 0; j < nCardInHand; ++j) {
            randomHands[i][j] = cards[i * nCardInHand + j];
        }
    }
    return randomHands;
}


ActionList::ActionList(int firstPlayer) : firstPlayer(firstPlayer) {
    for (int i = 0; i < N_PLAYER * N_CARD_IN_HAND; ++i) {
        actions.push_back(Action(static_cast<int>(i / N_PLAYER), i % N_PLAYER));
    }    
    actions[0].setPlayerToHit(firstPlayer);
}


void PlayerHands::deal() {
    Deck deck = Deck();
    assert(N_PLAYER * N_CARD_IN_HAND <= deck.getSize());
    deck.shuffle();    
    for (int i = 0; i < N_PLAYER; ++i) {
        for (int j = 0; j < N_CARD_IN_HAND; ++j) {
            playerCards[i][j] = deck.getCard(i * N_CARD_IN_HAND + j);
        }
    }
}


void PartyState::init() {
    playerHands.deal();
}

std::vector<Card> PartyState::getPlayableCards(int index_) {
    std::vector<Card> playableCards;
    int playerToHit = actionList.getPlayerToHit(index_);
    int posInRound = actionList.getPosInRound(index_);
    if (posInRound == 0) {
        //for (int i = 0; i < N_CARD_IN_HAND; ++i) {
        //    if (playerHands.getUsed(playerToHit, i) > )
        //    playerHands.getCard(playerToHit, i);
        //}
    }
    else {

    }

	return std::vector<Card>();
}

void PartyState::setNextPlayer() {

}

}