// OrdersDriver.cpp
// Driver for Orders and OrdersList classes
// Armaan Waels - 40196241
// COMP 345 - Assignment 1, Part 3

#include "Orders.h"
#include <iostream>

using std::cout;
using std::endl;

// Test creating each order type
void testOrderCreation() {
    cout << "\n========================================" << endl;
    cout << "TESTING ORDER CREATION" << endl;
    cout << "========================================\n" << endl;

    cout << "Creating Deploy order..." << endl;
    Deploy* deploy = new Deploy(10, "Canada");
    cout << *deploy << endl << endl;

    cout << "Creating Advance order..." << endl;
    Advance* advance = new Advance(5, "Canada", "USA");
    cout << *advance << endl << endl;

    cout << "Creating Bomb order..." << endl;
    Bomb* bomb = new Bomb("Germany");
    cout << *bomb << endl << endl;

    cout << "Creating Blockade order..." << endl;
    Blockade* blockade = new Blockade("France");
    cout << *blockade << endl << endl;

    cout << "Creating Airlift order..." << endl;
    Airlift* airlift = new Airlift(8, "Japan", "Australia");
    cout << *airlift << endl << endl;

    cout << "Creating Negotiate order..." << endl;
    Negotiate* negotiate = new Negotiate("Player2");
    cout << *negotiate << endl << endl;

    delete deploy;
    delete advance;
    delete bomb;
    delete blockade;
    delete airlift;
    delete negotiate;

    cout << "All orders created and destroyed successfully!" << endl;
}

// Test OrdersList add, move, remove
void testOrdersList() {
    cout << "\n========================================" << endl;
    cout << "TESTING ORDERSLIST" << endl;
    cout << "========================================\n" << endl;

    OrdersList* ordersList = new OrdersList();

    cout << "--- Adding orders to the list ---\n" << endl;

    ordersList->addOrder(new Deploy(15, "Brazil"));
    ordersList->addOrder(new Advance(7, "Mexico", "Cuba"));
    ordersList->addOrder(new Bomb("Spain"));
    ordersList->addOrder(new Blockade("Italy"));
    ordersList->addOrder(new Airlift(12, "China", "India"));
    ordersList->addOrder(new Negotiate("Player3"));

    cout << "\n--- Current OrdersList state ---" << endl;
    cout << *ordersList << endl;

    cout << "\n--- Testing move() method ---" << endl;
    cout << "Moving order from index 0 to index 3..." << endl;
    ordersList->move(0, 3);
    cout << *ordersList << endl;

    cout << "Moving order from index 5 to index 1..." << endl;
    ordersList->move(5, 1);
    cout << *ordersList << endl;

    cout << "\n--- Testing remove() method ---" << endl;
    cout << "Removing order at index 2..." << endl;
    ordersList->remove(2);
    cout << *ordersList << endl;

    cout << "\n--- Testing invalid operations ---" << endl;
    cout << "Trying to remove at invalid index -1:" << endl;
    ordersList->remove(-1);
    cout << "Trying to remove at invalid index 100:" << endl;
    ordersList->remove(100);
    cout << "Trying to move from invalid index:" << endl;
    ordersList->move(-1, 2);

    delete ordersList;
    cout << "\nOrdersList destroyed successfully!" << endl;
}

// Test validate and execute
void testOrderExecution() {
    cout << "\n========================================" << endl;
    cout << "TESTING ORDER EXECUTION" << endl;
    cout << "========================================\n" << endl;

    cout << "--- Testing valid orders ---\n" << endl;

    Deploy* validDeploy = new Deploy(10, "Egypt");
    cout << "Before execution: " << *validDeploy << endl;
    validDeploy->execute();
    cout << "After execution: " << *validDeploy << endl << endl;

    Advance* validAdvance = new Advance(5, "Libya", "Tunisia");
    cout << "Before execution: " << *validAdvance << endl;
    validAdvance->execute();
    cout << "After execution: " << *validAdvance << endl << endl;

    cout << "--- Testing invalid orders ---\n" << endl;

    Deploy* invalidDeploy = new Deploy(-5, "Morocco");
    cout << "Attempting to execute invalid deploy order (negative armies):" << endl;
    invalidDeploy->execute();
    cout << "Order state: " << *invalidDeploy << endl << endl;

    Advance* invalidAdvance = new Advance(0, "", "Algeria");
    cout << "Attempting to execute invalid advance order (zero armies, empty source):" << endl;
    invalidAdvance->execute();
    cout << "Order state: " << *invalidAdvance << endl << endl;

    delete validDeploy;
    delete validAdvance;
    delete invalidDeploy;
    delete invalidAdvance;
}

