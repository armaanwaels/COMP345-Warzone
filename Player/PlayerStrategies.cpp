#include "PlayerStrategies.h"
#include "Player.h"
#include "../Map/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"

#include <algorithm>
#include <set>
#include <iostream>

// Base Player Strategy
PlayerStrategy::PlayerStrategy() {
}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other) {
}

PlayerStrategy::~PlayerStrategy() {
}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
    }
    return *this;
}

// -----------------------------------------------------------------
//                      Human Player Strategy
// -----------------------------------------------------------------
HumanPlayerStrategy::HumanPlayerStrategy() {
}

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other)
    : PlayerStrategy(other) {
}

HumanPlayerStrategy::~HumanPlayerStrategy() {
}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

std::vector<Territory*> HumanPlayerStrategy::toDefend(Player* player) const {
    if (player == nullptr) return {};

    std::vector<Territory*> defend = *(player->getTerritories());

    std::sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmies() < b->getArmies();
    });

    return defend;
}

std::vector<Territory*> HumanPlayerStrategy::toAttack(Player* player) const {
    if (player == nullptr) return {};

    std::set<Territory*> attackSet;

    for (Territory* owned : *(player->getTerritories())) {
        for (Territory* neighbor : owned->getBorders()) {
            if (neighbor->getOwner() != player) {
                attackSet.insert(neighbor);
            }
        }
    }

    return std::vector<Territory*>(attackSet.begin(), attackSet.end());
}

