#include <algorithm>
#include <assert.h>
#include <iostream>
#include <random>
#include <string>

#include "game_rules.hpp"

namespace ulti_minimax {

// Compare for sorting
bool Card::compareCard(const Card& a, const Card& b) {
    if (a == b) return a < b;
    return a.suit < b.suit;
}


Deck::Deck() {
    for (int i = 0; i < N_SUIT; ++i) {
        for (int j = 0; j < N_VALUE; ++j) {
            cards[i * N_VALUE + j] = Card(i, j);
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
    // Deal the cards from the shuffled deck
    for (int i = 0; i < N_PLAYER; ++i) {
        for (int j = 0; j < N_CARD_IN_HAND; ++j) {
            playerCards[i][j] = deck.getCard(i * N_CARD_IN_HAND + j);
            playerUseds[i][j] = LAST_ACTION_INDEX + 1;
        }
        // Order the cards        
        std::sort(playerCards[i].begin(), playerCards[i].end(), Card::compareCard);
    }
}


void PartyState::init() {
    playerHands.deal();
}

void PartyState::getCardsInHand(CardVector& cardVector, int player_, int index_) {
    playerHands.clearActionIndex(index_);
    for (int i = 0; i < N_CARD_IN_HAND; ++i) {
        if (playerHands.getUsed(player_, i) < index_) continue;
        cardVector.push_back(playerHands.getCard(player_, i));
    }
}

void PartyState::getPlayableCards(CardVector& cardVector, int index_) {
    playerHands.clearActionIndex(index_);
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);

    if (posInRound == 0) {
        for (int i = 0; i < N_CARD_IN_HAND; ++i) {
            if (playerHands.getUsed(playerToHit, i) < index_) continue;
            cardVector.push_back(playerHands.getCard(playerToHit, i));            
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

void PartyState::simplifyPlayableCards(CardVector& cardVector) {
    for (int i = 0; i < cardVector.size() - 1; ++i) {
        int series_length = 0;
        Card cardI = cardVector[i];
        for (int j = i + 1; j < cardVector.size(); ++j) {
            Card cardJ = cardVector[j];
            if (cardI.isNextInSeries(cardJ)) {
                cardI = cardJ;
                series_length++;
            }
            else {
                break;
            }
        }
        for (int k = 0; k < series_length; ++k) {
            cardVector.erase(cardVector.begin() + i + 1);
        }
    }
}

void PartyState::setHitCard(int index_, Card& card_) {
    actionList.setCard(index_, card_);
    int playerToHit = actionList.getPlayerToHit(index_);
    playerHands.setUsed(playerToHit, card_, index_);
}

void PartyState::setNextPlayer(int index_) {
    int round = actionList.getRound(index_);
    int posInRound = actionList.getPosInRound(index_); 
    
    if (posInRound == 0 || posInRound == 1) {
        int playerToHit = actionList.getPlayerToHit(index_);
        actionList.setPlayerToHit(index_ + 1, (playerToHit + 1) % N_PLAYER);
    }
    else { // posInRound == 2
        // Evaluate round, determine winner player and won points
        Card card0 = actionList.getCard(index_ - 2);
        Card card1 = actionList.getCard(index_ - 1);
        Card card2 = actionList.getCard(index_);
        int winCardIndex = chooseWinnerCard(card0, card1, card2);
        int firstPlayerInRound = actionList.getPlayerToHit(index_ - 2);
        int winnerPlayer = (firstPlayerInRound + winCardIndex) % N_PLAYER;
        if (!actionList.isLastIndex(index_))
            actionList.setPlayerToHit(index_ + 1, winnerPlayer);
        roundResults.setWinner(round, winnerPlayer);
        roundResults.setPoint(round, card0.getPoint() + card1.getPoint() + card2.getPoint());
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

uint8_t PartyState::evaluateParty(int index_) {
    // TODO integrate with setNextPlayer() if possible
    int round = actionList.getRound(index_);
    int posInRound = actionList.getPosInRound(index_);

    int playerPoints = 0, opponentPoints = 0;
    if (posInRound == 0 || posInRound == 1) {
        return RESULT_UNDEFINED;
    }
    else { // posInRound == 2
        for (int i = 0; i <= round; ++i) {
            int point = roundResults.getPoint(i);
            if (roundResults.isLastRound(i)) point++;
            if (roundResults.getWinner(i) == actionList.getFirstPlayer()) 
                playerPoints += point;
            else 
                opponentPoints += point;
        }
    }
    if (playerPoints > MIN_POINT_TO_WIN) return PLAYER_WIN; // Player wins
    if (opponentPoints > MIN_POINT_TO_WIN) return OPPONENT_WIN; // Opponents win
    assert(!actionList.isLastIndex(index_));
    return RESULT_UNDEFINED;
}

void PartyState::print_current_state(int index_, const CardVector& playableCards) {
    int round = actionList.getRound(index_);
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);
    int roundStartPlayer = actionList.getRoundStartPlayer(index_); 

    std::cout << "=== Index: " << index_ << "  Round: " << round << "  Pos: " << posInRound << "  =====================" << std::endl;
    // Print each players' hand
    for (int player = 0; player < N_PLAYER; ++player) {
        // Signal current player to hit with asterisk
        std::cout << (player == roundStartPlayer ? "* " : "  ") << (player == playerToHit ? "->" : "  ") << " Player " << player << ": ";
        for (int i = 0; i < N_CARD_IN_HAND; ++i) {
            // Card is not played yet
            assert(playerHands.getUsed(player, i) != index_);
            if (playerHands.getUsed(player, i) > index_) {                 
                std::cout << playerHands.getCard(player, i) << " ";
            }
            // Card is already played
            else {                
                std::cout << "   ";
            }
        }
        // Print playable cards of current player
        if (player == playerToHit) {
            std::cout << "Playable: ";
            for (Card card : playableCards) {
                std::cout << card << " ";
            }
        }
        std::cout << std::endl;
    }
    // Print current table
    std::cout << "Table: ";
    for (int i = 0; i < posInRound; ++i) {
        Card card = actionList.getCard(index_ - posInRound + i);
        std::cout << card << " ";
    }
    std::cout << std::endl;
}

void PartyState::print_game_progression() {
    std::cout << "=== Game Progression: =====================" << std::endl;
    // Print each players' initial hand
    for (int player = 0; player < N_PLAYER; ++player) {
        std::cout << "Player " << player << ": ";
        for (int i = 0; i < N_CARD_IN_HAND; ++i) {
            std::cout << playerHands.getCard(player, i) << " ";
        }
        std::cout << std::endl;
    }
    // Print each round
    std::cout << "Rounds" << std::endl;
    for (int i = 0; i < N_PLAYER * N_CARD_IN_HAND; ++i) {
        std::cout << actionList.getRound(i) << " " << actionList.getPosInRound(i) << " " << actionList.getPlayerToHit(i) << " " << actionList.getCard(i);
    }

}

}