#include "Player.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.h"

#include <algorithm>
#include <set>

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

int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

// Territory Management
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

// returns all territories owned by this player, sorted by army count (weakest first)
std::vector<Territory*> Player::toDefend() const {
    std::vector<Territory*> defend = *territories;
    std::sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmies() < b->getArmies();
    });
    return defend;
}

// returns enemy territories adjacent to any owned territory
std::vector<Territory*> Player::toAttack() const {
    std::set<Territory*> attackSet;

    for (Territory* owned : *territories) {
        for (Territory* neighbor : owned->getBorders()) {
            if (neighbor->getOwner() != const_cast<Player*>(this)) {
                attackSet.insert(neighbor);
            }
        }
    }

    return std::vector<Territory*>(attackSet.begin(), attackSet.end());
}

// Issues ONE order per call. Returns true if an order was issued, false when done.
// Uses a simple phase tracker: deploy -> attack -> defend -> cards -> done
// The issuePhase counter persists across calls (reset when all phases are done)
bool Player::issueOrder(Deck* deck, Map* map) {
    (void)map;

    // Phase 1: deploy all reinforcements first
    if (*reinforcementPool > 0) {
        std::vector<Territory*> defend = toDefend();
        if (!defend.empty()) {
            Territory* target = defend[0];
            int toDeploy = std::min(*reinforcementPool, 5);
            if (toDeploy <= 0) toDeploy = *reinforcementPool;

            orders->addOrder(new Deploy(this, toDeploy, target));
            *reinforcementPool -= toDeploy;
            return true;
        }
        return false;
    }

    // Phase 2: issue one advance order per adjacent enemy territory
    // use a static-like approach: check if we've already issued advances
    // by counting existing advance orders vs available attack targets
    std::vector<Territory*> attackTargets = toAttack();
    int existingAdvances = 0;
    for (int i = 0; i < orders->getSize(); i++) {
        if (dynamic_cast<Advance*>(orders->getOrder(i)) != nullptr) {
            existingAdvances++;
        }
    }

    if (existingAdvances < static_cast<int>(attackTargets.size())) {
        // find the next attack target we haven't issued an advance for yet
        int attackIdx = existingAdvances;
        if (attackIdx < static_cast<int>(attackTargets.size())) {
            Territory* target = attackTargets[attackIdx];
            // find an owned territory adjacent to this target with armies > 1
            for (Territory* owned : *territories) {
                if (owned->getArmies() <= 1) continue;
                for (Territory* neighbor : owned->getBorders()) {
                    if (neighbor == target) {
                        int armiesToSend = owned->getArmies() - 1;
                        orders->addOrder(new Advance(this, armiesToSend, owned, target, deck));
                        return true;
                    }
                }
            }
        }
    }

    // Phase 3: play cards
    if (hand->getSize() > 0) {
        Card* card = hand->getCard(0);
        CardType type = card->getType();
        std::vector<Territory*> defend = toDefend();

        if (type == CardType::Bomb && !attackTargets.empty()) {
            orders->addOrder(new Bomb(this, attackTargets[0]));
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
            return true;
        } else if (type == CardType::Airlift && defend.size() >= 2) {
            Territory* src = defend.back();
            Territory* dst = defend.front();
            if (src->getArmies() > 1) {
                orders->addOrder(new Airlift(this, src->getArmies() / 2, src, dst));
                Card* played = hand->removeCard(0);
                if (deck != nullptr) deck->addCard(played);
                return true;
            }
        } else if (type == CardType::Blockade && !defend.empty()) {
            orders->addOrder(new Blockade(this, defend.front()));
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
            return true;
        } else if (type == CardType::Reinforcement) {
            addReinforcements(5);
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
            return true;
        } else {
            // discard cards we can't use (e.g. Diplomacy without a good target)
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
        }
    }

    // done issuing orders
    return false;
}

// Reinforcement Pool Management
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
