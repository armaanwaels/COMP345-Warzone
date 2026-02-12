#include "Cards.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostream;

// ==================== Utility ====================

string cardTypeToString(CardType type) {
    switch (type) {
        case CardType::Bomb:          return "Bomb";
        case CardType::Reinforcement: return "Reinforcement";
        case CardType::Blockade:      return "Blockade";
        case CardType::Airlift:       return "Airlift";
        case CardType::Diplomacy:     return "Diplomacy";
        default:                      return "Unknown";
    }
}

// ==================== Card ====================

Card::Card() {
    type = new CardType(CardType::Bomb);
}

Card::Card(CardType cardType) {
    type = new CardType(cardType);
}

Card::Card(const Card& other) {
    type = new CardType(*(other.type));
}

Card::~Card() {
    delete type;
}

Card& Card::operator=(const Card& other) {
    if (this != &other) {
        delete type;
        type = new CardType(*(other.type));
    }
    return *this;
}

CardType Card::getType() const {
    return *type;
}

void Card::play(Hand& hand, Deck& deck, int cardIndex) {
    cout << "Playing card: " << cardTypeToString(*type) << endl;

    // Create the corresponding order based on card type
    switch (*type) {
        case CardType::Bomb:
            cout << "  -> Creating a Bomb order." << endl;
            break;
        case CardType::Reinforcement:
            cout << "  -> Creating a Reinforcement (Deploy) order." << endl;
            break;
        case CardType::Blockade:
            cout << "  -> Creating a Blockade order." << endl;
            break;
        case CardType::Airlift:
            cout << "  -> Creating an Airlift order." << endl;
            break;
        case CardType::Diplomacy:
            cout << "  -> Creating a Negotiate order." << endl;
            break;
    }

    // Remove the card from the hand and return it to the deck
    Card* playedCard = hand.removeCard(cardIndex);
    if (playedCard != nullptr) {
        deck.addCard(playedCard);
        cout << "  -> Card returned to the deck." << endl;
    }
}

ostream& operator<<(ostream& os, const Card& card) {
    os << "Card [" << cardTypeToString(*(card.type)) << "]";
    return os;
}

// ==================== Hand ====================

Hand::Hand() {
    cards = new vector<Card*>();
}

Hand::Hand(const Hand& other) {
    cards = new vector<Card*>();
    for (Card* card : *(other.cards)) {
        cards->push_back(new Card(*card));
    }
}

Hand::~Hand() {
    for (Card* card : *cards) {
        delete card;
    }
    delete cards;
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        for (Card* card : *cards) {
            delete card;
        }
        cards->clear();

        for (Card* card : *(other.cards)) {
            cards->push_back(new Card(*card));
        }
    }
    return *this;
}

void Hand::addCard(Card* card) {
    if (card != nullptr) {
        cards->push_back(card);
    }
}

Card* Hand::removeCard(int index) {
    if (index >= 0 && index < static_cast<int>(cards->size())) {
        Card* card = (*cards)[index];
        cards->erase(cards->begin() + index);
        return card;
    }
    cout << "Error: Invalid index " << index << " for hand remove operation." << endl;
    return nullptr;
}

int Hand::getSize() const {
    return static_cast<int>(cards->size());
}

Card* Hand::getCard(int index) const {
    if (index >= 0 && index < static_cast<int>(cards->size())) {
        return (*cards)[index];
    }
    return nullptr;
}

ostream& operator<<(ostream& os, const Hand& hand) {
    os << "Hand (" << hand.cards->size() << " cards):";
    if (hand.cards->empty()) {
        os << " [empty]";
    } else {
        os << endl;
        int index = 0;
        for (Card* card : *(hand.cards)) {
            os << "  [" << index++ << "] " << *card << endl;
        }
    }
    return os;
}

// ==================== Deck ====================

Deck::Deck() {
    cards = new vector<Card*>();
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
}

Deck::Deck(const Deck& other) {
    cards = new vector<Card*>();
    for (Card* card : *(other.cards)) {
        cards->push_back(new Card(*card));
    }
}

Deck::~Deck() {
    for (Card* card : *cards) {
        delete card;
    }
    delete cards;
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        for (Card* card : *cards) {
            delete card;
        }
        cards->clear();

        for (Card* card : *(other.cards)) {
            cards->push_back(new Card(*card));
        }
    }
    return *this;
}

void Deck::addCard(Card* card) {
    if (card != nullptr) {
        cards->push_back(card);
    }
}

void Deck::draw(Hand& hand) {
    if (cards->empty()) {
        cout << "Error: Cannot draw from an empty deck." << endl;
        return;
    }

    // Pick a random index
    int randomIndex = rand() % static_cast<int>(cards->size());
    Card* drawnCard = (*cards)[randomIndex];
    cards->erase(cards->begin() + randomIndex);

    cout << "Drew card: " << *drawnCard << endl;
    hand.addCard(drawnCard);
}

int Deck::getSize() const {
    return static_cast<int>(cards->size());
}

ostream& operator<<(ostream& os, const Deck& deck) {
    os << "Deck (" << deck.cards->size() << " cards):";
    if (deck.cards->empty()) {
        os << " [empty]";
    } else {
        os << endl;
        int index = 0;
        for (Card* card : *(deck.cards)) {
            os << "  [" << index++ << "] " << *card << endl;
        }
    }
    return os;
}
