#include <iostream>
#include <random>
#include <thread>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


// template <typename T>
// T MessageQueue<T>::receive()
// {
//     // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
//     // to wait for and receive new messages and pull them from the queue using move semantics. 
//     // The received object should then be returned by the receive function.
// }

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Message " << msg << " has been sent to the queue" << std::endl;
	std::lock_guard<std::mutex> lock(_mutex);
	_messages.push_back(std::move(msg));
	_cond.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _type = ObjectType::objectTrafficLight;
	_queue = std::shared_ptr<MessageQueue<TrafficLightPhase>>(new MessageQueue<TrafficLightPhase>);
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // random cycle duration between 4 and 6 seconds
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4, 6);

    double cycleDuration = distr(eng); // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
          // Toggle traffic light
          _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
          _queue->send(std::move(_currentPhase));
        }
      
        // reset stop watch for next cycle
        lastUpdate = std::chrono::system_clock::now();
    }
}