#include "Map.h"

#include <queue>
#include <unordered_set>

using std::string;
using std::vector;
using std::queue;
using std::unordered_set;
using std::ostream;
using std::ifstream;
using std::istringstream;
using std::cerr;
using std::cout;
using std::getline;
using std::endl;
using std::to_string;
using std::exception;
using std::runtime_error;

// ==================== Map ====================

Map::Map(){
    continents = new vector<Continent*>();
    territories = new vector<Territory*>();
}

Map::Map(const Map& other){

    continents = new vector<Continent*>(*(other.continents));
    territories = new vector<Territory*>(*(other.territories));
}

Map::~Map(){


    for(Continent* c : *continents){
        delete c;
    }
    delete continents;

    for(Territory* t : *territories){
        delete t;
    }
    delete territories;
}

Map& Map::operator=(const Map& other){
    if(this != &other){
        if(this->continents->size() != 0){
            for(Continent* c : *continents){
                delete c;
            }
        }
        delete continents;
        
        if(this->continents->size() != 0){
            for(Territory* t : *territories){
                delete t;
            }
        }
        delete territories;

        continents = new vector<Continent*>(*(other.continents));
        territories = new vector<Territory*>(*(other.territories));
    }
    return *this;
}

bool Map::validate() const{
    if(isConnected() && validateContinents()){
        cout << "\033[32mThe Map is valid\033[0m" << endl;
        return true;
    }


    return false;
}

//Uses BFS to check connectivness
bool Map::isConnected() const {
    if (!territories || territories->empty()){
        cerr << "\033[31m" << "Error: Map is empty" << "\033[0m" << endl;
        return false;
    }

    int n = territories->size();
    std::vector<bool> visited(n, false);
    std::queue<Territory*> q;

    visited[0] = true;
    q.push(territories->at(0));
    int visitedCount = 1;

    while(!q.empty()){
        Territory* current = q.front();
        q.pop();

        for(Territory* neighboor : current->getBorders()){

            int index = neighboor->getNum()-1;

            if(!visited[index]){
                visited[index] = true;
                visitedCount++;
                q.push(neighboor);
            }
        }
    }

    if(visitedCount != n){
        cerr << "\033[31m" << "Error: Map is not a connected graph" << "\033[0m" << endl;
        return false;
    }

    return true;
}

bool Map::validateContinents() const {

    vector<bool> seen(territories->size(), false);

    for(Continent* c : *continents){
        if(!c->isConnectedSubgraph()){
            return false;
        }

        for(Territory* t : c->getCountries()){
            if(!seen[t->getNum()-1]){
                seen[t->getNum()-1] = true;
            }else{
                cerr << "\033[31m" << "Country " << t->getName() << " is in multiple continents" << "\033[0m" << endl;
                return false;
            }
        }
    }

    return true;

}

void Map::addContinent(Continent* continent){
    continents->push_back(continent);
}

void Map::addTerritory(Territory* territory){
    territories->push_back(territory);
}


ostream& operator<<(ostream& os,const Map& map){
    os  << "Continents : " << map.continents->size() << "\n";
    for(Continent* c : *(map.continents)){
        os << *c;
    }

    os << "\n";

    os << "Territories : " << map.territories->size() << "\n";
    for(Territory* t : *(map.territories)){
       os << *t;
    }

    return os;
}

// ==================== Continent ====================

Continent::Continent(){
    num = new int(0);
    name = new string("");
    bonus = new int(0);
    countries = new vector<Territory*>(); 
}

Continent::Continent(int continentNum, string& continentName, int continentBonus){
    num = new int(continentNum);
    name = new string(continentName);
    bonus = new int(continentBonus);
    countries = new vector<Territory*>();
}

Continent::Continent(const Continent& other){
    num = new int(*(other.num));
    name = new string(*(other.name));
    bonus = new int(*(other.bonus));
    countries = new vector<Territory*>(*(other.countries));
}

Continent::~Continent(){
    delete num;
    delete name;
    delete bonus;
    delete countries;
}

Continent& Continent::operator=(const Continent& other){
    if(this != &other){
        delete num;
        delete name;
        delete bonus;
        delete countries;

        num = new int(*(other.num));
        name = new string(*(other.name));
        bonus = new int(*(other.bonus));
        countries = new vector<Territory*>(*(other.countries));
    }
    return *this;
}

