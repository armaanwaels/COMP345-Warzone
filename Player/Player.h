#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <vector>

class Territory;
class Hand;
class OrdersList;
class Order;
class Deck;
class Map;
class PlayerStrategy;

class Player {
    private:
        std::string* name;
        std::vector<Territory*>* territories;
        Hand* hand;
        OrdersList* orders;
        PlayerStrategy* strategy;
        int* reinforcementPool;

        // order execution support
        bool* conqueredTerritoryThisTurn;
        std::vector<std::string>* negotiatedPlayers;

    public:
        // Constructors and Destructor
        Player();
        Player(const std::string& name);
        Player(const Player& other);
        Player& operator=(const Player& other);
        ~Player();

        // Getters
        const std::string& getName() const;
        std::vector<Territory*>* getTerritories() const;
        Hand* getHand() const;
        OrdersList* getOrders() const;
        PlayerStrategy* getStrategy() const;
        int getReinforcementPool() const;

        // Strategy Setter
        void setStrategy(PlayerStrategy* newStrategy);

        // Order Management - issues one order per call, returns false when done
        bool issueOrder(Deck* deck, Map* map);

        // Territory Management
        void addTerritory(Territory* territory);
        void removeTerritory(Territory* territory);
        std::vector<Territory*> toDefend() const;
        std::vector<Territory*> toAttack() const;

        // Reinforcement Pool Management
        void setReinforcementPool(int pool);
        void addReinforcements(int count);
        void removeReinforcements(int count);

        // Conquest tracking
        bool hasConqueredThisTurn() const;
        void setConqueredThisTurn(bool val);

        // Negotiation tracking
        void addNegotiatedPlayer(const std::string& playerName);
        bool hasNegotiationWith(const std::string& playerName) const;
        void clearNegotiations();

        // Stream Insertion Operator
        friend std::ostream& operator<<(std::ostream& os, const Player& player);

};

#endif
