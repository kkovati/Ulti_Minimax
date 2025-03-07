#include <algorithm>
#include <assert.h>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

#include "rules.hpp"

namespace ulti_minimax {

// Compare for sorting
//
bool Card::compareCard(const Card& a, const Card& b) {
    if (a == b) return a < b;
    return a.suit < b.suit;
}

// Check if the two cards are in series for simplified hitting
//
bool Card::isNextInSeries(const Card& other, uint8_t series_terminate_scheme, uint8_t trump) const {
    assert(suit <= other.suit);
    if (suit == other.suit) assert(value < other.value);

    if (series_terminate_scheme == NO_TERMINATE) {
        return suit == other.suit && value + 1 == other.value;
    }
    else if (series_terminate_scheme == TERMINATE_TRUMP_0) {
        // Terminate if this Card is trump 0
        return !(suit == trump && value == 0) && suit == other.suit && value + 1 == other.value;
    }
    else if (series_terminate_scheme == TERMINATE_6) {
        // Terminate between 5 and 6
        // this   other
        //    |   |
        //    V   V
        //  4 5 | 6 7 
        return value != 5 && suit == other.suit && value + 1 == other.value;
    }    
    else if (series_terminate_scheme == TERMINATE_7) {
        return value != 6 && suit == other.suit && value + 1 == other.value;
    }
    else if (series_terminate_scheme == TERMINATE_06) {
        // Terminate between 0 and 1 (and between 5 and 6 too)
        // this   other
        //    |   |
        //    V   V
        //    0 | 1 
        return value != 0 && value != 5 && suit == other.suit && value + 1 == other.value;
    }
    else if (series_terminate_scheme == TERMINATE_67) {
        return value != 5 && value != 6 && suit == other.suit && value + 1 == other.value;
    }
    else if (series_terminate_scheme == TERMINATE_067) {
        // This termination scheme is not in use
        throw std::runtime_error("Not implemented");
        return false;
    }
    else {
        throw std::runtime_error("Wrong argument 'series_terminate_scheme'");
        return false;
    }
};


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

// Find the two cards which were not dealt
//
std::array<Card, N_REST_CARD> Deck::findRestCards(const std::vector<Card>& dealtCards) const {
    assert(dealtCards.size() == N_PLAYER * N_CARD_IN_HAND);

    std::array<Card, N_REST_CARD> restCards;
    int i = 0;
    for (const Card& card : cards) {
        if (i >= N_REST_CARD) break;
        // Comparison logic b/c Card::operator== is used for suit comparison
        auto cardComparator = [card](const Card& c) {return card.equals(c); };
        if (std::find_if(dealtCards.begin(), dealtCards.end(), cardComparator) == dealtCards.end()) {
            restCards[i++] = card;
        }        
    }
    assert(i == N_REST_CARD);
    return restCards;
}


ActionList::ActionList(int firstPlayer_) : firstPlayer(firstPlayer_) {
    for (int i = 0; i < actions.size(); ++i) {
        actions[i] = Action(static_cast<int>(i / N_PLAYER), i % N_PLAYER);
    }    
    actions[0].setPlayerToHit(firstPlayer);
}


std::array<Card, N_REST_CARD> PlayerHands::init_deal(const std::vector<Card>& cardVector) {
    assert(cardVector.size() == N_PLAYER * N_CARD_IN_HAND);
    for (int i = 0; i < N_PLAYER; ++i) {
        for (int j = 0; j < N_CARD_IN_HAND; ++j) {
            playerCards[i][j] = cardVector[i * N_CARD_IN_HAND + j];
            playerUseds[i][j] = LAST_ACTION_INDEX + 1;
        }
        // Order the cards        
        std::sort(playerCards[i].begin(), playerCards[i].end(), Card::compareCard);
    }

    // Determine the two rest cards
    Deck deck = Deck();
    return deck.findRestCards(cardVector);
}

std::array<Card, N_REST_CARD> PlayerHands::random_deal() {
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

    // Determine the two rest cards
    return std::array<Card, N_REST_CARD> {deck.getCard(N_PLAYER * N_CARD_IN_HAND), deck.getCard(N_PLAYER * N_CARD_IN_HAND + 1)};
}


// Initialize based on the deal string or randomly
// Return true if prerequsites are okay
//
bool PartyState::init(const std::string& deal) {
    // Parse deal string
    if (!deal.empty()) {   
        // Deal code structure:
        // deal[0]		= game type
        // deal[1]		= trump [0-3]
        // deal[2...61]	= cards [suit, value]
        if (!(deal.length() == 2 + N_ACTION * 2)) throw std::invalid_argument("Invalid deal code length");

        // Game type
        char gameTypeChar = deal[0];
        if (!std::isdigit(gameTypeChar)) throw std::invalid_argument("Invalid game type code");
        gameType = gameTypeChar - '0'; // Convert char to int
        if (!(0 <= gameType && gameType <= 9)) throw std::invalid_argument("Invalid game type code");

        // Trump
        char trumpChar = deal[1];
        if (!std::isdigit(trumpChar)) throw std::invalid_argument("Invalid trump code");
        trump = trumpChar - '0'; // Convert char to int
        if (!(0 <= trump && trump <= 3)) throw std::invalid_argument("Invalid trump code");

        // Check if no-trump game type is selected
        // TODO this should be obsolete and done at each game type prerequisite part
        if (std::find(std::begin(NO_TRUMP_GAMES), std::end(NO_TRUMP_GAMES), gameType) != std::end(NO_TRUMP_GAMES)) {
            trump = NO_TRUMP_CODE;
        }   
        
        // Parse cards from deal
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
                if (c0.equals(c1)) throw std::invalid_argument("Duplicate card in code");
            }
        }
        
