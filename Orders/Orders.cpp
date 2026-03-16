#include "Orders.h"
#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostream;

// ==================== Order Base Class ====================

Order::Order() {
    description = new string("Generic Order");
    effect = new string("");
    executed = new bool(false);
    issuer = nullptr;
}

Order::Order(Player* issuer) {
    description = new string("Generic Order");
    effect = new string("");
    executed = new bool(false);
    this->issuer = issuer;
}

Order::Order(const Order& other) {
    description = new string(*(other.description));
    effect = new string(*(other.effect));
    executed = new bool(*(other.executed));
    issuer = other.issuer;  // shallow copy (non-owning)
}

Order::~Order() {
    delete description;
    delete effect;
    delete executed;
}

Order& Order::operator=(const Order& other) {
    if (this != &other) {
        delete description;
        delete effect;
        delete executed;

        description = new string(*(other.description));
        effect = new string(*(other.effect));
        executed = new bool(*(other.executed));
        issuer = other.issuer;
    }
    return *this;
}

string Order::stringToLog() const {
    if (*executed) {
        return "Order Executed: " + *description + " | Effect: " + *effect;
    }
    return "Order: " + *description + " (not yet executed)";
}

string Order::getDescription() const {
    return *description;
}

string Order::getEffect() const {
    return *effect;
}

bool Order::isExecuted() const {
    return *executed;
}

Player* Order::getIssuer() const {
    return issuer;
}

ostream& operator<<(ostream& os, const Order& order) {
    os << "Order: " << *(order.description);
    if (*(order.executed)) {
        os << " | Effect: " << *(order.effect);
    }
    return os;
}

// ==================== Deploy ====================
// Deploys armies from the reinforcement pool to a territory owned by the player.

Deploy::Deploy() : Order() {
    delete description;
    description = new string("Deploy Order");
    numArmies = new int(0);
    targetTerritory = nullptr;
}

Deploy::Deploy(Player* issuer, int armies, Territory* target) : Order(issuer) {
    delete description;
    string targetName = target ? target->getName() : "NULL";
    description = new string("Deploy " + std::to_string(armies) + " armies to " + targetName);
    numArmies = new int(armies);
    targetTerritory = target;
}

Deploy::Deploy(const Deploy& other) : Order(other) {
    numArmies = new int(*(other.numArmies));
    targetTerritory = other.targetTerritory;
}

Deploy::~Deploy() {
    delete numArmies;
}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        delete numArmies;

        numArmies = new int(*(other.numArmies));
        targetTerritory = other.targetTerritory;
    }
    return *this;
}

bool Deploy::validate() const {
    // Guard: issuer and target must exist
    if (issuer == nullptr || targetTerritory == nullptr) {
        cout << "Deploy validation FAILED: null issuer or target territory." << endl;
        return false;
    }
    // The target territory must belong to the player issuing the order
    if (targetTerritory->getOwner() != issuer) {
        cout << "Deploy validation FAILED: " << targetTerritory->getName()
             << " does not belong to " << issuer->getName() << "." << endl;
        return false;
    }
    if (*numArmies <= 0) {
        cout << "Deploy validation FAILED: number of armies must be positive." << endl;
        return false;
    }
    cout << "Deploy validation PASSED." << endl;
    return true;
}

void Deploy::execute() {
    if (validate()) {
        // Deduct from reinforcement pool and add to territory
        issuer->removeReinforcements(*numArmies);
        targetTerritory->addArmies(*numArmies);

        *effect = "Deployed " + std::to_string(*numArmies) + " armies to "
                  + targetTerritory->getName() + ". Territory now has "
                  + std::to_string(targetTerritory->getArmies()) + " armies.";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
    } else {
        *effect = "Deploy order not executed: validation failed.";
        cout << "  -> " << *effect << endl;
    }
}

Order* Deploy::clone() const {
    return new Deploy(*this);
}

