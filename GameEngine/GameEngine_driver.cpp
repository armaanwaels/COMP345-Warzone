#include "GameEngine.h"
#include <iostream>
#include <string>

// This driver creates a GameEngine object on the heap and prompts users to interact with it
// A game (while) loop is instantiated.  When processCommand returns true, the loop is
// broken and the game terminates.
int main()
{
    // Instantiate the engine on the heap
    GameEngine *gameEngine = new GameEngine(); //(+1 "new" will require +1 "delete" below )

    std::string userInput;

    // Introduce to user
    std::cout << "\n ---WARZONE GAME ENGINE TESTER--- " << std::endl;

    std::cout << "\n"
              << *gameEngine << std::endl;

    // Begin game loop
    while (true)
    {
        std::cout << "\nEnter command: ";
        std::getline(std::cin, userInput);

        // If processCommand returns true, loop break loop
        if (gameEngine->processCommand(&userInput))
        {
            break;
        }
        // Prints current state to console
        std::cout << *gameEngine << std::endl;
    }

    // Cleanup memory
    delete gameEngine; // (the +1 "delete" mentioned above)
    gameEngine = nullptr;

    // Exit Program
    return 0;
}
