// MIT License
//
// Copyright (c) 2021 Anders Busch
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _HEADER_FILE_act1_20210525191648_
#define _HEADER_FILE_act1_20210525191648_

#include <cstdint>
#include <functional>
#include <condition_variable>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <type_traits>

namespace Act1 {

    enum class ActorSignal {
        KILL
    };

    class MessageQueue {
    public:
        void enqueue(std::function<void(void)> && m);
        void dequeue(std::function<void(void)> & item);

    private:
        std::queue<std::function<void(void)>> m_queue;
        std::condition_variable m_queue_wait_condition;
        std::mutex m_queue_mutex;
    };

    template<typename Derived_t>
    class Actor {
    public:
        template<typename U, typename APtr, typename std::enable_if<!std::is_same<U, ActorSignal>::value, int>::type = 0>
        void send(APtr actor, U const && data) {
            actor->queue()
                .enqueue([actor, msg=std::move(data)] {
                    actor->reaction(msg);
                });
        }

        template<typename U, typename APtr, typename std::enable_if<!std::is_same<U, ActorSignal>::value, int>::type = 0>
        void send(APtr actor, U const & data) {
            actor->queue()
                .enqueue([actor, msg=std::move(data)] {
                    actor->reaction(msg);
                });
        }

        template<typename U, typename std::enable_if<std::is_same<U, ActorSignal>::value, int>::type = 0>
        void send(U const signal) {
            queue()
                .enqueue([this, signal] {
                    signal_reaction(signal);
                });
        }

        template<typename U, typename APtr, typename std::enable_if<std::is_same<U, ActorSignal>::value, int>::type = 0>
        void send(APtr actor, U const signal) {
            actor->queue()
                .enqueue([actor, signal] {
                    actor->signal_reaction(signal);
                });
        }

        void run(void) {
            thread_local std::function<void(void)> reaction;
            for (;;) {
                m_queue.dequeue(reaction);
                reaction();

                if (m_received_kill) {
                    break;
                }
            }
        }

        MessageQueue &queue() {
            return m_queue;
        }

    private:
        void signal_reaction(ActorSignal signal) {
            switch (signal) {
                case ActorSignal::KILL:
                    m_received_kill = true;
                    break;
            }
        }

        MessageQueue m_queue;
        bool m_received_kill = false;
    };

    template<typename ActorType>
    std::thread start_actor(std::shared_ptr<ActorType> &a) {
        return std::thread([&a] {a->run();});
    }
};

#endif