#include <iostream>
#include "CommandProcessing/CommandProcessing.h"
#include "GameEngine.h"

void testStartupPhase(int argc, char* argv[])
{
    CommandProcessor* cp = nullptr;
    
    // Check if the input is from the console or a file
    if (argc >= 2 && std::string(argv[1]) == "-console")
    {
        cp = new CommandProcessor();
        std::cout << "Using console input." << std::endl;
    }
    else if (argc >= 3 && std::string(argv[1]) == "-file")
    {
        cp = new FileCommandProcessorAdapter(argv[2]);
        std::cout << "Using file input: " << argv[2] << std::endl;
    }
    else
    {
        std::cout << "Usage:\n";
        std::cout << argv[0] << " -console\n";
        std::cout << argv[0] << " -file <commandfile>\n";
        return;
    }

    GameEngine* game = new GameEngine(cp);

    std::cout << "\n--- Starting startupPhase() test ---\n" << std::endl;
    game->startupPhase();

    delete game;
}


int main(int argc, char *argv[]){

    testStartupPhase(argc, argv);
    return 0;
}