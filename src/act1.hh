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

    template<typename T, typename Evelope_t = MessageEnvelope<T>>
    class Actor {
    protected:
        using EvelopeType = Evelope_t;
        using ReactionType = std::function<void(Actor<T, Evelope_t> &, Evelope_t const &)>;

    private:
        const static void DEFAULT_REACTION(Actor<T, Evelope_t> &, Evelope_t const &) {};

    public:
        explicit Actor(actor_id_t a_id, ReactionType &&r)
            : __details({
                moodycamel::BlockingReaderWriterQueue<Evelope_t>{},
                std::move(r),
                a_id
            }) {}

        explicit Actor(actor_id_t a_id)
            : __details({
                moodycamel::BlockingReaderWriterQueue<Evelope_t>{},
                DEFAULT_REACTION,
                a_id
            }) {}

        inline bool send(Actor<T, Evelope_t> &a, T const && msg) {
            return a.__details.queue.try_enqueue(Evelope_t{this->__details.id, std::move(msg)});
        }

        inline void run(void) {
            __details.__react(*this);
        }

        inline actor_id_t actor_id(void) const noexcept {
            return __details.id;
        }

        inline void reaction(ReactionType && f) {
            __details.reaction = std::move(f);
        }

    private:
        struct {
            moodycamel::BlockingReaderWriterQueue<Evelope_t> queue;
            ReactionType reaction;

            const actor_id_t id;

            void __react(Actor<T, Evelope_t> &a) {
                thread_local Evelope_t item;
                for (;;) {
                    queue.wait_dequeue(item);
                    reaction(a, item);
                }
            }
        } __details;
    };

};

#endif