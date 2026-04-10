#include "GameEngine.h"
#include "../Orders/Orders.h"
#include "../Player/PlayerStrategies.h"
#include "../LoggingObserver/LoggingObserver.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include <cctype>

#include "../CommandProcessing/CommandProcessing.h"

// ---------- Game Engine ----------

// Default Constructor: Initializes engine in "START" state.
GameEngine::GameEngine() : currentState(new State(State::START)),
                            commandProcessor(nullptr),
                            map(new Map()),
                            players(new std::vector<Player*>()),
                            deck(new Deck()),
                            verboseMode(new bool(true)) {}

GameEngine::GameEngine(CommandProcessor *cp) : currentState(new State(State::START)),
                                                commandProcessor(cp),
                                                map(new Map()),
                                                players(new std::vector<Player*>()),
                                                deck(new Deck()),
                                                verboseMode(new bool(true)) {}

// Destructor: Prevents memory leaks by deleting every heap-allocated member.
GameEngine::~GameEngine()
{
    delete currentState;
    delete commandProcessor;
    if (map != nullptr) {
        delete map;
    }
    if (players != nullptr) {
        for (Player *player : *players) delete player;
        delete players;
    }
    if (deck != nullptr) {
        delete deck;
    }
    delete verboseMode;
}


// Copy Constructor: deep copies the scalar state.  Model objects
// (map/players/deck) get fresh empty instances — this matches how the
// original Part 1 copy ctor handled them and avoids aliasing issues.
GameEngine::GameEngine(const GameEngine &gameEngine) : Subject(gameEngine),
                                                        currentState(nullptr),
                                                        commandProcessor(nullptr),
                                                        map(new Map()),
                                                        players(new std::vector<Player*>()),
                                                        deck(new Deck()),
                                                        verboseMode(new bool(*gameEngine.verboseMode))
{
    if (gameEngine.currentState != nullptr)
    {
        this->currentState = new State(*gameEngine.currentState);
    }
    else
    {
        this->currentState = nullptr;
    }
}

// Overloaded assignment operator:  Overloads "=" operator, producing deep copy of GameEngine.
GameEngine &GameEngine::operator=(const GameEngine &gameEngine)
{
    // self-check guard (provents deleting current memory )
    if (this == &gameEngine)
        return *this;

    Subject::operator=(gameEngine);

    // delete's existing currentState to prevent memory leak

    delete this->currentState;
    delete this->verboseMode;

    // Produces deep copy, as in copy constructor
    if (gameEngine.currentState != nullptr)
    {
        this->currentState = new State(*gameEngine.currentState);
    }
    else
    {
        this->currentState = nullptr;
    }
    this->verboseMode = new bool(*gameEngine.verboseMode);
    return *this;
}

// Getter: Returns pointers to the (private) State object.
// Allows inspection without modifying the State.
State *GameEngine::getState() const
{
    return this->currentState;
}

// Helper method for stream insertion operator.
// Conveniently convert Enum classes to readable strings when printed
std::string GameEngine::stateToString(State s) const
{
    switch (s)
    {
    case State::START:
        return "START";

    case State::MAP_LOADED:
        return "MAP_LOADED";

    case State::MAP_VALIDATED:
        return "MAP_VALIDATED";

    case State::PLAYERS_ADDED:
        return "PLAYERS_ADDED";

    case State::ASSIGN_REINFORCEMENT:
        return "ASSIGN_REINFORCEMENT";

    case State::ISSUE_ORDERS:
        return "ISSUE_ORDERS";

    case State::EXECUTE_ORDERS:
        return "EXECUTE_ORDERS";

    case State::WIN:
        return "WIN";

    default:
        return "UNKNOWN";
    }
}

// Overloaded stream insertion opertor: Displays current state of GameEngine
std::ostream &operator<<(std::ostream &os, const GameEngine &engine)
{
    // Null pointer check
    if (engine.currentState == nullptr)
    {
        os << "GameEngine State: NULL";
    }
    else
    {
        os << "GameEngine Current State: " << engine.stateToString(*engine.currentState);
    }
    return os;
}

// --------------- PROCESS COMMAND FUNCTION ---------------

