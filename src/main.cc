#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>

#include <thread>
#include <chrono>
#include "act1.hh"

class PongActor;

class PingActor : public Act1::Actor<PingActor> {
public:
    std::weak_ptr<PongActor> sender;

    void reaction(const int &m) {
        if ( std::shared_ptr<PongActor> p = sender.lock() ) {
            std::cout << "Ping " << m << "\n";
            send(p, m + 1);
        }
    }

    void reaction(const std::string &m) {
        if ( std::shared_ptr<PongActor> p = sender.lock() ) {
            std::cout << "Ping " << m << "\n";
            send(p, m);
        }
    }
};


class PongActor : public Act1::Actor<PingActor> {
public:
    std::weak_ptr<PingActor> sender;

    void reaction(const int &m) {
        if ( std::shared_ptr<PingActor> p = sender.lock() ) {
            std::cout << "Pong " << m << "\n";
            if (m >= 10) {
                send(p, Act1::ActorSignal::KILL);
                send(Act1::ActorSignal::KILL);
            }

            send(p, m + 1);
        }
    }

    void reaction(const std::string &m) {
        if ( std::shared_ptr<PingActor> p = sender.lock() ) {
            std::cout << "Pong " << m << "\n";
            send(p, m);
        }
    }
};


int main() {
    auto ping = std::make_shared<PingActor>();
    auto pong = std::make_shared<PongActor>();

    ping->sender = pong;
    pong->sender = ping;

    pong->send(ping, 0);
    ping->send<std::string>(pong, "Hello");

    std::thread w = Act1::start_actor(ping);
    std::thread r = Act1::start_actor(pong);

    w.join();
    r.join();
}