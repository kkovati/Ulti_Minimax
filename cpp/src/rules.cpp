#include <algorithm>
#include <assert.h>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

#include "rules.hpp"

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


void PlayerHands::init_deal(const std::vector<Card>& cardVector) {
    assert(cardVector.size() == N_PLAYER * N_CARD_IN_HAND);
    for (int i = 0; i < N_PLAYER; ++i) {
        for (int j = 0; j < N_CARD_IN_HAND; ++j) {
            playerCards[i][j] = cardVector[i * N_CARD_IN_HAND + j];
            playerUseds[i][j] = LAST_ACTION_INDEX + 1;
        }
        // Order the cards        
        std::sort(playerCards[i].begin(), playerCards[i].end(), Card::compareCard);
    }
}

void PlayerHands::random_deal() {
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


void PartyState::init(const std::string& deal) {
    // Parse deal string
    if (!deal.empty()) {   
        // Deal code structure:
        // deal[0]		= game type
        // deal[1]		= trump [0-3]
        // deal[2...61]	= cards
        if (!(deal.length() == 2 + N_ACTION * 2)) throw std::invalid_argument("Invalid deal code length");

        // Game type
        char gameTypeChar = deal[0];
        if (!std::isdigit(gameTypeChar)) throw std::invalid_argument("Invalid game type code");
        gameType = gameTypeChar - '0'; // Convert char to int
        if(!(0 <= gameType && gameType <= 3)) throw std::invalid_argument("Invalid game type code");

        // Trump
        char trumpChar = deal[1];
        if (!std::isdigit(trumpChar)) throw std::invalid_argument("Invalid trump code");
        trump = trumpChar - '0';
        if (!(0 <= trump && trump <= 3)) throw std::invalid_argument("Invalid trump code");
        // Check if no-trump game type is selected
        const std::unordered_set<uint8_t> noTrumpGameTypes(std::begin(NO_TRUMP_GAMES), std::end(NO_TRUMP_GAMES));
        if (noTrumpGameTypes.find(gameType) != noTrumpGameTypes.end()) {
            trump = NO_TRUMP_CODE;
            // TODO ace - king order !!!!!!!
        }
        
        std::vector<Card> cardVector;
        for (int i = 2; i < deal.length(); i += 2) {
            char suitChar = deal[i];
            char valueChar = deal[i + 1];
            if (!std::isdigit(suitChar) || !std::isdigit(valueChar)) throw std::invalid_argument("Invalid card code");
            int suit = suitChar - '0'; // Convert char to int
            int value = valueChar - '0';
            if (suit < 0 || 3 < suit || value < 0 || 7 < value) throw std::invalid_argument("Invalid card code");
            cardVector.push_back(Card(suit, value));
        }
        // Check duplicates
        for (int i = 0; i < cardVector.size(); ++i) {
            for (int j = i + 1; j < cardVector.size(); ++j) {
                Card c0 = cardVector[i];
                Card c1 = cardVector[j];
                if (c0 == c1 && !(c0 < c1) && !(c0 > c1)) throw std::invalid_argument("Duplicate card in code");
            }
        }
        // Init the deal
        playerHands.init_deal(cardVector);
    }
    // Random deal
    else {
        gameType = 0;
        trump = 0;
        playerHands.random_deal();
    }    
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

void PartyState::setHitCard(int index_, const Card& card_) {
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
        int winCardIndex = chooseWinnerCard(card0, card1, card2, trump);
        int firstPlayerInRound = actionList.getPlayerToHit(index_ - 2);
        int winnerPlayer = (firstPlayerInRound + winCardIndex) % N_PLAYER;
        if (!actionList.isLastIndex(index_))
            actionList.setPlayerToHit(index_ + 1, winnerPlayer);
        roundResults.setWinner(round, winnerPlayer);
        roundResults.setPoint(round, card0.getPoint() + card1.getPoint() + card2.getPoint());
    } 
}

// Chooses the best card in a round. The 3 input cards are in order of hit.
//
int PartyState::chooseWinnerCard(const Card c0, const Card c1, const Card c2, uint8_t trump) {
    // This method concentrates on cases when the second card can win
    // (c0 == c1 && c0 > c1) || (c0 != c1) == !(c0 == c1 && c0 < c1)
    // c0 beats c1
    bool c0bc1 = !((!c0.isTrump(trump) && c1.isTrump(trump)) || (c0 == c1 && c0 < c1));
    // c0 beats c2
    bool c0bc2 = !((!c0.isTrump(trump) && c2.isTrump(trump)) || (c0 == c2 && c0 < c2));

    if (c0bc1 && c0bc2) return 0;
    if (c0bc1 && !c0bc2) return 2;
    if (!c0bc1 && c0bc2) return 1;

    // both c1 and c2 beats c0
    if (!((!c1.isTrump(trump) && c2.isTrump(trump)) || (c1 == c2 && c1 < c2))) return 1;
    return 2;
}


uint8_t PartyState::evaluateParty(int index, bool print) {
    switch (gameType) {
    case NO_TRUMP_PARTY:
        return evaluateNoTrumpParty(index, print);
    case TRUMP_PARTY:
        return evaluateTrumpParty(index, print);
    case _40100:
        return evaluate40100(index, print);
    case _4ACES:
        return evaluate4Aces(index, print);
    case ULTI:
        return evaluateUlti(index, print);
    case BETLI:
        return evaluateBetli(index, print);
    case NO_TRUMP_DURCHMARS:
        return evaluateNoTrumpDurchmars(index, print);
    case DURCHMARS:
        return evaluateDurchmars(index, print);
    case _20100:
        return evaluate20100(index, print);
    case _4TENS:
        return evaluate4Tens(index, print);
    default:
        throw std::invalid_argument("Invalid game type code");
    }
}

uint8_t PartyState::evaluateNoTrumpParty(int index_, bool print) {
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

    if (DEBUG || print) std::cout << "Player Points: " << playerPoints << "  Opponent Points: " << opponentPoints << std::endl;

    if (playerPoints > MIN_POINT_TO_WIN) return PLAYER_WIN; // Player wins
    if (opponentPoints > MIN_POINT_TO_WIN) return OPPONENT_WIN; // Opponents win
    assert(!actionList.isLastIndex(index_));
    return RESULT_UNDEFINED;
}

uint8_t PartyState::evaluateTrumpParty(int index_, bool print) { return 0; }
uint8_t PartyState::evaluate40100(int index_, bool print) { return 0; }
uint8_t PartyState::evaluate4Aces(int index_, bool print) { return 0; }
uint8_t PartyState::evaluateUlti(int index_, bool print) { return 0; }
uint8_t PartyState::evaluateBetli(int index_, bool print) { return 0; }
uint8_t PartyState::evaluateNoTrumpDurchmars(int index_, bool print) { return 0; }
uint8_t PartyState::evaluateDurchmars(int index_, bool print) { return 0; }
uint8_t PartyState::evaluate20100(int index_, bool print) { return 0; }
uint8_t PartyState::evaluate4Tens(int index_, bool print) { return 0; }

void PartyState::print_current_state(int index_, const CardVector& playableCards) {
    int round = actionList.getRound(index_);
    int posInRound = actionList.getPosInRound(index_);
    int playerToHit = actionList.getPlayerToHit(index_);
    int roundStartPlayer = actionList.getRoundStartPlayer(index_); 

    std::cout << std::endl << "=== Index: " << index_ << "  Round: " << round << "  Pos: " << posInRound << "  =====================" << std::endl;
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

void PartyState::print_card(int index_) {
    std::cout << "Selected card: " << actionList.getCard(index_) << std::endl;
}

// NOTE: This function is not in use 
//
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