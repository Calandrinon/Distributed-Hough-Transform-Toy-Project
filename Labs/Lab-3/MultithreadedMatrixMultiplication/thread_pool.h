//
// Created by calandrinon on 11/1/21.
//
#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <list>

class ThreadPool {
    public:
        explicit ThreadPool(size_t numberOfThreads)
                :endBoolean(false)
        {
            threads.reserve(numberOfThreads);
            for (size_t i = 0; i < numberOfThreads; ++i) {
                threads.emplace_back([this]() {this->run(); });
            }
        }

        ~ThreadPool() {
            close();
            for (std::thread& t : threads) {
                t.join();
            }
        }

        void close() {
            std::unique_lock<std::mutex> lck(mutex);
            endBoolean = true;
            conditionVariable.notify_all();
        }

        void enqueue(std::function<void()> func) {
            std::unique_lock<std::mutex> lck(mutex);
            taskQueue.push_back(std::move(func));
            conditionVariable.notify_one();
        }
    private:
        void run() {
            while (true) {
                std::function<void()> toExec;
                {
                    std::unique_lock<std::mutex> lck(mutex);
                    while (taskQueue.empty() && !endBoolean) {
                        conditionVariable.wait(lck);
                    }
                    if (taskQueue.empty()) {
                        return;
                    }
                    toExec = std::move(taskQueue.front());
                    taskQueue.pop_front();
                }
                toExec();
            }
        }

        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::list<std::function<void()> > taskQueue;
        bool endBoolean;
        std::vector<std::thread> threads;
};
