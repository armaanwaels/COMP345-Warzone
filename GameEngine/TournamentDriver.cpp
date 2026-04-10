// TournamentDriver.cpp
// --------------------
// Part 2 driver.  Demonstrates the full tournament flow:
//   - reads a tournament command from console, file, or a built-in demo
//   - rejects a deliberately malformed tournament command (negative test)
//   - parses a well-formed one and runs the full tournament
//
// Usage:
//   ./tournament_test                       -- built-in demo (no args)
//   ./tournament_test -console              -- type commands on stdin
//   ./tournament_test -file <commandfile>   -- read commands from a file

#include "GameEngine.h"
#include "../CommandProcessing/CommandProcessing.h"
#include "../LoggingObserver/LoggingObserver.h"

#include <iostream>
#include <string>

// Validates and, if valid, runs one tournament command end-to-end.
// Shared by both the negative test and the real user command.
static void runOneTournamentCommand(GameEngine *engine, CommandProcessor *cp, Command *cmd)
{
    std::cout << "\n--- Processing command: " << *cmd << " ---" << std::endl;

    // validate() delegates to parseTournamentCommand for the heavy check
    if (!cp->validate(cmd, engine->getState()))
    {
        std::cout << "Command REJECTED: " << cmd->getEffect() << std::endl;
        return;
    }

    // validate() already parsed once to verify, so we parse again here to
    // actually own the config object.  Not the most efficient approach but
    // it keeps the validate/parse interface clean.
    TournamentConfig *cfg = cp->parseTournamentCommand(cmd);
    if (cfg == nullptr)
    {
        std::cout << "Parse failed after validation: " << cmd->getEffect() << std::endl;
        return;
    }

    std::cout << "Command ACCEPTED: " << cmd->getEffect() << std::endl;

    engine->playTournament(*cfg);
    delete cfg;
}

// testTournament: Part 2 entry point.  Picks a command source and runs
// both a negative and a positive test case against it.
void testTournament(int argc, char *argv[])
{
    // log observer writes every Command's effect + every engine state
    // transition to gamelog.txt
    LogObserver *logObs = new LogObserver();

    CommandProcessor *cp = nullptr;
    bool useBuiltIn = false;

    if (argc >= 2 && std::string(argv[1]) == "-console")
    {
        cp = new CommandProcessor();
        std::cout << "Tournament driver: reading commands from console." << std::endl;
    }
    else if (argc >= 3 && std::string(argv[1]) == "-file")
    {
        cp = new FileCommandProcessorAdapter(argv[2]);
        std::cout << "Tournament driver: reading commands from file " << argv[2] << std::endl;
    }
    else
    {
        // no args — run the built-in demo so the driver always does something useful
        cp = new CommandProcessor();
        useBuiltIn = true;
        std::cout << "Tournament driver: no -console/-file flag given; running built-in demo." << std::endl;
    }
    cp->attach(logObs);

    GameEngine *engine = new GameEngine(cp);
    engine->attach(logObs);

    // ===== Negative test: invalid tournament command =====
    // Only one strategy is given, which should be rejected by parseTournamentCommand.
    std::cout << "\n=== Negative test: invalid tournament command ===" << std::endl;
    Command *badCmd = new Command("tournament -M Map/Map_Files/kosova.map -P aggressive -G 3 -D 20");
    badCmd->attach(logObs);
    runOneTournamentCommand(engine, cp, badCmd);
    delete badCmd;

    // ===== Positive test: valid tournament command =====
    std::cout << "\n=== Positive test: valid tournament command ===" << std::endl;
    Command *goodCmd = nullptr;
    if (useBuiltIn)
    {
        // two maps, two strategies, two games each, 15 turns
        goodCmd = new Command(
            "tournament "
            "-M Map/Map_Files/kosova.map Map/Map_Files/german-Empire1871.map "
            "-P aggressive benevolent "
            "-G 2 "
            "-D 15");
    }
    else
    {
        // pull the next line from whatever source the user configured
        goodCmd = cp->getCommand();
    }
    goodCmd->attach(logObs);
    runOneTournamentCommand(engine, cp, goodCmd);

    if (useBuiltIn)
        delete goodCmd; // we allocated it, we clean it up
    // else: CommandProcessor owns it and frees it in its destructor

    // cleanup
    delete engine; // engine's dtor deletes cp
    delete logObs;
}

int main(int argc, char *argv[])
{
    testTournament(argc, argv);
    return 0;
}
