#include "CommandProcessing.h"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

// --------------- Command ---------------

// Constructor: Allocates command string on the heap; effect starts empty.
Command::Command(const std::string &cmd)
    : command(new std::string(cmd)), effect(new std::string("")) {}

// Destructor: Releases both heap-allocated strings.
Command::~Command()
{
    delete command;
    delete effect;
}

// Copy Constructor: Deep copies both string members independently.
Command::Command(const Command &other)
    : Subject(other),
      command(new std::string(*other.command)),
      effect(new std::string(*other.effect)) {}

// Overloaded Assignment Operator: Deep copy with self-assignment guard.
Command &Command::operator=(const Command &other)
{
    if (this == &other)
        return *this;
    Subject::operator=(other);
    delete command;
    delete effect;
    command = new std::string(*other.command);
    effect = new std::string(*other.effect);
    return *this;
}

// getCommand: Returns the raw command string stored in this object.
std::string Command::getCommand() const
{
    return *command;
}

// getEffect: Returns the effect string (empty until saveEffect is called).
std::string Command::getEffect() const
{
    return *effect;
}

// saveEffect: Records the outcome of executing command.
// Called by the game engine after the command is processed,
// or by validate() when the command is rejected,
// so that the Command object keeps a log of what happened.
void Command::saveEffect(const std::string &eff)
{
    *effect = eff;
    notify(this);
}

std::string Command::stringToLog() const
{
    return "Command's effect: " + *effect;
}

// Stream Insertion Operator: Shows command string and its effect side-by-side.
std::ostream &operator<<(std::ostream &os, const Command &cmd)
{
    os << "Command: [" << *cmd.command << "]";
    if (!cmd.effect->empty())
    {
        os << " | Effect: [" << *cmd.effect << "]";
    }
    return os;
}

// --------------- CommandProcessor ---------------

// Default Constructor: Allocates an empty command list on the heap.
CommandProcessor::CommandProcessor() : commands(new std::list<Command *>()) {}

// Destructor: Frees every Command object in the list, then the list itself.
// It deletes elements before deleting the container.
CommandProcessor::~CommandProcessor()
{
    for (Command *cmd : *commands)
        delete cmd;
    delete commands;
}

// Copy Constructor: Deep copies the list, allocating a new Command for each entry.
CommandProcessor::CommandProcessor(const CommandProcessor &other)
    : Subject(other), commands(new std::list<Command *>())
{
    for (Command *cmd : *other.commands)
        commands->push_back(new Command(*cmd)); // push_back appends pointer to the end of "commands"
}

// Overloaded Assignment Operator: Deep copy with self-assignment guard.
// Frees existing commands, create new list, loop through other.commands and append deep copy to Command
CommandProcessor &CommandProcessor::operator=(const CommandProcessor &other)
{
    if (this == &other)
        return *this;

    Subject::operator=(other);

    // Free existing list contents
    for (Command *cmd : *commands)
        delete cmd;
    delete commands;

    // Rebuild from other
    commands = new std::list<Command *>();
    for (Command *cmd : *other.commands)
        commands->push_back(new Command(*cmd));

    return *this;
}

// readCommand: Reads a single line from standard input and returns it as a string.
// Protected visibility: the file adapter overrides this method to read from a
// file instead, leaving all other CommandProcessor logic untouched.
std::string CommandProcessor::readCommand()
{
    std::string input;
    std::cout << "Enter command: ";
    std::getline(std::cin, input);
    return input;
}

// saveCommand: appends command and notifies observers
void CommandProcessor::saveCommand(Command *cmd)
{
    commands->push_back(cmd);
    notify(this);
}

std::string CommandProcessor::stringToLog() const
{
    if (!commands->empty())
        return "Command: " + commands->back()->getCommand();
    return "Command: (empty)";
}

// getCommand: Calls readCommand() to obtain a string, wraps it in a Command object,
// then saves it to the internal list, and returns it for validation/processing.
Command *CommandProcessor::getCommand()
{
    std::string input = readCommand();
    Command *cmd = new Command(input);
    saveCommand(cmd);
    return cmd;
}

// --------------- validate ---------------

