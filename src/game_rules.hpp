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
	Action getAction() { return actionList[index]; };


private:
	using ActionVector = std::vector<Action>;

	int nPlayer, nCardInHand, firstPlayer;
	int index;
	ActionVector actionList;	
};


class PlayerHands {
public:
	PlayerHands() = default;
	PlayerHands(int, int);

private:
	using PlayerCardVector = std::vector<std::vector<Card>>;
	using PlayerUsedVector = std::vector<std::vector<int>>;

	int nPlayer, nCardInHand;

};


class Deck {
public:
	Deck();

	std::vector<std::vector<Card>> deal(int a, int b, PlayerHands c);

private:
	static constexpr uint8_t N_SUIT = 4;
	static constexpr uint8_t N_VALUE = 8;

	std::vector<Card> cards;
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