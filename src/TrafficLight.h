#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <thread>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
	void send(T &&msg);
private:
	std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _messages;
};

// Trafic light phase types
enum TrafficLightPhase
{
    red,
    green,
};

class TrafficLight: public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    
private:
    TrafficLightPhase _currentPhase;
    // typical behaviour methods
    void cycleThroughPhases();
    
    std::shared_ptr<MessageQueue<TrafficLightPhase>> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif