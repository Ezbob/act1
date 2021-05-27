#include <cstdint>
#include <iostream>
#include <ostream>
#include "readerwriterqueue.h"
#include <thread>
#include <chrono>

namespace Act1 {

    using actor_id_t = std::uint64_t;

    template<typename T>
    struct MessageEnvelope {
        actor_id_t sender_id;
        T data;
    };

    template<typename T, typename Evelope_t = MessageEnvelope<T>>
    class Actor {
    protected:
        using EvelopeType = Evelope_t;

        virtual void react(EvelopeType const &envelope) = 0;

    public:
        explicit Actor(actor_id_t a_id)
            : __details({moodycamel::BlockingReaderWriterQueue<Evelope_t>{}, a_id}) {}

        bool send(Actor<T, Evelope_t> &a, T const && msg) {
            return a.__details.queue.try_enqueue(Evelope_t{a.__details.id, msg});
        }

        inline void run(void) {
            __details.__react(*this);
        }

        inline actor_id_t actor_id(void) const noexcept {
            return __details.id;
        }

    private:
        struct {
            moodycamel::BlockingReaderWriterQueue<Evelope_t> queue;
            const actor_id_t id;

            void __react(Actor<T, Evelope_t> &a) {
                thread_local Evelope_t item;
                for (;;) {
                    queue.wait_dequeue(item);
                    a.react(item);
                }
            }
        } __details;
    };

};

class MActor : public Act1::Actor<int> {
public:
    MActor() : Act1::Actor<int>(0) {}
protected:
    void react(EvelopeType const &m) {
        std::cout << "1 -> " << m.sender_id << " | " << m.data << "\n";
    }
};


int main() {

    MActor a;

    a.send(a, 2);

    std::cout << a.actor_id() << std::endl;

    std::thread w(&MActor::run, &a);
/*

    std::thread r([&] {

    });
    */

    w.join();
//    r.join();


//    assert(queue.size_approx() == 0);
}