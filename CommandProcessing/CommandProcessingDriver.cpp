#include "CommandProcessing.h"
#include "../GameEngine/GameEngine.h"
#include <iostream>
#include <string>

// --------------- helpers ---------------

// applyCommand: Applies a validated command to the GameEngine, updating the
// game state and recording the command's effect.  This bridges the
// CommandProcessor with the GameEngine state machine.
// Returns true only on "quit" so the calling loop knows to exit.
static bool applyCommand(Command *cmd, GameEngine &engine)
{
    std::string cmdStr = cmd->getCommand();

    // Extract keyword so commands are matched cleanly
    std::string keyword = cmdStr.substr(0, cmdStr.find(' ')); // gaps mark seperation

    if (keyword == "loadmap")
    {
        *engine.getState() = State::MAP_LOADED;
        cmd->saveEffect("Map loaded. Transitioned to MAP_LOADED.");
    }
    else if (keyword == "validatemap")
    {
        *engine.getState() = State::MAP_VALIDATED;
        cmd->saveEffect("Map validated. Transitioned to MAP_VALIDATED.");
    }
    else if (keyword == "addplayer")
    {
        *engine.getState() = State::PLAYERS_ADDED;
        std::string name = cmdStr.substr(cmdStr.find(' ') + 1);
        cmd->saveEffect("Player \"" + name + "\" added. Transitioned to PLAYERS_ADDED.");
    }
    else if (keyword == "gamestart")
    {
        *engine.getState() = State::ASSIGN_REINFORCEMENT;
        cmd->saveEffect("Game started. Transitioned to ASSIGN_REINFORCEMENT (play phase).");
    }
    else if (keyword == "replay")
    {
        *engine.getState() = State::START;
        cmd->saveEffect("Replaying. Transitioned to START.");
    }
    else if (keyword == "quit")
    {
        cmd->saveEffect("Quitting. Goodbye.");
        return true; // Signal the loop to exit
    }

    return false;
}

// printSeparator: Cosmetic helper that makes demo easier to read.
static void printSeparator(const std::string &text)
{
    std::cout << " \n \n------------ " << text << " ------------ \n \n";
}

// --------------- testCommandProcessing ---------------

// Accepts command-line arguments that mirror the full application:
//   -console         : read commands interactively from the console
//   -file <filename> : read commands from a text file

// Commands can be read from the console (CommandProcessor) or
// from a file (FileCommandProcessorAdapter).
// Invalid commands are rejected with an error effect.
// Valid commands produce the correct effect and state transition.
void testCommandProcessing(int argc, char *argv[])
{
    // ----- Parse command-line arguments -----

    bool useConsole = false;
    std::string filename = "";

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " -console  OR  " << argv[0] << " -file <filename>\n";
        std::cout << "Defaulting to -console mode.\n";
        useConsole = true;
    }
    else if (std::string(argv[1]) == "-console")
    {
        useConsole = true;
    }
    else if (std::string(argv[1]) == "-file" && argc >= 3)
    {
        filename = argv[2];
        useConsole = false;
    }
    else
    {
        std::cout << "Unrecognised arguments.  Defaulting to -console mode.\n";
        useConsole = true;
    }

    // ----- Demo :Validation rejects bad commands (always printed first) -----

    printSeparator("Demo: Validation Rejects Invalid Commands");

    GameEngine demoEngine;
    CommandProcessor demoProcessor; // Console processor used purely for validation demo

    std::cout << "Current state: START\n\n";

    // Attempting commands that are illegal in START
    std::string badCmds[] = {"validatemap", "addplayer Alice", "gamestart", "replay", "quit"};
    for (const std::string &s : badCmds)
    {
        Command *c = new Command(s);
        bool valid = demoProcessor.validate(c, demoEngine.getState());
        std::cout << (valid ? "[VALID]   " : "[INVALID] ") << *c << "\n";
        delete c;
    }

    // Attempting a valid command in START
    Command *goodCmd = new Command("loadmap world.map");
    bool valid = demoProcessor.validate(goodCmd, demoEngine.getState());
    std::cout << (valid ? "[VALID]   " : "[INVALID] ") << *goodCmd << "\n";
    delete goodCmd;

    // ----- Demonstration: valid commands produce correct state transitions -----

    printSeparator("Demo: Valid Command Sequence — State Transitions");

    GameEngine seqEngine;
    CommandProcessor seqProcessor;

    // Build the sequence manually so the demo does not require any user input here
    std::string validSeq[] = {
        "loadmap europe.map",
        "loadmap africa.map", // loadmap → loadmap  (self-loop is valid)
        "validatemap",
        "addplayer Alice",
        "addplayer Bob",
        "gamestart"};

    for (const std::string &s : validSeq)
    {
        Command *c = new Command(s);
        bool ok = seqProcessor.validate(c, seqEngine.getState());
        if (ok)
        {
            applyCommand(c, seqEngine);
            std::cout << "[VALID]   " << *c << "\n";
        }
        else
        {
            std::cout << "[INVALID] " << *c << "\n";
        }
        // Print current state after each command
        std::cout << "          -> " << seqEngine << "\n";
        delete c;
    }

    // Main connsole-based or file-based loop

    if (useConsole)
    {
        printSeparator("Mode: Console Input (CommandProcessor)");
        std::cout << "Valid commands: loadmap <file>, validatemap, addplayer <name>,\n"
                  << "               gamestart, replay, quit\n\n";

        GameEngine engine;
        CommandProcessor processor;
        std::cout << engine << "\n\n";

        bool done = false;
        while (!done)
        {
            // getCommand() reads input, wraps it, saves it internally, returns it
            Command *cmd = processor.getCommand();

            // "exit" lets the user escape the demo loop at any point
            if (cmd->getCommand() == "exit")
            {
                std::cout << "Exiting demo.\n";
                break;
            }

            bool isValid = processor.validate(cmd, engine.getState());
            if (isValid)
            {
                done = applyCommand(cmd, engine);
                std::cout << "Effect: " << cmd->getEffect() << "\n";
                std::cout << engine << "\n\n";
            }
            else
            {
                std::cout << "Rejected: " << cmd->getEffect() << "\n\n";
            }
        }
    }
    else
    {
        // ---- File mode: FileCommandProcessorAdapter ----
        printSeparator("Mode: File Input (FileCommandProcessorAdapter)");
        std::cout << "Reading commands from: " << filename << "\n\n";

        GameEngine engine;
        FileCommandProcessorAdapter processor(filename);
        std::cout << engine << "\n\n";

        bool done = false;
        while (!done)
        {
            Command *cmd = processor.getCommand();

            // Empty string signals EOF — stop reading
            if (cmd->getCommand().empty())
            {
                std::cout << "[EOF reached — no more commands in file.]\n";
                delete cmd;
                break;
            }

            bool isValid = processor.validate(cmd, engine.getState());
            if (isValid)
            {
                done = applyCommand(cmd, engine);
                std::cout << "Effect: " << cmd->getEffect() << "\n";
                std::cout << engine << "\n\n";
            }
            else
            {
                std::cout << "Rejected: " << cmd->getEffect() << "\n\n";
            }
        }
    }
}

// --------------- main ---------------

int main(int argc, char *argv[])
{
    testCommandProcessing(argc, argv);
    return 0;
}
