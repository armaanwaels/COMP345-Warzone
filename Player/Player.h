#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <vector>

class Territory;
class Hand;
class OrdersList;
class Order;

class Player {
    private:
        // Attributes
        std::string* name;
        std::vector<Territory*>* territories;
        Hand* hand;
        OrdersList* orders;
        int* reinforcementPool;

    public:
        // Constructors and Destructor
        Player();
        Player(const std::string& name);
        Player(const Player& other);
        Player& operator=(const Player& other);
        ~Player();

        // Getter Methods
        const std::string& getName() const;
        std::vector<Territory*>* getTerritories() const;
        Hand* getHand() const;
        OrdersList* getOrders() const;
        int getReinforcementPool() const;

        // Order Management
        void issueOrder();

        // Territory Management
        void addTerritory(Territory* territory);
        void removeTerritory(Territory* territory);
        std::vector<Territory*> toDefend() const;
        std::vector<Territory*> toAttack() const;

        // Reinforcement Management
        void receiveReinforcements(int numOfArmies);
        void removeReinforcements(int numOfArmies);
        void setReinforcementPool(int armies);

        // Stream Insertion Operator
        friend std::ostream& operator<<(std::ostream& os, const Player& player);

};

#endif