ostream& operator<<(ostream& os, const Deploy& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== Advance ====================
// Moves armies from source to adjacent target territory.
// If target is friendly: armies are moved.
// If target is enemy: battle simulation (60% attacker kill, 70% defender kill).

Advance::Advance() : Order() {
    delete description;
    description = new string("Advance Order");
    numArmies = new int(0);
    sourceTerritory = nullptr;
    targetTerritory = nullptr;
    deck = nullptr;
}

Advance::Advance(Player* issuer, int armies, Territory* source, Territory* target, Deck* deck)
    : Order(issuer) {
    delete description;
    string srcName = source ? source->getName() : "NULL";
    string tgtName = target ? target->getName() : "NULL";
    description = new string("Advance " + std::to_string(armies) + " from " + srcName + " to " + tgtName);
    numArmies = new int(armies);
    sourceTerritory = source;
    targetTerritory = target;
    this->deck = deck;
}

Advance::Advance(const Advance& other) : Order(other) {
    numArmies = new int(*(other.numArmies));
    sourceTerritory = other.sourceTerritory;
    targetTerritory = other.targetTerritory;
    deck = other.deck;
}

Advance::~Advance() {
    delete numArmies;
}

Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        delete numArmies;

        numArmies = new int(*(other.numArmies));
        sourceTerritory = other.sourceTerritory;
        targetTerritory = other.targetTerritory;
        deck = other.deck;
    }
    return *this;
}

bool Advance::validate() const {
    if (issuer == nullptr || sourceTerritory == nullptr || targetTerritory == nullptr) {
        cout << "Advance validation FAILED: null issuer, source, or target." << endl;
        return false;
    }
    // Source territory must belong to the issuer
    if (sourceTerritory->getOwner() != issuer) {
        cout << "Advance validation FAILED: " << sourceTerritory->getName()
             << " does not belong to " << issuer->getName() << "." << endl;
        return false;
    }
    // Target must be adjacent to source
    bool adjacent = false;
    for (Territory* neighbor : sourceTerritory->getBorders()) {
        if (neighbor == targetTerritory) {
            adjacent = true;
            break;
        }
    }
    if (!adjacent) {
        cout << "Advance validation FAILED: " << targetTerritory->getName()
             << " is not adjacent to " << sourceTerritory->getName() << "." << endl;
        return false;
    }
    if (*numArmies <= 0) {
        cout << "Advance validation FAILED: number of armies must be positive." << endl;
        return false;
    }
    cout << "Advance validation PASSED." << endl;
    return true;
}

void Advance::execute() {
    if (!validate()) {
        *effect = "Advance order not executed: validation failed.";
        cout << "  -> " << *effect << endl;
        return;
    }

    // Cap armies to what's available on source
    int armiesToMove = std::min(*numArmies, sourceTerritory->getArmies());
    if (armiesToMove <= 0) {
        *effect = "Advance order: no armies available to move from " + sourceTerritory->getName() + ".";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
        return;
    }

    // Check if target belongs to the same player (friendly move)
    if (targetTerritory->getOwner() == issuer) {
        sourceTerritory->removeArmies(armiesToMove);
        targetTerritory->addArmies(armiesToMove);
        *effect = "Moved " + std::to_string(armiesToMove) + " armies from "
                  + sourceTerritory->getName() + " to " + targetTerritory->getName() + " (friendly).";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
        return;
    }

    // ----- Enemy territory: check for negotiate -----
    Player* defender = targetTerritory->getOwner();
    if (defender != nullptr && issuer->hasNegotiationWith(defender->getName())) {
        *effect = "Advance BLOCKED by negotiation between " + issuer->getName()
                  + " and " + defender->getName() + ".";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
        return;
    }

    // ----- Battle simulation -----
    sourceTerritory->removeArmies(armiesToMove);

    int attackers = armiesToMove;
    int defenders = targetTerritory->getArmies();

    // Each attacker has 60% chance to kill a defender
    int defendersKilled = 0;
    for (int i = 0; i < attackers; i++) {
        if (rand() % 100 < 60) defendersKilled++;
    }
    defendersKilled = std::min(defendersKilled, defenders);

    // Each defender has 70% chance to kill an attacker
    int attackersKilled = 0;
    for (int i = 0; i < defenders; i++) {
        if (rand() % 100 < 70) attackersKilled++;
    }
    attackersKilled = std::min(attackersKilled, attackers);

    int survivingAttackers = attackers - attackersKilled;
    int survivingDefenders = defenders - defendersKilled;

    cout << "  Battle: " << attackers << " attackers vs " << defenders << " defenders." << endl;
    cout << "  Attackers killed: " << attackersKilled << ", Defenders killed: " << defendersKilled << endl;
    cout << "  Surviving attackers: " << survivingAttackers << ", Surviving defenders: " << survivingDefenders << endl;

    if (survivingDefenders <= 0) {
        // Attacker conquers the territory
        // Remove territory from defender's list
        if (defender != nullptr) {
            defender->removeTerritory(targetTerritory);
        }
        // Transfer ownership
        targetTerritory->setOwner(issuer);
        issuer->addTerritory(targetTerritory);
        targetTerritory->setArmies(survivingAttackers);

        // Mark that the player conquered a territory this turn (for card awarding)
        issuer->setConqueredThisTurn(true);

        *effect = issuer->getName() + " conquered " + targetTerritory->getName()
                  + " with " + std::to_string(survivingAttackers) + " surviving armies!";
    } else {
        // Attack failed, surviving defenders remain
        targetTerritory->setArmies(survivingDefenders);
        *effect = "Attack on " + targetTerritory->getName() + " failed. "
                  + std::to_string(survivingDefenders) + " defenders remain.";
    }

    *executed = true;
    cout << "  -> " << *effect << endl;
    notify(this);
}

