#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>

#include <thread>
#include <chrono>
#include "act1.hh"
#include <vector>

class PongActor;

class PingActor : public Act1::Actor<PingActor> {
public:
    Act1::Actor<PongActor> *pong_actor = nullptr;

    PingActor(Act1::actor_id_t id) : BaseClass(id) {}

    void reaction(const Act1::MessageEnvelope<int> &m) {
        std::cout << "Ping " << m.data << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        send(*pong_actor, m.data + 1);
    }

    void reaction(const Act1::MessageEnvelope<std::string> &m) {
        std::cout << "Ping " << m.data << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        send(*pong_actor, m.data);
    }
};

class PongActor : public Act1::Actor<PongActor> {
public:
    Act1::Actor<PingActor> *ping_actor = nullptr;

    PongActor(Act1::actor_id_t id) : BaseClass(id) {}

    void reaction(const Act1::MessageEnvelope<int> &m) {
        std::cout << "Pong " << m.data << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        send(*ping_actor, m.data + 1);
    }

    void reaction(const Act1::MessageEnvelope<std::string> &m) {
        std::cout << "Pong " << m.data << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        send(*ping_actor, m.data);
    }
};


int main() {


    PingActor ping(0);
    PongActor pong(1);

    ping.pong_actor = &pong;
    pong.ping_actor = &ping;

    pong.send(ping, 0);
    ping.send<std::string>(pong, "Hello");

    std::thread w(&PingActor::run, &ping);
    std::thread r(&PongActor::run, &pong);

    w.join();
    r.join();

}