#include "GameEngine.h"
#include "../Map/Map.h"
#include "../Player/Player.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using std::cout;
using std::endl;

// builds a small test map with 6 territories and 2 continents using a temp file
Map* buildTestMap() {
    // write a temp .map file in the correct Warzone format
    std::ofstream mapFile("test_game_loop.map");
    mapFile << "[files]" << endl;
    mapFile << endl;
    mapFile << "[continents]" << endl;
    mapFile << "NorthLand 3 #FF0000" << endl;
    mapFile << "SouthLand 2 #0000FF" << endl;
    mapFile << endl;
    mapFile << "[countries]" << endl;
    mapFile << "1 Alpha 1 0 0" << endl;
    mapFile << "2 Bravo 1 1 0" << endl;
    mapFile << "3 Charlie 1 2 0" << endl;
    mapFile << "4 Delta 2 3 0" << endl;
    mapFile << "5 Echo 2 4 0" << endl;
    mapFile << "6 Foxtrot 2 5 0" << endl;
    mapFile << endl;
    mapFile << "[borders]" << endl;
    mapFile << "1 2" << endl;
    mapFile << "2 1 3" << endl;
    mapFile << "3 2 4" << endl;
    mapFile << "4 3 5" << endl;
    mapFile << "5 4 6" << endl;
    mapFile << "6 5" << endl;
    mapFile.close();

    Map* loadedMap = new Map();
    MapLoader ml;
    ml.loadMap(*loadedMap, "test_game_loop.map");
    std::remove("test_game_loop.map");

    return loadedMap;
}

// free function to demonstrate the main game loop (Part 3)
void testMainGameLoop() {
    srand(static_cast<unsigned>(time(0)));

    cout << "========================================" << endl;
    cout << "  Part 3: Main Game Loop Demo           " << endl;
    cout << "========================================" << endl;
    cout << endl;

    // build a small map
    Map* map = buildTestMap();
    std::vector<Territory*> territories = map->getTerritories();

    cout << "Map loaded with " << territories.size() << " territories." << endl;
    for (Territory* t : territories) {
        cout << "  " << t->getName() << " (continent " << t->getContinent() << ")" << endl;
    }
    cout << endl;

    // create 2 players
    Player* p1 = new Player("Alice");
    Player* p2 = new Player("Bob");

    // distribute territories: Alice gets first 3, Bob gets last 3
    for (size_t i = 0; i < territories.size(); i++) {
        Player* owner = (i < territories.size() / 2) ? p1 : p2;
        territories[i]->setOwner(owner);
        owner->addTerritory(territories[i]);
        territories[i]->setArmies(3);
    }

    cout << "Territory distribution:" << endl;
    for (Territory* t : territories) {
        cout << "  " << t->getName() << " -> " << t->getOwnerName()
             << " (" << t->getArmies() << " armies)" << endl;
    }
    cout << endl;

    // create a deck with assorted cards
    Deck* deck = new Deck();
    for (int i = 0; i < 20; i++) {
        deck->addCard(new Card(static_cast<CardType>(i % 5)));
    }

    // give each player 2 cards
    deck->draw(*(p1->getHand()));
    deck->draw(*(p1->getHand()));
    deck->draw(*(p2->getHand()));
    deck->draw(*(p2->getHand()));

    cout << *p1 << endl;
    cout << *p2 << endl;
    cout << endl;

    // ===== Demonstrate reinforcementPhase =====
    cout << "--- Demonstrating reinforcementPhase ---" << endl;
    p1->addReinforcements(10);
    p2->addReinforcements(10);
    cout << p1->getName() << " reinforcement pool: " << p1->getReinforcementPool() << endl;
    cout << p2->getName() << " reinforcement pool: " << p2->getReinforcementPool() << endl;
    cout << endl;

    // ===== Demonstrate issueOrdersPhase =====
    cout << "--- Demonstrating issueOrdersPhase ---" << endl;

    cout << "Issuing orders for " << p1->getName() << "..." << endl;
    while (p1->issueOrder(deck, map)) {}
    cout << p1->getName() << " issued " << p1->getOrders()->getSize() << " orders." << endl;
    cout << endl;

    cout << "Issuing orders for " << p2->getName() << "..." << endl;
    while (p2->issueOrder(deck, map)) {}
    cout << p2->getName() << " issued " << p2->getOrders()->getSize() << " orders." << endl;
    cout << endl;

    cout << "--- " << p1->getName() << "'s Orders ---" << endl;
    cout << *(p1->getOrders()) << endl;

    cout << "--- " << p2->getName() << "'s Orders ---" << endl;
    cout << *(p2->getOrders()) << endl;

    // ===== Demonstrate executeOrdersPhase =====
    cout << "--- Demonstrating executeOrdersPhase ---" << endl;
    cout << "Executing deploy orders first..." << endl;

    // execute deploys from both players
    for (Player* p : {p1, p2}) {
        OrdersList* ol = p->getOrders();
        for (int j = ol->getSize() - 1; j >= 0; j--) {
            if (dynamic_cast<Deploy*>(ol->getOrder(j)) != nullptr) {
                ol->getOrder(j)->execute();
                ol->remove(j);
            }
        }
    }

    cout << endl << "Executing remaining orders..." << endl;
    for (Player* p : {p1, p2}) {
        OrdersList* ol = p->getOrders();
        while (ol->getSize() > 0) {
            ol->getOrder(0)->execute();
            ol->remove(0);
        }
    }
    cout << endl;

    // show final state
    cout << "--- Final Territory State ---" << endl;
    for (Territory* t : territories) {
        cout << "  " << t->getName() << " -> " << t->getOwnerName()
             << " (" << t->getArmies() << " armies)" << endl;
    }
    cout << endl;

    cout << "--- Final Player State ---" << endl;
    cout << *p1 << endl;
    cout << *p2 << endl;

    // check if anyone won
    if (p1->getTerritories()->empty()) {
        cout << "\n" << p2->getName() << " wins!" << endl;
    } else if (p2->getTerritories()->empty()) {
        cout << "\n" << p1->getName() << " wins!" << endl;
    } else {
        cout << "\nGame still in progress (no winner yet after 1 round)." << endl;
    }

    cout << "\nMain Game Loop demo complete." << endl;

    // cleanup
    delete p1;
    delete p2;
    delete map;
    delete deck;
}

int main() {
    testMainGameLoop();
    return 0;
}