        // Init the deal and determine rest cards
        auto restCards = playerHands.init_deal(cardVector);
        restCard0 = restCards[0];
        restCard1 = restCards[1];
    }
    // Random deal (deal string is empty)
    else {
        gameType = NO_TRUMP_PARTY;
        trump = NO_TRUMP_CODE;
        // Random deal and determine rest cards
        auto restCards = playerHands.random_deal();
        restCard0 = restCards[0];
        restCard1 = restCards[1];
    } 
        
    // Check if selected trump matches the game type
    if (std::find(std::begin(NO_TRUMP_GAMES), std::end(NO_TRUMP_GAMES), gameType) != std::end(NO_TRUMP_GAMES)) {
        if (trump != NO_TRUMP_CODE) throw std::invalid_argument("No trump game has trump selected");
    } 
    else {
        if (!(trump < NO_TRUMP_CODE)) throw std::invalid_argument("Trump game has no trump selected");
    }

    // Check prerequisites and series terminating scheme for simplification
    bool prerequisite = false;
    assert(series_terminate_scheme == 0);
    // NO_TRUMP_PARTY
    if (gameType == NO_TRUMP_PARTY) {
        prerequisite = true; // No prerequisites
        series_terminate_scheme = TERMINATE_6;
    }
    // TRUMP_PARTY
    else if (gameType == TRUMP_PARTY) {
        prerequisite = true; // No prerequisites
        series_terminate_scheme = TERMINATE_6;
    }
    // 40100
    // Check if player has the trump3 and trump4
    else if (gameType == _40100) {
        bool trump3 = playerHands.checkCard(actionList.getFirstPlayer(), Card(trump, 3));
        bool trump4 = playerHands.checkCard(actionList.getFirstPlayer(), Card(trump, 4));
        prerequisite = trump3 && trump4;
        series_terminate_scheme = TERMINATE_6;
    }
    // 4ACES
    // Check if player has the trump7 (ace) or any ace is not in the rest cards
    else if (gameType == _4ACES) {
        prerequisite = playerHands.checkCard(actionList.getFirstPlayer(), Card(trump, 7)) && 
            restCard0.getValue() != 7 && restCard1.getValue() != 7;
        series_terminate_scheme = TERMINATE_7;
    }
    // ULTI
    // Check if player has the trump0 (lowest trump)
    else if (gameType == ULTI) {
        prerequisite = playerHands.checkCard(actionList.getFirstPlayer(), Card(trump, 0));
        series_terminate_scheme = TERMINATE_TRUMP_0; // NOTE: it can be TERMINATE_06
    }
    // BETLI
    else if (gameType == BETLI) {
        prerequisite = true; // No prerequisites
        series_terminate_scheme = NO_TERMINATE;
    }
    // NO_TRUMP_DURCHMARS
    else if (gameType == NO_TRUMP_DURCHMARS) {
        prerequisite = true; // No prerequisites
        series_terminate_scheme = NO_TERMINATE;
    }
    // DURCHMARS
    else if (gameType == DURCHMARS) {
        prerequisite = true; // No prerequisites
        series_terminate_scheme = NO_TERMINATE;
    }
    // 20100
    // Check if player has any non-trump 3 and 4
    else if (gameType == _20100) {
        for (int i = 0; i < NO_TRUMP_CODE; ++i) {
            if (i == trump) continue;
            bool card3 = playerHands.checkCard(actionList.getFirstPlayer(), Card(i, 3));
            bool card4 = playerHands.checkCard(actionList.getFirstPlayer(), Card(i, 4));
            if (card3 && card4) prerequisite = true;
        }
        series_terminate_scheme = TERMINATE_6;
    }
    // 4TENS
    // Check if any ten is not in the rest cards
    else if (gameType == _4TENS) {
        prerequisite = restCard0.getValue() != 6 && restCard1.getValue() != 6;
        series_terminate_scheme = TERMINATE_67;
    }

    return prerequisite;
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

