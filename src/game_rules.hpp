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
	Action(int round, int posInRound) : round(round), posInRound(posInRound) {};

	inline int getRound() { return round; };
	inline int getPosInRound() { return posInRound; };
	inline int getPlayerToHit() { return playerToHit; };
	inline void setPlayerToHit(int playerToHit) { playerToHit = playerToHit; };

private:
	int round;  // Current round
	int posInRound; // Position in round
	int playerToHit;  // Player to hit
};


class ActionList {
public:
	ActionList(int nPlayer, int nCardInHand, int firstPlayer = 0);

	inline int getIndex() { return index; };
	inline int setIndex(int index_) { index = index_; };
	Action getAction() { return actions[index]; };
	inline int getPosInRound() { return getAction().getPosInRound(); };
	inline int getPlayerToHit() { return getAction().getPlayerToHit(); };

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
	inline const Card getCard(int player_, int index_) const { 
		return playerCards[player_][index_];
	};
	inline int getUsed(int player_, int index_) const {
		return playerUseds[player_][index_];
	};

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

	void init();

	std::vector<Card> getPlayableCards();
	void setNextPlayer();

private:
	const int nPlayer = 3;
	const int nCardInHand = 10;	

	PlayerHands playerHands = PlayerHands(nPlayer, nCardInHand);
	ActionList actionList = ActionList(nPlayer, nCardInHand);		
};

}