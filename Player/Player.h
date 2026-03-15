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

        // Order execution support
        int* reinforcementPool;
        bool* conqueredTerritoryThisTurn;
        std::vector<std::string>* negotiatedPlayers;  // Names of players with active negotiations

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

        // Order Management
        void issueOrder();

        // Territory Management
        void addTerritory(Territory* territory);
        void removeTerritory(Territory* territory);
        std::vector<Territory*> toDefend() const;
        std::vector<Territory*> toAttack() const;

        // Reinforcement Pool Management
        int getReinforcementPool() const;
        void setReinforcementPool(int pool);
        void addReinforcements(int count);
        void removeReinforcements(int count);

        // Conquest tracking (for card awarding at end of turn)
        bool hasConqueredThisTurn() const;
        void setConqueredThisTurn(bool val);

        // Negotiation tracking (for Negotiate/Diplomacy order)
        void addNegotiatedPlayer(const std::string& playerName);
        bool hasNegotiationWith(const std::string& playerName) const;
        void clearNegotiations();

        // Stream Insertion Operator
        friend std::ostream& operator<<(std::ostream& os, const Player& player);

};

#endif