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

void Act1::Actor::signal(Actor &actor, ActorSignal signal) {
    actor.queue()
        .enqueue([&actor, signal] {
            actor.signal_reaction(signal);
        });
}

void Act1::Actor::signal(ActorSignal signal) {
    __queue.enqueue([this, signal] {
        signal_reaction(signal);
    });
}

void Act1::Actor::signal_reaction(ActorSignal signal) {
    switch (signal) {
        case ActorSignal::KILL:
            __received_kill = true;
            break;
    }
}

void Act1::Actor::run(void) {
    thread_local std::function<void(void)> reaction;
    for (;;) {
        __queue.dequeue(reaction);
        reaction();

        if (__received_kill) {
            break;
        }
    }
}

MessageQueue<std::function<void(void)>> &Act1::Actor::queue() {
    return __queue;
}