// MapDriver.cpp
// Driver for Map, Continent, Territory and MapLoader classes
// Ulysse Etienne - 40238669
// Comp 345 - Assignment 1, Part 1

#include <iostream>

#include "Map.h"

using std::cerr;
using std::cout;
using std::endl;
using std::exception;

void testMapLoad(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/bigeurope.map");
    } catch(const exception& e){
        throw;
    }

    cout << map;
    map.validate();

}

int main(){
    
    try{
        testMapLoad();
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m" <<endl;
        return EXIT_FAILURE;
    }

    return 0;
}