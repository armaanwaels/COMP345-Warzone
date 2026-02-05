#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>

// Forward declarations
class Player;
class Territory;

// Base class for all order types
class Order {
protected:
    std::string* description;
    std::string* effect;
    bool* executed;

public:
    Order();
    Order(const Order& other);
    virtual ~Order();

    Order& operator=(const Order& other);

    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual Order* clone() const = 0;

    std::string getDescription() const;
    std::string getEffect() const;
    bool isExecuted() const;

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

// Deploy armies to a territory
class Deploy : public Order {
private:
    int* numArmies;
    std::string* targetTerritory;

public:
    Deploy();
    Deploy(int armies, const std::string& territory);
    Deploy(const Deploy& other);
    ~Deploy();

    Deploy& operator=(const Deploy& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Deploy& order);
};

// Move armies from one territory to another (attack if enemy)
class Advance : public Order {
private:
    int* numArmies;
    std::string* sourceTerritory;
    std::string* targetTerritory;

public:
    Advance();
    Advance(int armies, const std::string& source, const std::string& target);
    Advance(const Advance& other);
    ~Advance();

    Advance& operator=(const Advance& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Advance& order);
};

// Destroy half of armies on an enemy territory
class Bomb : public Order {
private:
    std::string* targetTerritory;

public:
    Bomb();
    Bomb(const std::string& territory);
    Bomb(const Bomb& other);
    ~Bomb();

    Bomb& operator=(const Bomb& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Bomb& order);
};

// Triple armies on a territory and make it neutral
class Blockade : public Order {
private:
    std::string* targetTerritory;

public:
    Blockade();
    Blockade(const std::string& territory);
    Blockade(const Blockade& other);
    ~Blockade();

    Blockade& operator=(const Blockade& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Blockade& order);
};

// Move armies to any territory (not just adjacent)
class Airlift : public Order {
private:
    int* numArmies;
    std::string* sourceTerritory;
    std::string* targetTerritory;

public:
    Airlift();
    Airlift(int armies, const std::string& source, const std::string& target);
    Airlift(const Airlift& other);
    ~Airlift();

    Airlift& operator=(const Airlift& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Airlift& order);
};

// Prevent attacks between two players for a turn
class Negotiate : public Order {
private:
    std::string* targetPlayer;

public:
    Negotiate();
    Negotiate(const std::string& player);
    Negotiate(const Negotiate& other);
    ~Negotiate();

    Negotiate& operator=(const Negotiate& other);

    bool validate() const override;
    void execute() override;
    Order* clone() const override;

    friend std::ostream& operator<<(std::ostream& os, const Negotiate& order);
};

// Container for Order objects
class OrdersList {
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

    int getSize() const;
    Order* getOrder(int index) const;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ordersList);
};

#endif
