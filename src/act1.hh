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

    using actor_id_t = std::uint64_t;

    template<typename T>
    struct MessageEnvelope {
        actor_id_t sender_id;
        T data;
    };

    class Actor {
    public:
        explicit Actor(actor_id_t a_id)
            : __details({
                moodycamel::BlockingReaderWriterQueue<std::function<void(void)>>{},
                a_id
            }) {}

        template<typename U, typename ActorSubType>
        inline bool send(ActorSubType &actor, U const & msg) {
            return actor.queue()
                .try_enqueue([&actor, env=MessageEnvelope<U>{actor_id(), std::move(msg)}] {
                    actor.reaction(env);
                }
            );
        }

        template<typename U, typename ActorSubType>
        inline bool send(ActorSubType &actor, U const && msg) {
            return actor.queue()
                .try_enqueue([&actor, env=MessageEnvelope<U>{actor_id(), std::move(msg)}] {
                    actor.reaction(env);
                }
            );
        }

        inline void run(void) {
            thread_local std::function<void(void)> reaction;
            for (;;) {
                __details.queue.wait_dequeue(reaction);
                reaction();
            }
        }

        inline actor_id_t actor_id(void) const noexcept {
            return __details.id;
        }

        inline moodycamel::BlockingReaderWriterQueue<std::function<void(void)>> &queue() {
            return __details.queue;
        }

    private:
        struct {
            moodycamel::BlockingReaderWriterQueue<std::function<void(void)>> queue;
            const actor_id_t id;
        } __details;
    };

};

#endif