// validate: Determines whether a command is legal.
// Returns true and leaves the effect blank when valid.
// Returns false and writes an error into the command's effect when invalid.
//
// Commands with arguments (loadmap, addplayer) are matched by their keyword
// prefix; a missing argument is treated as an unknown command.
bool CommandProcessor::validate(Command *cmd, State *currentState)
{
    // Guard: both pointers must be valid
    if (cmd == nullptr || currentState == nullptr)
        return false;

    std::string cmdStr = cmd->getCommand();

    // Extract the first token (keyword) to handle argument-bearing commands uniformly
    std::istringstream iss(cmdStr); // iss == inputStringStream
    std::string keyword;
    iss >> keyword; // extracts first word

    //----------- Logic for switching between states -----------

    // ----- "loadmap <mapfile>": valid in START and MAP_LOADED -----

    if (keyword == "loadmap")
    {
        std::string arg;
        if (!(iss >> arg)) // Missing filename argument
        {
            cmd->saveEffect("ERROR: loadmap requires a filename argument.  Usage: loadmap <mapfile>");
            return false;
        }
        if (*currentState == State::START || *currentState == State::MAP_LOADED)
        {
            return true;
        }
        cmd->saveEffect("ERROR: loadmap is not valid in the current state.");
        return false;
    }

    // ----- "validatemap": valid in MAP_LOADED -----

    if (keyword == "validatemap")
    {
        if (*currentState == State::MAP_LOADED)
        {
            return true;
        }
        cmd->saveEffect("ERROR: validatemap is not valid in the crrent state.");
        return false;
    }

    // ----- "addplayer <playername>": valid in MAP_VALIDATED and PLAYERS_ADDED -----

    if (keyword == "addplayer")
    {
        std::string arg;
        if (!(iss >> arg)) // Missing player name argument
        {
            cmd->saveEffect("ERROR: addplayer requires a player name.  Usage: addplayer <playername>");
            return false;
        }
        if (*currentState == State::MAP_VALIDATED || *currentState == State::PLAYERS_ADDED)
        {
            return true;
        }
        cmd->saveEffect("ERROR: addplayer is not valid in the current state.");
        return false;
    }

    // ----- "gamestart": valid in PLAYERS_ADDED -----

    if (keyword == "gamestart")
    {
        if (*currentState == State::PLAYERS_ADDED)
        {
            return true;
        }
        cmd->saveEffect("ERROR: gamestart is not valid in the current state.");
        return false;
    }

    // ----- "replay": valid in WIN — restarts a new game -----

    if (keyword == "replay")
    {
        if (*currentState == State::WIN)
        {
            return true;
        }
        cmd->saveEffect("ERROR: replay is only valid after winning the game.");
        return false;
    }

    // ----- "quit": valid in WIN — exits the program -----

    if (keyword == "quit")
    {
        if (*currentState == State::WIN)
        {
            return true;
        }
        cmd->saveEffect("ERROR: quit is only valid after winning the game.");
        return false;
    }

    // ----- "tournament ...": valid in START, full grammar check happens in parseTournamentCommand -----

    if (keyword == "tournament")
    {
        if (*currentState != State::START)
        {
            cmd->saveEffect("ERROR: tournament is only valid in the START state.");
            return false;
        }
        // parseTournamentCommand will write the error effect on failure
        TournamentConfig *cfg = parseTournamentCommand(cmd);
        if (cfg == nullptr)
            return false;
        delete cfg; // driver will re-parse when it actually wants to run the tournament
        return true;
    }

    // Unknown command: not in the recognised set

    cmd->saveEffect("ERROR: Unknown command \"" + cmdStr + "\".  Valid commands: loadmap, validatemap, addplayer, gamestart, replay, quit, tournament.");
    return false;
}

// Stream Insertion Operator: Prints a summary of all stored commands.

std::ostream &operator<<(std::ostream &os, const CommandProcessor &cp)
{
    os << "CommandProcessor [" << cp.commands->size() << " command(s) stored]:\n";
    int i = 1;
    for (Command *cmd : *cp.commands)
        os << "  " << i++ << ". " << *cmd << "\n";
    return os;
}

// --------------- FileLineReader ---------------

// Constructor: Opens the named file.  Prints a diagnostic if it cannot be opened,
// so callers can detect the failure without the program crashing silently.
FileLineReader::FileLineReader(const std::string &fname)
    : filename(new std::string(fname)),
      file(new std::ifstream(fname))
{
    if (!file->is_open())
        std::cerr << "FileLineReader Error: could not open \"" << fname << "\"." << std::endl;
}