// Process Command enacts the game engine's state machine logic.
// It is a boolean, returning false in two cases:
// (1) If command or currentState are null pointers
// (2) At the bottom of the method
// This ensures that the game-loop continues to run for all input,
// except "end", which returns true and thus exits the game-loop
// Each command is associated with possible valid states where
// it can be used as input, or else invalid states to the user.
bool GameEngine::processCommand(std::string *command)
{
    // guard clause: checking both pointers are not null
    if (command == nullptr || currentState == nullptr)
    {
        return false;
    }

    // ------ "start" : Transition from START to MAP_LOADED ?? ------

    if (*command == "start")
    {
        if (*currentState == State::WIN)
        {
            *currentState = State::START;
            std ::cout << "Starting New Game. Welcome!" << std::endl;
        }
        else
        {
            std ::cout << "Error: Cannot start new game before exiting." << std::endl;
        }
    }

    // ----- "loadmap: Transition from START to MAP_LOADED (or MAP_LOADED to itself) " -----
    else if (*command == "loadmap")
    {
        if (*currentState == State::START || *currentState == State::MAP_LOADED)
        {
            *currentState = State::MAP_LOADED;
            std ::cout << "A new map has been loaded." << std::endl;
        }

        else
        {
            std ::cout << "Error:  Cannot load map in current state." << std::endl;
        }
    }

    // ----- "validatemap: Transition from MAP_LOADED to MAP_VALIDATED" -----

    else if (*command == "validatemap")
    {
        if (*currentState == State::MAP_LOADED)
        {
            *currentState = State::MAP_VALIDATED;
            std ::cout << "Map validated." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot validate map in current state." << std::endl;
        }
    }

    // ----- "addplayer: Transition from MAP_VALIDATED to PLAYERS_ADDED (or PLAYERS_ADDED to itself)" -----

    else if (*command == "addplayer")
    {
        if (*currentState == State::MAP_VALIDATED || *currentState == State::PLAYERS_ADDED)
        {
            *currentState = State::PLAYERS_ADDED;
            std ::cout << "A player has been added." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot add player in current state." << std::endl;
        }
    }

    // ----- "assigncountries: Transition from PLAYERS_ADDED to ASSIGN_REINFORCEMENT" -----

    else if (*command == "assigncountries")
    {
        if (*currentState == State::PLAYERS_ADDED)
        {
            *currentState = State::ASSIGN_REINFORCEMENT;
            std ::cout << "Countries assigned." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot assign countries in current state." << std::endl;
        }
    }

    // ----- "issueorder": Transition from ASSIGN_REINFORCEMENT to ISSUE_ORDERS (or ISSUE_ORDERS to itself) -----

    else if (*command == "issueorder")
    {
        if (*currentState == State::ASSIGN_REINFORCEMENT || *currentState == State::ISSUE_ORDERS)
        {
            *currentState = State::ISSUE_ORDERS;
            std ::cout << "Order issued." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot issue orders in current state." << std::endl;
        }
    }
    // ----- "endissueorders: TRANSITION FROM ISSUE_ORDERS TO EXECUTE_ORDERS" -----
    else if (*command == "endissueorders")
    {
        if (*currentState == State::ISSUE_ORDERS)
        {
            *currentState = State::EXECUTE_ORDERS;
            std ::cout << "Finished issuing orders." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot (end) issue orders in current state." << std::endl;
        }
    }

    // ----- "execorder": Transition from EXECUTE_ORDERS to itself -----

    else if (*command == "execorder")
    {
        if (*currentState == State::EXECUTE_ORDERS)
        {
            *currentState = State::EXECUTE_ORDERS;
            std ::cout << "Order(s) executed." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot execute orders in current state." << std::endl;
        }
    }

    // ----- "endexecorder": Transition from EXECUTE_ORDERS to ASSIGN_REINFORCEMENT -----

    else if (*command == "endexecorders")
    {
        if (*currentState == State::EXECUTE_ORDERS)
        {
            *currentState = State::ASSIGN_REINFORCEMENT;
            std ::cout << "Returning to assign reinforcements" << std::endl;
        }
        else
        {
            std ::cout << "Error:  Cannot terminate orders in current state." << std::endl;
        }
    }
    // ----- "win": Transition from EXECUTE_ORDERS to WIN -----

    else if (*command == "win")
    {
        if (*currentState == State::EXECUTE_ORDERS)
        {
            *currentState = State::WIN;
            std ::cout << "Congratulations! You Won!  Type \"end\" to exit or \"start\" to start a new game." << std::endl;
        }
        else
        {
            std ::cout << "Error:  Win conditions not met." << std::endl;
        }
    }

    // ----- "end": Transition to exit state -----

    else if (*command == "end") // This condition allows users to exit game-loop in driver

    {
        std ::cout << "Thanks for playing.  Goodbye." << std::endl;
        return true;
    }

    // ----- "help": Prints list of valid commands -----

    else if (*command == "help")
    {

        std::cout << "Help Menu: Below are a list of valid commands.  Make sure you use lowercase and that you are in the correct game phase: " << std::endl;
        std::cout << "loadmap" << std::endl;
        std::cout << "validatemap" << std::endl;
        std::cout << "addplayer" << std::endl;
        std::cout << "assigncountries" << std::endl;
        std::cout << "issueorders" << std::endl;
        std::cout << "endissueorders" << std::endl;
        std::cout << "execorder" << std::endl;
        std::cout << "endexecorders" << std::endl;
        std::cout << "win" << std::endl;
        std::cout << "end" << std::endl;
        std::cout << "start" << std::endl;
    }
    else
    {
        std::cout << "Error: Invalid Command.  Enter \" help \" for list of commands" << std::endl;
    }

    return false; // Default case: the command was proccessed (or invalid)
                  // This keeps the game loop running.
}