Order* Advance::clone() const {
    return new Advance(*this);
}

ostream& operator<<(ostream& os, const Advance& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== Bomb ====================
// Destroys half of armies on an enemy territory adjacent to one of the player's territories.

Bomb::Bomb() : Order() {
    delete description;
    description = new string("Bomb Order");
    targetTerritory = nullptr;
}

Bomb::Bomb(Player* issuer, Territory* target) : Order(issuer) {
    delete description;
    string targetName = target ? target->getName() : "NULL";
    description = new string("Bomb " + targetName);
    targetTerritory = target;
}

Bomb::Bomb(const Bomb& other) : Order(other) {
    targetTerritory = other.targetTerritory;
}

Bomb::~Bomb() {
    // No owned heap data beyond base class
}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        targetTerritory = other.targetTerritory;
    }
    return *this;
}

bool Bomb::validate() const {
    if (issuer == nullptr || targetTerritory == nullptr) {
        cout << "Bomb validation FAILED: null issuer or target territory." << endl;
        return false;
    }
    // Target must NOT belong to the player issuing the order
    if (targetTerritory->getOwner() == issuer) {
        cout << "Bomb validation FAILED: cannot bomb your own territory ("
             << targetTerritory->getName() << ")." << endl;
        return false;
    }
    // Target must be adjacent to at least one of the issuer's territories
    bool adjacent = false;
    for (Territory* ownedT : *(issuer->getTerritories())) {
        for (Territory* neighbor : ownedT->getBorders()) {
            if (neighbor == targetTerritory) {
                adjacent = true;
                break;
            }
        }
        if (adjacent) break;
    }
    if (!adjacent) {
        cout << "Bomb validation FAILED: " << targetTerritory->getName()
             << " is not adjacent to any of " << issuer->getName() << "'s territories." << endl;
        return false;
    }
    // Check negotiate
    Player* targetOwner = targetTerritory->getOwner();
    if (targetOwner != nullptr && issuer->hasNegotiationWith(targetOwner->getName())) {
        cout << "Bomb validation FAILED: negotiation active between "
             << issuer->getName() << " and " << targetOwner->getName() << "." << endl;
        return false;
    }
    cout << "Bomb validation PASSED." << endl;
    return true;
}

void Bomb::execute() {
    if (validate()) {
        int armiesBefore = targetTerritory->getArmies();
        int armiesRemoved = armiesBefore / 2;
        targetTerritory->setArmies(armiesBefore - armiesRemoved);

        *effect = "Bombed " + targetTerritory->getName() + ": removed "
                  + std::to_string(armiesRemoved) + " armies (had " + std::to_string(armiesBefore)
                  + ", now " + std::to_string(targetTerritory->getArmies()) + ").";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
    } else {
        *effect = "Bomb order not executed: validation failed.";
        cout << "  -> " << *effect << endl;
    }
}

Order* Bomb::clone() const {
    return new Bomb(*this);
}

ostream& operator<<(ostream& os, const Bomb& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== Blockade ====================
// Doubles armies on a territory and transfers ownership to the Neutral player.

// Static neutral player initialization
Player* Blockade::neutralPlayer = nullptr;

Blockade::Blockade() : Order() {
    delete description;
    description = new string("Blockade Order");
    targetTerritory = nullptr;
}

Blockade::Blockade(Player* issuer, Territory* target) : Order(issuer) {
    delete description;
    string targetName = target ? target->getName() : "NULL";
    description = new string("Blockade " + targetName);
    targetTerritory = target;
}

Blockade::Blockade(const Blockade& other) : Order(other) {
    targetTerritory = other.targetTerritory;
}

Blockade::~Blockade() {
    // No owned heap data beyond base class
}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        targetTerritory = other.targetTerritory;
    }
    return *this;
}

