#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include "../GameEngine/GameEngine.h"

// Forward declarations
class FileLineReader;

// ---------- Command ----------

// Command: Encapsulates a single user-issued game command and the effect
// produced by its execution.  Stores both as heap-allocated strings so that
// the class satisfies the pointer-member requirement and manages its own memory.
class Command
{
private:
    std::string *command; // Raw command string  (e.g. "loadmap world.map")
    std::string *effect;  // Effect string set after the command is processed

public:
    // Generic constructor: creates a Command from a given string
    Command(const std::string &cmd);

    // Destructor
    ~Command();

    // Copy Constructor
    Command(const Command &other);

    // Overloaded Assignment Operator
    Command &operator=(const Command &other);

    //------ Accessors ------
    std::string getCommand() const; // Returns raw command string
    std::string getEffect() const;  // Returns effect string

    // saveEffect: Records the outcome of this command after execution.
    // Called by the game engine (or validate) to annotate the command object.
    void saveEffect(const std::string &eff);

    // Overloaded Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &os, const Command &cmd);
};

// ---------- CommandProcessor ----------

// CommandProcessor: Reads commands from the console, stores them in an
// internal list of Command objects, validates them against the current game
// state, and exposes them to other components (e.g. GameEngine) via getCommand().
//
// readCommand() and saveCommand() are private/protected by design:
//   - readCommand() is overridden by the file adapter subclass.
//   - saveCommand() is an internal bookkeeping detail.
class CommandProcessor
{
private:
    std::list<Command *> *commands; // Heap-allocated collection of all commands seen so far

    // saveCommand: Appends a newly created Command to the internal list.
    // Called only by getCommand(); not exposed publicly.
    void saveCommand(Command *cmd);

protected:
    // readCommand: Reads a single command string from standard input.
    // Protected so the file adapter can override it without exposing raw I/O.
    virtual std::string readCommand();

public:
    // Generic constructor
    CommandProcessor();

    // Destructor: frees every Command in the list, then the list itself
    ~CommandProcessor();

    // Copy Constructor
    CommandProcessor(const CommandProcessor &other);

    // Overloaded Assignment Operator
    CommandProcessor &operator=(const CommandProcessor &other);

    // getCommand: Runs full read-save cycle.
    // Calls readCommand() → wraps result in a Command → calls saveCommand() → returns it.
    Command *getCommand();

    // validate: Checks whether a command is legal in the given game state.
    // Writes an error string into the command's effect field if it is not.
    // Returns true if valid, false otherwise.
    bool validate(Command *cmd, State *currentState);

    // Overloaded Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &os, const CommandProcessor &cp);
};

// ---------- FileLineReader ----------

// FileLineReader: Reads lines sequentially from a text file.
// Acts as the "Adaptee" in the Adapter pattern: it knows how to read a file
// but knows nothing about game commands or state machines.
class FileLineReader
{
private:
    std::string *filename; // Path to the source file
    std::ifstream *file;   // Heap-allocated file stream (pointer satisfies member rule)

public:
    // Constructor: opens the named file for reading
    FileLineReader(const std::string &fname);

    // Destructor: closes and frees the stream
    ~FileLineReader();

    // Copy Constructor: re-opens the same file (stream state cannot be copied)
    FileLineReader(const FileLineReader &other);

    // Overloaded Assignment Operator
    FileLineReader &operator=(const FileLineReader &other);

    // readLineFromFile: Returns the next line in the file.
    // Returns an empty string when EOF is reached or the file is not open.
    std::string readLineFromFile();

    // hasMoreLines: Returns true while there is still data left to read.
    bool hasMoreLines() const;

    // Overloaded Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &os, const FileLineReader &flr);
};

// ---------- FileCommandProcessorAdapter ----------

// FileCommandProcessorAdapter: Adapts CommandProcessor to get commands from
// from a text file rather than the console.  Follows the Adapter design
// pattern strictly: it inherits the Target interface (CommandProcessor) and
// holds an instance of the Adaptee (FileLineReader) as a private member.
// Only readCommand() is overridden; all other CommandProcessor behaviour
// (validation, bookkeeping) is inherited unchanged.
class FileCommandProcessorAdapter : public CommandProcessor
{
private:
    FileLineReader *flr; // Adaptee: the object that actually touches the file

protected:
    // readCommand: Overrides the console read with a file read via FileLineReader,
    // fulfilling the Adapter pattern contract without changing any other logic.
    std::string readCommand() override;

public:
    // Constructor: initialises with the path to the command file
    FileCommandProcessorAdapter(const std::string &filename);

    // Destructor: frees the FileLineReader
    ~FileCommandProcessorAdapter();

    // Copy Constructor
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other);

    // Overloaded Assignment Operator
    FileCommandProcessorAdapter &operator=(const FileCommandProcessorAdapter &other);

    // Overloaded Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &os, const FileCommandProcessorAdapter &fcp);
};

#endif
