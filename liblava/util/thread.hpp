/**
 * @file         liblava/util/thread.hpp
 * @brief        Thread pool
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/id.hpp"
#include "liblava/core/time.hpp"
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace lava {

/**
 * @brief Sleep for seconds
 * @param time    Seconds to sleep
 */
inline void sleep(seconds time) {
    std::this_thread::sleep_for(time);
}

/**
 * @brief Sleep for milliseconds
 * @param time    Milliseconds to sleep
 */
inline void sleep(ms time) {
    std::this_thread::sleep_for(time);
}

/**
 * @brief Sleep for microseconds
 * @param time    Microseconds to sleep
 */
inline void sleep(us time) {
    std::this_thread::sleep_for(time);
}

/**
 * @brief Thread pool
 */
struct thread_pool {
    /// Task function (with thread id)
    using task = std::function<void(id::ref)>;

    /**
     * @brief Set up the thread pool
     * @param count    Number of threads
     */
    void setup(ui32 count = 2) {
        for (auto i = 0u; i < count; ++i)
            m_workers.emplace_back(worker(*this));
    }

    /**
     * @brief Tear down the thread pool
     */
    void teardown() {
        m_stop = true;
        m_condition.notify_all();

        for (auto& worker : m_workers)
            worker.join();

        m_workers.clear();
    }

    /**
     * @brief Enqueue a task
     * @param f    Task function
     */
    void enqueue(auto f) {
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_tasks.push_back(task(f));
        }
        m_condition.notify_one();
    }

private:
    /**
     * @brief Thread worker
     */
    struct worker {
        /**
         * @brief Construct a new worker
         * @param pool    Thread pool
         */
        explicit worker(thread_pool& pool)
        : m_pool(pool) {}

        /**
         * @brief Run task operator
         */
        void operator()() {
            auto thread_id = ids::instance().next();

            task task;
            while (true) {
                {
                    std::unique_lock<std::mutex> lock(m_pool.m_queue_mutex);

                    while (!m_pool.m_stop && m_pool.m_tasks.empty())
                        m_pool.m_condition.wait(lock);

                    if (m_pool.m_stop)
                        break;

                    task = m_pool.m_tasks.front();
                    m_pool.m_tasks.pop_front();
                }

                task(thread_id);
            }
        }

    private:
        /// Thread pool
        thread_pool& m_pool;
    };

    /// List of workers
    std::vector<std::thread> m_workers;

    /// List of tasks
    std::deque<task> m_tasks;

    /// Queue mutex
    std::mutex m_queue_mutex;

    /// Condition variable
    std::condition_variable m_condition;

    /// Stop state
    bool m_stop = false;
};

} // namespace lava
