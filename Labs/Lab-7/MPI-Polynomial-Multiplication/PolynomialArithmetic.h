//
// Created by calandrinon on 11/27/21.
//

#ifndef KARATSUBA_S_ALGORITHM_POLYNOMIALARITHMETIC_H
#define KARATSUBA_S_ALGORITHM_POLYNOMIALARITHMETIC_H


#include "Polynomial.h"
#include <vector>

class PolynomialArithmetic {
    public:
        static Polynomial computeRegularPolynomialMultiplication(Polynomial firstPolynomial,
                                                                 Polynomial secondPolynomial);

        static Polynomial computeSimplePolynomialOperation(Polynomial firstPolynomial,
                                                           Polynomial secondPolynomial,
                                                           char operation);

        static Polynomial computeKaratsubaPolynomialMultiplication(Polynomial firstPolynomial,
                                                                   Polynomial secondPolynomial);

        static void computeMasterMultiplicationTask(Polynomial& firstPolynomial, Polynomial& secondPolynomial,
                                                    int numberOfProcesses);

        static Polynomial reconstructFinalPolynomial(std::vector<Polynomial> results);

        static void runWorkerComputingRegularMultiplication(int processId);

        static void runWorkerComputingKaratsubaMultiplication(int processId);

        static Polynomial computeRegularPolynomialMultiplicationBetweenXAndY(Polynomial& firstPolynomial, Polynomial& secondPolynomial,
                                                                           int intervalStart, int intervalEnd);
};


#endif //KARATSUBA_S_ALGORITHM_POLYNOMIALARITHMETIC_H
