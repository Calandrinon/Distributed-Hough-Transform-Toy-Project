#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>
#include "thread_pool.h"
#define min(a, b) (a < b ? a : b)

typedef std::vector<std::vector<int> > Matrix;

int computeMatrixResultElement(Matrix* firstMatrix, Matrix* secondMatrix, int row, int column) {
    if (row >= firstMatrix->size() || column >= (*secondMatrix)[0].size())
        throw std::invalid_argument("The row or the column given as a parameter doesn't match the size of the matrix.");

    int result = 0;

    for (int index = 0; index < (*firstMatrix)[0].size(); index++)
        result += (*firstMatrix)[row][index] * (*secondMatrix)[index][column];

    return result;
}

Matrix generateMatrix(int rows, int columns) {
    Matrix matrix(rows);

    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
        matrix[rowIndex].resize(columns);
        for (int columnIndex = 0; columnIndex < columns; columnIndex++)
            matrix[rowIndex][columnIndex] = 1;
    }

    return matrix;
}

void computeSubtaskByRows(Matrix* firstMatrix, Matrix* secondMatrix, Matrix* resultingMatrix, int startingRowIndex, int rowsComputed, int taskIndex) {
    uint endingRow = min((int)resultingMatrix->size(), startingRowIndex + rowsComputed);
    // std::cout << "Task no. " << taskIndex << " computed " << endingRow - startingRowIndex << " rows\n\n";

    for (int rowIndex = startingRowIndex; rowIndex < endingRow; rowIndex++) {
        for (int columnIndex = 0; columnIndex < (*resultingMatrix)[0].size(); columnIndex++)
            (*resultingMatrix)[rowIndex][columnIndex] = computeMatrixResultElement(firstMatrix, secondMatrix, rowIndex, columnIndex);
    }
}

void computeSubtaskByColumns(Matrix* firstMatrix, Matrix* secondMatrix, Matrix* resultingMatrix, int startingColumnIndex, int columnsComputed, int taskIndex) {
    uint endingColumn = min((int)(*resultingMatrix)[0].size(), startingColumnIndex + columnsComputed);
    // std::cout << "Task no. " << taskIndex << " computed " << endingColumn - startingColumnIndex << " columns\n\n";

    for (int columnIndex = startingColumnIndex; columnIndex < endingColumn; columnIndex++) {
        for (int rowIndex = 0; rowIndex < (*resultingMatrix)[0].size(); rowIndex++)
            (*resultingMatrix)[rowIndex][columnIndex] = computeMatrixResultElement(firstMatrix, secondMatrix, rowIndex, columnIndex);
    }
}

Matrix multiplyMatrices(Matrix* firstMatrix, Matrix* secondMatrix) {
    uint numberOfConcurrentThreads = std::thread::hardware_concurrency();
    std::cout << "Maximum possible number of concurrent threads: " << numberOfConcurrentThreads << "\n";
    uint rowsOfTheResult = firstMatrix->size(), columnsOfTheResult = (*secondMatrix)[0].size();
    uint rowsComputedByOneThread = (uint)round((double)rowsOfTheResult / numberOfConcurrentThreads);

    Matrix matrix(rowsOfTheResult);
    for (int rowIndex = 0; rowIndex < rowsOfTheResult; rowIndex++)
        matrix[rowIndex].resize(columnsOfTheResult);
    std::vector<std::thread> threads;

    uint rowIndex = 0;
    for (int threadIndex = 0; threadIndex < numberOfConcurrentThreads - 1; threadIndex++) {
        threads.emplace_back(computeSubtaskByRows, firstMatrix, secondMatrix, &matrix, rowIndex, rowsComputedByOneThread, threadIndex);
        rowIndex += rowsComputedByOneThread;
    }

    threads.emplace_back(computeSubtaskByRows, firstMatrix, secondMatrix,
                         &matrix, rowIndex,
                         rowsComputedByOneThread + firstMatrix->size() % rowsComputedByOneThread,
                         numberOfConcurrentThreads - 1);

    for (auto& thread : threads)
        thread.join();

    return matrix;
}

void printMatrix(Matrix& matrix) {
    for (int rowIndex = 0; rowIndex < matrix.size(); rowIndex++) {
        for (int columnIndex = 0; columnIndex < matrix[0].size(); columnIndex++)
            std::cout << matrix[rowIndex][columnIndex] << " ";
        std::cout << "\n";
    }
}

void computeMultiplicationWithThreads(Matrix* firstMatrix, Matrix* secondMatrix) {
    auto start = std::chrono::steady_clock::now();
    Matrix result = multiplyMatrices(firstMatrix, secondMatrix);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Normal threads version - elapsed time: " << elapsed_seconds.count() << " seconds\n";
}

Matrix createMatrix(int numberOfRows, int numberOfColumns) {
    Matrix matrix(numberOfRows);
    for (int rowIndex = 0; rowIndex < numberOfRows; rowIndex++)
        matrix[rowIndex].resize(numberOfColumns);

    return matrix;
}

void computeMultiplicationWithThreadPool(Matrix* firstMatrix, Matrix* secondMatrix, Matrix* resultingMatrix) {
    uint numberOfConcurrentThreads = std::thread::hardware_concurrency();
    uint columnsOfTheResult = (*secondMatrix)[0].size();
    uint columnsComputedByOneTask = (uint)round((double)columnsOfTheResult / numberOfConcurrentThreads);

    ThreadPool threadPool(numberOfConcurrentThreads);

    for (int taskIndex = 0; taskIndex < numberOfConcurrentThreads; taskIndex++) {
        threadPool.enqueue([=](){
            int columnsAssignedToTask = (taskIndex == numberOfConcurrentThreads - 1 ? columnsComputedByOneTask + (*secondMatrix)[0].size() % columnsComputedByOneTask : columnsComputedByOneTask);
            computeSubtaskByColumns(firstMatrix, secondMatrix, resultingMatrix,
            taskIndex * columnsComputedByOneTask, columnsAssignedToTask, taskIndex);
        });
    }

    threadPool.close();
}

int main() {
    int n=2000, m=2000, p=2000;
    Matrix firstMatrix = generateMatrix(n, m);
    Matrix secondMatrix = generateMatrix(m, p);
    computeMultiplicationWithThreads(&firstMatrix, &secondMatrix);

    Matrix threadPoolMatrix = createMatrix(firstMatrix.size(), secondMatrix[0].size());
    auto start = std::chrono::steady_clock::now();
    computeMultiplicationWithThreadPool(&firstMatrix, &secondMatrix, &threadPoolMatrix);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Thread pool version - elapsed time: " << elapsed_seconds.count() << " seconds\n";
    return 0;
}
