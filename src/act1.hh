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

#include "readerwriterqueue.h"
#include <cstdint>
#include <functional>

namespace Act1 {

    class Actor;

    template<typename T>
    struct MessageEnvelope {
        Actor *sender_id;
        T data;
    };

    enum class ActorSignal {
        KILL
    };

    class Actor {
    public:
        template<typename U, typename ActorSubType>
        bool send(ActorSubType &actor, U const & msg) {
            return actor.queue()
                .try_enqueue([&actor, env=MessageEnvelope<U>{this, std::move(msg)}] {
                    actor.reaction(env);
                });
        }

        template<typename U, typename ActorSubType>
        bool send(ActorSubType &actor, U const && msg) {
            return actor.queue()
                .try_enqueue([&actor, env=MessageEnvelope<U>{this, std::move(msg)}] {
                    actor.reaction(env);
                });
        }

        bool signal(Actor &actor, ActorSignal signal);

        bool signal(ActorSignal signal);

        void run(void);

        moodycamel::BlockingReaderWriterQueue<std::function<void(void)>> &queue();

    private:
        void signal_reaction(ActorSignal signal);

        moodycamel::BlockingReaderWriterQueue<std::function<void(void)>> __queue;
        bool __received_kill = false;
    };

};

#endif