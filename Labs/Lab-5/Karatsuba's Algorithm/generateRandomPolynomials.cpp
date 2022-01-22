#include <iostream>
#include <fstream>
#include "Polynomial.h"
#define POLYNOMIAL_LENGTH 31 // must be of the form (2^n - 1)

int main() {
    Polynomial firstPolynomial = Polynomial::generateRandomPolynomialOfSpecificDegree(POLYNOMIAL_LENGTH);
    Polynomial secondPolynomial = Polynomial::generateRandomPolynomialOfSpecificDegree(POLYNOMIAL_LENGTH);

    std::string firstPolynomialAsString = firstPolynomial.coefficientsAsString();
    std::string secondPolynomialAsString = secondPolynomial.coefficientsAsString();

    std::ofstream out("polynomials.txt");
    out << firstPolynomial.getDegree() + 1 << "\n";
    out << firstPolynomialAsString << "\n";
    out << secondPolynomial.getDegree() + 1 << "\n";
    out << secondPolynomialAsString << "\n";

    return 0;
}