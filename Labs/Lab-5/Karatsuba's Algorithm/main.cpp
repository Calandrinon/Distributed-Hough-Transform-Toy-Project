#include <iostream>
#include "Polynomial.h"
#include "PolynomialArithmetic.h"
#include <fstream>
#include <tuple>
#include <chrono>

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
    std::cout << "Regular multiplication result:\n";
    regularPolynomialMultiplicationResult.print(POLYNOMIAL_PRINT_FLAG);
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";
}


void parallelizedRegularMultiplication(Polynomial* firstPolynomial, Polynomial* secondPolynomial) {
    auto start = std::chrono::steady_clock::now();
    Polynomial parallelRegularPolynomialMultiplicationResult = PolynomialArithmetic::computeParallelizedRegularPolynomialMultiplication(
            *firstPolynomial, *secondPolynomial);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Parallel regular multiplication result:\n";
    parallelRegularPolynomialMultiplicationResult.print(POLYNOMIAL_PRINT_FLAG);
    std::chrono::duration<double> elapsed_seconds = end-start;
    // 0.5 seconds were subtracted, because a thread sleep is used
    std::cout << "Elapsed time: " << elapsed_seconds.count() - 0.5  << " seconds\n";
}

void karatsubaMultiplication(Polynomial* firstPolynomial, Polynomial* secondPolynomial) {
    auto start = std::chrono::steady_clock::now();
    Polynomial karatsubaPolynomialMultiplicationResult = PolynomialArithmetic::computeKaratsubaPolynomialMultiplication(
            *firstPolynomial, *secondPolynomial);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Karatsuba multiplication result:\n";
    karatsubaPolynomialMultiplicationResult.print(POLYNOMIAL_PRINT_FLAG);
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";
}

void parallelizedKaratsubaMultiplication(Polynomial* firstPolynomial, Polynomial* secondPolynomial) {
    auto start = std::chrono::steady_clock::now();
    Polynomial parallelKaratsubaPolynomialMultiplicationResult = PolynomialArithmetic::computeParallelizedKaratsubaPolynomialMultiplication(
            *firstPolynomial, *secondPolynomial, 4);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Parallel Karatsuba multiplication result:\n";
    parallelKaratsubaPolynomialMultiplicationResult.print(POLYNOMIAL_PRINT_FLAG);
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";
}


int main() {
    // CLion only reads files from the debug directory, hence the ../ expression used in the filepath
    auto [firstPolynomial, secondPolynomial] = readPolynomials("../polynomials.txt");

    firstPolynomial.print(POLYNOMIAL_PRINT_FLAG);
    secondPolynomial.print(POLYNOMIAL_PRINT_FLAG);

    regularMultiplication(&firstPolynomial, &secondPolynomial);
    karatsubaMultiplication(&firstPolynomial, &secondPolynomial);
    parallelizedRegularMultiplication(&firstPolynomial, &secondPolynomial);
    parallelizedKaratsubaMultiplication(&firstPolynomial, &secondPolynomial);
    return 0;
}
