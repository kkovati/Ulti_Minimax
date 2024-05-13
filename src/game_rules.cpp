#include <algorithm>
#include <assert.h>
#include <random>

#include "game_rules.hpp"

namespace ulti_minimax {

Card::Card(uint8_t suit, uint8_t value) : suit(suit), value(value) {}

Card::Card(const Card& other) : suit(other.suit), value(other.value) {}

bool Card::operator==(const Card& other) const {
	return suit == other.suit && value == other.value;
}

bool Card::operator>(const Card& other) const {
	return suit == other.suit ? value > other.value : true;
}

bool Card::operator<(const Card& other) const {
	return suit == other.suit ? value > other.value : false;
}


ActionList::ActionList(int nPlayer, int nCardInHand, int firstPlayer = 0) : 
    nPlayer(nPlayer), nCardInHand(nCardInHand), firstPlayer(firstPlayer) {
    index = 0;
    actionList = ActionVector(nPlayer * nCardInHand);
    actionList[0] = Action(0, firstPlayer);
}


PlayerHands::PlayerHands(int nPlayer, int nCardInHand) :
    nPlayer(nPlayer), nCardInHand(nCardInHand) {
    int a = 1;
}


Deck::Deck() {
    for (int i = 0; i < N_SUIT; ++i) {
        for (int j = 0; j < N_VALUE; ++j) {
            cards.push_back(Card(i, j));
        }
    }
}

std::vector<std::vector<Card>> Deck::deal(int nPlayer, int nCardInHand, PlayerHands playerHands) {
    // Check sizes
    //assert();

    // Fix seed
    std::mt19937 g(0);

    // Shuffle cards
    std::shuffle(cards.begin(), cards.end(), g);

    // Create random sets each containing cards
    std::vector<std::vector<Card>> randomHands(nPlayer, std::vector<Card>(nCardInHand));
    for (int i = 0; i < nPlayer; ++i) {
        for (int j = 0; j < nCardInHand; ++j) {
            randomHands[i][j] = cards[i * nCardInHand + j];
        }
    }
    return randomHands;
}



void PartyState::deal() {
    Deck deck;


}

std::vector<Card> PartyState::get_playable_cards() {
	return std::vector<Card>();
}

}