// Destructor: Closes the stream before freeing it to avoid resource leaks.
FileLineReader::~FileLineReader()
{
    if (file->is_open())
        file->close();
    delete file;
    delete filename;
}

// Copy Constructor: Re-opens the same file from scratch.
// A file stream cannot be copied in the usual way.
// We have to open a new stream pointing at the same path.

FileLineReader::FileLineReader(const FileLineReader &other)
    : filename(new std::string(*other.filename)),
      file(new std::ifstream(*other.filename)) {}

// Overloaded Assignment Operator: Same logic as copy constructor.
FileLineReader &FileLineReader::operator=(const FileLineReader &other)
{
    if (this == &other)
        return *this;
    if (file->is_open())
        file->close();
    delete file;
    delete filename;
    filename = new std::string(*other.filename);
    file = new std::ifstream(*filename);
    return *this;
}

// readLineFromFile: Returns the next non-empty line from the file.
// Returns an empty string once EOF is reached or if the file is not open,
// which the adapter uses to know to stop processing.
std::string FileLineReader::readLineFromFile()
{
    std::string line;
    if (file->is_open() && std::getline(*file, line))
        return line;
    return "";
}

// hasMoreLines: Peeks one character ahead to test whether data remains.
// Used by the adapter's read loop to avoid requesting past EOF.
bool FileLineReader::hasMoreLines() const
{
    return file->is_open() && file->peek() != EOF;
}

// Stream Insertion Operator: Identifies the source file and its open status.
std::ostream &operator<<(std::ostream &os, const FileLineReader &flr) // flr == the FileLineReader instance the adapter wraps
{
    os << "FileLineReader [file: \"" << *flr.filename
       << "\", open: " << (flr.file->is_open() ? "yes" : "no") << "]";
    return os;
}

// ------------- FileCommandProcessorAdapter -------------

// Constructor: Initialises the base CommandProcessor, then creates a
// FileLineReader pointed at the given file — the only adapter-specific setup.
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const std::string &filename)
    : CommandProcessor(), flr(new FileLineReader(filename)) {}

// Destructor: CommandProcessor base handles its own list; we only free the FileLineReader.
FileCommandProcessorAdapter::~FileCommandProcessorAdapter()
{
    delete flr;
}

// Copy Constructor: Deep copies the adaptee (FileLineReader).
// Base copy constructor handles the command list.
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other)
    : CommandProcessor(other), flr(new FileLineReader(*other.flr)) {}

// Overloaded Assignment Operator: Deep copy with self-assignment guard.
FileCommandProcessorAdapter &FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter &other)
{
    if (this == &other)
        return *this;
    CommandProcessor::operator=(other); // Delegate list copy to base
    delete flr;
    flr = new FileLineReader(*other.flr);
    return *this;
}

// readCommand: Overrides CommandProcessor::readCommand().
// Delegates to FileLineReader instead of reading from cin, which is the
// entire purpose of the Adapter pattern here.  We echo the line so
// we can observe file-sourced commands in the console output.
std::string FileCommandProcessorAdapter::readCommand()
{
    std::string line = flr->readLineFromFile();
    std::cout << "[File] " << line << std::endl;
    return line;
}

// Stream Insertion Operator: Identifies adapter and its reader.
std::ostream &operator<<(std::ostream &os, const FileCommandProcessorAdapter &fcp)
{
    os << "FileCommandProcessorAdapter -> " << *fcp.flr;
    return os;
}

// --------------- TournamentConfig ---------------

// Default ctor: starts with empty vectors and zeroed ints.
TournamentConfig::TournamentConfig()
    : mapFiles(new std::vector<std::string>()),
      playerStrategies(new std::vector<std::string>()),
      numberOfGames(new int(0)),
      maxNumberOfTurns(new int(0)) {}

// Parameterized constructor used by parseTournamentCommand once all
// values have been collected and validated.
TournamentConfig::TournamentConfig(const std::vector<std::string> &maps,
                                   const std::vector<std::string> &strategies,
                                   int games,
                                   int maxTurns)
{
    mapFiles         = new std::vector<std::string>(maps);
    playerStrategies = new std::vector<std::string>(strategies);
    numberOfGames    = new int(games);
    maxNumberOfTurns = new int(maxTurns);
}

