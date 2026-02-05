#include "Orders.h"
#include <algorithm>

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
}

Order::Order(const Order& other) {
    description = new string(*(other.description));
    effect = new string(*(other.effect));
    executed = new bool(*(other.executed));
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
    }
    return *this;
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

ostream& operator<<(ostream& os, const Order& order) {
    os << "Order: " << *(order.description);
    if (*(order.executed)) {
        os << " | Effect: " << *(order.effect);
    }
    return os;
}

// ==================== Deploy ====================

Deploy::Deploy() : Order() {
    delete description;
    description = new string("Deploy Order");
    numArmies = new int(0);
    targetTerritory = new string("");
}

Deploy::Deploy(int armies, const string& territory) : Order() {
    delete description;
    description = new string("Deploy " + std::to_string(armies) + " armies to " + territory);
    numArmies = new int(armies);
    targetTerritory = new string(territory);
}

Deploy::Deploy(const Deploy& other) : Order(other) {
    numArmies = new int(*(other.numArmies));
    targetTerritory = new string(*(other.targetTerritory));
}

Deploy::~Deploy() {
    delete numArmies;
    delete targetTerritory;
}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        delete numArmies;
        delete targetTerritory;

        numArmies = new int(*(other.numArmies));
        targetTerritory = new string(*(other.targetTerritory));
    }
    return *this;
}

bool Deploy::validate() const {
    if (*numArmies <= 0) {
        cout << "Deploy order invalid: Number of armies must be positive." << endl;
        return false;
    }
    if (targetTerritory->empty()) {
        cout << "Deploy order invalid: Target territory must be specified." << endl;
        return false;
    }
    return true;
}

void Deploy::execute() {
    if (validate()) {
        *effect = "Deployed " + std::to_string(*numArmies) + " armies to " + *targetTerritory;
        *executed = true;
        cout << "Executing Deploy: " << *effect << endl;
    } else {
        cout << "Deploy order not executed due to validation failure." << endl;
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

Advance::Advance() : Order() {
    delete description;
    description = new string("Advance Order");
    numArmies = new int(0);
    sourceTerritory = new string("");
    targetTerritory = new string("");
}

Advance::Advance(int armies, const string& source, const string& target) : Order() {
    delete description;
    description = new string("Advance " + std::to_string(armies) + " armies from " + source + " to " + target);
    numArmies = new int(armies);
    sourceTerritory = new string(source);
    targetTerritory = new string(target);
}

Advance::Advance(const Advance& other) : Order(other) {
    numArmies = new int(*(other.numArmies));
    sourceTerritory = new string(*(other.sourceTerritory));
    targetTerritory = new string(*(other.targetTerritory));
}

Advance::~Advance() {
    delete numArmies;
    delete sourceTerritory;
    delete targetTerritory;
}

Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        delete numArmies;
        delete sourceTerritory;
        delete targetTerritory;

        numArmies = new int(*(other.numArmies));
        sourceTerritory = new string(*(other.sourceTerritory));
        targetTerritory = new string(*(other.targetTerritory));
    }
    return *this;
}

bool Advance::validate() const {
    if (*numArmies <= 0) {
        cout << "Advance order invalid: Number of armies must be positive." << endl;
        return false;
    }
    if (sourceTerritory->empty() || targetTerritory->empty()) {
        cout << "Advance order invalid: Source and target territories must be specified." << endl;
        return false;
    }
    return true;
}

void Advance::execute() {
    if (validate()) {
        *effect = "Advanced " + std::to_string(*numArmies) + " armies from " + *sourceTerritory + " to " + *targetTerritory;
        *executed = true;
        cout << "Executing Advance: " << *effect << endl;
    } else {
        cout << "Advance order not executed due to validation failure." << endl;
    }
}

Order* Advance::clone() const {
    return new Advance(*this);
}

ostream& operator<<(ostream& os, const Advance& order) {
    os << static_cast<const Order&>(order);
    return os;
}

// ==================== Bomb ====================

Bomb::Bomb() : Order() {
    delete description;
    description = new string("Bomb Order");
    targetTerritory = new string("");
}

Bomb::Bomb(const string& territory) : Order() {
    delete description;
    description = new string("Bomb " + territory);
    targetTerritory = new string(territory);
}

Bomb::Bomb(const Bomb& other) : Order(other) {
    targetTerritory = new string(*(other.targetTerritory));
}

Bomb::~Bomb() {
    delete targetTerritory;
}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        delete targetTerritory;

        targetTerritory = new string(*(other.targetTerritory));
    }
    return *this;
}

