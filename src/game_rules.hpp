#pragma once
#include <cstdint>
#include <vector>

namespace ulti_minimax {

class Card {
public:
	Card(uint8_t suit, uint8_t value);

	Card(const Card& other);

	bool operator==(const Card& other) const;
	bool operator>(const Card& other) const;
	bool operator<(const Card& other) const;

private:
	uint8_t suit;
	uint8_t value;
};

class Deck {
public:
	void deal();
	
private:
	//constexpr std::vector<Card> cards = 
	static constexpr std::vector<Card> cards = initCards();	
	
	static constexpr std::vector<Card> initCards() {
        std::vector<Card> objs;
        for (size_t i = 0; i < N; ++i) {
            // Add your initialization logic here
            // For demonstration, let's just add N objects with default constructor
            objs.push_back(Object());
        }
        return objs;
    }

};

class PartyState {
public:
	PartyState() = default;

	void deal();

	std::vector<Card> get_playable_cards();

private:
	int player;

};

}