bool Blockade::validate() const {
    if (issuer == nullptr || targetTerritory == nullptr) {
        cout << "Blockade validation FAILED: null issuer or target territory." << endl;
        return false;
    }
    // Target territory must belong to the player issuing the order
    if (targetTerritory->getOwner() != issuer) {
        cout << "Blockade validation FAILED: " << targetTerritory->getName()
             << " does not belong to " << issuer->getName() << "." << endl;
        return false;
    }
    cout << "Blockade validation PASSED." << endl;
    return true;
}

void Blockade::execute() {
    if (validate()) {
        // Create the Neutral player if it doesn't exist yet
        if (neutralPlayer == nullptr) {
            neutralPlayer = new Player("Neutral");
            cout << "  (Created Neutral player)" << endl;
        }

        // Double the armies
        int currentArmies = targetTerritory->getArmies();
        targetTerritory->setArmies(currentArmies * 2);

        // Remove from issuer's territory list
        issuer->removeTerritory(targetTerritory);

        // Transfer ownership to Neutral player
        targetTerritory->setOwner(neutralPlayer);
        neutralPlayer->addTerritory(targetTerritory);

        *effect = "Blockaded " + targetTerritory->getName() + ": armies doubled from "
                  + std::to_string(currentArmies) + " to " + std::to_string(targetTerritory->getArmies())
                  + ", ownership transferred to Neutral.";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
    } else {
        *effect = "Blockade order not executed: validation failed.";
        cout << "  -> " << *effect << endl;
    }
}

Order* Blockade::clone() const {
    return new Blockade(*this);
}

ostream& operator<<(ostream& os, const Blockade& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== Airlift ====================
// Moves armies from a source to any target territory, both owned by the player.

Airlift::Airlift() : Order() {
    delete description;
    description = new string("Airlift Order");
    numArmies = new int(0);
    sourceTerritory = nullptr;
    targetTerritory = nullptr;
}

Airlift::Airlift(Player* issuer, int armies, Territory* source, Territory* target)
    : Order(issuer) {
    delete description;
    string srcName = source ? source->getName() : "NULL";
    string tgtName = target ? target->getName() : "NULL";
    description = new string("Airlift " + std::to_string(armies) + " from " + srcName + " to " + tgtName);
    numArmies = new int(armies);
    sourceTerritory = source;
    targetTerritory = target;
}

Airlift::Airlift(const Airlift& other) : Order(other) {
    numArmies = new int(*(other.numArmies));
    sourceTerritory = other.sourceTerritory;
    targetTerritory = other.targetTerritory;
}

Airlift::~Airlift() {
    delete numArmies;
}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        delete numArmies;

        numArmies = new int(*(other.numArmies));
        sourceTerritory = other.sourceTerritory;
        targetTerritory = other.targetTerritory;
    }
    return *this;
}

bool Airlift::validate() const {
    if (issuer == nullptr || sourceTerritory == nullptr || targetTerritory == nullptr) {
        cout << "Airlift validation FAILED: null issuer, source, or target." << endl;
        return false;
    }
    // Source must belong to issuer
    if (sourceTerritory->getOwner() != issuer) {
        cout << "Airlift validation FAILED: " << sourceTerritory->getName()
             << " does not belong to " << issuer->getName() << "." << endl;
        return false;
    }
    // Target must belong to issuer
    if (targetTerritory->getOwner() != issuer) {
        cout << "Airlift validation FAILED: " << targetTerritory->getName()
             << " does not belong to " << issuer->getName() << "." << endl;
        return false;
    }
    if (*numArmies <= 0) {
        cout << "Airlift validation FAILED: number of armies must be positive." << endl;
        return false;
    }
    cout << "Airlift validation PASSED." << endl;
    return true;
}

void Airlift::execute() {
    if (validate()) {
        int armiesToMove = std::min(*numArmies, sourceTerritory->getArmies());
        sourceTerritory->removeArmies(armiesToMove);
        targetTerritory->addArmies(armiesToMove);

        *effect = "Airlifted " + std::to_string(armiesToMove) + " armies from "
                  + sourceTerritory->getName() + " to " + targetTerritory->getName() + ".";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
    } else {
        *effect = "Airlift order not executed: validation failed.";
        cout << "  -> " << *effect << endl;
    }
}

Order* Airlift::clone() const {
    return new Airlift(*this);
}

