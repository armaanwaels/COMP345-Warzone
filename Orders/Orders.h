#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>
#include "../LoggingObserver/LoggingObserver.h"

// Forward declarations
class Player;
class Territory;
class Deck;

// ==================== Order Base Class ====================
// Abstract base class for all order types.
// Each subclass stores non-owning pointers to game objects (Player*, Territory*).
class Order : public Subject, public ILoggable {
protected:
    std::string* description;
    std::string* effect;
    bool* executed;
    Player* issuer;  // Non-owning pointer to the player who issued this order

public:
    Order();
    Order(Player* issuer);
    Order(const Order& other);
    virtual ~Order();

    Order& operator=(const Order& other);

    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual Order* clone() const = 0;

    std::string stringToLog() const override;

    std::string getDescription() const;
    std::string getEffect() const;
    bool isExecuted() const;
    Player* getIssuer() const;

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

// ==================== Deploy ====================
// Deploy armies from the reinforcement pool to a territory owned by the player.
class Deploy : public Order {
private:
    int* numArmies;
    Territory* targetTerritory;  // Non-owning

public:
    Deploy();
    Deploy(Player* issuer, int armies, Territory* target);
    Deploy(const Deploy& other);
    ~Deploy();

    Deploy& operator=(const Deploy& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Deploy& order);
};

// ==================== Advance ====================
// Move armies from a source to an adjacent target territory.
// If target is enemy, a battle is simulated.
class Advance : public Order {
private:
    int* numArmies;
    Territory* sourceTerritory;  // Non-owning
    Territory* targetTerritory;  // Non-owning
    Deck* deck;                  // Non-owning: used to draw card on conquest

public:
    Advance();
    Advance(Player* issuer, int armies, Territory* source, Territory* target, Deck* deck);
    Advance(const Advance& other);
    ~Advance();

    Advance& operator=(const Advance& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Advance& order);
};

// ==================== Bomb ====================
// Destroy half of the armies on an enemy territory adjacent to one of the player's.
class Bomb : public Order {
private:
    Territory* targetTerritory;  // Non-owning

public:
    Bomb();
    Bomb(Player* issuer, Territory* target);
    Bomb(const Bomb& other);
    ~Bomb();

    Bomb& operator=(const Bomb& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Bomb& order);
};

// ==================== Blockade ====================
// Double armies on a territory and transfer it to the Neutral player.
class Blockade : public Order {
private:
    Territory* targetTerritory;  // Non-owning

public:
    // Static neutral player: created on first Blockade execution if needed
    static Player* neutralPlayer;

    Blockade();
    Blockade(Player* issuer, Territory* target);
    Blockade(const Blockade& other);
    ~Blockade();

    Blockade& operator=(const Blockade& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Blockade& order);
};

// ==================== Airlift ====================
// Move armies from a source to any target territory, both owned by the player.
class Airlift : public Order {
private:
    int* numArmies;
    Territory* sourceTerritory;  // Non-owning
    Territory* targetTerritory;  // Non-owning

public:
    Airlift();
    Airlift(Player* issuer, int armies, Territory* source, Territory* target);
    Airlift(const Airlift& other);
    ~Airlift();

    Airlift& operator=(const Airlift& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Airlift& order);
};

// ==================== Negotiate ====================
// Prevents attacks between two players for the remainder of the turn.
class Negotiate : public Order {
private:
    Player* targetPlayer;  // Non-owning

public:
    Negotiate();
    Negotiate(Player* issuer, Player* target);
    Negotiate(const Negotiate& other);
    ~Negotiate();

    Negotiate& operator=(const Negotiate& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Negotiate& order);
};

// ==================== OrdersList ====================
// Container for Order objects with add, remove, move operations.
class OrdersList : public Subject, public ILoggable {
private:
    std::vector<Order*>* orders;

public:
    OrdersList();
    OrdersList(const OrdersList& other);
    ~OrdersList();

    OrdersList& operator=(const OrdersList& other);

    void addOrder(Order* order);
    void remove(int index);
    void move(int fromIndex, int toIndex);

    std::string stringToLog() const override;

    int getSize() const;
    Order* getOrder(int index) const;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ordersList);
};

#endif
