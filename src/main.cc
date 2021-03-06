#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>

#include <thread>
#include <chrono>
#include "act1.hh"

class PongActor;


class PingActor : public Act1::Actor {
public:
    PongActor *pong_actor = nullptr;

    void reaction(const Act1::Message<int> &m) {
        std::cout << "Ping " << m.data << "\n";
        send(*pong_actor, m.data + 1);
    }

    void reaction(const Act1::Message<std::string> &m) {
        std::cout << "Ping " << m.data << "\n";
        send(*pong_actor, m.data);
    }
};


class PongActor : public Act1::Actor {
public:
    PingActor *ping_actor = nullptr;

    void reaction(const Act1::Message<int> &m) {
        std::cout << "Pong " << m.data << "\n";

        if (m.data >= 10) {
            signal(*ping_actor, Act1::ActorSignal::KILL);
            signal(Act1::ActorSignal::KILL);
        }

        send(*ping_actor, m.data + 1);
    }

    void reaction(const Act1::Message<std::string> &m) {
        std::cout << "Pong " << m.data << "\n";
        send(*ping_actor, m.data);
    }
};


int main() {

    PingActor ping;
    PongActor pong;

    ping.pong_actor = &pong;
    pong.ping_actor = &ping;

    pong.send(ping, 0);
    ping.send<std::string>(pong, "Hello");

    std::thread w = Act1::start_actor(ping);
    std::thread r = Act1::start_actor(pong);

    w.join();
    r.join();
}