// ---------------- Startup Phase Method -------------------


// The startupPhase method, takes care of the logic of the startup of a game
// It loops over either or a file or console input using the getCommand() method
// It takes care of changing the state of the engine up until the ASSINGE_REINFORCEMENT state
// Finally if the map is loaded, valid, and the number of players is correct, it starts the game. 
void GameEngine::startupPhase(){
    std::cout << "---- STARTUP PHASE ----" << std::endl;

    // loops until the startup phase is done or a break is encountered
    while(*currentState != State::ASSIGN_REINFORCEMENT){
        Command *cmd = commandProcessor->getCommand();

        // Check if the command isn't empty
        if (cmd == nullptr || cmd->getCommand().empty())
        {
            std::cout << "No command received. Startup phase stopped." << std::endl;
            break;
        }

        // Validate the command
        if (!commandProcessor->validate(cmd, currentState))
        {
            std::cout << cmd->getEffect() << std::endl;
            continue;
        }

        // This is in case the command requires a second part
        std::istringstream iss(cmd->getCommand());
        std::string keyword;
        // extract the first part of the command (which can be the whole command)
        iss >> keyword;
        
        // For each gamestartup commands executes the right logic
        // This includes calling the right help function 
        // Deciding wether or not to change game state
        // And setting the effect of the currant command
        if (keyword == "loadmap")
        {
            MapLoader ml;
            std::string filename;
            iss >> filename;

            bool loaded = loadMapCommand(filename);   

            // If everything loaded correctly set state to MAP_LOADED
            if (loaded)
            {   
                *currentState = State::MAP_LOADED;
                cmd->saveEffect("Map loaded successfully: " + filename);
            }
            else
            {
                cmd->saveEffect("ERROR: failed to load map \"" + filename + "\".");
            }
        }else if(keyword == "validatemap"){
            if (map == nullptr)  // Safety check 
            {
                cmd->saveEffect("ERROR: no map is currently loaded.");
            }
            else if (map->validate())
            {
                *currentState = State::MAP_VALIDATED;
                cmd->saveEffect("Map validated successfully.");
            }
            else // don't change game state if map not valid
            {
                cmd->saveEffect("ERROR: map validation failed.");
            }
        }else if (keyword == "addplayer")
        {   
            // get the second part of the command which should be the name
            std::string playerName;
            iss >> playerName;

            
            if (addPlayerCommand(playerName))
            {   
                *currentState =  State::PLAYERS_ADDED;
                cmd->saveEffect("Player added successfully: " + playerName);
            }
            else
            {
                cmd->saveEffect("ERROR: could not add player \"" + playerName + "\".");
            }
        }else if (keyword == "gamestart")
        {
            // make sure that there are between 2 and 6 players curently in the game
            if (players == nullptr || players->size() < 2 || players->size() > 6)
            {
                cmd->saveEffect("ERROR: gamestart requires 2 to 6 players.");
            }
            else if (map == nullptr) // Another safety check
            {
                cmd->saveEffect("ERROR: no map loaded.");
            }
            else
            {
                prepareGameStart();
                *currentState = State::ASSIGN_REINFORCEMENT;
                cmd->saveEffect("Game started successfully.");
            }
        }
    }
}

