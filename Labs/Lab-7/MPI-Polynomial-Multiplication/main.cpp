#include <iostream>
#include "Polynomial.h"
#include "PolynomialArithmetic.h"
#include <fstream>
#include <tuple>
#include <chrono>
#include "mpi.h"

#define POLYNOMIAL_LENGTH 1023 // must be of the form (2^n - 1)
#define POLYNOMIAL_PRINT_FLAG true

std::tuple<Polynomial, Polynomial> readPolynomials(std::string filename) {
    std::ifstream in(filename);
    std::deque<int> coefficientsOfTheFirstPolynomial, coefficientsOfTheSecondPolynomial;
    int polynomialSize, currentNumber;

    in >> polynomialSize;
    for (int i = 0; i < polynomialSize; i++) {
        in >> currentNumber;
        coefficientsOfTheFirstPolynomial.push_back(currentNumber);
    }

    in >> polynomialSize;
    for (int i = 0; i < polynomialSize; i++) {
        in >> currentNumber;
        coefficientsOfTheSecondPolynomial.push_back(currentNumber);
    }

    Polynomial firstPolynomial(coefficientsOfTheFirstPolynomial, polynomialSize - 1);
    Polynomial secondPolynomial(coefficientsOfTheSecondPolynomial, polynomialSize - 1);

    return std::make_tuple(firstPolynomial, secondPolynomial);
}

void regularMultiplication(Polynomial* firstPolynomial, Polynomial* secondPolynomial) {
    auto start = std::chrono::steady_clock::now();
    Polynomial regularPolynomialMultiplicationResult = PolynomialArithmetic::computeRegularPolynomialMultiplication(
            *firstPolynomial, *secondPolynomial);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Non-distributed regular polynomial multiplication result for checking:\n";
    // regularPolynomialMultiplicationResult.print(POLYNOMIAL_PRINT_FLAG);
    std::chrono::duration<double> elapsed_seconds = end-start;
    //std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";
}


void karatsubaMultiplication(Polynomial* firstPolynomial, Polynomial* secondPolynomial) {
    auto start = std::chrono::steady_clock::now();
    Polynomial karatsubaPolynomialMultiplicationResult = PolynomialArithmetic::computeKaratsubaPolynomialMultiplication(
            *firstPolynomial, *secondPolynomial);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Karatsuba multiplication result:\n";
    // karatsubaPolynomialMultiplicationResult.print(POLYNOMIAL_PRINT_FLAG);
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";
}


void runMasterAndWorker(int argc, char* argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (!rank) {
        Polynomial firstPolynomial = Polynomial::generateRandomPolynomialOfSpecificDegree(POLYNOMIAL_LENGTH);
        Polynomial secondPolynomial = Polynomial::generateRandomPolynomialOfSpecificDegree(POLYNOMIAL_LENGTH);

        std::cout << "\n";
        printf("I am master process %d.\n", rank);

        regularMultiplication(&firstPolynomial, &secondPolynomial);

        // firstPolynomial.print(POLYNOMIAL_PRINT_FLAG);
        // secondPolynomial.print(POLYNOMIAL_PRINT_FLAG);

        PolynomialArithmetic::computeMasterMultiplicationTask(firstPolynomial, secondPolynomial, size);
    } else {
        printf("I am worker process %d.\n", rank);
        auto start = std::chrono::steady_clock::now();
        // PolynomialArithmetic::runWorkerComputingRegularMultiplication(rank);
        PolynomialArithmetic::runWorkerComputingKaratsubaMultiplication(rank);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";
    }

    MPI_Finalize();
}


int main(int argc, char* argv[]) {
    runMasterAndWorker(argc, argv);
    return 0;
}
