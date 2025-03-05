#pragma once
#include <array>
#include <assert.h>
#include <cstdint>
#include <string>
#include <vector>

namespace ulti_minimax {

// Deck info
constexpr int N_SUIT = 4;
constexpr int N_VALUE = 8;
constexpr int LOWEST_CARD_VALUE_WITH_POINT = 6;
constexpr int N_POINT_IN_DECK = (N_VALUE - LOWEST_CARD_VALUE_WITH_POINT) * N_SUIT;

// Game rules info
constexpr int N_PLAYER = 3;
constexpr int N_CARD_IN_HAND = 10;
constexpr int N_ACTION = N_PLAYER * N_CARD_IN_HAND;
constexpr int LAST_ACTION_INDEX = N_ACTION - 1;
constexpr int N_REST_CARD = N_SUIT * N_VALUE - N_PLAYER * N_CARD_IN_HAND;

// Party results
constexpr uint8_t RESULT_UNDEFINED	= 0;
constexpr uint8_t PLAYER_WIN		= 1;
constexpr uint8_t OPPONENT_WIN		= 2;

// Game types
constexpr uint8_t NO_TRUMP_PARTY		= 0;
constexpr uint8_t TRUMP_PARTY			= 1;
constexpr uint8_t _40100				= 2;
constexpr uint8_t _4ACES				= 3;
constexpr uint8_t ULTI					= 4;
constexpr uint8_t BETLI					= 5;
constexpr uint8_t NO_TRUMP_DURCHMARS	= 6;
constexpr uint8_t DURCHMARS				= 7;
constexpr uint8_t _20100				= 8;
constexpr uint8_t _4TENS				= 9;

// Trumps: [0, 1, 2, 3]
constexpr uint8_t NO_TRUMP_CODE = 4;
constexpr uint8_t NO_TRUMP_GAMES[] = { NO_TRUMP_PARTY, BETLI, NO_TRUMP_DURCHMARS };

// Card order
constexpr uint8_t ACE_KING_ORDER	= 1;
constexpr uint8_t ACE_TEN_ORDER		= 2;
constexpr uint8_t ACE_KING_ORDER_GAMES[] = { BETLI, NO_TRUMP_DURCHMARS };

// Series terminating card schemes (to simplify playable cards)
constexpr uint8_t NO_TERMINATE	= 1;
constexpr uint8_t TERMINATE_6	= 2; // Terminate at ten
constexpr uint8_t TERMINATE_7	= 3; // Terminate at ace
constexpr uint8_t TERMINATE_06	= 4; // Terminate at seven and ten
constexpr uint8_t TERMINATE_67	= 5; // Terminate at ten and ace
constexpr uint8_t TERMINATE_067	= 6; // Terminate at seven, ten and ace


constexpr int SEED = 0;
constexpr bool DEBUG = false;


class Card {
public:
	Card() = default;

	Card(int suit_, int value_) : suit(suit_), value(value_) 
	{
		assert(0 <= suit && suit < N_SUIT);
		assert(0 <= value && value < N_VALUE);
	};

	Card(const Card& other) : suit(other.suit), value(other.value) {};

	Card& operator=(const Card& other) { 
		if (this != &other) {
			suit = other.suit;
			value = other.value;
		}
		return *this;
	};

	int getSuit() const { return suit; };
	int getValue() const { return value; };
	int getPoint() const { return value >= LOWEST_CARD_VALUE_WITH_POINT; };
	bool isTrump(uint8_t trump) const { return suit == trump; }

	// The following comparisons are based on the fact that there are no two equal cards in a deck
	bool operator==(const Card& other) const { return suit == other.suit; }; // True if same suit
	bool operator!=(const Card& other) const { return suit != other.suit; }; // True if different suit
	bool operator>(const Card& other) const { return value > other.value; }; // True if greater value
	bool operator<(const Card& other) const { return value < other.value; }; // True if smaller value
	static bool compareCard(const Card&, const Card&);

	// Check if two cards are equal (which cannot be in a normal decK)
	bool equals(const Card& other) const { return suit == other.suit && value == other.value; };
	
	// Check if the two cards are in series for simplified hitting
	bool isNextInSeries(const Card&, uint8_t) const;

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
	std::array<Card, N_REST_CARD> findRestCards(const std::vector<Card>&) const;

	int size() const { return static_cast<int>(cards.size()); };
	Card getCard(int index_) const { return cards[index_]; };

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

	int firstPlayer; // Index of the player who playes against the opponents
	ActionVector actions;	

	Action getAction(int index_) { return actions[index_]; };
};


class RoundResults {
public:
	RoundResults() = default;
	RoundResults(const RoundResults& other) : 
		winner(other.winner), point(other.point), 
		trump0(other.trump0), ace(other.ace), ten(other.ten) {};
	RoundResults& operator=(const RoundResults& other) {
		if (this != &other) {
			winner = other.winner; point = other.point; 
			trump0 = other.trump0; ace = other.ace; ten = other.ten;
		}
		return *this;
	};