bool Continent::isConnectedSubgraph() const {
    if(!countries || countries->empty()){
       cerr << "\033[31m" << "Error : Continent "<< name << " is empty" << "\033[0m" << endl;
       return false;
    }

    unordered_set<int> inContinent;
    inContinent.reserve(countries->size()*2); //double the size to accelerate lookup

    for(Territory* t : *countries){
        inContinent.insert(t->getNum());
    }

    queue<Territory*> q;
    unordered_set<int> visited;
    visited.reserve(countries->size()*2); //double the size to accelerate lookup)

    q.push(countries->at(0));
    visited.insert(countries->at(0)->getNum());

    while(!q.empty()){
        Territory* current = q.front();
        q.pop();

        for(Territory* t : current->getBorders()){

            if(inContinent.find(t->getNum()) == inContinent.end()) continue;

            if(visited.insert(t->getNum()).second){
                q.push(t);
            }
        }
    }

    if(visited.size() != inContinent.size()){
        cerr << "\033[31m" << "Error: Continent " << *name << " is not a connected sub-graph" << "\033[0m" << endl;
        return false;
    }

    return true;
}

string Continent::getName() const {
    return *name;
}

int Continent::getNum() const {
    return *num;
}

vector<Territory*> Continent::getCountries() const {
    return *countries;
}

int Continent::getBonus() const {
    return *bonus;
}

ostream& operator<<(ostream& os,const Continent& continent){
    os << *continent.num << " " << *continent.name << ", bonus : " << *continent.bonus << ", countries : ";
    for(Territory* t : *(continent.countries)){
        os << t->getName() << " ";
    }
    os << "\n";
    return os;
}

// ==================== Territory ====================

Territory::Territory(int territoryNum, string& territoryName, int cntnt, int x, int y) {
    num = new int(territoryNum);
    name = new string(territoryName);
    continent = new int(cntnt);
    posX = new int(x);
    posY = new int(y);
    owner = nullptr;
    borders = new vector<Territory*>();
    owner = new string("");
}

Territory::Territory(const Territory& other){
    num = new int(*(other.num));
    name = new string(*(other.name));
    continent = new int(*(other.continent));
    posX = new int(*(other.posX));
    posY = new int(*(other.posY));
    owner = new string(*(other.owner));
    borders = new vector<Territory*>(*(other.borders));

}

Territory::~Territory(){
    delete num;
    delete name;
    delete continent;
    delete posX;
    delete posY;
    delete owner;
    delete borders;
}

Territory& Territory::operator=(const Territory& other){
    if(this != &other){
        delete num;
        delete name;
        delete continent;
        delete posX;
        delete posY;
        delete owner;
        delete borders;
        

        num = new int(*(other.num));
        name = new string(*(other.name));
        continent = new int(*(other.continent));
        posX = new int(*(other.posX));
        posY = new int(*(other.posY));

        borders = new vector<Territory*>(*(other.borders));
        owner = new string(*(other.owner));

    }
    return *this;
}

string Territory::getName() const {
    return *name;
}

int Territory::getNum() const {
    return *num;
}

vector<Territory*> Territory::getBorders() const{
    return *borders;
}

int Territory::getContinent() const {
    return *continent;
}

int Territory::getPosX() const {
    return *posX;
}

int Territory::getPosY() const {
    return *posY;
}

string Territory::getOwner() const {
    return *owner;
}

void Territory::setOwner(string owner_name) {
    delete owner;
    owner = new string(owner_name);

}

ostream& operator<<(ostream& os, const Territory& territory){
    os << *territory.num << " "
        << *territory.name 
        << ", continent : " << *territory.continent ;
    if(territory.owner){
        // waiting for Player class 
         os << ", owner : " << *territory.owner;
    }
    if(territory.borders){
        os << ", borders : ";
        for(Territory* t : *territory.borders){
            os << *(t->name) << " ";
        }
    }
    os << endl;

    return os;
}

// ==================== Map Loader ====================

MapLoader::MapLoader(){

}

MapLoader::MapLoader(const MapLoader& other){
}

MapLoader::~MapLoader(){

}

MapLoader& MapLoader::operator=(const MapLoader& other){
    return *this;
} 

void MapLoader::loadMap(Map& map,const string path){

    ifstream mapFile(path);
    string line;
    int lineNum = 0;

    // ----------Continents Section-------------

    while(getline(mapFile, line)){
        lineNum++;
        if(line == "[continents]\r" || line == "[continents]") break;
    }
    if(line != "[continents]\r" && line != "[continents]"){ 
        mapFile.close();
        throw runtime_error("Error: No continents section in the File\n");
    }

   cout << "Starting to parse Continents" << endl ;
   try{
       parseContinents(mapFile, line, map, lineNum);
    } catch (const exception& e) {
        mapFile.close();
        throw;
    }

    // ----------Countries Section-------------

    while(getline(mapFile, line)){ 
        lineNum++;
        if(line == "[countries]\r" || line == "[countries]") break;
    }
    if(line != "[countries]\r" && line != "[countries]"){ 
        mapFile.close();
        throw runtime_error("Error: No countries section in the File\n");
    }

   cout << "Starting to parse Territories" << endl;

   try{
       parseTerritories(mapFile, line, map, lineNum);
    } catch (const exception& e) {
        mapFile.close();
        throw;
    }

    // ----------Borders Section-------------

    while(getline(mapFile, line)){ 
        lineNum++;
        if(line == "[borders]\r" || (line == "[borders]")) break;
    }
    if(line != "[borders]\r" && line != "[borders]"){ 
        mapFile.close();
        throw runtime_error("Error: No borders section in the File\n");
    }

    cout << "Starting to parse Borders" << endl;

    try{
        parseBorders(mapFile, line, map, lineNum);
    }catch (const exception& e){
        mapFile.close();
        throw;
    }

   mapFile.close();

}

