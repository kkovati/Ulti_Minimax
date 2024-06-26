#pragma once
#include <array>
#include <assert.h>
#include <cstdint>
#include <string>
#include <vector>

namespace ulti_minimax {

constexpr int N_SUIT = 4;
constexpr int N_VALUE = 8;
constexpr int LOWEST_CARD_VALUE_WITH_POINT = 6;
constexpr int N_POINT_IN_DECK = (N_VALUE - LOWEST_CARD_VALUE_WITH_POINT) * N_SUIT;

constexpr int N_PLAYER = 3; // Do not modify
constexpr int N_CARD_IN_HAND = 10;
constexpr int LAST_ACTION_INDEX = N_PLAYER * N_CARD_IN_HAND - 1;

constexpr int SEED = 0;

class Card {
public:
	Card() = default;
	Card(int suit_, int value_) : suit(suit_), value(value_) {};
	Card(const Card& other) : suit(other.suit), value(other.value) {};
	Card& operator=(const Card& other) { suit = other.suit; value = other.value; return *this; };

	inline int getSuit() { return suit; };
	inline int getValue() { return value; };
	inline int getPoint() { return value >= LOWEST_CARD_VALUE_WITH_POINT; };

	inline bool operator==(const Card& other) const { return suit == other.suit; } // True if same suit
	inline bool operator!=(const Card& other) const { return suit != other.suit; } // True if different suit
	inline bool operator>(const Card& other) const { return value > other.value; }	// True if greater value
	inline bool operator<(const Card& other) const { return value < other.value; }	// True if smaller value
		
	inline bool isNextInSeries(const Card& other) const { return suit == other.suit && value + 1 == other.value && other.value != LOWEST_CARD_VALUE_WITH_POINT; }

	static bool compareCard(const Card&, const Card&);

private:
	int suit;
	int value;
};


class Deck {
public:
	Deck();

	void shuffle();
	std::vector<std::vector<Card>> deal(int, int);
	inline int size() { return cards.size(); };
	inline Card getCard(int index_) { return cards[index_]; };

private:
	std::array<Card, N_SUIT * N_VALUE> cards;
};


class Action {
public:
	Action() = default;
	Action(int round_, int posInRound_) : round(round_), posInRound(posInRound_), 
		playerToHit(-1), card() {};
	Action(const Action& other) : round(other.round), posInRound(other.posInRound), 
		playerToHit(other.playerToHit), card(other.card) {};
	Action& operator=(const Action& other) {
		round = other.round; posInRound = other.posInRound;
		playerToHit = other.playerToHit; card = other.card; return *this;
	};

	inline int getRound() { return round; };
	inline int getPosInRound() { return posInRound; };
	inline int getPlayerToHit() { return playerToHit; };
	inline void setPlayerToHit(int playerToHit_) { playerToHit = playerToHit_; };
	inline Card getCard() { return card; };
	inline void setCard(Card card_) { card = card_; };

private:
	int round;			// Current round
	int posInRound;		// Position in round
	int playerToHit;	// Player to hit in this round in this position
	Card card;			// Card hit by the player
};


class ActionList {
public:
	ActionList(int firstPlayer_ = 0);	
	ActionList(const ActionList& other) : firstPlayer(other.firstPlayer), actions(other.actions) {};
	ActionList& operator=(const ActionList& other) { 
		firstPlayer = other.firstPlayer; actions = other.actions; return *this; 
	};

	inline int getFirstPlayer() { return firstPlayer; };
	inline int getRound(int index_) { return getAction(index_).getRound(); };
	inline int getPosInRound(int index_) { return getAction(index_).getPosInRound(); };
	inline int getPlayerToHit(int index_) { return getAction(index_).getPlayerToHit(); };
	inline void setPlayerToHit(int index_, int player_) { actions[index_].setPlayerToHit(player_); };
	inline Card getCard(int index_) { return getAction(index_).getCard(); };
	inline void setCard(int index_, Card card_) { actions[index_].setCard(card_); };

