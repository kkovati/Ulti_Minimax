#pragma once
#include <array>
#include <assert.h>
#include <cstdint>
#include <string>
#include <vector>

namespace ulti_minimax {

constexpr int N_SUIT = 4;
constexpr int N_VALUE = 8;
constexpr int N_PLAYER = 3; // Do not modify
constexpr int N_CARD_IN_HAND = 10;
constexpr int LAST_ACTION_INDEX = N_PLAYER * N_CARD_IN_HAND - 1;
constexpr int SEED = 0;

class Card {
public:
	Card() = default;
	Card(int suit_, int value_) : suit(suit_), value(value_) {};
	Card(const Card& other) : suit(other.suit), value(other.value) {};
	Card& operator=(const Card& other) { suit = other.suit; value = other.value; return *this; }

	inline int getSuit() { return suit; };
	inline int getValue() { return value; };

	inline bool operator==(const Card& other) const { return suit == other.suit; } // True if same suit
	inline bool operator!=(const Card& other) const { return suit != other.suit; } // True if different suit
	inline bool operator>(const Card& other) const { return value > other.value; }	// True if greater value
	inline bool operator<(const Card& other) const { return value < other.value; }	// True if smaller value
		
	inline bool isNextInSeries(const Card& other) const { return suit == other.suit && value + 1 == other.value; }

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
	Action(int round_, int posInRound_) : round(round_), posInRound(posInRound_), playerToHit(-1), card() {};

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

	inline int getRound(int index_) { return getAction(index_).getRound(); };
	inline int getPosInRound(int index_) { return getAction(index_).getPosInRound(); };
	inline int getPlayerToHit(int index_) { return getAction(index_).getPlayerToHit(); };
	inline void setPlayerToHit(int index_, int player_) { actions[index_].setPlayerToHit(player_); };
	inline Card getCard(int index_) { return getAction(index_).getCard(); };
	inline void setCard(int index_, Card card_) { actions[index_].setCard(card_); };

	inline bool isLastIndex(int index_) { return index_ >= actions.size() - 1; };

private:
	using ActionVector = std::array<Action, N_PLAYER * N_CARD_IN_HAND>;

	int firstPlayer;
	ActionVector actions;	

	inline Action getAction(int index_) { return actions[index_]; };
};


class RoundResults {
public:
	RoundResults() = default;

	inline uint8_t getWinner(uint8_t round) { return winner[round]; };
	inline uint8_t getPouint8_ts(uint8_t round) { return pouint8_ts[round]; };
	inline void setWinner(uint8_t round, uint8_t player_) { winner[round] = player_; };
	inline void setPouint8_ts(uint8_t round, uint8_t pouint8_ts_) { pouint8_ts[round] = pouint8_ts_; };

private:
	std::array<uint8_t, N_CARD_IN_HAND> winner;
	std::array<uint8_t, N_CARD_IN_HAND> pouint8_ts;
};


class PlayerHands {
public:
	PlayerHands() = default;

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

	using CardVector = std::vector<Card>;

	void init();

	void getCardsInHand(CardVector&, int, int);
	void getPlayableCards(CardVector&, int);
	void simplifyPlayableCards(CardVector&);
	void setHitCard(int, Card);
	void setNextPlayer(int);
	int chooseWinnerCard(Card, Card, Card);

	inline bool isLastIndex(int index_) { return actionList.isLastIndex(index_); };

	int evaluateParty();

	void print(int, const CardVector&);

private:
	ActionList actionList = ActionList();
	PlayerHands playerHands = PlayerHands();			
};

}