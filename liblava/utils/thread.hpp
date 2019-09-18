// file      : liblava/thread.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/id.hpp>
#include <liblava/core/time.hpp>

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace lava {

inline void sleep(time seconds) {

    std::this_thread::sleep_for(std::chrono::milliseconds(seconds_in_ms(seconds)));
}

struct thread_pool {

    using task = std::function<void(id::ref)>; // thread id

    void setup(ui32 count = 2) {

        for (auto i = 0u; i < count; ++i)
            workers.emplace_back(worker(*this));
    }

    void teardown() {

        stop = true;
        condition.notify_all();

        for (auto& worker : workers)
            worker.join();

        workers.clear();
    }

    template <typename F>
    void enqueue(F f) {

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push_back(task(f));
        }
        condition.notify_one();
    }

private:
    struct worker {

        explicit worker(thread_pool& pool) : pool(pool) {}

        void operator()() {

            auto thread_id = ids::next();

            task task;
            while (true) {

                {
                    std::unique_lock<std::mutex> lock(pool.queueMutex);

                    while (!pool.stop && pool.tasks.empty())
                        pool.condition.wait(lock);

                    if (pool.stop)
                        break;

                    task = pool.tasks.front();
                    pool.tasks.pop_front();
                }

                task(thread_id);
            }

            ids::free(thread_id);
        }

    private:
        thread_pool& pool;
    };

    std::vector<std::thread> workers;
    std::deque<task> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    bool stop = false;
};

} // lava
