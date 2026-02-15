#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <string>

// State: Representation of each possible state in the game state machine.
enum class State
{
    START,
    MAP_LOADED,
    MAP_VALIDATED,
    PLAYERS_ADDED,
    ASSIGN_REINFORCEMENT,
    ISSUE_ORDERS,
    EXECUTE_ORDERS,
    WIN
};

// GameEngine: Manages control flow for the game state machine,
// enforcing valid state transitions and flagging invalid ones.
// Manages memory for current state.

class GameEngine
{

private:
    State *currentState; // Pointer to current state in heap

public:
    // Generic constructor
    GameEngine();

    // Destructor
    ~GameEngine();

    // Copy Constructor
    GameEngine(const GameEngine &gameEng);

    // Overloaded Assignment Operator
    GameEngine &operator=(const GameEngine &gameEng);

    //------ Accessors ------
    State *getState() const;                  // getter for (private) currentState
    std::string stateToString(State s) const; // helper method for stream insertion

    // Overloaded Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &os, const GameEngine &gameEng);

    // Command Processor: Enforces state transtions
    // Game loop is terminated when function returns true
    bool processCommand(std::string *command);
};

#endif