// loadMapCommand: ensures that loadMap executed correctly
// by returning false if it didn't
bool GameEngine::loadMapCommand(std::string filename){
    
    Map* tempMap = new Map();

    try{
        MapLoader ml;
        ml.loadMap(*tempMap, filename);

        if(map != nullptr){
            delete map;
        }

        map = tempMap;
        return true;
    }catch(const std::exception& e){
        std::cerr << e.what();
        delete tempMap;
        return false;
    }


}

// addPlayerCommand: ensures that Players are added correctly
// (1) Checks for duplicate inputs 
// (2) Checks for empty name 
// Only returns true if players was added successfully 
bool GameEngine::addPlayerCommand(const std::string& playerName)
{
    // reject empty name
    if (playerName.empty())
        return false;

    // make sure the player container exists
    if (players == nullptr)
        return false;

    // max 6 players
    if (players->size() >= 6)
        return false;

    // reject duplicate names
    for (Player* p : *players)
    {
        if (p != nullptr && p->getName() == playerName)
        {
            return false;
        }
    }

    // create and store the new player
    players->push_back(new Player(playerName));

    return true;
}

// Helper that takes care of setting up the game:
// (1) distribution of Territory
// (2) Player order
// (3) Giving out the Players Hands
void GameEngine::prepareGameStart(){

    distributeTerritories();
    shufflePlayerOrder();
    giveInitialArmies();

    // Initialise the deck 
    // Temporary as for the moment we aren't populating the deck from a game file
    // We fill the deck with bomb cards as default
    for(int i = 0; i<50 ; i++){
        deck->addCard(new Card());
    }

    giveInitialCards();

}

// Helper function to prepareGameSart that distributes the territories
// fairly accross all players
void GameEngine::distributeTerritories()
{
    if (map == nullptr || players == nullptr || players->empty())
        return;

    std::vector<Territory*> territories = map->getTerritories();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(territories.begin(), territories.end(), gen);

    int playerCount = static_cast<int>(players->size());

    for (size_t i = 0; i < territories.size(); ++i)
    {
        Player* p = (*players)[i % playerCount];
        Territory* t = territories[i];

        t->setOwner(p);
        p->addTerritory(t);
    }
}

// Helper function to prepareGameStart that randomly sets the Player order
void GameEngine::shufflePlayerOrder()
{
    if (players == nullptr || players->empty())
        return;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(players->begin(), players->end(), gen);
}

// Helper function to prepareGameStart that gives each player 50 armies
void GameEngine::giveInitialArmies()
{
    if (players == nullptr)
        return;

    for (Player* p : *players)
    {
        if (p != nullptr)
        {
            p->addReinforcements(50);
        }
    }
}

// Helper function that draws two cards for each Player and gives it to them. 
void GameEngine::giveInitialCards(){
    if (players == nullptr) 
        return;

    for (Player* p : *players)
    {
        if(p != nullptr)
        {
            deck->draw(*(p->getHand()));
            deck->draw(*(p->getHand()));
        }
    }
}

// transition + logging

void GameEngine::transition(State newState) {
    *currentState = newState;
    notify(this);
}

std::string GameEngine::stringToLog() const {
    return "GameEngine new state: " + stateToString(*currentState);
}

