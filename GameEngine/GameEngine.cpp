#include "GameEngine.h"
#include <iostream>
#include <string>
#include <random>

#include "CommandProcessing/CommandProcessing.h"

// ---------- Game Engine ----------

// Default Constructor: Initializes engine in "START" state.
GameEngine::GameEngine() : currentState(new State(State::START)), 
                            map(new Map()), 
                            players(new std::vector<Player*>()), 
                            deck(new Deck()) {}

GameEngine::GameEngine(CommandProcessor *cp) : currentState(new State(State::START)), 
                                                commandProcessor(cp),
                                                map(new Map()),
                                                players(new std::vector<Player*>()),
                                                deck(new Deck()) {} 

// Destructor: Prevents memory leaks by deleting currentState pointer
GameEngine::~GameEngine()
{
    delete currentState;
    delete commandProcessor;
    delete map;
    for(Player *player : *players) delete player;
    delete players;
    delete deck;
}


// Copy Constructor: Produces deep copy of another GameEngine instance
GameEngine::GameEngine(const GameEngine &gameEngine)
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

    // delete's existing currentState to prevent memory leak

    delete this->currentState;

    // Produces deep copy, as in copy constructor
    if (gameEngine.currentState != nullptr)
    {
        this->currentState = new State(*gameEngine.currentState);
    }
    else
    {
        this->currentState = nullptr;
    }
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

        t->setOwner(p->getName());
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
            p->receiveReinforcements(50);
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