// Parses the continents section and populates the continents vector of the map
void MapLoader::parseContinents(ifstream& mapFile, string& line, Map& map, int& lineNum){

    string name, color;
    int bonus, num = 1;

    while(getline(mapFile, line)){
        lineNum++;
        if(line.empty() || line == "\r") break;
        
        if(line.back() == '\r') line.pop_back();

        istringstream iss(line);

        if(!(iss >> name >> bonus >> color)){
            throw runtime_error("Error: Malformed Continent entry : \""+line+"\" at line "+to_string(lineNum)+"\n");
        }

        map.addContinent(new Continent(num++, name, bonus));
    } 

    cout << "\033[32mContinents successfully loaded\033[0m" << endl;

}

// Parses the territories section, populates the territories vector of the map and continents
void MapLoader::parseTerritories(ifstream& mapFile, string& line, Map& map, int& lineNum){
    string name;
    int num, continent, posX, posY;

    while(getline(mapFile, line)){
        lineNum++;
        if(line.empty() || line == "\r") break;

        if(line.back() == '\r') line.pop_back();

        istringstream iss(line);

        if(!(iss >> num >> name >> continent >> posX >> posY)){
            throw runtime_error("Error: Malformed Territory entry or entries :\""+line+"\" at line "+to_string(lineNum)+"\n");
        }

        if(continent-1 < 0 || continent > (int)map.continents->size()){
            throw runtime_error("Error: Continent index "+to_string(continent)+"out of bounds : \""+line+"\" at line "+to_string(lineNum)+"\n");
        }

        map.addTerritory(new Territory(num,name,continent,posX,posY));

        // now that we are creating territories we can start populating the continents
        populateContinentTerritories(continent, map, num);
        
    }

    cout << "\033[32mTerritories successfully loaded\033[0m" << endl;

}

// Parses the borders section and populates the territories border vector
void MapLoader::parseBorders(ifstream& mapFile, string& line, Map& map, int& lineNum){
    int territoryNumber, territory;
    
    while(getline(mapFile, line)){
        lineNum++;
        if(line.empty() || line == "\r") break;

        if(line.back() == '\r') line.pop_back();
        
        istringstream iss(line);

        if(!(iss >> territoryNumber)){
            throw runtime_error("Error: Malformed borders :\""+line+"\" at line "+to_string(lineNum)+"\n");
        }else if(territoryNumber-1 < 0 || territoryNumber > (int)map.territories->size()){
            throw runtime_error("Error: Territory index "+to_string(territoryNumber)+" out of bounds : \""+line+"\" at line "+to_string(lineNum)+"\n");
        }
        
        while(iss >> territory){
            if(territory == territoryNumber){
                throw runtime_error("Error: Country bordering itself \""+line+"\" at line "+to_string(lineNum)+"\n");
            }
            if(territory-1 < 0 || territory > (int)map.territories->size()){
                throw runtime_error("Error: Border territory index "+to_string(territory)+" out of bounds : \""+line+"\" at line "+to_string(lineNum)+"\n");
            }

            map.territories->at(territoryNumber-1)->borders->push_back(map.territories->at(territory-1));
        }

        if (iss.fail() && !iss.eof()) {
            throw std::runtime_error("Error: None integer token \""+ line+"\" at line "+to_string(lineNum)+"\n");
        } 
        
        if (map.territories->at(territoryNumber-1)->borders->empty()) {
            throw std::runtime_error("Error: Malformed borders line (no borders) \" "+line+" \" at line "+to_string(lineNum)+"\n");
        }
    }

    cout << "\033[32mBorders successfully loaded\033[0m" << endl;
}

void MapLoader::populateContinentTerritories(int continentNumber, Map& map,int territoryNumber){
    map.continents->at(continentNumber-1)->countries->push_back(map.territories->at(territoryNumber-1));

}

ostream& operator<<(ostream& os, const MapLoader& mapLoader){
    os << "MapLoader" << endl;
    return os;
}