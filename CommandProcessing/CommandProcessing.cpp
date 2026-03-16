#include "CommandProcessing.h"
#include <iostream>
#include <sstream>
#include <string>

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

    // Unknown command: not in the recognised set

    cmd->saveEffect("ERROR: Unknown command \"" + cmdStr + "\".  Valid commands: loadmap, validatemap, addplayer, gamestart, replay, quit.");
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
