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

void testValidMap(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/kosova.map");
        cout << map;
        map.validate();
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
    
}

void testInvalidContinent(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/invalidContinent1.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
}

void testMissingContinent(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/invalidContinent2.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
}

void testInvalidCountry(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/invalidCountry1.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
}

void testMissingCountry(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/invalidCountry2.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
}

void testInvalidBorder(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/invalidBorder1.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
}

void testMissingBorder(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/invalidBorder2.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }
}

void testUnconnnectedMap(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/unconnected.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }

    map.validate();
}

void testUnconnectedContinentMap(){
    MapLoader ml;
    Map map;

    try{
        ml.loadMap(map, "./Map_Files/unconnnectedContinent.map");
    } catch(const exception& e){
        cerr << "\033[31m" << e.what() << "\033[0m";
    }

    map.validate();
}

int main(){

    cout << "\033[34mMap file with invalid continent : \033[0m" << endl;
    testInvalidContinent();
    cout << "\033[34mMap file with missing continent section :\033[0m" << endl;
    testMissingContinent();
    cout << "\033[34mMap file with invalid country :\033[0m" << endl ;
    testInvalidCountry();
    cout << "\033[34mMap file with missing country :\033[0m" << endl;
    testMissingCountry();
    cout << "\033[34mMap file with invalid border :\033[0m" << endl;
    testInvalidBorder();
    cout << "\033[34mMap file with missing borders :\033[0m" << endl;
    testMissingBorder();
    cout << "\033[34mMap file with unconnected map graph :\033[0m" << endl;
    testUnconnnectedMap();
    cout << "\033[34mMap file with unconnected continent sub-graph :\033[0m" << endl;
    testUnconnectedContinentMap();
    cout << "\033[34mValid Map File :\033[0m" << endl;
    testValidMap();

    return 0;
}

