#include "Player.h"
#include "Map/Map.h"
#include "Cards/Cards.h"
#include "Orders/Orders.h"

#include <algorithm>

// Default Constructor
Player::Player() {
    name = new std::string("Unnamed");
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
    reinforcementPool = new int(0);
}

// Parameterized Constructor
Player::Player(const std::string& nameParam) {
    name = new std::string(nameParam);
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
    reinforcementPool = new int(0);
}

// Copy Constructor
Player::Player(const Player& other) {
    name = new std::string(*other.name);
    territories = new std::vector<Territory*>(*other.territories);
    hand = new Hand(*other.hand);
    orders = new OrdersList(*other.orders);
    reinforcementPool = new int(*other.reinforcementPool);
}

// Assignment Operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territories;
        delete hand;
        delete orders;
        delete reinforcementPool;

        name = new std::string(*other.name);
        territories = new std::vector<Territory*>(*other.territories);
        hand = new Hand(*other.hand);
        orders = new OrdersList(*other.orders);
        reinforcementPool = new int(*other.reinforcementPool);
    }
    return *this;
}

// Destructor
Player::~Player() {
    delete name;
    delete territories;
    delete hand;
    delete orders;
    delete reinforcementPool;
}

// Getter Methods
const std::string& Player::getName() const {
    return *name;
}

std::vector<Territory*>* Player::getTerritories() const {
    return territories;
}

Hand* Player::getHand() const {
    return hand;
}

OrdersList* Player::getOrders() const {
    return orders;
}

int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

// Territory Management (For demo purposes to show the player owns territories)
void Player::addTerritory(Territory* territory) {
    if (territory == nullptr) return;

    auto it = std::find(territories->begin(), territories->end(), territory);
    if (it == territories->end()) {
        territories->push_back(territory);
    }
}

void Player::removeTerritory(Territory* territory) {
    if (territory == nullptr) return;

    auto it = std::find(territories->begin(), territories->end(), territory);
    if (it != territories->end()) {
        territories->erase(it);
    }
}

// For now, toDefend returns all territories owned by the player and toAttack returns an empty list
std::vector<Territory*> Player::toDefend() const {
    return *territories;
}

std::vector<Territory*> Player::toAttack() const {
    return std::vector<Territory*>();
}

// Order Management
void Player::issueOrder() {
    // Create concrete order and add to orders list (demo purposes)
    Order* o = new Deploy(5, "Territory");
    orders->addOrder(o);
}

// Reinforcement Management
void Player::receiveReinforcements(int numOfArmies) {
    if (numOfArmies > 0) {
        *reinforcementPool += numOfArmies;
    }
}

void Player::removeReinforcements(int numOfArmies) {
    if (numOfArmies <= 0) {
        return;
    }

    if (numOfArmies > *reinforcementPool) {
        *reinforcementPool = 0;
    } else {
        *reinforcementPool -= numOfArmies;
    }
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player{name = " << player.getName();
    os << ", territories = " << (player.territories->size());
    os << ", hand = " << (player.hand->getSize());
    os << ", orders = " << (player.orders->getSize());
    os << "}";
    return os;
}