ostream& operator<<(ostream& os, const Airlift& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== Negotiate ====================
// Prevents attacks between two players for the remainder of the turn.

Negotiate::Negotiate() : Order() {
    delete description;
    description = new string("Negotiate Order");
    targetPlayer = nullptr;
}

Negotiate::Negotiate(Player* issuer, Player* target) : Order(issuer) {
    delete description;
    string targetName = target ? target->getName() : "NULL";
    description = new string("Negotiate with " + targetName);
    targetPlayer = target;
}

Negotiate::Negotiate(const Negotiate& other) : Order(other) {
    targetPlayer = other.targetPlayer;
}

Negotiate::~Negotiate() {
    // No owned heap data beyond base class
}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        targetPlayer = other.targetPlayer;
    }
    return *this;
}

bool Negotiate::validate() const {
    if (issuer == nullptr || targetPlayer == nullptr) {
        cout << "Negotiate validation FAILED: null issuer or target player." << endl;
        return false;
    }
    // Cannot negotiate with yourself
    if (targetPlayer == issuer) {
        cout << "Negotiate validation FAILED: cannot negotiate with yourself." << endl;
        return false;
    }
    cout << "Negotiate validation PASSED." << endl;
    return true;
}

void Negotiate::execute() {
    if (validate()) {
        // Add mutual negotiation — both players cannot attack each other this turn
        issuer->addNegotiatedPlayer(targetPlayer->getName());
        targetPlayer->addNegotiatedPlayer(issuer->getName());

        *effect = "Negotiation established between " + issuer->getName()
                  + " and " + targetPlayer->getName()
                  + ". No attacks allowed between them this turn.";
        *executed = true;
        cout << "  -> " << *effect << endl;
        notify(this);
    } else {
        *effect = "Negotiate order not executed: validation failed.";
        cout << "  -> " << *effect << endl;
    }
}

Order* Negotiate::clone() const {
    return new Negotiate(*this);
}

ostream& operator<<(ostream& os, const Negotiate& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== OrdersList ====================

OrdersList::OrdersList() {
    orders = new vector<Order*>();
}

OrdersList::OrdersList(const OrdersList& other) {
    orders = new vector<Order*>();
    for (Order* order : *(other.orders)) {
        orders->push_back(order->clone());
    }
}

OrdersList::~OrdersList() {
    for (Order* order : *orders) {
        delete order;
    }
    delete orders;
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        for (Order* order : *orders) {
            delete order;
        }
        orders->clear();

        for (Order* order : *(other.orders)) {
            orders->push_back(order->clone());
        }
    }
    return *this;
}

void OrdersList::addOrder(Order* order) {
    if (order != nullptr) {
        orders->push_back(order);
        cout << "Order added to list: " << *order << endl;
        notify(this);
    }
}

void OrdersList::remove(int index) {
    if (index >= 0 && index < static_cast<int>(orders->size())) {
        cout << "Removing order at index " << index << ": " << *((*orders)[index]) << endl;
        delete (*orders)[index];
        orders->erase(orders->begin() + index);
    } else {
        cout << "Error: Invalid index " << index << " for remove operation." << endl;
    }
}

void OrdersList::move(int fromIndex, int toIndex) {
    int size = static_cast<int>(orders->size());

    if (fromIndex < 0 || fromIndex >= size || toIndex < 0 || toIndex >= size) {
        cout << "Error: Invalid indices for move operation. From: " << fromIndex << ", To: " << toIndex << endl;
        return;
    }

    if (fromIndex == toIndex) {
        cout << "Order already at position " << toIndex << endl;
        return;
    }

    Order* orderToMove = (*orders)[fromIndex];
    orders->erase(orders->begin() + fromIndex);
    orders->insert(orders->begin() + toIndex, orderToMove);

    cout << "Moved order from index " << fromIndex << " to index " << toIndex << endl;
}

// logs the most recently added order
string OrdersList::stringToLog() const {
    if (orders->empty()) {
        return "OrdersList: (empty)";
    }
    Order* lastOrder = orders->back();
    return "Order Added: " + lastOrder->getDescription();
}

int OrdersList::getSize() const {
    return static_cast<int>(orders->size());
}

Order* OrdersList::getOrder(int index) const {
    if (index >= 0 && index < static_cast<int>(orders->size())) {
        return (*orders)[index];
    }
    return nullptr;
}

ostream& operator<<(ostream& os, const OrdersList& ordersList) {
    os << "OrdersList (" << ordersList.orders->size() << " orders):" << endl;
    int index = 0;
    for (Order* order : *(ordersList.orders)) {
        os << "  [" << index++ << "] " << *order << endl;
    }
    return os;
}
