// OrdersDriver.cpp
// Demonstrates the execution of all Warzone order types following official game rules.
//
// This driver demonstrates:
// (1) Each order is validated before being executed.
// (2) Ownership of a territory is transferred to the attacking player on conquest (Advance).
// (3) One card is given to a player if they conquer at least one territory in a turn.
// (4) The negotiate order prevents attacks between the two players involved.
// (5) The blockade order transfers ownership to the Neutral player.
// (6) All orders can be issued by a player and executed by the game engine.

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "Orders/Orders.h"
#include "Player/Player.h"
#include "Map/Map.h"
#include "Cards/Cards.h"

using std::cout;
using std::endl;
using std::string;

// ========================== Helper Functions ==========================

static void printSeparator(const string& title) {
    cout << "\n========================================" << endl;
    cout << "  " << title << endl;
    cout << "========================================\n" << endl;
}

static void printTerritoryStatus(Territory* t) {
    cout << "  " << t->getName() << " -> Owner: " << t->getOwnerName()
         << ", Armies: " << t->getArmies() << endl;
}

// ========================== Main Driver ==========================

int main() {
    srand(static_cast<unsigned int>(time(0)));

    // -------------------- Setup: Create a test map --------------------
    // We create a small graph of 6 territories arranged as:
    //
    //   T1(Alpha) -- T2(Bravo) -- T3(Charlie)
    //   |                          |
    //   T4(Delta) -- T5(Echo)  -- T6(Foxtrot)

    // Create a temporary .map file and load it via MapLoader
    {
        std::ofstream testFile("test_orders.map");
        testFile << "[files]\n\n";
        testFile << "[continents]\n";
        testFile << "TestContinent 5 red\n\n";
        testFile << "[countries]\n";
        testFile << "1 Alpha 1 0 0\n";
        testFile << "2 Bravo 1 1 0\n";
        testFile << "3 Charlie 1 2 0\n";
        testFile << "4 Delta 1 0 1\n";
        testFile << "5 Echo 1 1 1\n";
        testFile << "6 Foxtrot 1 2 1\n\n";
        testFile << "[borders]\n";
        testFile << "1 2 4\n";
        testFile << "2 1 3\n";
        testFile << "3 2 6\n";
        testFile << "4 1 5\n";
        testFile << "5 4 6\n";
        testFile << "6 3 5\n";
        testFile.close();
    }

    // Load the test map
    Map* gameMap = new Map();
    MapLoader loader;
    try {
        loader.loadMap(*gameMap, "test_orders.map");
    } catch (const std::exception& e) {
        cout << "Failed to load test map: " << e.what() << endl;
        delete gameMap;
        return 1;
    }

    // Get territory pointers from the loaded map
    std::vector<Territory*> territories = gameMap->getTerritories();
    Territory* tAlpha   = territories[0];
    Territory* tBravo   = territories[1];
    Territory* tCharlie = territories[2];
    Territory* tDelta   = territories[3];
    Territory* tEcho    = territories[4];
    Territory* tFoxtrot = territories[5];

    // -------------------- Setup: Create players --------------------
    Player* player1 = new Player("Alice");
    Player* player2 = new Player("Bob");

    // Assign territories:
    //   Alice owns: Alpha (10 armies), Bravo (15 armies), Delta (8 armies)
    //   Bob owns:   Charlie (5 armies), Echo (12 armies), Foxtrot (20 armies)
    tAlpha->setOwner(player1);   tAlpha->setArmies(10);   player1->addTerritory(tAlpha);
    tBravo->setOwner(player1);   tBravo->setArmies(15);   player1->addTerritory(tBravo);
    tDelta->setOwner(player1);   tDelta->setArmies(8);    player1->addTerritory(tDelta);

    tCharlie->setOwner(player2); tCharlie->setArmies(5);  player2->addTerritory(tCharlie);
    tEcho->setOwner(player2);    tEcho->setArmies(12);    player2->addTerritory(tEcho);
    tFoxtrot->setOwner(player2); tFoxtrot->setArmies(20); player2->addTerritory(tFoxtrot);

    // Give players reinforcement pools
    player1->addReinforcements(20);
    player2->addReinforcements(15);

    // Create a deck for card drawing on conquest
    Deck* gameDeck = new Deck();
    for (int i = 0; i < 20; i++) {
        gameDeck->addCard(new Card(static_cast<CardType>(i % 5)));
    }

    // -------------------- Print initial state --------------------
    printSeparator("INITIAL GAME STATE");

    cout << *player1 << endl;
    cout << *player2 << endl;
    cout << "\nTerritory Status:" << endl;
    for (Territory* t : territories) {
        printTerritoryStatus(t);
    }
    cout << "\nAlice's hand: " << player1->getHand()->getSize() << " cards" << endl;
    cout << "Bob's hand:   " << player2->getHand()->getSize() << " cards" << endl;
    cout << "Deck size: " << gameDeck->getSize() << " cards" << endl;

    // ====================================================================
    // TEST 1: DEPLOY ORDER
    // ====================================================================
    printSeparator("TEST 1: DEPLOY ORDER");

    // 1a: Valid deploy — Alice deploys 10 armies to Alpha (which she owns)
    cout << "--- 1a: Valid Deploy (Alice -> Alpha, 10 armies) ---" << endl;
    cout << "Alice reinforcement pool before: " << player1->getReinforcementPool() << endl;
    {
        Deploy* deploy = new Deploy(player1, 10, tAlpha);
        deploy->execute();
        cout << "Alice reinforcement pool after: " << player1->getReinforcementPool() << endl;
        printTerritoryStatus(tAlpha);
        delete deploy;
    }

    // 1b: Invalid deploy — Alice tries to deploy to Charlie (owned by Bob)
    cout << "\n--- 1b: Invalid Deploy (Alice -> Charlie, Bob's territory) ---" << endl;
    {
        Deploy* deploy = new Deploy(player1, 5, tCharlie);
        deploy->execute();
        printTerritoryStatus(tCharlie);
        delete deploy;
    }

    // ====================================================================
    // TEST 2: ADVANCE ORDER - Friendly Move
    // ====================================================================
    printSeparator("TEST 2: ADVANCE ORDER - FRIENDLY MOVE");

    // 2a: Alice moves 5 armies from Alpha to Bravo (both hers, adjacent)
    cout << "--- 2a: Valid Advance friendly (Alice: Alpha -> Bravo, 5 armies) ---" << endl;
    cout << "Before:" << endl;
    printTerritoryStatus(tAlpha);
    printTerritoryStatus(tBravo);
    {
        Advance* advance = new Advance(player1, 5, tAlpha, tBravo, gameDeck);
        advance->execute();
        cout << "After:" << endl;
        printTerritoryStatus(tAlpha);
        printTerritoryStatus(tBravo);
        delete advance;
    }

    // 2b: Invalid Advance — Alice tries to advance from Charlie (not hers)
    cout << "\n--- 2b: Invalid Advance (Alice from Charlie - not her territory) ---" << endl;
    {
        Advance* advance = new Advance(player1, 5, tCharlie, tBravo, gameDeck);
        advance->execute();
        delete advance;
    }

    // 2c: Invalid Advance — non-adjacent territories (Alpha to Foxtrot)
    cout << "\n--- 2c: Invalid Advance (Alpha -> Foxtrot - not adjacent) ---" << endl;
    {
        Advance* advance = new Advance(player1, 5, tAlpha, tFoxtrot, gameDeck);
        advance->execute();
        delete advance;
    }

    // ====================================================================
    // TEST 3: ADVANCE ORDER - Attack (Battle Simulation + Conquest)
    // ====================================================================
    printSeparator("TEST 3: ADVANCE ORDER - ATTACK & CONQUEST");

    // Alice attacks Charlie (owned by Bob) from Bravo with overwhelming force.
    tBravo->setArmies(50);
    tCharlie->setArmies(3);

    cout << "--- 3: Alice attacks Charlie from Bravo (50 vs 3) ---" << endl;
    cout << "Before:" << endl;
    printTerritoryStatus(tBravo);
    printTerritoryStatus(tCharlie);
    cout << "Alice territories: " << player1->getTerritories()->size() << endl;
    cout << "Bob territories:   " << player2->getTerritories()->size() << endl;
    cout << "Alice conquered this turn (before): " << (player1->hasConqueredThisTurn() ? "YES" : "NO") << endl;
    {
        Advance* advance = new Advance(player1, 50, tBravo, tCharlie, gameDeck);
        advance->execute();
        cout << "\nAfter:" << endl;
        printTerritoryStatus(tBravo);
        printTerritoryStatus(tCharlie);
        cout << "Charlie owner: " << tCharlie->getOwnerName() << endl;
        cout << "Alice territories: " << player1->getTerritories()->size() << endl;
        cout << "Bob territories:   " << player2->getTerritories()->size() << endl;
        cout << "Alice conquered this turn (after): " << (player1->hasConqueredThisTurn() ? "YES" : "NO") << endl;
        delete advance;
    }

    // ====================================================================
    // TEST 4: CARD AWARDED ON CONQUEST (one per turn)
    // ====================================================================
    printSeparator("TEST 4: CARD AWARDED ON CONQUEST");

    cout << "Alice's hand before card award: " << player1->getHand()->getSize() << " cards" << endl;
    cout << "Alice conquered this turn: " << (player1->hasConqueredThisTurn() ? "YES" : "NO") << endl;

    // Award card at end of turn if player conquered at least one territory
    if (player1->hasConqueredThisTurn()) {
        cout << "Awarding Alice one card for conquering a territory this turn..." << endl;
        gameDeck->draw(*(player1->getHand()));
        cout << "Alice's hand after card award: " << player1->getHand()->getSize() << " cards" << endl;
    }

    // Reset for next turn
    player1->setConqueredThisTurn(false);
    cout << "Turn ended. Conquest flag reset." << endl;

    // ====================================================================
    // TEST 5: BOMB ORDER
    // ====================================================================
    printSeparator("TEST 5: BOMB ORDER");

    // 5a: Valid bomb — Alice bombs Echo (owned by Bob, adjacent to Delta which Alice owns)
    tEcho->setArmies(20);
    cout << "--- 5a: Valid Bomb (Alice bombs Echo, adjacent to her Delta) ---" << endl;
    cout << "Before:" << endl;
    printTerritoryStatus(tEcho);
    {
        Bomb* bomb = new Bomb(player1, tEcho);
        bomb->execute();
        cout << "After:" << endl;
        printTerritoryStatus(tEcho);
        delete bomb;
    }

    // 5b: Invalid bomb — Alice tries to bomb her own territory (Alpha)
    cout << "\n--- 5b: Invalid Bomb (Alice bombs her own Alpha) ---" << endl;
    {
        Bomb* bomb = new Bomb(player1, tAlpha);
        bomb->execute();
        delete bomb;
    }

    // ====================================================================
    // TEST 6: BLOCKADE ORDER
    // ====================================================================
    printSeparator("TEST 6: BLOCKADE ORDER");

    // 6a: Alice blockades Delta (her territory with 8 armies)
    tDelta->setArmies(8);
    cout << "--- 6a: Valid Blockade (Alice blockades Delta) ---" << endl;
    cout << "Before:" << endl;
    printTerritoryStatus(tDelta);
    cout << "Alice territories: " << player1->getTerritories()->size() << endl;
    {
        Blockade* blockade = new Blockade(player1, tDelta);
        blockade->execute();
        cout << "After:" << endl;
        printTerritoryStatus(tDelta);
        cout << "Delta owner: " << tDelta->getOwnerName() << endl;
        cout << "Alice territories: " << player1->getTerritories()->size() << endl;
        delete blockade;
    }

    // 6b: Invalid blockade — Alice tries to blockade Echo (Bob's territory)
    cout << "\n--- 6b: Invalid Blockade (Alice blockades Bob's Echo) ---" << endl;
    {
        Blockade* blockade = new Blockade(player1, tEcho);
        blockade->execute();
        delete blockade;
    }

    // ====================================================================
    // TEST 7: AIRLIFT ORDER
    // ====================================================================
    printSeparator("TEST 7: AIRLIFT ORDER");

    // 7a: Alice airlifts armies from Alpha to Charlie (both hers now, NOT adjacent)
    tAlpha->setArmies(15);
    cout << "--- 7a: Valid Airlift (Alice: Alpha -> Charlie, 10 armies, non-adjacent) ---" << endl;
    cout << "Before:" << endl;
    printTerritoryStatus(tAlpha);
    printTerritoryStatus(tCharlie);
    {
        Airlift* airlift = new Airlift(player1, 10, tAlpha, tCharlie);
        airlift->execute();
        cout << "After:" << endl;
        printTerritoryStatus(tAlpha);
        printTerritoryStatus(tCharlie);
        delete airlift;
    }

    // 7b: Invalid airlift — Alice airlifts from Echo (not hers)
    cout << "\n--- 7b: Invalid Airlift (Alice from Echo - not her territory) ---" << endl;
    {
        Airlift* airlift = new Airlift(player1, 5, tEcho, tAlpha);
        airlift->execute();
        delete airlift;
    }

    // ====================================================================
    // TEST 8: NEGOTIATE ORDER
    // ====================================================================
    printSeparator("TEST 8: NEGOTIATE ORDER");

    // 8a: Valid negotiate — Alice negotiates with Bob
    cout << "--- 8a: Valid Negotiate (Alice <-> Bob) ---" << endl;
    {
        Negotiate* negotiate = new Negotiate(player1, player2);
        negotiate->execute();
        delete negotiate;
    }

    // 8b: Invalid negotiate — Alice negotiates with herself
    cout << "\n--- 8b: Invalid Negotiate (Alice with herself) ---" << endl;
    {
        Negotiate* negotiate = new Negotiate(player1, player1);
        negotiate->execute();
        delete negotiate;
    }

    // ====================================================================
    // TEST 9: NEGOTIATE PREVENTS ATTACK
    // ====================================================================
    printSeparator("TEST 9: NEGOTIATE PREVENTS ATTACK");

    // After negotiation, Alice tries to advance-attack Bob's Foxtrot from Charlie.
    tCharlie->setArmies(30);
    tFoxtrot->setArmies(10);

    cout << "--- 9a: Advance attack BLOCKED by negotiation ---" << endl;
    cout << "Alice has negotiation with Bob: "
         << (player1->hasNegotiationWith("Bob") ? "YES" : "NO") << endl;
    {
        Advance* advance = new Advance(player1, 20, tCharlie, tFoxtrot, gameDeck);
        advance->execute();
        cout << "Foxtrot still owned by: " << tFoxtrot->getOwnerName() << endl;
        cout << "Foxtrot armies unchanged: " << tFoxtrot->getArmies() << endl;
        delete advance;
    }

    // 9b: Bomb also blocked by negotiation
    cout << "\n--- 9b: Bomb BLOCKED by negotiation ---" << endl;
    {
        Bomb* bomb = new Bomb(player1, tFoxtrot);
        bomb->execute();
        delete bomb;
    }

    // Clear negotiations (simulating end of turn)
    cout << "\n--- Clearing negotiations (end of turn) ---" << endl;
    player1->clearNegotiations();
    player2->clearNegotiations();
    cout << "Negotiations cleared." << endl;

    // 9c: After clearing, attack now works
    cout << "\n--- 9c: Advance attack SUCCEEDS after negotiation cleared ---" << endl;
    cout << "Alice has negotiation with Bob: "
         << (player1->hasNegotiationWith("Bob") ? "YES" : "NO") << endl;
    {
        Advance* advance = new Advance(player1, 25, tCharlie, tFoxtrot, gameDeck);
        advance->execute();
        cout << "Foxtrot owner after attack: " << tFoxtrot->getOwnerName() << endl;
        delete advance;
    }

    // ====================================================================
    // TEST 10: ALL ORDERS ISSUED AND EXECUTED VIA ORDERSLIST
    // ====================================================================
    printSeparator("TEST 10: ALL ORDERS ISSUED & EXECUTED VIA ORDERSLIST");

    // Reset game state for a clean demo
    player1->clearNegotiations();
    player2->clearNegotiations();
    player1->setConqueredThisTurn(false);
    player2->setConqueredThisTurn(false);

    // Reset territories for this final test
    tAlpha->setOwner(player1);   tAlpha->setArmies(30);
    tBravo->setOwner(player1);   tBravo->setArmies(20);
    tEcho->setOwner(player2);    tEcho->setArmies(5);
    player1->addReinforcements(10);

    cout << "Creating an OrdersList and adding all order types..." << endl;
    OrdersList* ordersList = new OrdersList();

    // Add one of each order type to demonstrate all can be issued and executed
    ordersList->addOrder(new Deploy(player1, 5, tAlpha));
    ordersList->addOrder(new Advance(player1, 10, tAlpha, tBravo, gameDeck));  // friendly move
    ordersList->addOrder(new Bomb(player1, tEcho));
    ordersList->addOrder(new Airlift(player1, 5, tBravo, tAlpha));
    ordersList->addOrder(new Blockade(player1, tBravo));
    ordersList->addOrder(new Negotiate(player1, player2));

    cout << "\n" << *ordersList << endl;

    cout << "Executing all orders in the list:" << endl;
    for (int i = 0; i < ordersList->getSize(); i++) {
        cout << "\n--- Executing order [" << i << "] ---" << endl;
        ordersList->getOrder(i)->execute();
    }

    // ====================================================================
    // FINAL STATE
    // ====================================================================
    printSeparator("FINAL GAME STATE");

    cout << *player1 << endl;
    cout << *player2 << endl;
    cout << "\nTerritory Status:" << endl;
    for (Territory* t : territories) {
        printTerritoryStatus(t);
    }
    cout << "\nAlice's hand: " << player1->getHand()->getSize() << " cards" << endl;
    cout << "Bob's hand:   " << player2->getHand()->getSize() << " cards" << endl;

    // ====================================================================
    // Cleanup
    // ====================================================================
    printSeparator("CLEANUP");

    delete ordersList;
    delete gameDeck;
    delete player1;
    delete player2;

    // Clean up static neutral player if it was created
    if (Blockade::neutralPlayer != nullptr) {
        delete Blockade::neutralPlayer;
        Blockade::neutralPlayer = nullptr;
    }

    delete gameMap;

    // Remove temporary map file
    std::remove("test_orders.map");

    cout << "All memory freed. Driver complete." << endl;

    return 0;
}
