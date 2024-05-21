#include <algorithm>
#include <assert.h>
#include <random>

#include "game_rules.hpp"

namespace ulti_minimax {

bool Card::operator==(const Card& other) const {
    return suit == other.suit;
}

bool Card::operator!=(const Card& other) const {
    return suit != other.suit;
}

bool Card::operator>(const Card& other) const {
	return value > other.value;
}

bool Card::operator<(const Card& other) const {
	return value < other.value;
}

//bool Card::operator>(const Card& other) const {
//    return suit == other.suit ? value > other.value : true;
//}
//
//bool Card::operator<(const Card& other) const {
//    return suit == other.suit ? value < other.value : false;
//}


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

void PartyState::getPlayableCards(CardVector& cardVector, int index_) {
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);

    if (posInRound == 0) {
        for (int i = 0; i < N_CARD_IN_HAND; ++i) {
            if (playerHands.getUsed(playerToHit, i) >= index_) {
                cardVector.push_back(playerHands.getCard(playerToHit, i));
            }
        }
    }

    else if (posInRound == 1) {
        Card opponentCard = actionList.getCard(index_ - 1);
        CardVector sameSuitGreaterValue, sameSuitLowerValue, differentSuit;
        for (int i = 0; i < N_CARD_IN_HAND; ++i) {
            if (playerHands.getUsed(playerToHit, i) < index_) continue;
            Card playerCard = playerHands.getCard(playerToHit, i);
            if (playerCard == opponentCard && playerCard > opponentCard) {
                sameSuitGreaterValue.push_back(playerCard);
            }
            else if (playerCard == opponentCard) {
                sameSuitLowerValue.push_back(playerCard);
            }
            else {
                differentSuit.push_back(playerCard);
            }
        }
        if (sameSuitGreaterValue.size()) cardVector = sameSuitGreaterValue;
        else if (sameSuitLowerValue.size()) cardVector = sameSuitLowerValue;
        else cardVector = differentSuit;
    }

    else { // posInRound == 2
        Card opponentCard0 = actionList.getCard(index_ - 2);
        Card opponentCard1 = actionList.getCard(index_ - 1);
        CardVector sameSuitGreaterValue, sameSuitLowerValue, differentSuit;
        for (int i = 0; i < N_CARD_IN_HAND; ++i) {
            if (playerHands.getUsed(playerToHit, i) < index_) continue;
            Card playerCard = playerHands.getCard(playerToHit, i);
            if (playerCard == opponentCard0 && playerCard > opponentCard0 && playerCard > opponentCard1) {
                sameSuitGreaterValue.push_back(playerCard);
            }
            else if (playerCard == opponentCard0) {
                sameSuitLowerValue.push_back(playerCard);
            }
            else {
                differentSuit.push_back(playerCard);
            }
        }
        if (sameSuitGreaterValue.size()) cardVector = sameSuitGreaterValue;
        else if (sameSuitLowerValue.size()) cardVector = sameSuitLowerValue;
        else cardVector = differentSuit;
    }
    assert(cardVector.size());
}

void PartyState::setHitCard(int index_, Card card_) {
    actionList.setCard(index_, card_);
    playerHands.setUsed();
}

void PartyState::setNextPlayer(int index_) {
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);

    if (actionList.isLastIndex(index_)) return;

    if (posInRound == 0 || posInRound == 1) {
        actionList.setPlayerToHit(index_ + 1, (playerToHit + 1) % 3);
    }
    else { // posInRound == 2
        Card card0 = actionList.getCard(index_ - 2);
        Card card1 = actionList.getCard(index_ - 1);
        Card card2 = actionList.getCard(index_);
        int winCardIndex = chooseWinnerCard(card0, card1, card2);
        actionList.setPlayerToHit(index_ + 1, winCardIndex);
    } 
}

int PartyState::chooseWinnerCard(Card c0, Card c1, Card c2) {
    // c0 beats c1
    bool c0bc1_ = (c0 == c1 && c0 > c1) || (c0 != c1);
    bool c0bc1 = !(c0 == c1 && c0 < c1);
    assert(c0bc1 == c0bc1_);
    // c0 beats c2
    bool c0bc2 = !(c0 == c2 && c0 < c2);

    if (c0bc1 && c0bc2) return 0;
    if (c0bc1 && !c0bc2) return 2;
    if (!c0bc1 && c0bc2) return 1;

    // both c1 and c2 beats c0
    assert(c0 == c1 && c1 == c2);
    if (c1 > c2) return 1;
    return 2;
}

int PartyState::evaluateParty() {
    // TODO
    return 0;
}

}