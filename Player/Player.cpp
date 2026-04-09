#include "Player.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.h"
#include "PlayerStrategies.h"

#include <algorithm>
#include <set>

// Default Constructor
Player::Player() {
    name = new std::string("Unnamed");
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
    strategy = nullptr;
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
    strategy = nullptr;
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
    strategy = (other.strategy != nullptr) ? other.strategy->clone() : nullptr;
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
        delete strategy;
        delete reinforcementPool;
        delete conqueredTerritoryThisTurn;
        delete negotiatedPlayers;

        name = new std::string(*other.name);
        territories = new std::vector<Territory*>(*other.territories);
        hand = new Hand(*other.hand);
        orders = new OrdersList(*other.orders);
        strategy = (other.strategy != nullptr) ? other.strategy->clone() : nullptr;
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
    delete strategy;
    delete reinforcementPool;
    delete conqueredTerritoryThisTurn;
    delete negotiatedPlayers;
}

// Getters
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

PlayerStrategy* Player::getStrategy() const {
    return strategy;
}

int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

// Territory management
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

// delegates to strategy
std::vector<Territory*> Player::toDefend() const {
    if (strategy == nullptr) return {};
    return strategy->toDefend(const_cast<Player*>(this));
}

// delegates to strategy
std::vector<Territory*> Player::toAttack() const {
    if (strategy == nullptr) return {};
    return strategy->toAttack(const_cast<Player*>(this));
}

// delegates to strategy
bool Player::issueOrder(Deck* deck, Map* map) {
    if (strategy == nullptr) return false;
    return strategy->issueOrder(this, deck, map);
}

// Reinforcement pool
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

// Strategy Management
void Player::setStrategy(PlayerStrategy* newStrategy) {
    if (strategy != nullptr) {
        delete strategy;
    }
    strategy = newStrategy;
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player{name = " << player.getName();
    os << ", territories = " << (player.territories->size());
    os << ", hand = " << (player.hand->getSize());
    os << ", orders = " << (player.orders->getSize());
    os << ", strategy = " << (player.strategy ? player.strategy->getStrategyName() : "None");
    os << ", reinforcements = " << *player.reinforcementPool;
    os << "}";
    return os;
}