// mainGameLoop: runs the main play loop until someone owns all territories
// or maxTurns rounds have passed.  Returns the winner's name (or "Draw").
// verbose=false lets tournament runs stay quiet.
std::string GameEngine::mainGameLoop(int maxTurns, bool verbose) {
    // sync the engine's verbose flag so the phase helpers pick it up
    *verboseMode = verbose;

    if (verbose) {
        std::cout << "\n==== MAIN GAME LOOP STARTED (maxTurns=" << maxTurns << ") ====" << std::endl;
    }

    int round = 0;
    std::string result = "Draw";

    while (true) {
        round++;
        if (verbose) {
            std::cout << "\n--- Round " << round << " ---" << std::endl;
        }

        // reset each Cheater's per-turn flag so they act every round, not
        // just round 1  (edge case: without this, games with a cheater stall)
        for (Player *p : *players) {
            CheaterPlayerStrategy *cs = dynamic_cast<CheaterPlayerStrategy *>(p->getStrategy());
            if (cs != nullptr)
                cs->resetTurn();
        }

        reinforcementPhase();
        issueOrdersPhase();
        executeOrdersPhase();

        // remove eliminated players
        std::vector<Player*> eliminated;
        for (auto it = players->begin(); it != players->end(); ) {
            if ((*it)->getTerritories()->empty()) {
                if (verbose) {
                    std::cout << (*it)->getName() << " has been eliminated!" << std::endl;
                }
                eliminated.push_back(*it);
                it = players->erase(it);
            } else {
                ++it;
            }
        }
        for (Player* p : eliminated) {
            delete p;
        }

        // award cards for conquests
        for (Player* p : *players) {
            if (p->hasConqueredThisTurn()) {
                if (deck->getSize() > 0) {
                    deck->draw(*(p->getHand()));
                    if (verbose) {
                        std::cout << p->getName() << " earned a card for conquering a territory." << std::endl;
                    }
                }
                p->setConqueredThisTurn(false);
            }
            p->clearNegotiations();
        }

        // win condition: last player standing
        if (players->size() == 1) {
            result = (*players)[0]->getName();
            if (verbose) {
                std::cout << "\n**** " << result << " WINS THE GAME! ****" << std::endl;
            }
            transition(State::WIN);
            break;
        }

        if (players->empty()) {
            if (verbose) {
                std::cout << "\nNo players remaining. Game over." << std::endl;
            }
            result = "Draw";
            break;
        }

        if (round >= maxTurns) {
            if (verbose) {
                std::cout << "\nMax turns reached (" << maxTurns << "). Declaring a draw." << std::endl;
            }
            result = "Draw";
            break;
        }
    }

    return result;
}

// Round-robin issue orders.  Each player is asked until it returns false
// (done) or until it hits the per-turn cap.  The cap is needed because the
// Aggressive strategy bases decisions on the current world state, which
// doesn't change until execute — so without it, it would queue Advance
// orders forever.  8 actions/turn is plenty for the test maps.
void GameEngine::issueOrdersPhase() {
    const bool verbose = (verboseMode != nullptr && *verboseMode);

    if (verbose) std::cout << "\n-- Issue Orders Phase --" << std::endl;

    if (players == nullptr || players->empty()) return;

    const int MAX_ORDERS_PER_TURN = 8; // safety cap per player per turn

    std::vector<bool> doneIssuing(players->size(), false);
    std::vector<int> issuedCount(players->size(), 0);
    bool allDone = false;

    while (!allDone) {
        allDone = true;
        for (size_t i = 0; i < players->size(); i++) {
            if (doneIssuing[i]) continue;

            Player* p = (*players)[i];
            bool issued = p->issueOrder(deck, map);

            if (!issued) {
                doneIssuing[i] = true;
                if (verbose)
                    std::cout << p->getName() << " is done issuing orders." << std::endl;
            } else {
                issuedCount[i]++;
                if (issuedCount[i] >= MAX_ORDERS_PER_TURN) {
                    doneIssuing[i] = true;
                    if (verbose)
                        std::cout << p->getName() << " hit the per-turn order cap." << std::endl;
                } else {
                    allDone = false;
                }
            }
        }
    }

    if (verbose) {
        for (Player* p : *players) {
            std::cout << p->getName() << " has " << p->getOrders()->getSize() << " orders queued." << std::endl;
        }
    }
}

