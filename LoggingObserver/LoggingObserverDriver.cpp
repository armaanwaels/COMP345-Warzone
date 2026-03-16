#include "LoggingObserver.h"
#include "../Orders/Orders.h"
#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../GameEngine/GameEngine.h"
#include "../CommandProcessing/CommandProcessing.h"
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

    LogObserver* logObserver = new LogObserver();
    cout << "Created: " << *logObserver << endl;
    cout << endl;

    // ============================================================
    // 1) GameEngine::transition() logging
    //    GameEngine is a subclass of Subject and ILoggable
    // ============================================================
    cout << "--- GameEngine transition() logging ---" << endl;

    GameEngine* engine = new GameEngine();
    engine->attach(logObserver);

    engine->transition(State::MAP_LOADED);
    cout << "  Transitioned to MAP_LOADED" << endl;
    engine->transition(State::MAP_VALIDATED);
    cout << "  Transitioned to MAP_VALIDATED" << endl;
    engine->transition(State::PLAYERS_ADDED);
    cout << "  Transitioned to PLAYERS_ADDED" << endl;
    engine->transition(State::ASSIGN_REINFORCEMENT);
    cout << "  Transitioned to ASSIGN_REINFORCEMENT" << endl;
    cout << endl;

    // ============================================================
    // 2) CommandProcessor::saveCommand() logging
    //    CommandProcessor is a subclass of Subject and ILoggable
    //    Using FileCommandProcessorAdapter to read from a file
    // ============================================================
    cout << "--- CommandProcessor saveCommand() logging ---" << endl;

    // write a temp command file
    std::ofstream cmdFile("test_commands.txt");
    cmdFile << "loadmap test_logging.map" << endl;
    cmdFile << "validatemap" << endl;
    cmdFile << "addplayer Alice" << endl;
    cmdFile.close();

    FileCommandProcessorAdapter* fcp = new FileCommandProcessorAdapter("test_commands.txt");
    fcp->attach(logObserver);

    // reading commands triggers saveCommand() which calls notify()
    Command* cmd1 = fcp->getCommand();
    Command* cmd2 = fcp->getCommand();
    Command* cmd3 = fcp->getCommand();
    cout << endl;

    // ============================================================
    // 3) Command::saveEffect() logging
    //    Command is a subclass of Subject and ILoggable
    // ============================================================
    cout << "--- Command saveEffect() logging ---" << endl;

    cmd1->attach(logObserver);
    cmd2->attach(logObserver);
    cmd3->attach(logObserver);

    // saving effects triggers notify() on each Command
    cmd1->saveEffect("Map loaded successfully.");
    cout << "  Effect saved for: " << cmd1->getCommand() << endl;
    cmd2->saveEffect("Map validated successfully.");
    cout << "  Effect saved for: " << cmd2->getCommand() << endl;
    cmd3->saveEffect("Player Alice added.");
    cout << "  Effect saved for: " << cmd3->getCommand() << endl;
    cout << endl;

    // ============================================================
    // 4) OrdersList::addOrder() and Order::execute() logging
    //    Order and OrdersList are subclasses of Subject and ILoggable
    // ============================================================
    cout << "--- OrdersList and Order logging ---" << endl;

    // build a small map for realistic orders
    std::ofstream mapFile("test_logging.map");
    mapFile << "[files]" << endl;
    mapFile << endl;
    mapFile << "[continents]" << endl;
    mapFile << "TestContinent 3 #FF0000" << endl;
    mapFile << endl;
    mapFile << "[countries]" << endl;
    mapFile << "1 Canada 1 0 0" << endl;
    mapFile << "2 USA 1 1 0" << endl;
    mapFile << "3 Mexico 1 2 0" << endl;
    mapFile << endl;
    mapFile << "[borders]" << endl;
    mapFile << "1 2" << endl;
    mapFile << "2 1 3" << endl;
    mapFile << "3 2" << endl;
    mapFile.close();

    Map* map = new Map();
    MapLoader ml;
    ml.loadMap(*map, "test_logging.map");
    std::remove("test_logging.map");

    std::vector<Territory*> territories = map->getTerritories();
    Territory* canada = territories[0];
    Territory* usa = territories[1];
    Territory* mexico = territories[2];

    Player* player1 = new Player("Alice");
    Player* player2 = new Player("Bob");

    canada->setOwner(player1);
    player1->addTerritory(canada);
    canada->setArmies(10);

    usa->setOwner(player1);
    player1->addTerritory(usa);
    usa->setArmies(5);

    mexico->setOwner(player2);
    player2->addTerritory(mexico);
    mexico->setArmies(5);

    player1->addReinforcements(15);

    // attach observer to order list
    OrdersList* ordersList = new OrdersList();
    ordersList->attach(logObserver);

    // create orders and attach observer to each
    Deploy* deploy = new Deploy(player1, 5, canada);
    deploy->attach(logObserver);

    Advance* advance = new Advance(player1, 3, usa, mexico, nullptr);
    advance->attach(logObserver);

    Bomb* bomb = new Bomb(player1, mexico);
    bomb->attach(logObserver);

    Blockade* blockade = new Blockade(player1, usa);
    blockade->attach(logObserver);

    Airlift* airlift = new Airlift(player1, 2, canada, usa);
    airlift->attach(logObserver);

    Negotiate* negotiate = new Negotiate(player1, player2);
    negotiate->attach(logObserver);

    // adding orders triggers OrdersList::notify via addOrder()
    cout << "Adding orders (each logged via OrdersList)..." << endl;
    ordersList->addOrder(deploy);
    ordersList->addOrder(advance);
    ordersList->addOrder(bomb);
    ordersList->addOrder(blockade);
    ordersList->addOrder(airlift);
    ordersList->addOrder(negotiate);
    cout << endl;

    // executing orders triggers Order::notify via execute()
    cout << "Executing orders (each effect logged via Order)..." << endl;
    for (int i = 0; i < ordersList->getSize(); i++) {
        ordersList->getOrder(i)->execute();
    }
    cout << endl;

    // ============================================================
    // Print gamelog.txt to verify everything was logged
    // ============================================================
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
    std::remove("test_commands.txt");
    delete ordersList;
    delete logObserver;
    delete player1;
    delete player2;
    delete map;
    delete engine;
    delete fcp;

    cout << "Logging Observer demo complete." << endl;
}

int main() {
    testLoggingObserver();
    return 0;
}
