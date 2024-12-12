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

constexpr int N_PLAYER = 3;
constexpr int N_CARD_IN_HAND = 10;
constexpr int N_ACTION = N_PLAYER * N_CARD_IN_HAND;
constexpr int LAST_ACTION_INDEX = N_ACTION - 1;

constexpr int MIN_POINT_TO_WIN = static_cast<int>(N_POINT_IN_DECK / 2);

constexpr uint8_t RESULT_UNDEFINED = 0;
constexpr uint8_t PLAYER_WIN = 1;
constexpr uint8_t OPPONENT_WIN = 2;

constexpr int SEED = 0;
constexpr bool DEBUG = false;


class Card {
public:
	Card() = default;
	Card(int suit_, int value_) : suit(suit_), value(value_) {};
	Card(const Card& other) : suit(other.suit), value(other.value) {};
	Card& operator=(const Card& other) { 
		if (this != &other) {
			suit = other.suit;
			value = other.value;
		}
		return *this;
	};

	int getSuit() { return suit; };
	int getValue() { return value; };
	int getPoint() { return value >= LOWEST_CARD_VALUE_WITH_POINT; };

	bool operator==(const Card& other) const { return suit == other.suit; }; // True if same suit
	bool operator!=(const Card& other) const { return suit != other.suit; }; // True if different suit
	bool operator>(const Card& other) const { return value > other.value; }; // True if greater value
	bool operator<(const Card& other) const { return value < other.value; }; // True if smaller value
	static bool compareCard(const Card&, const Card&);
		
	bool isNextInSeries(const Card& other) const { return suit == other.suit && value + 1 == other.value && other.value != LOWEST_CARD_VALUE_WITH_POINT; };

	std::string toString() const { 
		std::string s = std::string(1, (char)('A' + suit)) + std::to_string(value);
		return s;
	};

	friend std::ostream& operator<< (std::ostream& os, const Card& card) {
		os << card.toString();
		return os;
	};

private:
	int suit;
	int value;
};


class Deck {
public:
	Deck();

	void shuffle();
	std::vector<std::vector<Card>> deal(int, int);
	int size() { return cards.size(); };
	Card getCard(int index_) { return cards[index_]; };

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
		if (this != &other) {
			round = other.round;
			posInRound = other.posInRound;
			playerToHit = other.playerToHit;
			card = other.card;
		}
		return *this;
	};

	int getRound() { return round; };
	int getPosInRound() { return posInRound; };
	int getPlayerToHit() { return playerToHit; };
	void setPlayerToHit(int playerToHit_) { playerToHit = playerToHit_; };
	const Card& getCard() { return card; };
	void setCard(const Card& card_) { card = card_; };

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
		if (this != &other) {
			firstPlayer = other.firstPlayer; actions = other.actions;
		}
		return *this;
	};

	int getFirstPlayer() { return firstPlayer; };
	int getRound(int index_) { return getAction(index_).getRound(); };
	int getPosInRound(int index_) { return getAction(index_).getPosInRound(); };
	int getPlayerToHit(int index_) { return getAction(index_).getPlayerToHit(); };
	void setPlayerToHit(int index_, int player_) { actions[index_].setPlayerToHit(player_); };
	const Card& getCard(int index_) { return getAction(index_).getCard(); };
	void setCard(int index_, const Card& card_) { actions[index_].setCard(card_); };

	bool isLastIndex(int index_) { return index_ >= LAST_ACTION_INDEX; };
	bool isFirstPlayerToHit(int index_) { return firstPlayer == getPlayerToHit(index_); };

	int getRoundStartPlayer(int index_) { return getPlayerToHit(index_ - getPosInRound(index_)); };

private:
	using ActionVector = std::array<Action, N_ACTION>;

	int firstPlayer;
	ActionVector actions;	

	Action getAction(int index_) { return actions[index_]; };
};


class RoundResults {
public:
	RoundResults() = default;
	RoundResults(const RoundResults& other) : winner(other.winner), point(other.point) {};
	RoundResults& operator=(const RoundResults& other) {
		if (this != &other) {
			winner = other.winner; point = other.point;
		}
		return *this;
	};

	uint8_t getWinner(int round) const { return winner[round]; };
	uint8_t getPoint(int round) const { return point[round]; };
	void setWinner(int round, int player_) { winner[round] = player_; };
	void setPoint(int round, int point_) { point[round] = point_; };
	bool isLastRound(int round) {
		assert(0 <= round && round <= N_CARD_IN_HAND - 1);
		return round == N_CARD_IN_HAND - 1;
	};

private:
	std::array<int, N_CARD_IN_HAND> winner;
	std::array<int, N_CARD_IN_HAND> point;
};


class PlayerHands {
public:
	PlayerHands() = default;
	PlayerHands(const PlayerHands& other) : playerCards(other.playerCards), playerUseds(other.playerUseds) {};
	PlayerHands& operator=(const PlayerHands& other) {
		if (this != &other) {
			playerCards = other.playerCards; playerUseds = other.playerUseds;
		}
		return *this;
	};

	void deal();

	const Card& getCard(int player_, int index_) const { 
		return playerCards[player_][index_];
	};
	int getUsed(int player_, int index_) const {
		return playerUseds[player_][index_];
	};
	void setUsed(int player_, const Card& card_, int actionIndex_) {
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
	void clearActionIndex(int actionIndex_) {
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

	void setUsed(int player_, int index_, int actionIndex_) {
		playerUseds[player_][index_] = actionIndex_;
	}
};


class PartyState {
public:
	PartyState() = default;
	PartyState(const PartyState& other) : actionList(other.actionList), roundResults(other.roundResults), 
		playerHands(other.playerHands) {};
	PartyState& operator=(const PartyState& other) {
		if (this != &other) {
			actionList = other.actionList; roundResults = other.roundResults; playerHands = other.playerHands;
		}
		return *this;
	};

	using CardVector = std::vector<Card>;

	void init();

	void getCardsInHand(CardVector&, int, int);
	void getPlayableCards(CardVector&, int);
	void simplifyPlayableCards(CardVector&);
	void setHitCard(int, const Card&);
	void setNextPlayer(int);
	int chooseWinnerCard(const Card, const Card, const Card);

	bool isLastIndex(int index_) { return actionList.isLastIndex(index_); };
	bool isFirstPlayerToHit(int index_) { return actionList.isFirstPlayerToHit(index_); };

	uint8_t evaluateParty(int);

	void print_current_state(int, const CardVector&);
	void print_game_progression();

private:
	ActionList actionList = ActionList();
	RoundResults roundResults = RoundResults();
	PlayerHands playerHands = PlayerHands();			
};

}