bool HumanPlayerStrategy::issueOrder(Player* player, Deck* deck, Map* map) {
    (void)map;

    if (player == nullptr) return false;

    std::vector<Territory*> defendList = toDefend(player);
    std::vector<Territory*> attackList = toAttack(player);

    std::cout << "\n--- " << player->getName() << " Human Strategy ---" << std::endl;

    // If reinforcements remain player must deploy
    if (player->getReinforcementPool() > 0) {
        if (defendList.empty()) {
            std::cout << "No territories to deploy to." << std::endl;
            return false;
        }

        std::cout << "Reinforcements remaining: " << player->getReinforcementPool() << std::endl;
        std::cout << "Choose a territory to deploy to:" << std::endl;

        for (int i = 0; i < static_cast<int>(defendList.size()); i++) {
            std::cout << i << ": " << defendList[i]->getName()
                      << " (" << defendList[i]->getArmies() << " armies)" << std::endl;
        }

        int territoryChoice;
        int armies;

        std::cout << "Enter territory index: ";
        std::cin >> territoryChoice;

        if (territoryChoice < 0 || territoryChoice >= static_cast<int>(defendList.size())) {
            std::cout << "Invalid territory choice." << std::endl;
            return false;
        }

        std::cout << "Enter number of armies to deploy: ";
        std::cin >> armies;

        if (armies <= 0 || armies > player->getReinforcementPool()) {
            std::cout << "Invalid number of armies." << std::endl;
            return false;
        }

        player->getOrders()->addOrder(new Deploy(player, armies, defendList[territoryChoice]));
        player->removeReinforcements(armies);
        return true;
    }

    // Player has no more reinforcements
    // Player can now choose an action
    std::cout << "Choose an action:" << std::endl;
    std::cout << "1. Advance to defend" << std::endl;
    std::cout << "2. Advance to attack" << std::endl;
    std::cout << "3. Play first card in hand" << std::endl;
    std::cout << "4. Done issuing orders" << std::endl;

    int choice;
    std::cin >> choice;

    // Advance to defend
    if (choice == 1) {
        if (defendList.size() < 2) {
            std::cout << "Not enough territories to defend with advance." << std::endl;
            return false;
        }

        std::cout << "Your territories:" << std::endl;
        for (int i = 0; i < static_cast<int>(defendList.size()); i++) {
            std::cout << i << ": " << defendList[i]->getName()
                      << " (" << defendList[i]->getArmies() << " armies)" << std::endl;
        }

        int takeIndex, giveIndex, armies;
        std::cout << "Enter index of territory to take reinforcements from: ";
        std::cin >> takeIndex;
        std::cout << "Enter index of territory to give reinforcements to: ";
        std::cin >> giveIndex;
        std::cout << "Enter number of armies: ";
        std::cin >> armies;

        if (takeIndex < 0 || takeIndex >= static_cast<int>(defendList.size()) ||
            giveIndex < 0 || giveIndex >= static_cast<int>(defendList.size()) ||
            armies <= 0) {
            std::cout << "Invalid input." << std::endl;
            return false;
        }

        Territory* source = defendList[takeIndex];
        Territory* target = defendList[giveIndex];

        player->getOrders()->addOrder(new Advance(player, armies, source, target, deck));
        return true;
    }

    // Advance to attack
    if (choice == 2) {
        if (attackList.empty() || defendList.empty()) {
            std::cout << "No valid attack available." << std::endl;
            return false;
        }

        std::cout << "Your territories:" << std::endl;
        for (int i = 0; i < static_cast<int>(defendList.size()); i++) {
            std::cout << i << ": " << defendList[i]->getName()
                      << " (" << defendList[i]->getArmies() << " armies)" << std::endl;
        }

        std::cout << "Enemy territories to attack:" << std::endl;
        for (int i = 0; i < static_cast<int>(attackList.size()); i++) {
            std::cout << i << ": " << attackList[i]->getName()
                      << " (" << attackList[i]->getArmies() << " armies, owner: "
                      << attackList[i]->getOwnerName() << ")" << std::endl;
        }

        int sourceIndex, targetIndex, armies;
        std::cout << "Enter source territory index: ";
        std::cin >> sourceIndex;
        std::cout << "Enter target territory index: ";
        std::cin >> targetIndex;
        std::cout << "Enter number of armies: ";
        std::cin >> armies;

        if (sourceIndex < 0 || sourceIndex >= static_cast<int>(defendList.size()) ||
            targetIndex < 0 || targetIndex >= static_cast<int>(attackList.size()) ||
            armies <= 0) {
            std::cout << "Invalid input." << std::endl;
            return false;
        }

        Territory* source = defendList[sourceIndex];
        Territory* target = attackList[targetIndex];

        player->getOrders()->addOrder(new Advance(player, armies, source, target, deck));
        return true;
    }

    // Play first card in hand
    if (choice == 3) {
        Hand* hand = player->getHand();

        if (hand == nullptr || hand->getSize() == 0) {
            std::cout << "No cards in hand." << std::endl;
            return false;
        }

        Card* card = hand->getCard(0);
        CardType type = card->getType();

        if (type == CardType::Bomb && !attackList.empty()) {
            std::cout << "Playing Bomb on " << attackList[0]->getName() << std::endl;
            player->getOrders()->addOrder(new Bomb(player, attackList[0]));
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
            return true;
        }
        else if (type == CardType::Blockade && !defendList.empty()) {
            std::cout << "Playing Blockade on " << defendList[0]->getName() << std::endl;
            player->getOrders()->addOrder(new Blockade(player, defendList[0]));
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
            return true;
        }
        else if (type == CardType::Airlift && defendList.size() >= 2) {
            Territory* source = defendList.back();
            Territory* target = defendList.front();
            int armies = source->getArmies() / 2;

            if (armies > 0) {
                std::cout << "Playing Airlift from " << source->getName()
                          << " to " << target->getName() << std::endl;
                player->getOrders()->addOrder(new Airlift(player, armies, source, target));
                Card* played = hand->removeCard(0);
                if (deck != nullptr) deck->addCard(played);
                return true;
            }
        }
        else if (type == CardType::Reinforcement) {
            std::cout << "Playing Reinforcement card." << std::endl;
            player->addReinforcements(5);
            Card* played = hand->removeCard(0);
            if (deck != nullptr) deck->addCard(played);
            return true;
        }

        std::cout << "First card could not be used." << std::endl;
        return false;
    }

    // Done issuing
    if (choice == 4) {
        return false;
    }

    std::cout << "Invalid action choice." << std::endl;
    return false;
}

