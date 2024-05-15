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


ActionList::ActionList(int firstPlayer_) : firstPlayer(firstPlayer_) {
    for (int i = 0; i < actions.size(); ++i) {
        actions[i] = Action(static_cast<int>(i / N_PLAYER), i % N_PLAYER);
    }    
    actions[0].setPlayerToHit(firstPlayer);
}


void PlayerHands::deal() {
    Deck deck = Deck();
    assert(N_PLAYER * N_CARD_IN_HAND <= deck.size());
    deck.shuffle();    
    for (int i = 0; i < N_PLAYER; ++i) {
        for (int j = 0; j < N_CARD_IN_HAND; ++j) {
            playerCards[i][j] = deck.getCard(i * N_CARD_IN_HAND + j);
            playerUseds[i][j] = LAST_ACTION_INDEX + 1;
        }
    }
}


void PartyState::init() {
    playerHands.deal();
}

bool PartyState::isSameSuit(const CardVector& cardVector, const Card card) const {
    return false;
}

bool PartyState::isLargerValue(const CardVector&, const Card& card) const {
    return false;
}

bool PartyState::isLargerValue(const CardVector&, const Card& card0, const Card& card1) const {
    return false;
}

void PartyState::getNotUsedCards(CardVector& cardVector, int index_) {
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);
    for (int i = 0; i < N_CARD_IN_HAND; ++i) {
        if (playerHands.getUsed(playerToHit, i) >= index_) {
            cardVector.push_back(playerHands.getCard(playerToHit, i));
        }
    }
    assert(cardVector.size() > 0);
}

void PartyState::getPlayableCards(CardVector& playableCards, int index_) {
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);
    //for (int i = 0; i < N_CARD_IN_HAND; ++i) {
    //    if (playerHands.getUsed(playerToHit, i) < index_) continue;
    //    if (posInRound == 0) {
    //        playableCards.push_back(playerHands.getCard(playerToHit, i));
    //    }
    //    else if (posInRound == 1) {
    //
    //    }
    //    else { // posInRound == 2
    //
    //    }
    //}
    assert(playableCards.size() > 0);
}

void PartyState::setNextPlayer() {

}

}