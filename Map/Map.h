#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

//#include "Player.h"

class Continent;
class Territory;
class MapLoader;

class Map{
private:

    std::vector<Continent*>* continents;
    std::vector<Territory*>* territories;

    void addContinent(Continent* continent);
    void addTerritory(Territory* territory);
    bool isConnected() const;
    bool validateContinents() const;
public:
    // constructors & destructor
    Map();
    Map(const Map& other);
    ~Map();

    // operators
    Map& operator=(const Map& other);
    
    // methods
    bool validate() const;
    

    //friends
    friend std::ostream& operator<<(std::ostream& os,const Map& map);
    friend class MapLoader;
};

class Continent{
private:
    const int* num;
    const std::string* name;
    const int* bonus;

    std::vector<Territory*>* countries;

public:
    // constructors & destructor
    Continent();
    Continent(int num,std::string& string,int bonus);
    Continent(const Continent& other);
    ~Continent();

    // operators
    Continent& operator=(const Continent& other);

    // Methods
    bool isConnectedSubgraph() const;
    int getNum() const;
    std::string getName() const;
    std::vector<Territory*> getCountries() const;
    int getBonus() const;
    
    //friends
    friend std::ostream& operator<<(std::ostream& os,const Continent& continent);
    friend class MapLoader;

};

class Territory{
private:
    // constants
    const int* num;
    const std::string* name;
    const int* continent;
    const int* posX;
    const int* posY;

    // not set at construction
    // Player* owner; 
    std::vector<Territory*>* borders;
public:
    // constructors & destructor 
    Territory(int num, std::string &name, int continent, int posX, int posY);
    Territory(const Territory& other);
    ~Territory();

    // operators
    Territory& operator=(const Territory& other);

    // methods
    int getNum() const;
    std::string getName() const;
    std::vector<Territory*> getBorders() const;
    int getContinent() const;
    int getPosX() const;
    int getPosY() const;

    // friends
    friend std::ostream& operator<<(std::ostream& os,const Territory& territory);
    friend class MapLoader;
};

class MapLoader{
    private:
    public:
        // constructor & destructor
        MapLoader();
        MapLoader(const MapLoader& other);
        ~MapLoader();
    
        // operators
        MapLoader& operator=(const MapLoader& other);

        // methods
        void loadMap(Map& map, std::string path);
        void parseContinents(std::ifstream& mapFile , std::string& line, Map& map, int& lineNum);
        void parseTerritories(std::ifstream& mapFile, std::string& line, Map& map, int& lineNum);
        void parseBorders(std::ifstream& mapFile, std::string& line, Map& map, int& lineNum);
        void populateContinentTerritories(int continentNumber,Map& map, int territoryNumber);


        // friends
        friend std::ostream& operator<<(std::ostream& os, const MapLoader& mapLoader);

};