PlayerStrategy* HumanPlayerStrategy::clone() const {
    return new HumanPlayerStrategy(*this);
}

std::string HumanPlayerStrategy::getStrategyName() const {
    return "HumanPlayerStrategy";
}

// -----------------------------------------------------------------
//                      Aggressive Player Strategy
// -----------------------------------------------------------------
AggressivePlayerStrategy::AggressivePlayerStrategy() {
}

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other)
    : PlayerStrategy(other) {
}

AggressivePlayerStrategy::~AggressivePlayerStrategy() {
}

AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

std::vector<Territory*> AggressivePlayerStrategy::toDefend(Player* player) const {
    if (player == nullptr) return {};

    std::vector<Territory*> defend = *(player->getTerritories());

    std::sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmies() > b->getArmies();
    });

    return defend;
}

std::vector<Territory*> AggressivePlayerStrategy::toAttack(Player* player) const {
    if (player == nullptr) return {};

    std::set<Territory*> attackSet;

    for (Territory* owned : *(player->getTerritories())) {
        for (Territory* neighbor : owned->getBorders()) {
            if (neighbor->getOwner() != player) {
                attackSet.insert(neighbor);
            }
        }
    }

    return std::vector<Territory*>(attackSet.begin(), attackSet.end());
}

bool AggressivePlayerStrategy::issueOrder(Player* player, Deck* deck, Map* map) {
    (void)map;

    if (player == nullptr) return false;

    std::vector<Territory*> defendList = toDefend(player);
    if (defendList.empty()) return false;

    Territory* strongest = defendList[0];

    // Deploys all reinforcements to strongest territory
    if (player->getReinforcementPool() > 0) {
        int armies = player->getReinforcementPool();
        player->getOrders()->addOrder(new Deploy(player, armies, strongest));
        player->removeReinforcements(armies);
        return true;
    }

    // Attacks from strongest territory to first adjacent enemy territory
    if (strongest->getArmies() > 1) {
        for (Territory* neighbor : strongest->getBorders()) {
            if (neighbor->getOwner() != player) {
                int armiesToSend = strongest->getArmies() - 1;
                player->getOrders()->addOrder(
                    new Advance(player, armiesToSend, strongest, neighbor, deck)
                );
                return true;
            }
        }
    }

    return false;
}

PlayerStrategy* AggressivePlayerStrategy::clone() const {
    return new AggressivePlayerStrategy(*this);
}

std::string AggressivePlayerStrategy::getStrategyName() const {
    return "AggressivePlayerStrategy";
}

// -----------------------------------------------------------------
//                    Benevolent Player Strategy
// -----------------------------------------------------------------
BenevolentPlayerStrategy::BenevolentPlayerStrategy() {
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other)
    : PlayerStrategy(other) {
}

BenevolentPlayerStrategy::~BenevolentPlayerStrategy() {
}

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

std::vector<Territory*> BenevolentPlayerStrategy::toDefend(Player* player) const {
    if (player == nullptr) return {};

    std::vector<Territory*> defend = *(player->getTerritories());

    std::sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmies() < b->getArmies();
    });

    return defend;
}

std::vector<Territory*> BenevolentPlayerStrategy::toAttack(Player* player) const {
    (void)player;
    return {};
}

