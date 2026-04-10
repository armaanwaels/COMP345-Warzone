#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

#include "Player.h"
#include "PlayerStrategies.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.h"

using namespace std;

void initializeDeck(Deck& deck) {
    for (int i = 0; i < 10; ++i) {
        deck.addCard(new Card(CardType::Bomb));
        deck.addCard(new Card(CardType::Reinforcement));
        deck.addCard(new Card(CardType::Blockade));
        deck.addCard(new Card(CardType::Airlift));
        deck.addCard(new Card(CardType::Diplomacy));
    }
}

void assignTerritoriesRandomly(Map& map, vector<Player*>& players) {
    vector<Territory*> territories = map.getTerritories();

    random_device rd;
    mt19937 g(rd());
    shuffle(territories.begin(), territories.end(), g);

    int playerIndex = 0;
    for (Territory* t : territories) {
        Player* p = players[playerIndex];
        p->addTerritory(t);
        t->setOwner(p);
        playerIndex = (playerIndex + 1) % players.size();
    }
}

void assignInitialArmies(vector<Player*>& players) {
    for (Player* p : players) {
        p->setReinforcementPool(50);
    }
}

void assignInitialCards(vector<Player*>& players, Deck& deck, int cardsPerPlayer) {
    for (Player* p : players) {
        for (int i = 0; i < cardsPerPlayer; ++i) {
            deck.draw(*(p->getHand()));
        }
    }
}

void initializeTerritoryArmies(vector<Player*>& players) {
    for (Player* p : players) {
        for (Territory* t : *(p->getTerritories())) {
            t->setArmies(3);
        }
    }
}

void printTerritoryList(const string& label, const vector<Territory*>& list) {
    cout << label;
    if (list.empty()) {
        cout << " none";
    } else {
        for (Territory* t : list) {
            cout << " " << t->getName();
        }
    }
    cout << endl;
}

void printPlayerState(Player* player) {
    cout << "\nPlayer: " << player->getName() << endl;
    cout << "Strategy: " << player->getStrategy()->getStrategyName() << endl;

    cout << "Territories:";
    for (Territory* t : *(player->getTerritories())) {
        cout << " " << t->getName() << "(" << t->getArmies() << ")";
    }
    cout << endl;

    cout << "Reinforcement pool: " << player->getReinforcementPool() << endl;
    cout << "Hand size: " << player->getHand()->getSize() << endl;

    printTerritoryList("toDefend:", player->toDefend());
    printTerritoryList("toAttack:", player->toAttack());
}

void executeOrdersForPlayer(Player* player) {
    if (player == nullptr || player->getOrders() == nullptr) {
        return;
    }

    cout << "Executing orders for " << player->getName() << "..." << endl;

    while (player->getOrders()->getSize() > 0) {
        Order* nextOrder = player->getOrders()->getOrder(0);

        if (nextOrder != nullptr) {
            nextOrder->execute();
        }

        // OrderList::remove(0) is expected to clean up the stored order.
        // Do not delete nextOrder here or the order may be freed twice.
        player->getOrders()->remove(0);
    }
}

void issueOneRound(vector<Player*>& players, Deck& deck, Map& map) {
    cout << "\n===== ISSUE ORDERS ROUND =====" << endl;
    for (Player* p : players) {
        cout << "\nIssuing order for " << p->getName() << "..." << endl;
        bool issued = p->issueOrder(&deck, &map);
        cout << "Issued order? " << (issued ? "yes" : "no") << endl;
        executeOrdersForPlayer(p);
    }
}



// Helper: Find an adjacent territory pair for attack (aggressive -> neutral)
bool findAdjacentAttackPair(Player* attacker, Player* defender, Territory*& source, Territory*& target) {
    source = nullptr;
    target = nullptr;

    if (attacker == nullptr || defender == nullptr) {
        return false;
    }

    for (Territory* owned : *(attacker->getTerritories())) {
        for (Territory* border : owned->getBorders()) {
            if (border != nullptr && border->getOwner() == defender) {
                source = owned;
                target = border;
                return true;
            }
        }
    }

    return false;
}

