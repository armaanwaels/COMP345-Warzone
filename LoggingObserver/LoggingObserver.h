#ifndef LOGGING_OBSERVER_H
#define LOGGING_OBSERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class ILoggable;
class Observer;

// ILoggable interface
class ILoggable {
public:
    virtual ~ILoggable();
    virtual std::string stringToLog() const = 0;
};

// Observer base class
class Observer {
public:
    virtual ~Observer();
    virtual void update(ILoggable* loggable) = 0;
};

// Subject base class
class Subject {
private:
    std::vector<Observer*>* observers;

public:
    Subject();
    Subject(const Subject& other);
    virtual ~Subject();
    Subject& operator=(const Subject& other);

    void attach(Observer* observer);
    void detach(Observer* observer);
    void notify(ILoggable* loggable);

    friend std::ostream& operator<<(std::ostream& os, const Subject& subject);
};

// LogObserver - writes to gamelog.txt on notify
class LogObserver : public Observer {
private:
    std::string* logFilePath;

public:
    LogObserver();
    LogObserver(const std::string& filePath);
    LogObserver(const LogObserver& other);
    ~LogObserver();
    LogObserver& operator=(const LogObserver& other);

    void update(ILoggable* loggable) override;

    friend std::ostream& operator<<(std::ostream& os, const LogObserver& observer);
};

#endif
