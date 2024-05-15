#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace ulti_minimax {

constexpr int N_SUIT = 4;
constexpr int N_VALUE = 8;
constexpr int N_PLAYER = 3;
constexpr int N_CARD_IN_HAND = 10;
constexpr int SEED = 0;

class Card {
public:
	Card() = default;
	Card(int suit_, int value_) : suit(suit_), value(value_) {};
	Card(const Card& other) : suit(other.suit), value(other.value) {};

	bool operator==(const Card&) const;
	bool operator>(const Card&) const;
	bool operator<(const Card&) const;

private:
	int suit;
	int value;
};


class Deck {
public:
	Deck();

	void shuffle();
	std::vector<std::vector<Card>> deal(int, int);
	inline int getSize() { return cards.size(); };
	inline Card getCard(int index_) { return cards[index_]; };

private:
	std::array<Card, N_SUIT * N_VALUE> cards;
};


class Action {
public:
	Action() = default;
	Action(int round_, int posInRound_) : round(round_), posInRound(posInRound_), playerToHit(-1) {};

	inline int getRound() { return round; };
	inline int getPosInRound() { return posInRound; };
	inline int getPlayerToHit() { return playerToHit; };
	inline void setPlayerToHit(int playerToHit_) { playerToHit = playerToHit_; };

private:
	int round;  // Current round
	int posInRound; // Position in round
	int playerToHit;  // Player to hit
};


class ActionList {
public:
	ActionList(int firstPlayer = 0);

	Action getAction(int index_) { return actions[index_]; };
	inline int getPosInRound(int index_) { return getAction(index_).getPosInRound(); };
	inline int getPlayerToHit(int index_) { return getAction(index_).getPlayerToHit(); };

private:
	using ActionVector = std::vector<Action>;

	int firstPlayer;
	ActionVector actions;	
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

private:
	using CardArray = std::array<Card, N_CARD_IN_HAND>;
	using PlayerCardArray = std::array<CardArray, N_PLAYER>;
	using IntArray = std::array<int, N_CARD_IN_HAND>;
	using PlayerUsedArray = std::array<IntArray, N_PLAYER>;

	PlayerCardArray playerCards;
	PlayerUsedArray playerUseds;
};


class PartyState {
public:
	PartyState() = default;

	void init();

	std::vector<Card> getPlayableCards(int);
	void setNextPlayer();

private:
	ActionList actionList = ActionList();
	PlayerHands playerHands = PlayerHands();			
};

}