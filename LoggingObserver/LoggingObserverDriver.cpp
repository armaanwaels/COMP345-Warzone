#include "LoggingObserver.h"
#include "../Orders/Orders.h"
#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

// free function to test the logging observer (Part 5)
void testLoggingObserver() {
    cout << "========================================" << endl;
    cout << "  Part 5: Game Logging Observer Demo    " << endl;
    cout << "========================================" << endl;
    cout << endl;

    // clear old log file
    std::remove("gamelog.txt");

    // set up the log observer
    LogObserver* logObserver = new LogObserver();
    cout << "Created: " << *logObserver << endl;
    cout << endl;

    // create orders list and attach the observer
    OrdersList* ordersList = new OrdersList();
    ordersList->attach(logObserver);
    cout << "Attached LogObserver to OrdersList." << endl;
    cout << endl;

    // create some orders and attach observer to each
    Deploy* deploy = new Deploy(5, "Canada");
    deploy->attach(logObserver);

    Advance* advance = new Advance(3, "Canada", "USA");
    advance->attach(logObserver);

    Bomb* bomb = new Bomb("Mexico");
    bomb->attach(logObserver);

    Blockade* blockade = new Blockade("Brazil");
    blockade->attach(logObserver);

    Airlift* airlift = new Airlift(10, "France", "Germany");
    airlift->attach(logObserver);

    Negotiate* negotiate = new Negotiate("Player2");
    negotiate->attach(logObserver);

    // adding orders should trigger logging via OrdersList::notify
    cout << "--- Adding orders to the list (logged to gamelog.txt) ---" << endl;
    ordersList->addOrder(deploy);
    ordersList->addOrder(advance);
    ordersList->addOrder(bomb);
    ordersList->addOrder(blockade);
    ordersList->addOrder(airlift);
    ordersList->addOrder(negotiate);
    cout << endl;

    cout << "--- Current Orders List ---" << endl;
    cout << *ordersList << endl;

    // executing orders should also trigger logging via Order::notify
    cout << "--- Executing all orders (logged to gamelog.txt) ---" << endl;
    for (int i = 0; i < ordersList->getSize(); i++) {
        ordersList->getOrder(i)->execute();
    }
    cout << endl;

    // print gamelog.txt to show everything was logged
    cout << "========================================" << endl;
    cout << "  Contents of gamelog.txt:              " << endl;
    cout << "========================================" << endl;

    ifstream logFile("gamelog.txt");
    if (logFile.is_open()) {
        string line;
        while (std::getline(logFile, line)) {
            cout << line << endl;
        }
        logFile.close();
    } else {
        cout << "Error: Could not open gamelog.txt for reading." << endl;
    }
    cout << "========================================" << endl;
    cout << endl;

    // cleanup
    delete ordersList;
    delete logObserver;

    cout << "Logging Observer demo complete." << endl;
}

int main() {
    testLoggingObserver();
    return 0;
}
