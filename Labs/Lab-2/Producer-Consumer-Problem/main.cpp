#include <iostream>
#include <thread>
#include <deque>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <mutex>
#include <chrono>
#include <condition_variable>

const int maximumBufferSize = 100;
std::ifstream in("../vectors.txt");
std::deque<int> productsBuffer;
int sum = 0;
std::mutex mutex;
std::condition_variable conditionVariable;

void productsProducer(int numberOfElements) {
    while (numberOfElements > 0) {
        std::unique_lock uniqueLock(mutex);
        conditionVariable.wait(uniqueLock, []() { return productsBuffer.size() < maximumBufferSize; });
        std::cout << "Producer's turn...\n";
        int firstValue, secondValue;
        in >> firstValue >> secondValue;
        productsBuffer.push_back(firstValue*secondValue);
        numberOfElements--;
        uniqueLock.unlock();
        conditionVariable.notify_one();
    }
}

void sumConsumer() {
    while (true) {
        std::unique_lock uniqueLock(mutex);
        std::cout << "Consumer's turn...\n";
        conditionVariable.wait_for(uniqueLock, std::chrono::milliseconds(3000), []() {return !productsBuffer.empty();});
        if (productsBuffer.empty())
            return;
        sum += productsBuffer.front();
        productsBuffer.pop_front();
        uniqueLock.unlock();
        conditionVariable.notify_one();
        std::cout << "Sum: " << sum <<"\n";
    }
}

int main() {
    int numberOfElements;
    in >> numberOfElements;

    auto start = std::chrono::steady_clock::now();

    std::thread producerThread(productsProducer, numberOfElements);
    std::thread consumerThread(sumConsumer);
    producerThread.join();
    consumerThread.join();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << sum << "\n";

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";

    return 0;
}
