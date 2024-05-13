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


Deck::Deck() {
    for (int i = 0; i < nSuit; ++i) {
        for (int j = 0; j < nValue; ++j) {
            cards.push_back(Card(i, j));
        }
    }
}

void Deck::shuffle() {
    // Fix seed
    std::mt19937 g(0);
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


ActionList::ActionList(int nPlayer, int nCardInHand, int firstPlayer = 0) : 
    nPlayer(nPlayer), nCardInHand(nCardInHand), firstPlayer(firstPlayer) {
    index = 0;
    actions = ActionVector(nPlayer * nCardInHand);
    actions[0] = Action(0, firstPlayer);
}


PlayerHands::PlayerHands(int nPlayer, int nCardInHand) :
    nPlayer(nPlayer), nCardInHand(nCardInHand) {
    playerCards = PlayerCardVector(nPlayer, CardVector(nCardInHand));
    playerUseds = PlayerUsedVector(nPlayer, IntVector(nCardInHand));
}

void PlayerHands::deal() {
    Deck deck = Deck();
    assert(nPlayer * nCardInHand <= deck.getSize());

    deck.shuffle();    
    for (int i = 0; i < nPlayer; ++i) {
        for (int j = 0; j < nCardInHand; ++j) {
            playerCards[i][j] = deck.getCard(i * nCardInHand + j);
        }
    }
}


void PartyState::deal() {
    

}

std::vector<Card> PartyState::get_playable_cards() {
	return std::vector<Card>();
}

}