// Deploy orders run first, then everything else (round-robin).
// When verbose is off we temporarily swap cout with a string buffer, because
// the Order::execute methods always print and we can't easily silence them.
void GameEngine::executeOrdersPhase() {
    const bool verbose = (verboseMode != nullptr && *verboseMode);

    std::streambuf *oldCout = nullptr;
    std::ostringstream sink;
    if (!verbose) {
        oldCout = std::cout.rdbuf(sink.rdbuf());
    }

    if (verbose) std::cout << "\n-- Execute Orders Phase --" << std::endl;

    if (players == nullptr || players->empty()) {
        if (!verbose && oldCout != nullptr) std::cout.rdbuf(oldCout);
        return;
    }

    // ---- deploy pass ----
    if (verbose) std::cout << "Executing deploy orders..." << std::endl;
    bool hasDeployOrders = true;
    while (hasDeployOrders) {
        hasDeployOrders = false;
        for (Player* p : *players) {
            OrdersList* ol = p->getOrders();
            for (int j = 0; j < ol->getSize(); j++) {
                Order* order = ol->getOrder(j);
                if (dynamic_cast<Deploy*>(order) != nullptr) {
                    order->execute();
                    ol->remove(j);
                    hasDeployOrders = true;
                    break;
                }
            }
        }
    }

    // ---- everything else ----
    if (verbose) std::cout << "Executing remaining orders..." << std::endl;
    bool hasOrders = true;
    while (hasOrders) {
        hasOrders = false;
        for (Player* p : *players) {
            OrdersList* ol = p->getOrders();
            if (ol->getSize() > 0) {
                ol->getOrder(0)->execute();
                ol->remove(0);
                hasOrders = true;
            }
        }
    }

    if (verbose) std::cout << "All orders executed." << std::endl;

    // restore cout
    if (!verbose && oldCout != nullptr)
        std::cout.rdbuf(oldCout);
}

// reinforcement phase: territories/3 + continent bonus, min 3
void GameEngine::reinforcementPhase() {
    const bool verbose = (verboseMode != nullptr && *verboseMode);

    if (players == nullptr || map == nullptr)
        return;

    std::vector<Territory*> territories = map->getTerritories();
    std::vector<Continent*> continents = map->getContinents();

    for (Player* player : *players) {
        if (player == nullptr)
            continue;

        int territoryCount = player->getTerritories()->size();

        int reinforcements = territoryCount / 3;

        if (reinforcements < 3)
            reinforcements = 3;

        // continent bonus
        for (Continent* continent : continents) {
            bool ownsEntireContinent = true;

            for (Territory* territory : territories) {
                if (territory->getContinent() == continent->getNum()) {
                    if (territory->getOwner() != player) {
                        ownsEntireContinent = false;
                        break;
                    }
                }
            }

            if (ownsEntireContinent) {
                reinforcements += continent->getBonus();
            }
        }

        player->addReinforcements(reinforcements);

        if (verbose) {
            std::cout << player->getName() << " receives " << reinforcements << " reinforcement armies." << std::endl;
        }
    }
}

// ---------- Tournament Mode ----------

// Lowercase a string so strategy keyword matching is case-insensitive.
static std::string toLowerString(const std::string &s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++)
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    return out;
}

// TournamentLogMessage
// --------------------
// Tiny ILoggable used by printTournamentResults to push a one-line summary
// through the engine's observer chain so it ends up in gamelog.txt.  It is
// only used inside this translation unit.
class TournamentLogMessage : public ILoggable {
private:
    std::string *message;
public:
    TournamentLogMessage(const std::string &m);
    TournamentLogMessage(const TournamentLogMessage &other);
    ~TournamentLogMessage();
    TournamentLogMessage &operator=(const TournamentLogMessage &other);

    std::string stringToLog() const override;

    friend std::ostream &operator<<(std::ostream &os, const TournamentLogMessage &lm);
};

TournamentLogMessage::TournamentLogMessage(const std::string &m)
    : message(new std::string(m)) {}

// Copy Constructor
TournamentLogMessage::TournamentLogMessage(const TournamentLogMessage &other)
    : message(new std::string(*other.message)) {}

TournamentLogMessage::~TournamentLogMessage() {
    delete message;
}

// Assignment Operator (deep copy)
TournamentLogMessage &TournamentLogMessage::operator=(const TournamentLogMessage &other) {
    if (this != &other) {
        delete message;
        message = new std::string(*other.message);
    }
    return *this;
}

std::string TournamentLogMessage::stringToLog() const {
    return *message;
}

