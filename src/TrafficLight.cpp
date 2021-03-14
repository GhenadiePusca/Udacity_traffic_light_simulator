#include <iostream>
#include <random>
#include <thread>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
	std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] { return !_messages.empty(); });

    T msg = std::move(_messages.back());
    _messages.pop_back();

    return msg;
}

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
	while(true) {
    	TrafficLightPhase phase = _queue->receive();
    	if(phase == TrafficLightPhase::green) { return; }
    }
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
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
            // Toggle traffic light
        	_currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            _queue->send(std::move(_currentPhase));
            // reset stop watch for next cycle
        	lastUpdate = std::chrono::system_clock::now();
        }
    }
}