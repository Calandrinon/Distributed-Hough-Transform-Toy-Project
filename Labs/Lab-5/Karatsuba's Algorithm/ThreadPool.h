//
// Created by calandrinon on 11/27/21.
//

#ifndef KARATSUBA_S_ALGORITHM_THREADPOOL_H
#define KARATSUBA_S_ALGORITHM_THREADPOOL_H
#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <list>
#include <vector>

class ThreadPool {
    public:
        explicit ThreadPool(size_t numberOfThreads = std::thread::hardware_concurrency())
                :endBoolean(false) {
            threads.reserve(numberOfThreads);
            for (size_t index = 0; index < numberOfThreads; index++) {
                threads.emplace_back([this]() { this->run(); });
            }
        }

        ~ThreadPool() {
            close();
            for (std::thread& t : threads)
                t.join();
        }

        int getNumberOfThreads() {
            return threads.size();
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
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lck(mutex);
                    while (taskQueue.empty() && !endBoolean) {
                        conditionVariable.wait(lck);
                    }

                    if (taskQueue.empty())
                        return;

                    task = std::move(taskQueue.front());
                    taskQueue.pop_front();
                }
                task();
            }
        }

        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::list<std::function<void()>> taskQueue;
        bool endBoolean;
        std::vector<std::thread> threads;
};

#endif //KARATSUBA_S_ALGORITHM_THREADPOOL_H