// Test copy constructor and assignment operator
void testCopyAndAssignment() {
    cout << "\n========================================" << endl;
    cout << "TESTING COPY CONSTRUCTOR AND ASSIGNMENT" << endl;
    cout << "========================================\n" << endl;

    cout << "--- Testing Order copy constructor ---" << endl;
    Deploy* original = new Deploy(20, "Russia");
    original->execute();
    cout << "Original: " << *original << endl;

    Deploy* copy = new Deploy(*original);
    cout << "Copy: " << *copy << endl << endl;

    cout << "--- Testing OrdersList copy constructor ---" << endl;
    OrdersList* originalList = new OrdersList();
    originalList->addOrder(new Deploy(5, "Norway"));
    originalList->addOrder(new Bomb("Sweden"));
    originalList->addOrder(new Negotiate("Player4"));

    cout << "\nOriginal list:" << endl;
    cout << *originalList << endl;

    OrdersList* copiedList = new OrdersList(*originalList);
    cout << "Copied list:" << endl;
    cout << *copiedList << endl;

    cout << "--- Modifying original list to verify deep copy ---" << endl;
    originalList->remove(0);
    cout << "Original list after removing first element:" << endl;
    cout << *originalList << endl;
    cout << "Copied list (should be unchanged):" << endl;
    cout << *copiedList << endl;

    cout << "--- Testing OrdersList assignment operator ---" << endl;
    OrdersList* assignedList = new OrdersList();
    assignedList->addOrder(new Airlift(3, "Finland", "Denmark"));
    cout << "Before assignment:" << endl;
    cout << *assignedList << endl;

    *assignedList = *copiedList;
    cout << "After assignment from copied list:" << endl;
    cout << *assignedList << endl;

    delete original;
    delete copy;
    delete originalList;
    delete copiedList;
    delete assignedList;

    cout << "All copy/assignment tests completed successfully!" << endl;
}

// Test executing all orders in a list
void testExecuteAllOrders() {
    cout << "\n========================================" << endl;
    cout << "TESTING EXECUTE ALL ORDERS IN LIST" << endl;
    cout << "========================================\n" << endl;

    OrdersList* ordersList = new OrdersList();

    ordersList->addOrder(new Deploy(10, "Poland"));
    ordersList->addOrder(new Advance(5, "Germany", "France"));
    ordersList->addOrder(new Bomb("Belgium"));
    ordersList->addOrder(new Blockade("Netherlands"));
    ordersList->addOrder(new Airlift(8, "UK", "Ireland"));
    ordersList->addOrder(new Negotiate("Player5"));

    cout << "\n--- Orders before execution ---" << endl;
    cout << *ordersList << endl;

    cout << "\n--- Executing all orders ---\n" << endl;
    for (int i = 0; i < ordersList->getSize(); i++) {
        cout << "Executing order [" << i << "]:" << endl;
        ordersList->getOrder(i)->execute();
        cout << endl;
    }

    cout << "--- Orders after execution ---" << endl;
    cout << *ordersList << endl;

    delete ordersList;
}

int main() {
    cout << "********************************************************" << endl;
    cout << "*     COMP 345 - Assignment 1 - Part 3: Orders List    *" << endl;
    cout << "*              Armaan Waels - 40196241                  *" << endl;
    cout << "********************************************************" << endl;

    testOrderCreation();
    testOrdersList();
    testOrderExecution();
    testCopyAndAssignment();
    testExecuteAllOrders();

    cout << "\n========================================" << endl;
    cout << "ALL TESTS COMPLETED SUCCESSFULLY!" << endl;
    cout << "========================================\n" << endl;

    return 0;
}
