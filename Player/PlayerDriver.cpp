#include <iostream>
#include <string>
#include <vector>

#include "Player.h"
#include "Map/Map.h"
#include "Cards/Cards.h"
#include "Orders/Orders.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

// Helper function to print territory info from vector
static void printTerritoryVector(const string& title, const vector<Territory*>& v) {
    cout << title << " (" << v.size() << "):" << endl;
    if (v.empty()) {
        cout << "  [none]" << endl << endl;
        return;
    }

    for (Territory* t : v) {
        cout << "  - " << *t;
    }
    cout << endl;
}

void testPlayerTerritories(Player &player) {
    // Player owns a collection of territories
    cout << "--- Testing Player Territories ---" << endl;
    string n1 = "Alaska";
    string n2 = "Ontario";
    string n3 = "Quebec";

    Territory* t1 = new Territory(1, n1, 1, 10, 10);
    Territory* t2 = new Territory(2, n2, 1, 20, 15);
    Territory* t3 = new Territory(3, n3, 2, 30, 25);

    player.addTerritory(t1);
    player.addTerritory(t2);
    player.addTerritory(t3);

    cout << "After adding 3 territories: " << player << endl << endl;

    // toDefend() and toAttack()
    vector<Territory*> defendList = player.toDefend();
    vector<Territory*> attackList = player.toAttack();

    printTerritoryVector("toDefend() returned", defendList);
    printTerritoryVector("toAttack() returned", attackList);

    // Cleanup
    delete t1;
    delete t2;
    delete t3;
}

void testPlayerHand(Player &player) {
    cout << "--- Testing Player Hand ---" << endl;
    Deck deck;

    // Demo deck creation
    deck.addCard(new Card(CardType::Bomb));
    deck.addCard(new Card(CardType::Reinforcement));
    deck.addCard(new Card(CardType::Blockade));
    deck.addCard(new Card(CardType::Airlift));
    deck.addCard(new Card(CardType::Diplomacy));

    cout << "Player hand:\n" << *(player.getHand()) << endl;

    cout << "\nDrawing a card" << endl;
    deck.draw(*(player.getHand()));

    cout << "\nPlayer hand:\n" << *(player.getHand()) << endl;
}

void testIssueOrder(Player &player) {
    cout << "--- Testing Player Orders ---" << endl;
    cout << "Orders before issueOrder(): size = " << player.getOrders()->getSize() << endl;
    cout << *(player.getOrders()) << endl;

    cout << "Calling issueOrder()" << endl;
    player.issueOrder();

    cout << "\nOrders after issueOrder(): size = " << player.getOrders()->getSize() << endl;
    cout << *(player.getOrders()) << endl;
}

int main() {
    cout << "==============================" << endl;
    cout << "      PLAYER DRIVER DEMO      " << endl;
    cout << "==============================" << endl << endl;

    cout << "--- Testing Player Creation ---" << endl;
    Player player("Tester");
    cout << "Created player: " << player << endl << endl;
    testPlayerTerritories(player);
    testPlayerHand(player);
    testIssueOrder(player);

    cout << "\nFinal player summary: " << player << endl;

    cout << "\n=== End of demo ===" << endl;
    return 0;
}