// Copy Constructor: deep copy every pointer member.
TournamentConfig::TournamentConfig(const TournamentConfig &other)
    : mapFiles(new std::vector<std::string>(*other.mapFiles)),
      playerStrategies(new std::vector<std::string>(*other.playerStrategies)),
      numberOfGames(new int(*other.numberOfGames)),
      maxNumberOfTurns(new int(*other.maxNumberOfTurns)) {}

// Destructor
TournamentConfig::~TournamentConfig()
{
    delete mapFiles;
    delete playerStrategies;
    delete numberOfGames;
    delete maxNumberOfTurns;
}

// Assignment Operator (deep copy, self-assignment guarded).
TournamentConfig &TournamentConfig::operator=(const TournamentConfig &other)
{
    if (this == &other)
        return *this;

    // free existing storage before allocating the new copies
    delete mapFiles;
    delete playerStrategies;
    delete numberOfGames;
    delete maxNumberOfTurns;

    mapFiles         = new std::vector<std::string>(*other.mapFiles);
    playerStrategies = new std::vector<std::string>(*other.playerStrategies);
    numberOfGames    = new int(*other.numberOfGames);
    maxNumberOfTurns = new int(*other.maxNumberOfTurns);
    return *this;
}

// ----- accessors -----
const std::vector<std::string> &TournamentConfig::getMapFiles() const { return *mapFiles; }
const std::vector<std::string> &TournamentConfig::getPlayerStrategies() const { return *playerStrategies; }

int TournamentConfig::getNumberOfGames() const
{
    return *numberOfGames;
}

int TournamentConfig::getMaxNumberOfTurns() const
{
    return *maxNumberOfTurns;
}

// Stream Insertion Operator: used for logging and for the banner that
// playTournament prints before running any games.
std::ostream &operator<<(std::ostream &os, const TournamentConfig &cfg)
{
    os << "Tournament Configuration:\n";

    os << "  M = ";
    for (size_t i = 0; i < cfg.mapFiles->size(); ++i)
    {
        os << (*cfg.mapFiles)[i];
        if (i + 1 < cfg.mapFiles->size()) os << ", ";
    }

    os << "\n  P = ";
    for (size_t i = 0; i < cfg.playerStrategies->size(); ++i)
    {
        os << (*cfg.playerStrategies)[i];
        if (i + 1 < cfg.playerStrategies->size()) os << ", ";
    }

    os << "\n  G = " << *cfg.numberOfGames;
    os << "\n  D = " << *cfg.maxNumberOfTurns;
    return os;
}

// --------------- parseTournamentCommand ---------------

// Lower-cases a string copy so strategy name matching is case-insensitive.
static std::string toLowerCopy(const std::string &s)
{
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++)
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    return out;
}

