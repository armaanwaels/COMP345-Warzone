// CardsDriver.cpp
// Driver for Deck, Hand, and Card classes
// COMP 345 - Assignment 1, Part 4

#include "Cards.h"
#include <iostream>

using std::cout;
using std::endl;

// Test creating a deck with all card types and drawing into a hand
void testDeckAndHand() {
    cout << "\n========================================" << endl;
    cout << "TESTING DECK CREATION AND DRAWING" << endl;
    cout << "========================================\n" << endl;

    // Create a deck with multiple cards of each type
    Deck* deck = new Deck();

    cout << "--- Populating deck with cards of all types ---\n" << endl;

    // Add 3 of each type (15 cards total)
    for (int i = 0; i < 3; i++) {
        deck->addCard(new Card(CardType::Bomb));
        deck->addCard(new Card(CardType::Reinforcement));
        deck->addCard(new Card(CardType::Blockade));
        deck->addCard(new Card(CardType::Airlift));
        deck->addCard(new Card(CardType::Diplomacy));
    }

    cout << "Deck after populating:" << endl;
    cout << *deck << endl;

    // Create a hand and draw cards from the deck
    Hand* hand = new Hand();

    cout << "\n--- Drawing 5 cards from deck into hand ---\n" << endl;

    for (int i = 0; i < 5; i++) {
        deck->draw(*hand);
    }

    cout << "\nDeck after drawing 5 cards:" << endl;
    cout << *deck << endl;

    cout << "Hand after drawing 5 cards:" << endl;
    cout << *hand << endl;

    delete hand;
    delete deck;

    cout << "Deck and hand destroyed successfully!" << endl;
}

// Test playing all cards from a hand
void testPlayCards() {
    cout << "\n========================================" << endl;
    cout << "TESTING PLAYING CARDS FROM HAND" << endl;
    cout << "========================================\n" << endl;

    Deck* deck = new Deck();

    // Add 2 of each type (10 cards total)
    for (int i = 0; i < 2; i++) {
        deck->addCard(new Card(CardType::Bomb));
        deck->addCard(new Card(CardType::Reinforcement));
        deck->addCard(new Card(CardType::Blockade));
        deck->addCard(new Card(CardType::Airlift));
        deck->addCard(new Card(CardType::Diplomacy));
    }

    Hand* hand = new Hand();

    cout << "--- Drawing 6 cards into hand ---\n" << endl;
    for (int i = 0; i < 6; i++) {
        deck->draw(*hand);
    }

    cout << "\nDeck size before playing: " << deck->getSize() << endl;
    cout << "Hand size before playing: " << hand->getSize() << endl;
    cout << "\n" << *hand << endl;

    cout << "--- Playing all cards from hand ---\n" << endl;

    // Play all cards (always play index 0, since cards shift after each removal)
    while (hand->getSize() > 0) {
        hand->getCard(0)->play(*hand, *deck, 0);
        cout << endl;
    }

    cout << "Deck size after playing all cards: " << deck->getSize() << endl;
    cout << "Hand size after playing all cards: " << hand->getSize() << endl;

    cout << "\n" << *deck << endl;
    cout << *hand << endl;

    delete hand;
    delete deck;

    cout << "Play cards test completed successfully!" << endl;
}

// Test copy constructor and assignment operator
void testCopyAndAssignment() {
    cout << "\n========================================" << endl;
    cout << "TESTING COPY CONSTRUCTOR AND ASSIGNMENT" << endl;
    cout << "========================================\n" << endl;

    cout << "--- Testing Card copy constructor ---" << endl;
    Card* original = new Card(CardType::Airlift);
    Card* copy = new Card(*original);
    cout << "Original: " << *original << endl;
    cout << "Copy:     " << *copy << endl << endl;

    cout << "--- Testing Hand copy constructor ---" << endl;
    Hand* originalHand = new Hand();
    originalHand->addCard(new Card(CardType::Bomb));
    originalHand->addCard(new Card(CardType::Diplomacy));
    originalHand->addCard(new Card(CardType::Blockade));

    cout << "Original hand:" << endl;
    cout << *originalHand << endl;

    Hand* copiedHand = new Hand(*originalHand);
    cout << "Copied hand:" << endl;
    cout << *copiedHand << endl;

    cout << "--- Modifying original to verify deep copy ---" << endl;
    originalHand->removeCard(0);
    cout << "Original hand after removing first card:" << endl;
    cout << *originalHand << endl;
    cout << "Copied hand (should be unchanged):" << endl;
    cout << *copiedHand << endl;

    cout << "--- Testing Deck copy constructor ---" << endl;
    Deck* originalDeck = new Deck();
    originalDeck->addCard(new Card(CardType::Reinforcement));
    originalDeck->addCard(new Card(CardType::Airlift));

    cout << "Original deck:" << endl;
    cout << *originalDeck << endl;

    Deck* copiedDeck = new Deck(*originalDeck);
    cout << "Copied deck:" << endl;
    cout << *copiedDeck << endl;

    // Clean up the card removed from hand (removeCard returns ownership)
    delete original;
    delete copy;
    delete originalHand;
    delete copiedHand;
    delete originalDeck;
    delete copiedDeck;

    cout << "All copy/assignment tests completed successfully!" << endl;
}

// Test edge cases
void testEdgeCases() {
    cout << "\n========================================" << endl;
    cout << "TESTING EDGE CASES" << endl;
    cout << "========================================\n" << endl;

    cout << "--- Drawing from empty deck ---" << endl;
    Deck* emptyDeck = new Deck();
    Hand* hand = new Hand();
    emptyDeck->draw(*hand);

    cout << "\n--- Removing from empty hand ---" << endl;
    Card* result = hand->removeCard(0);
    if (result == nullptr) {
        cout << "Correctly returned nullptr for empty hand." << endl;
    }

    cout << "\n--- Invalid hand index ---" << endl;
    hand->removeCard(-1);
    hand->removeCard(100);

    delete emptyDeck;
    delete hand;

    cout << "\nEdge case tests completed successfully!" << endl;
}

int main() {
    cout << "********************************************************" << endl;
    cout << "*   COMP 345 - Assignment 1 - Part 4: Cards Deck/Hand  *" << endl;
    cout << "********************************************************" << endl;

    testDeckAndHand();
    testPlayCards();
    testCopyAndAssignment();
    testEdgeCases();

    cout << "\n========================================" << endl;
    cout << "ALL TESTS COMPLETED SUCCESSFULLY!" << endl;
    cout << "========================================\n" << endl;

    return 0;
}
