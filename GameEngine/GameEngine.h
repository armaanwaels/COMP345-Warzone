#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <string>

#include "../Map/Map.h"
#include "../Player/Player.h"
#include "../Cards/Cards.h"
#include "../LoggingObserver/LoggingObserver.h"

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

class CommandProcessor;
class TournamentConfig;

// GameEngine: Manages control flow for the game state machine,
// enforcing valid state transitions and flagging invalid ones.
// Manages memory for current state.

class GameEngine : public Subject, public ILoggable
{

private:
    State *currentState; // Pointer to current state in heap
    CommandProcessor *commandProcessor;

    // Model Objects
    Map *map;
    std::vector<Player*> *players;
    Deck *deck;

    // verbose flag — when false the phase helpers keep quiet so tournament
    // runs don't flood the console
    bool *verboseMode;

    // Helper functions 
    bool loadMapCommand(std::string filename);
    bool addPlayerCommand(const std::string& playerName);
    void prepareGameStart();
    void distributeTerritories();
    void shufflePlayerOrder();
    void giveInitialArmies();
    void giveInitialCards();
public:
    // Generic constructor
    GameEngine();

    GameEngine(CommandProcessor* cp);

    // Destructor
    ~GameEngine();

    // Copy Constructor
    GameEngine(const GameEngine &gameEng);

    // Overloaded Assignment Operator
    GameEngine &operator=(const GameEngine &gameEng);

    //------ Accessors ------
    State *getState() const;                  // getter for (private) currentState
    std::string stateToString(State s) const; // helper method for stream insertion

    // state transition + observer notify
    void transition(State newState);
    std::string stringToLog() const override;

    // Main game loop: runs reinforcement -> issue -> execute until one player
    // owns everything OR maxTurns rounds have passed.  Returns the winner's
    // name, or "Draw" when the turn cap is hit.  verbose=false keeps it quiet.
    std::string mainGameLoop(int maxTurns, bool verbose);

    // Game Loop Methods
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    // ---------- Tournament Mode (Part 2) ----------

    // Runs every map x game combination from the config and prints the
    // results table at the end.  Fully cleans up between games.
    void playTournament(const TournamentConfig &config);

    // Prints the results table in the format shown in the assignment PDF.
    // Also logs a one-line summary to gamelog.txt via the observer chain.
    void printTournamentResults(const TournamentConfig &config,
                                const std::vector<std::vector<std::string> *> &results) const;

    // Wipes map/players/deck and replaces them with fresh empty instances.
    // Called before each tournament game.
    void resetGameState();

    // Loads + validates a map, then creates fresh Player objects with the
    // requested strategies and calls prepareGameStart().  Returns false if
    // the map failed to load or validate.
    bool setupTournamentGame(const std::string &mapFile,
                             const std::vector<std::string> &strategies);

    // Overloaded Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &os, const GameEngine &gameEng);

    // Command Processor: Enforces state transtions
    // Game loop is terminated when function returns true
    bool processCommand(std::string *command);

    void startupPhase();
};

#endif