bool BenevolentPlayerStrategy::issueOrder(Player* player, Deck* deck, Map* map) {
    (void)deck;
    (void)map;

    if (player == nullptr) return false;

    std::vector<Territory*> defendList = toDefend(player);
    if (defendList.empty()) return false;

    Territory* weakest = defendList[0];

    // deploy to weakest territory first
    if (player->getReinforcementPool() > 0) {
        int armies = player->getReinforcementPool();
        player->getOrders()->addOrder(new Deploy(player, armies, weakest));
        player->removeReinforcements(armies);
        return true;
    }

    // move armies from strongest owned territory to weakest owned territory
    if (defendList.size() >= 2) {
        Territory* strongest = defendList.back();

        if (strongest->getArmies() > 1) {
            // must be adjacent because Advance requires adjacency
            for (Territory* neighbor : strongest->getBorders()) {
                if (neighbor == weakest && neighbor->getOwner() == player) {
                    int armiesToSend = strongest->getArmies() / 2;
                    if (armiesToSend > 0) {
                        player->getOrders()->addOrder(
                            new Advance(player, armiesToSend, strongest, weakest, deck)
                        );
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

PlayerStrategy* BenevolentPlayerStrategy::clone() const {
    return new BenevolentPlayerStrategy(*this);
}

std::string BenevolentPlayerStrategy::getStrategyName() const {
    return "BenevolentPlayerStrategy";
}

// -----------------------------------------------------------------
//                      Neutral Player Strategy
// -----------------------------------------------------------------
NeutralPlayerStrategy::NeutralPlayerStrategy() {
}

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other)
    : PlayerStrategy(other) {
}

NeutralPlayerStrategy::~NeutralPlayerStrategy() {
}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

std::vector<Territory*> NeutralPlayerStrategy::toDefend(Player* player) const {
    return {};
}

std::vector<Territory*> NeutralPlayerStrategy::toAttack(Player* player) const {
    return {};
}

bool NeutralPlayerStrategy::issueOrder(Player* player, Deck* deck, Map* map) {
    return false;
}

PlayerStrategy* NeutralPlayerStrategy::clone() const {
    return new NeutralPlayerStrategy(*this);
}

std::string NeutralPlayerStrategy::getStrategyName() const {
    return "NeutralPlayerStrategy";
}

// -----------------------------------------------------------------
//                      Cheater Player Strategy
// -----------------------------------------------------------------
CheaterPlayerStrategy::CheaterPlayerStrategy() {
    conqueredThisTurn = new bool(false);
}

CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other)
    : PlayerStrategy(other) {
    conqueredThisTurn = new bool(*other.conqueredThisTurn);
}

CheaterPlayerStrategy::~CheaterPlayerStrategy() {
    delete conqueredThisTurn;
}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        delete conqueredThisTurn;
        conqueredThisTurn = new bool(*other.conqueredThisTurn);
    }
    return *this;
}

std::vector<Territory*> CheaterPlayerStrategy::toDefend(Player* player) const {
    if (player == nullptr) return {};

    std::vector<Territory*> defend = *(player->getTerritories());

    std::sort(defend.begin(), defend.end(), [](Territory* a, Territory* b) {
        return a->getArmies() < b->getArmies();
    });

    return defend;
}

std::vector<Territory*> CheaterPlayerStrategy::toAttack(Player* player) const {
    if (player == nullptr) return {};

    std::set<Territory*> attackSet;

    for (Territory* owned : *(player->getTerritories())) {
        for (Territory* neighbor : owned->getBorders()) {
            if (neighbor->getOwner() != player) {
                attackSet.insert(neighbor);
            }
        }
    }

    return std::vector<Territory*>(attackSet.begin(), attackSet.end());
}

bool CheaterPlayerStrategy::issueOrder(Player* player, Deck* deck, Map* map) {
    (void)deck;
    (void)map;

    if (player == nullptr) return false;

    if (*conqueredThisTurn) {
        return false;
    }

    std::vector<Territory*> targets = toAttack(player);

    if (targets.empty()) {
        *conqueredThisTurn = true;
        return false;
    }

    for (Territory* target : targets) {
        Player* oldOwner = target->getOwner();

        if (oldOwner != nullptr && oldOwner != player) {
            oldOwner->removeTerritory(target);
        }

        target->setOwner(player);
        player->addTerritory(target);
    }

    *conqueredThisTurn = true;
    player->setConqueredThisTurn(true);

    return true;
}

void CheaterPlayerStrategy::resetTurn() {
    *conqueredThisTurn = false;
}

PlayerStrategy* CheaterPlayerStrategy::clone() const {
    return new CheaterPlayerStrategy(*this);
}

std::string CheaterPlayerStrategy::getStrategyName() const {
    return "CheaterPlayerStrategy";
}