bool Bomb::validate() const {
    if (targetTerritory->empty()) {
        cout << "Bomb order invalid: Target territory must be specified." << endl;
        return false;
    }
    return true;
}

void Bomb::execute() {
    if (validate()) {
        *effect = "Bombed " + *targetTerritory + ", destroying half of enemy armies";
        *executed = true;
        cout << "Executing Bomb: " << *effect << endl;
    } else {
        cout << "Bomb order not executed due to validation failure." << endl;
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

Blockade::Blockade() : Order() {
    delete description;
    description = new string("Blockade Order");
    targetTerritory = new string("");
}

Blockade::Blockade(const string& territory) : Order() {
    delete description;
    description = new string("Blockade " + territory);
    targetTerritory = new string(territory);
}

Blockade::Blockade(const Blockade& other) : Order(other) {
    targetTerritory = new string(*(other.targetTerritory));
}

Blockade::~Blockade() {
    delete targetTerritory;
}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        delete targetTerritory;

        targetTerritory = new string(*(other.targetTerritory));
    }
    return *this;
}

bool Blockade::validate() const {
    if (targetTerritory->empty()) {
        cout << "Blockade order invalid: Target territory must be specified." << endl;
        return false;
    }
    return true;
}

void Blockade::execute() {
    if (validate()) {
        *effect = "Blockaded " + *targetTerritory + ", tripling armies and making it neutral";
        *executed = true;
        cout << "Executing Blockade: " << *effect << endl;
    } else {
        cout << "Blockade order not executed due to validation failure." << endl;
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

Airlift::Airlift() : Order() {
    delete description;
    description = new string("Airlift Order");
    numArmies = new int(0);
    sourceTerritory = new string("");
    targetTerritory = new string("");
}

Airlift::Airlift(int armies, const string& source, const string& target) : Order() {
    delete description;
    description = new string("Airlift " + std::to_string(armies) + " armies from " + source + " to " + target);
    numArmies = new int(armies);
    sourceTerritory = new string(source);
    targetTerritory = new string(target);
}

Airlift::Airlift(const Airlift& other) : Order(other) {
    numArmies = new int(*(other.numArmies));
    sourceTerritory = new string(*(other.sourceTerritory));
    targetTerritory = new string(*(other.targetTerritory));
}

Airlift::~Airlift() {
    delete numArmies;
    delete sourceTerritory;
    delete targetTerritory;
}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        delete numArmies;
        delete sourceTerritory;
        delete targetTerritory;

        numArmies = new int(*(other.numArmies));
        sourceTerritory = new string(*(other.sourceTerritory));
        targetTerritory = new string(*(other.targetTerritory));
    }
    return *this;
}

bool Airlift::validate() const {
    if (*numArmies <= 0) {
        cout << "Airlift order invalid: Number of armies must be positive." << endl;
        return false;
    }
    if (sourceTerritory->empty() || targetTerritory->empty()) {
        cout << "Airlift order invalid: Source and target territories must be specified." << endl;
        return false;
    }
    return true;
}

void Airlift::execute() {
    if (validate()) {
        *effect = "Airlifted " + std::to_string(*numArmies) + " armies from " + *sourceTerritory + " to " + *targetTerritory;
        *executed = true;
        cout << "Executing Airlift: " << *effect << endl;
    } else {
        cout << "Airlift order not executed due to validation failure." << endl;
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

Negotiate::Negotiate() : Order() {
    delete description;
    description = new string("Negotiate Order");
    targetPlayer = new string("");
}

Negotiate::Negotiate(const string& player) : Order() {
    delete description;
    description = new string("Negotiate with " + player);
    targetPlayer = new string(player);
}

Negotiate::Negotiate(const Negotiate& other) : Order(other) {
    targetPlayer = new string(*(other.targetPlayer));
}

Negotiate::~Negotiate() {
    delete targetPlayer;
}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        delete targetPlayer;

        targetPlayer = new string(*(other.targetPlayer));
    }
    return *this;
}

bool Negotiate::validate() const {
    if (targetPlayer->empty()) {
        cout << "Negotiate order invalid: Target player must be specified." << endl;
        return false;
    }
    return true;
}

void Negotiate::execute() {
    if (validate()) {
        *effect = "Negotiated with " + *targetPlayer + ", preventing attacks until end of turn";
        *executed = true;
        cout << "Executing Negotiate: " << *effect << endl;
    } else {
        cout << "Negotiate order not executed due to validation failure." << endl;
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
