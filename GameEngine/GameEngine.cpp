#include "GameEngine.h"
#include <iostream>
#include <string>

// ---------- Game Engine ----------

// Default Constructor: Initializes engine in "START" state.
GameEngine::GameEngine() : currentState(new State(State::START)) {}

// Destructor: Prevents memory leaks by deleting currentState pointer
GameEngine::~GameEngine()
{
    delete currentState;
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