// Stream Insertion Operator
std::ostream &operator<<(std::ostream &os, const TournamentLogMessage &lm) {
    os << *lm.message;
    return os;
}

// Creates the PlayerStrategy object for one of the four computer strategies.
// parseTournamentCommand already filtered the list so this should not
// return nullptr in practice.
static PlayerStrategy *makeStrategyByName(const std::string &name) {
    std::string key = toLowerString(name);
    if (key == "aggressive") return new AggressivePlayerStrategy();
    if (key == "benevolent") return new BenevolentPlayerStrategy();
    if (key == "neutral")    return new NeutralPlayerStrategy();
    if (key == "cheater")    return new CheaterPlayerStrategy();
    return nullptr;
}

// Wipes map/players/deck + the Blockade static neutral and re-creates empty
// ones.  Called between games in a tournament so nothing leaks.
void GameEngine::resetGameState() {
    if (map != nullptr) {
        delete map;
        map = nullptr;
    }
    if (players != nullptr) {
        for (Player *p : *players)
            delete p;
        delete players;
        players = nullptr;
    }
    if (deck != nullptr) {
        delete deck;
        deck = nullptr;
    }

    // Blockade keeps a static "Neutral" player; if we leave it alone it will
    // point at territories from the previous game and segfault in the next.
    if (Blockade::neutralPlayer != nullptr) {
        delete Blockade::neutralPlayer;
        Blockade::neutralPlayer = nullptr;
    }

    map = new Map();
    players = new std::vector<Player *>();
    deck = new Deck();
    *currentState = State::START;
}

// Loads and validates the map, creates the players with their strategies,
// then reuses prepareGameStart() for territory/army/card setup.  No user
// interaction at any point.  Returns false on map load/validate failure.
bool GameEngine::setupTournamentGame(const std::string &mapFile,
                                     const std::vector<std::string> &strategies) {
    if (!loadMapCommand(mapFile)) {
        std::cout << "[tournament] Map \"" << mapFile
                  << "\" failed to load - skipping." << std::endl;
        return false;
    }

    if (!map->validate()) {
        std::cout << "[tournament] Map \"" << mapFile
                  << "\" failed validation - skipping." << std::endl;
        return false;
    }

    // one Player per strategy in the command; names like "Aggressive1"
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::string key = toLowerString(strategies[i]);
        std::string display = key;
        if (!display.empty())
            display[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(display[0])));
        std::string playerName = display + std::to_string(i + 1);

        Player *p = new Player(playerName);
        PlayerStrategy *strat = makeStrategyByName(key);
        if (strat == nullptr) {
            // should be unreachable — parseTournamentCommand filtered the list
            delete p;
            return false;
        }
        p->setStrategy(strat);
        players->push_back(p);
    }

    prepareGameStart();                          // distribute territories, armies, etc.
    *currentState = State::ASSIGN_REINFORCEMENT;
    return true;
}

// Runs the full tournament.  For each (map, game) we reset state, set up a
// fresh game, silence cout while the game plays, and record the outcome.
void GameEngine::playTournament(const TournamentConfig &config) {
    const std::vector<std::string> &mapFiles   = config.getMapFiles();
    const std::vector<std::string> &strategies = config.getPlayerStrategies();
    int numGames = config.getNumberOfGames();
    int maxTurns = config.getMaxNumberOfTurns();

    std::cout << "\n==== STARTING TOURNAMENT ====" << std::endl;
    std::cout << config << std::endl;

    // results[mapIndex] = vector of per-game result strings.
    // pointer-of-vector to satisfy the "user-defined type members must be pointers" rule
    std::vector<std::vector<std::string> *> results;
    for (size_t i = 0; i < mapFiles.size(); ++i)
        results.push_back(new std::vector<std::string>());

    for (size_t m = 0; m < mapFiles.size(); ++m) {
        const std::string &mapFile = mapFiles[m];
        std::cout << "\n---- Map " << (m + 1) << ": " << mapFile << " ----" << std::endl;

        for (int g = 0; g < numGames; ++g) {
            std::cout << "\n>> Game " << (g + 1) << " of " << numGames
                      << " on " << mapFile << "... " << std::flush;

            resetGameState();

            // swap cout/cerr to a sink so the game runs silently
            std::ostringstream sink;
            std::streambuf *savedCout = std::cout.rdbuf(sink.rdbuf());
            std::streambuf *savedCerr = std::cerr.rdbuf(sink.rdbuf());

            std::string outcome;
            if (!setupTournamentGame(mapFile, strategies)) {
                outcome = "InvalidMap";
            } else {
                outcome = mainGameLoop(maxTurns, false);
            }

            // restore cout/cerr before printing the result
            std::cout.rdbuf(savedCout);
            std::cerr.rdbuf(savedCerr);

            std::cout << outcome << std::endl;
            results[m]->push_back(outcome);
        }
    }

    printTournamentResults(config, results);

    // clean up the results table
    for (std::vector<std::string> *row : results)
        delete row;

    // final reset so the destructor has very little to do
    resetGameState();
}

