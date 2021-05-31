#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>

#include <thread>
#include <chrono>
#include <tuple>

#include "act1.hh"

class ActorReactionB {
    Act1::Actor<int> &a_actor;
public:
    ActorReactionB(Act1::Actor<int> &a): a_actor(a) {}

    void operator()(Act1::Actor<int> &actor, Act1::MessageEnvelope<int> const &msg) {
        std::cout << "Sending to actor b.. " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        actor.send(a_actor, -1);
        std::cout << "Sent\n";
        actor.stop();
    }
};

class ActorReactionA {
    Act1::Actor<int> &b_actor;
public:
    ActorReactionA(Act1::Actor<int> &b): b_actor(b) {}

    void operator()(Act1::Actor<int> &actor, Act1::MessageEnvelope<int> const &msg) {
        if (msg.sender_id == actor.actor_id()) {
            return;
        }
        if (msg.data == -1) {
            std::cout << "good bye" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            actor.stop();
        }
    }
};

int main() {

    Act1::Actor<int> a(0);
    Act1::Actor<int> b(1);

    a.reaction(ActorReactionA(b));
    b.reaction(ActorReactionB(a));

/*
    a.reaction([&b](auto &actor, auto const & r) {
        if (r.sender_id != actor.actor_id()) {
            if (r.data == -1) {
                std::cout << "good bye" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                actor.stop();
            }
        }
    });

    b.reaction([&a](auto &actor, auto const & r) {
        std::cout << "Sending to actor b.. " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        actor.send(a, -1);
        std::cout << "Sent\n";
        actor.stop();
    });
*/

    b.send(b, -1);

    std::thread w(&Act1::Actor<int>::run, &a);
    std::thread r(&Act1::Actor<int>::run, &b);

    w.join();
    r.join();


//    assert(queue.size_approx() == 0);
}