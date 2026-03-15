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
    conqueredTerritoryThisTurn = new bool(false);
    negotiatedPlayers = new std::vector<std::string>();
}

// Parameterized Constructor
Player::Player(const std::string& nameParam) {
    name = new std::string(nameParam);
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
    reinforcementPool = new int(0);
    conqueredTerritoryThisTurn = new bool(false);
    negotiatedPlayers = new std::vector<std::string>();
}

// Copy Constructor
Player::Player(const Player& other) {
    name = new std::string(*other.name);
    territories = new std::vector<Territory*>(*other.territories);
    hand = new Hand(*other.hand);
    orders = new OrdersList(*other.orders);
    reinforcementPool = new int(*other.reinforcementPool);
    conqueredTerritoryThisTurn = new bool(*other.conqueredTerritoryThisTurn);
    negotiatedPlayers = new std::vector<std::string>(*other.negotiatedPlayers);
}

// Assignment Operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territories;
        delete hand;
        delete orders;
        delete reinforcementPool;
        delete conqueredTerritoryThisTurn;
        delete negotiatedPlayers;

        name = new std::string(*other.name);
        territories = new std::vector<Territory*>(*other.territories);
        hand = new Hand(*other.hand);
        orders = new OrdersList(*other.orders);
        reinforcementPool = new int(*other.reinforcementPool);
        conqueredTerritoryThisTurn = new bool(*other.conqueredTerritoryThisTurn);
        negotiatedPlayers = new std::vector<std::string>(*other.negotiatedPlayers);
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
    delete conqueredTerritoryThisTurn;
    delete negotiatedPlayers;
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
    Order* o = new Deploy(this, 5, nullptr);
    orders->addOrder(o);
}

// Reinforcement Pool Management
int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

void Player::setReinforcementPool(int pool) {
    *reinforcementPool = std::max(0, pool);
}

void Player::addReinforcements(int count) {
    if (count > 0) {
        *reinforcementPool += count;
    }
}

void Player::removeReinforcements(int count) {
    *reinforcementPool = std::max(0, *reinforcementPool - count);
}

// Conquest tracking
bool Player::hasConqueredThisTurn() const {
    return *conqueredTerritoryThisTurn;
}

void Player::setConqueredThisTurn(bool val) {
    *conqueredTerritoryThisTurn = val;
}

// Negotiation tracking
void Player::addNegotiatedPlayer(const std::string& playerName) {
    // Avoid duplicates
    auto it = std::find(negotiatedPlayers->begin(), negotiatedPlayers->end(), playerName);
    if (it == negotiatedPlayers->end()) {
        negotiatedPlayers->push_back(playerName);
    }
}

bool Player::hasNegotiationWith(const std::string& playerName) const {
    auto it = std::find(negotiatedPlayers->begin(), negotiatedPlayers->end(), playerName);
    return it != negotiatedPlayers->end();
}

void Player::clearNegotiations() {
    negotiatedPlayers->clear();
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player{name = " << player.getName();
    os << ", territories = " << (player.territories->size());
    os << ", hand = " << (player.hand->getSize());
    os << ", orders = " << (player.orders->getSize());
    os << ", reinforcements = " << *player.reinforcementPool;
    os << "}";
    return os;
}