// Prints the results table and sends a one-line summary to the log file.
// Format matches the tournament-results example in the assignment PDF:
//
//           Game 1  Game 2  Game 3  Game 4  Game 5
//   Map 1:  W       D       W       D       W
//   Map 2:  ...
void GameEngine::printTournamentResults(const TournamentConfig &config,
                                        const std::vector<std::vector<std::string> *> &results) const {
    const std::vector<std::string> &mapFiles   = config.getMapFiles();
    const std::vector<std::string> &strategies = config.getPlayerStrategies();
    int numGames = config.getNumberOfGames();
    int maxTurns = config.getMaxNumberOfTurns();

    // column width wide enough for the longest cell, clamped to a minimum
    size_t colWidth = 10;
    for (size_t i = 0; i < results.size(); i++) {
        const std::vector<std::string> &row = *results[i];
        for (size_t j = 0; j < row.size(); j++) {
            if (row[j].size() + 2 > colWidth)
                colWidth = row[j].size() + 2;
        }
    }

    // row label: "Map N: <filename>"
    size_t mapLabelWidth = 10;
    for (size_t i = 0; i < mapFiles.size(); ++i) {
        std::string label = "Map " + std::to_string(i + 1) + ": " + mapFiles[i];
        if (label.size() + 2 > mapLabelWidth)
            mapLabelWidth = label.size() + 2;
    }

    std::cout << "\n==== TOURNAMENT RESULTS ====" << std::endl;
    std::cout << "Tournament mode:" << std::endl;

    std::cout << "M: ";
    for (size_t i = 0; i < mapFiles.size(); ++i) {
        std::cout << mapFiles[i];
        if (i + 1 < mapFiles.size()) std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "P: ";
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::cout << strategies[i];
        if (i + 1 < strategies.size()) std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "G: " << numGames << std::endl;
    std::cout << "D: " << maxTurns << std::endl;
    std::cout << std::endl;

    // header row
    std::cout << std::left << std::setw(mapLabelWidth) << "";
    for (int g = 0; g < numGames; ++g) {
        std::string header = "Game " + std::to_string(g + 1);
        std::cout << std::left << std::setw(colWidth) << header;
    }
    std::cout << std::endl;

    // data rows
    for (size_t m = 0; m < mapFiles.size(); ++m) {
        std::string label = "Map " + std::to_string(m + 1) + ": " + mapFiles[m];
        std::cout << std::left << std::setw(mapLabelWidth) << label;

        const std::vector<std::string> &row = *results[m];
        for (int g = 0; g < numGames; ++g) {
            std::string cell = (g < (int)row.size()) ? row[g] : "-";
            std::cout << std::left << std::setw(colWidth) << cell;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // build the log summary line and notify observers
    std::ostringstream logLine;
    logLine << "Tournament results: ";
    for (size_t m = 0; m < mapFiles.size(); ++m) {
        logLine << mapFiles[m] << "=[";
        const std::vector<std::string> &row = *results[m];
        for (size_t g = 0; g < row.size(); ++g) {
            logLine << row[g];
            if (g + 1 < row.size()) logLine << ", ";
        }
        logLine << "]";
        if (m + 1 < mapFiles.size()) logLine << "; ";
    }

    TournamentLogMessage logMsg(logLine.str());
    const_cast<GameEngine *>(this)->notify(&logMsg);
}