	inline bool isLastIndex(int index_) { return index_ >= actions.size() - 1; };
	inline bool isFirstPlayerToHit(int index_) { return firstPlayer == getPlayerToHit(index_); };

private:
	using ActionVector = std::array<Action, N_PLAYER * N_CARD_IN_HAND>;

	int firstPlayer;
	ActionVector actions;	

	inline Action getAction(int index_) { return actions[index_]; };
};


class RoundResults {
public:
	RoundResults() = default;
	RoundResults(const RoundResults& other) : winner(other.winner), point(other.point) {};
	RoundResults& operator=(const RoundResults& other) {
		winner = other.winner; point = other.point; return *this;
	};

	inline uint8_t getWinner(int round) { return winner[round]; };
	inline uint8_t getPoint(int round) { return point[round]; };
	inline void setWinner(int round, int player_) { winner[round] = player_; };
	inline void setPoint(int round, int point_) { point[round] = point_; };

private:
	std::array<int, N_CARD_IN_HAND> winner;
	std::array<int, N_CARD_IN_HAND> point;
};


class PlayerHands {
public:
	PlayerHands() = default;
	PlayerHands(const PlayerHands& other) : playerCards(other.playerCards), playerUseds(other.playerUseds) {};
	PlayerHands& operator=(const PlayerHands& other) {
		playerCards = other.playerCards; playerUseds = other.playerUseds; return *this;
	};

	void deal();

	inline const Card getCard(int player_, int index_) const { 
		return playerCards[player_][index_];
	};
	inline int getUsed(int player_, int index_) const {
		return playerUseds[player_][index_];
	};
	inline void setUsed(int player_, const Card& card_, int actionIndex_) {
		clearActionIndex(actionIndex_);
		for (int i = 0; i < N_CARD_IN_HAND; ++i) {
			Card curr_card = getCard(player_, i);
			if (curr_card != card_ || curr_card > card_ || curr_card < card_) continue;
			assert(curr_card == card_ && !(curr_card > card_) && !(curr_card < card_));
			assert(getUsed(player_, i) > actionIndex_);
			setUsed(player_, i, actionIndex_);
			return;
		}
		assert(false);
	};
	inline void clearActionIndex(int actionIndex_) {
		int counter = 0;
		for (int player = 0; player < N_PLAYER; ++player) {
			for (int i = 0; i < N_CARD_IN_HAND; ++i) {
				if (playerUseds[player][i] == actionIndex_) {
					playerUseds[player][i] = LAST_ACTION_INDEX + 1;
					counter++;
				}
			}
		}
		assert(counter <= 1);
	}

private:
	using CardArray = std::array<Card, N_CARD_IN_HAND>;
	using PlayerCardArray = std::array<CardArray, N_PLAYER>;
	using IntArray = std::array<int, N_CARD_IN_HAND>;
	using PlayerUsedArray = std::array<IntArray, N_PLAYER>;

	PlayerCardArray playerCards;
	PlayerUsedArray playerUseds; // Action index when that card was used

	inline void setUsed(int player_, int index_, int actionIndex_) {
		playerUseds[player_][index_] = actionIndex_;
	}
};


class PartyState {
public:
	PartyState() = default;
	PartyState(const PartyState& other) : actionList(other.actionList), roundResults(other.roundResults), 
		playerHands(other.playerHands) {};
	PartyState& operator=(const PartyState& other) {
		actionList = other.actionList; roundResults = other.roundResults; playerHands = other.playerHands;
		return *this;
	};

	using CardVector = std::vector<Card>;

	void init();

	void getCardsInHand(CardVector&, int, int);
	void getPlayableCards(CardVector&, int);
	void simplifyPlayableCards(CardVector&);
	void setHitCard(int, Card);
	void setNextPlayer(int);
	int chooseWinnerCard(Card, Card, Card);

	inline bool isLastIndex(int index_) { return actionList.isLastIndex(index_); };
	inline bool isFirstPlayerToHit(int index_) { return actionList.isFirstPlayerToHit(index_); };

	int evaluateParty(int);

	void print_current_state(int, const CardVector&);
	void print_game_progression();

private:
	ActionList actionList = ActionList();
	RoundResults roundResults = RoundResults();
	PlayerHands playerHands = PlayerHands();			
};

}