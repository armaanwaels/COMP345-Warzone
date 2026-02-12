#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <string>
#include <vector>

// Forward declarations
class Deck;
class Hand;

// Enum for card types
enum class CardType {
    Bomb,
    Reinforcement,
    Blockade,
    Airlift,
    Diplomacy
};

// Utility function to convert CardType to string
std::string cardTypeToString(CardType type);

// Represents a single Warzone card
class Card {
private:
    CardType* type;

public:
    Card();
    Card(CardType cardType);
    Card(const Card& other);
    ~Card();

    Card& operator=(const Card& other);

    CardType getType() const;

    // Plays the card: creates an order, adds it to the player's orders, returns card to deck
    void play(Hand& hand, Deck& deck, int cardIndex);

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};

// Represents a player's hand of cards
class Hand {
private:
    std::vector<Card*>* cards;

public:
    Hand();
    Hand(const Hand& other);
    ~Hand();

    Hand& operator=(const Hand& other);

    void addCard(Card* card);
    Card* removeCard(int index);

    int getSize() const;
    Card* getCard(int index) const;

    friend std::ostream& operator<<(std::ostream& os, const Hand& hand);
};

// Represents the deck of Warzone cards
class Deck {
private:
    std::vector<Card*>* cards;

public:
    Deck();
    Deck(const Deck& other);
    ~Deck();

    Deck& operator=(const Deck& other);

    void addCard(Card* card);

    // Draws a random card from the deck and places it in the given hand
    void draw(Hand& hand);

    int getSize() const;

    friend std::ostream& operator<<(std::ostream& os, const Deck& deck);
};

#endif