	int getWinner(int round) const { return winner[round]; };
	int getPoint(int round) const { return point[round]; };
	void setWinner(int round, int player_) { winner[round] = player_; };
	void setPoint(int round, int point_) { point[round] = point_; };

	int getTrump0(int round) const { return trump0[round]; };
	int getAce(int round) const { return ace[round]; };
	int getTen(int round) const { return ten[round]; };

	void setSpecialCard(int round, const Card c0, const Card c1, const Card c2, int trump) {
		trump0[round] = 0;
		ace[round] = 0;
		ten[round] = 0;
		for (const Card& card : std::array<Card, 3>{c0, c1, c2}) {
			if (card.getSuit() == trump && card.getValue() == 0) trump0[round] = 1;
			if (card.getValue() == N_VALUE - 1) ace[round]++;
			if (card.getValue() == N_VALUE - 2) ten[round]++;
		}
	}	

	bool isLastRound(int round) const {
		assert(0 <= round && round <= N_CARD_IN_HAND - 1);
		return round == N_CARD_IN_HAND - 1;
	};

private:
	// Keep track of each rounds' results
	std::array<int, N_CARD_IN_HAND> winner; // Winner player of the round
	std::array<int, N_CARD_IN_HAND> point; // No. points won
	std::array<int, N_CARD_IN_HAND> trump0; // Is Trump0 played in the round
	std::array<int, N_CARD_IN_HAND> ace; // Is Ace played in the round
	std::array<int, N_CARD_IN_HAND> ten; // Is Ten played in the round
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

	std::array<Card, N_REST_CARD> init_deal(const std::vector<Card>&);
	std::array<Card, N_REST_CARD> random_deal();

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
			assert(getUsed(player_, i) > actionIndex_); // Card cannot be used before current action index
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

	// Check if card is in player's hand
	bool checkCard(int player_, const Card& card) const {
		for (int i = 0; i < N_CARD_IN_HAND; ++i) {
			if (playerCards[player_][i].equals(card)) return true;
		}
		return false;
	}

private:
	using CardArray = std::array<Card, N_CARD_IN_HAND>;
	using PlayerCardArray = std::array<CardArray, N_PLAYER>;
	using IntArray = std::array<int, N_CARD_IN_HAND>;
	using PlayerUsedArray = std::array<IntArray, N_PLAYER>;

	PlayerCardArray playerCards;
	PlayerUsedArray playerUseds; // Action index when that card was used

	// Private version of setUsed(), where card is index int, instead of Card object
	//
	void setUsed(int player_, int index_, int actionIndex_) {
		playerUseds[player_][index_] = actionIndex_;
	}
};


class PartyState {
public:
	PartyState() = default;

	PartyState(const PartyState& other) : gameType(other.gameType), trump(other.trump),
		series_terminate_scheme(other.series_terminate_scheme),
		restCard0(other.restCard0), restCard1(other.restCard1),
		actionList(other.actionList), roundResults(other.roundResults), playerHands(other.playerHands) {};

	PartyState& operator=(const PartyState& other) {
		if (this != &other) {
			gameType = other.gameType; trump = other.trump;
			series_terminate_scheme = other.series_terminate_scheme;
			restCard0 = other.restCard0; restCard1 = other.restCard1;
			actionList = other.actionList; roundResults = other.roundResults; playerHands = other.playerHands;
		}
		return *this;
	};

	using CardVector = std::vector<Card>;

	bool init(const std::string& = "");

	uint8_t getGameType() const { return gameType; };
	uint8_t getTrump() const { return trump; };

	std::array<Card, N_REST_CARD> getRestCards() const { 
		return std::array<Card, N_REST_CARD> {restCard0, restCard1}; 
	};

	void getCardsInHand(CardVector&, int, int);
	void getPlayableCards(CardVector&, int);
	void simplifyPlayableCards(CardVector&);
	void setHitCard(int, const Card&);
	void setNextPlayer(int);
	int chooseWinnerCard(const Card, const Card, const Card, uint8_t);

	bool isLastIndex(int index_) { return actionList.isLastIndex(index_); };
	bool isFirstPlayerToHit(int index_) { return actionList.isFirstPlayerToHit(index_); };

	uint8_t evaluateParty(int, bool); // Evaluate the current game type	

	// Printing to console
	void printCurrentState(int, const CardVector&);
	void printCard(int);
	void printGameProgression();

	std::string getGameProgressionStr(int);

private:
	uint8_t gameType = NO_TRUMP_PARTY;
	uint8_t trump = NO_TRUMP_CODE;
	uint8_t series_terminate_scheme = 0;
	Card restCard0, restCard1;

	ActionList actionList = ActionList();
	RoundResults roundResults = RoundResults();
	PlayerHands playerHands = PlayerHands();	
};

}