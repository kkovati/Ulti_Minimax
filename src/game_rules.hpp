#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace ulti_minimax {

constexpr int N_SUIT = 4;
constexpr int N_VALUE = 8;
constexpr int N_PLAYER = 3;
constexpr int N_CARD_IN_HAND = 10;
constexpr int LAST_ACTION_INDEX = N_PLAYER * N_CARD_IN_HAND - 1;
constexpr int SEED = 0;

class Card {
public:
	Card() = default;
	Card(int suit_, int value_) : suit(suit_), value(value_) {};
	Card(const Card& other) : suit(other.suit), value(other.value) {};

	bool operator==(const Card&) const; // True if equal suit
	bool operator>(const Card&) const;	// True greater value
	bool operator<(const Card&) const;	// True smaller value
	//bool operator>(const Card&) const;	// True if different suit or equal suit and greater value
	//bool operator<(const Card&) const;	// True if equal suit and smaller value

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
	inline Card getCard(int index_) { return getAction(index_).getCard(); };

private:
	using ActionVector = std::array<Action, N_PLAYER * N_CARD_IN_HAND>;

	int firstPlayer;
	ActionVector actions;	

	Action getAction(int index_) { return actions[index_]; };
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
	inline void setUsed(int player_, int index_, int actionIndex_) {
		playerUseds[player_][index_] = actionIndex_;
	};

private:
	using CardArray = std::array<Card, N_CARD_IN_HAND>;
	using PlayerCardArray = std::array<CardArray, N_PLAYER>;
	using IntArray = std::array<int, N_CARD_IN_HAND>;
	using PlayerUsedArray = std::array<IntArray, N_PLAYER>;

	PlayerCardArray playerCards;
	PlayerUsedArray playerUseds; // Action index when that card was used
};


class PartyState {
public:
	PartyState() = default;

	using CardVector = std::vector<Card>;

	void init();

	bool isSameSuit(const CardVector&, const Card) const;
	bool isLargerValue(const CardVector&, const Card&) const;
	bool isLargerValue(const CardVector&, const Card&, const Card&) const;

	void getPlayableCards(CardVector&, int);
	void setNextPlayer();

private:
	ActionList actionList = ActionList();
	PlayerHands playerHands = PlayerHands();			
};

}