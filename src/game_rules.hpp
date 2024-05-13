#pragma once
#include <cstdint>
#include <vector>

namespace ulti_minimax {

class Card {
public:
	Card() = default;
	Card(uint8_t suit, uint8_t value);
	Card(const Card& other);

	bool operator==(const Card&) const;
	bool operator>(const Card&) const;
	bool operator<(const Card&) const;

private:
	uint8_t suit;
	uint8_t value;
};


class Deck {
public:
	Deck();

	void shuffle();
	std::vector<std::vector<Card>> deal(int, int);
	inline int getSize() { return cards.size(); };
	inline Card getCard(int index) { return cards[index]; };

private:
	static constexpr uint8_t nSuit = 4;
	static constexpr uint8_t nValue = 8;

	std::vector<Card> cards;
};


class Action {
public:
	Action() = default;
	Action(int round, int playerHits) : round(round), playerHits(playerHits) {};

private:
	int round;
	int playerHits;
};


class ActionList {
public:
	ActionList() = default;
	ActionList(int, int, int);

	inline int getIndex() { return index; };
	Action getAction() { return actions[index]; };


private:
	using ActionVector = std::vector<Action>;

	int nPlayer, nCardInHand, firstPlayer;
	int index;
	ActionVector actions;	
};


class PlayerHands {
public:
	PlayerHands() = default;
	PlayerHands(int, int);

	void deal();

	inline int getNPlayer() { return nPlayer; };
	inline int getNCardInHand() { return nCardInHand; };
	inline const Card getCard(int player, int index) const {};

private:
	using CardVector = std::vector<Card>;
	using PlayerCardVector = std::vector<CardVector>;
	using IntVector = std::vector<int>;
	using PlayerUsedVector = std::vector<IntVector>;

	int nPlayer, nCardInHand;
	PlayerCardVector playerCards;
	PlayerUsedVector playerUseds;
};


class PartyState {
public:
	PartyState() = default;

	void deal();

	std::vector<Card> get_playable_cards();

private:
	const int nPlayer = 3;
	const int nCardInHand = 10;	

	PlayerHands playerHands = PlayerHands(nPlayer, nCardInHand);
	//ActionList actionList = ActionList(nPlayer, nCardInHand);
		
};

}