// parseTournamentCommand
// ----------------------
// Parses a full "tournament -M ... -P ... -G <n> -D <n>" command.  Flags
// can appear in any order.  Everything that follows -M / -P is collected
// into the corresponding list until we hit the next flag or end-of-line.
//
// Validation rules (per assignment spec):
//   1 <= M <= 5
//   2 <= P <= 4
//   1 <= G <= 5
//   10 <= D <= 50
//   strategies must be aggressive|benevolent|neutral|cheater (human rejected)
//
// Returns a newly allocated TournamentConfig on success, or nullptr with
// cmd->effect set to a human-readable error message on failure.
TournamentConfig *CommandProcessor::parseTournamentCommand(Command *cmd)
{
    if (cmd == nullptr)
        return nullptr;

    std::istringstream iss(cmd->getCommand());
    std::string token;
    iss >> token;
    if (token != "tournament")
    {
        cmd->saveEffect("ERROR: tournament command must start with 'tournament'.");
        return nullptr;
    }

    std::vector<std::string> maps;
    std::vector<std::string> strategies;
    int numGames = -1;
    int maxTurns = -1;

    // duplicate-flag detection
    bool seenM = false, seenP = false, seenG = false, seenD = false;

    std::string current;
    if (!(iss >> current))
    {
        cmd->saveEffect("ERROR: tournament command is missing all flags (-M, -P, -G, -D).");
        return nullptr;
    }

    // walk tokens: each flag eats following non-flag tokens until the next flag
    while (!current.empty())
    {
        if (current == "-M")
        {
            if (seenM) { cmd->saveEffect("ERROR: -M flag specified more than once."); return nullptr; }
            seenM = true;

            std::string next;
            while (iss >> next && !next.empty() && next[0] != '-')
                maps.push_back(next);
            current = next; // next flag token, or empty at EOF
        }
        else if (current == "-P")
        {
            if (seenP) { cmd->saveEffect("ERROR: -P flag specified more than once."); return nullptr; }
            seenP = true;

            std::string next;
            while (iss >> next && !next.empty() && next[0] != '-')
                strategies.push_back(next);
            current = next;
        }
        else if (current == "-G")
        {
            if (seenG) { cmd->saveEffect("ERROR: -G flag specified more than once."); return nullptr; }
            seenG = true;

            std::string next;
            if (!(iss >> next))
            {
                cmd->saveEffect("ERROR: -G flag is missing its integer value.");
                return nullptr;
            }
            try {
                numGames = std::stoi(next);
            } catch (const std::exception &) {
                cmd->saveEffect("ERROR: -G value '" + next + "' is not an integer.");
                return nullptr;
            }
            if (!(iss >> current)) current.clear();
        }
        else if (current == "-D")
        {
            if (seenD) { cmd->saveEffect("ERROR: -D flag specified more than once."); return nullptr; }
            seenD = true;

            std::string next;
            if (!(iss >> next))
            {
                cmd->saveEffect("ERROR: -D flag is missing its integer value.");
                return nullptr;
            }
            try {
                maxTurns = std::stoi(next);
            } catch (const std::exception &) {
                cmd->saveEffect("ERROR: -D value '" + next + "' is not an integer.");
                return nullptr;
            }
            if (!(iss >> current)) current.clear();
        }
        else
        {
            cmd->saveEffect("ERROR: unexpected token '" + current + "' in tournament command.");
            return nullptr;
        }
    }

    // presence checks
    if (!seenM || !seenP || !seenG || !seenD)
    {
        cmd->saveEffect("ERROR: tournament command requires all four flags (-M, -P, -G, -D).");
        return nullptr;
    }

    // range checks per assignment spec
    if (maps.size() < 1 || maps.size() > 5)
    {
        cmd->saveEffect("ERROR: -M expects between 1 and 5 map files (got "
                        + std::to_string(maps.size()) + ").");
        return nullptr;
    }
    if (strategies.size() < 2 || strategies.size() > 4)
    {
        cmd->saveEffect("ERROR: -P expects between 2 and 4 player strategies (got "
                        + std::to_string(strategies.size()) + ").");
        return nullptr;
    }
    if (numGames < 1 || numGames > 5)
    {
        cmd->saveEffect("ERROR: -G expects a value between 1 and 5 (got "
                        + std::to_string(numGames) + ").");
        return nullptr;
    }
    if (maxTurns < 10 || maxTurns > 50)
    {
        cmd->saveEffect("ERROR: -D expects a value between 10 and 50 (got "
                        + std::to_string(maxTurns) + ").");
        return nullptr;
    }

    // strategy keyword check — tournaments are computer-only (no human)
    for (size_t i = 0; i < strategies.size(); ++i)
    {
        std::string s = toLowerCopy(strategies[i]);
        if (s == "human")
        {
            cmd->saveEffect("ERROR: 'human' strategy is not allowed in a tournament.");
            return nullptr;
        }
        if (s != "aggressive" && s != "benevolent" && s != "neutral" && s != "cheater")
        {
            cmd->saveEffect("ERROR: unknown player strategy '" + strategies[i]
                            + "'. Allowed: aggressive, benevolent, neutral, cheater.");
            return nullptr;
        }
        strategies[i] = s; // normalise to lowercase for the engine
    }

    // all good — stash a summary in the effect so the log observer records it
    std::ostringstream okMsg;
    okMsg << "Tournament command parsed OK. Maps=" << maps.size()
          << ", Strategies=" << strategies.size()
          << ", Games=" << numGames
          << ", MaxTurns=" << maxTurns << ".";
    cmd->saveEffect(okMsg.str());

    return new TournamentConfig(maps, strategies, numGames, maxTurns);
}
