#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>

#include <thread>
#include <chrono>
#include "act1.hh"


int main() {

    Act1::Actor<int> a(0);
    Act1::Actor<int> b(1);

    a.reaction([&b](auto &actor, auto const & msg) {
        std::cout << "Ping " << msg.data << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        actor.send(b, msg.data + 1);
    });

    b.reaction([&a](auto &actor, auto const & msg) {
        std::cout << "Pong " << msg.data << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        actor.send(a, msg.data + 1);
    });

    b.send(a, 0);

    std::thread w(&Act1::Actor<int>::run, &a);
    std::thread r(&Act1::Actor<int>::run, &b);

    w.join();
    r.join();

}