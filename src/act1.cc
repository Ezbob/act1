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

#include "act1.hh"

using namespace Act1;

void Actor::signal_reaction(ActorSignal signal) {
    switch (signal) {
        case ActorSignal::KILL:
            m_received_kill = true;
            break;
    }
}

void Actor::run(void) {
    thread_local std::function<void(void)> reaction;
    for (;;) {
        m_queue.dequeue(reaction);
        reaction();

        if (m_received_kill) {
            break;
        }
    }
}

MessageQueue &Actor::queue() {
    return m_queue;
}

void MessageQueue::enqueue(std::function<void(void)> &&m) {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_queue.emplace(std::move(m));
    m_queue_wait_condition.notify_one();
}

void MessageQueue::dequeue(std::function<void(void)> & item) {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    m_queue_wait_condition.wait(lock, [this] {
        return ! m_queue.empty();
    });

    item = std::move(m_queue.front());
    m_queue.pop();
}

std::thread Act1::start_actor(Actor &a) {
    return std::thread(&Actor::run, &a);
}