// Decrease the number of playable cards. 
// If the player must choose between multiple cards, and these cards are in a series,
// it is indifferent which card the player chooses.
// This is because the other players do not have cards which can go between these cards so
// it does not have any effect on the final outcome of the game. The player can choose any of the
// cards is that series. 
// If the game type has special cards, like ace, ten or trump 7, then these cards cannot form a series.
//
void PartyState::simplifyPlayableCards(CardVector& cardVector) {
    for (int i = 0; i < cardVector.size() - 1; ++i) {
        int series_length = 0;
        Card cardI = cardVector[i];
        for (int j = i + 1; j < cardVector.size(); ++j) {
            Card cardJ = cardVector[j];
            if (cardI.isNextInSeries(cardJ, series_terminate_scheme, trump)) {
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
        roundResults.setSpecialCard(round, card0, card1, card2, trump);
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

// Evaluate if the party at the current state does have a final result.
// It always returns a definit result at the last index.
//
uint8_t PartyState::evaluateParty(int index_, bool print) {
    // TODO integrate with setNextPlayer() if possible
    int round = actionList.getRound(index_);
    int posInRound = actionList.getPosInRound(index_);

    int playerPoints = 0, opponentPoints = 0; // No. points won
    int playerRounds = 0, opponentRounds = 0; // No. rounds won
    int playerAces = 0, opponentAces = 0; // No. aces won
    int playerTens = 0, opponentTens = 0; // No. tens won
    // Note: Player owns trump0 is a prerequisite that is checked during initialization
    // True if player wins the last round with trump0
    bool playerUlti = false; 
    // True if opponents win any round where trump0 was played by player OR
    // player wins any round where trump0 was played and NOT last round
    bool opponentUlti = false; 

    // Count points from rest cards
    if (restCard0.getValue() >= LOWEST_CARD_VALUE_WITH_POINT) opponentPoints++;
    if (restCard1.getValue() >= LOWEST_CARD_VALUE_WITH_POINT) opponentPoints++;
    
    if (posInRound == 0 || posInRound == 1) {
        return RESULT_UNDEFINED;
    }
    else { // posInRound == 2
        for (int i = 0; i <= round; ++i) {
            int point = roundResults.getPoint(i);
            if (roundResults.isLastRound(i)) point++;
            int trump0 = roundResults.getTrump0(i);

            // Player wins round
            if (roundResults.getWinner(i) == actionList.getFirstPlayer()) {
                playerPoints += point;
                playerRounds++;
                playerAces += roundResults.getAce(i);
                playerTens += roundResults.getTen(i);
                if (trump0) {
                    if (roundResults.isLastRound(i)) playerUlti = true;
                    else opponentUlti = true;
                }
            }                
            else { // Opponents win round
                opponentPoints += point;
                opponentRounds++;
                opponentAces += roundResults.getAce(i);
                opponentTens += roundResults.getTen(i);
                if (trump0) opponentUlti = true;
            }   
        }
    }
    assert(playerPoints + opponentPoints <= N_POINT_IN_DECK + 1);
    assert(playerRounds + opponentRounds <= N_CARD_IN_HAND);
    assert(playerAces + opponentAces <= N_SUIT);
    assert(playerTens + opponentTens <= N_SUIT);
    assert(!(playerUlti && opponentUlti));

    if (DEBUG || print) std::cout << "Player Points: " << playerPoints << "  Opponent Points: " << opponentPoints << std::endl;

    // Evaluate the result of different game types    
    switch (gameType) {
    case NO_TRUMP_PARTY:
        if (playerPoints >= 5) return PLAYER_WIN;       // Player wins
        if (opponentPoints >= 5) return OPPONENT_WIN;   // Opponents win
        break;
    case TRUMP_PARTY:
        if (playerPoints >= 5) return PLAYER_WIN;       // Player wins
        if (opponentPoints >= 5) return OPPONENT_WIN;   // Opponents win
        break;
    case _40100:
        if (playerPoints >= 6) return PLAYER_WIN;       // Player wins
        if (opponentPoints >= 4) return OPPONENT_WIN;   // Opponents win
        break;
    case _4ACES:
        if (playerAces >= 4) return PLAYER_WIN;         // Player wins
        if (opponentAces >= 1) return OPPONENT_WIN;     // Opponents win
        break;
    case ULTI:
        if (playerUlti) return PLAYER_WIN;              // Player wins
        if (opponentUlti) return OPPONENT_WIN;          // Opponents win
        break;
    case BETLI:
        if (opponentRounds >= 10) return PLAYER_WIN;    // Player wins
        if (playerRounds >= 1) return OPPONENT_WIN;     // Opponents win
        break;
    case NO_TRUMP_DURCHMARS:
        if (playerRounds >= 10) return PLAYER_WIN;      // Player wins
        if (opponentRounds >= 1) return OPPONENT_WIN;   // Opponents win
        break;
    case DURCHMARS:
        if (playerRounds >= 10) return PLAYER_WIN;      // Player wins
        if (opponentRounds >= 1) return OPPONENT_WIN;   // Opponents win
        break;
    case _20100:
        if (playerPoints >= 8) return PLAYER_WIN;       // Player wins
        if (opponentPoints >= 2) return OPPONENT_WIN;   // Opponents win
        break;
    case _4TENS:
        if (playerTens >= 4) return PLAYER_WIN;         // Player wins
        if (opponentTens >= 1) return OPPONENT_WIN;     // Opponents win
        break;
    default:
        throw std::invalid_argument("Invalid game type code");
    }

    // There must be result in the final round (at the end of the game), 
    // so this code cannot be reached in final round. 
    // In simpler terms:
    // assert(!actionList.isLastIndex(index_)); 
    if (actionList.isLastIndex(index_)) {
        std::cout << std::endl << "Error: No result at the end of the game" << std::endl;
        std::cout << "gameType: " << static_cast<int>(gameType) << std::endl;
        std::cout << "playerPoints: " << playerPoints << " opponentPoints: " << opponentPoints << std::endl;
        std::cout << "playerRounds: " << playerRounds << " opponentRounds: " << opponentRounds << std::endl;
        std::cout << "playerAces: " << playerAces << " opponentAces: " << opponentAces << std::endl;
        std::cout << "playerTens: " << playerTens << " opponentTens: " << opponentTens << std::endl;
        std::cout << "playerUlti: " << playerUlti << " opponentUlti: " << opponentUlti << std::endl;
        throw std::runtime_error("No result at the end of the game");
    }

    return RESULT_UNDEFINED;
}

// Print the current state of the party to the console.
// This acts like a snapshot of the party at a given moment or in other words at a given node in the tree.
// The following infos are printed:
// - Where the game is at (index, round, position in round)
// - Player hands and that which cards a player can use in this moment
//   (highlighted player to hit and player started the current round)  
// - Cards on the table
//
void PartyState::printCurrentState(int index_, const CardVector& playableCards) {
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

// Print the card which was played at the given index
//
void PartyState::printCard(int index_) {
    std::cout << "Selected card: " << actionList.getCard(index_) << std::endl;
}

// Print the party step-by-step 
// NOTE: This function is not in use 
//
void PartyState::printGameProgression() {
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

// Generate a string which codes the game progression of each round
// gameProgression[0] = round starting player index 
// gameProgression[1] = card suit 
// gameProgression[2] = card value
// gameProgression[3] = next player index in the round
//
std::string PartyState::getGameProgressionStr(int last_index) {
    std::string gameProgression;
    for (int i = 0; i <= last_index; ++i) {
        gameProgression += std::to_string(actionList.getPlayerToHit(i));
        gameProgression += std::to_string(actionList.getCard(i).getSuit());
        gameProgression += std::to_string(actionList.getCard(i).getValue());
    }
    for (int i = last_index + 1; i < N_ACTION; ++i) {
        gameProgression += "999";
    }
    assert(gameProgression.size() == 3 * N_ACTION);
    return gameProgression;
}

}