// Ensure that aggressive can attack neutral by adjusting territory ownership if needed
void ensureNeutralCanBeAttacked(Player* aggressive, Player* neutral) {
    if (aggressive == nullptr || neutral == nullptr) {
        return;
    }

    Territory* source = nullptr;
    Territory* target = nullptr;

    if (findAdjacentAttackPair(aggressive, neutral, source, target)) {
        return;
    }

    vector<Territory*>* aggressiveTerritories = aggressive->getTerritories();
    vector<Territory*>* neutralTerritories = neutral->getTerritories();

    if (aggressiveTerritories == nullptr || neutralTerritories == nullptr ||
        aggressiveTerritories->empty() || neutralTerritories->empty()) {
        return;
    }

    Territory* aggressiveTerritory = (*aggressiveTerritories)[0];
    Territory* neutralTerritory = nullptr;

    for (Territory* border : aggressiveTerritory->getBorders()) {
        if (border != nullptr && border->getOwner() != aggressive) {
            neutralTerritory = border;
            break;
        }
    }

    if (neutralTerritory == nullptr) {
        return;
    }

    Player* previousOwner = neutralTerritory->getOwner();
    if (previousOwner != nullptr && previousOwner != neutral) {
        previousOwner->removeTerritory(neutralTerritory);
    }

    if (previousOwner == aggressive) {
        aggressive->removeTerritory(neutralTerritory);
    }

    neutral->addTerritory(neutralTerritory);
    neutralTerritory->setOwner(neutral);
    neutralTerritory->setArmies(3);
}

int main() {
    MapLoader loader;
    Map map;
    loader.loadMap(map, "../Map/Map_Files/kosova.map");

    Deck* deck = new Deck();
    initializeDeck(*deck);

    Player* human = new Player("Human");
    Player* aggressive = new Player("Aggressive");
    Player* benevolent = new Player("Benevolent");
    Player* neutral = new Player("Neutral");
    Player* cheater = new Player("Cheater");

    human->setStrategy(new HumanPlayerStrategy());
    aggressive->setStrategy(new AggressivePlayerStrategy());
    benevolent->setStrategy(new BenevolentPlayerStrategy());
    neutral->setStrategy(new NeutralPlayerStrategy());
    cheater->setStrategy(new CheaterPlayerStrategy());

    vector<Player*> players = {human, aggressive, benevolent, neutral, cheater};

    assignTerritoriesRandomly(map, players);
    assignInitialArmies(players);
    assignInitialCards(players, *deck, 2);
    initializeTerritoryArmies(players);
    ensureNeutralCanBeAttacked(aggressive, neutral);

    cout << "===== INITIAL STATE =====" << endl;
    for (Player* p : players) {
        printPlayerState(p);
    }

    issueOneRound(players, *deck, map);

    cout << "\n===== AFTER FIRST ROUND =====" << endl;
    for (Player* p : players) {
        printPlayerState(p);
    }

    // Force neutral attacked -> aggressive demo
    cout << "\n===== NEUTRAL ATTACKED DEMO =====" << endl;
    Territory* source = nullptr;
    Territory* target = nullptr;

    if (findAdjacentAttackPair(aggressive, neutral, source, target)) {
        cout << "Aggressive attacks Neutral from " << source->getName()
             << " to " << target->getName() << endl;
        Advance* attack = new Advance(aggressive, 1, source, target, deck);
        attack->execute();
        delete attack;
    } else {
        cout << "Could not find an adjacent aggressive-neutral pair." << endl;
    }

    cout << "\nNeutral after being attacked:" << endl;
    printPlayerState(neutral);

    cout << "\n===== SECOND ROUND =====" << endl;
    issueOneRound(players, *deck, map);

    for (Player* p : players) {
        delete p;
    }
    delete deck;

    return 0;
}