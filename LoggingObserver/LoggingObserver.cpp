#include "LoggingObserver.h"
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostream;
using std::ofstream;

// ===== ILoggable =====

ILoggable::~ILoggable() {
}

// ===== Observer =====

Observer::~Observer() {
}

// ===== Subject =====

Subject::Subject() {
    observers = new vector<Observer*>();
}

// shallow copy (we don't own observers)
Subject::Subject(const Subject& other) {
    observers = new vector<Observer*>(*(other.observers));
}

Subject::~Subject() {
    delete observers;
}

Subject& Subject::operator=(const Subject& other) {
    if (this != &other) {
        delete observers;
        observers = new vector<Observer*>(*(other.observers));
    }
    return *this;
}

void Subject::attach(Observer* observer) {
    if (observer != nullptr) {
        observers->push_back(observer);
    }
}

void Subject::detach(Observer* observer) {
    observers->erase(
        std::remove(observers->begin(), observers->end(), observer),
        observers->end()
    );
}

void Subject::notify(ILoggable* loggable) {
    for (Observer* observer : *observers) {
        observer->update(loggable);
    }
}

ostream& operator<<(ostream& os, const Subject& subject) {
    os << "Subject with " << subject.observers->size() << " observer(s) attached";
    return os;
}

// ===== LogObserver =====

LogObserver::LogObserver() {
    logFilePath = new string("gamelog.txt");
}

LogObserver::LogObserver(const string& filePath) {
    logFilePath = new string(filePath);
}

LogObserver::LogObserver(const LogObserver& other) {
    logFilePath = new string(*(other.logFilePath));
}

LogObserver::~LogObserver() {
    delete logFilePath;
}

LogObserver& LogObserver::operator=(const LogObserver& other) {
    if (this != &other) {
        delete logFilePath;
        logFilePath = new string(*(other.logFilePath));
    }
    return *this;
}

// append to log file
void LogObserver::update(ILoggable* loggable) {
    if (loggable != nullptr) {
        ofstream logFile(*logFilePath, std::ios::app);
        if (logFile.is_open()) {
            logFile << loggable->stringToLog() << endl;
            logFile.close();
        } else {
            cout << "Error: Could not open log file: " << *logFilePath << endl;
        }
    }
}

ostream& operator<<(ostream& os, const LogObserver& observer) {
    os << "LogObserver logging to: " << *(observer.logFilePath);
    return os;
}
