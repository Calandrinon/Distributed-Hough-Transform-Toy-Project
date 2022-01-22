//
// Created by calandrinon on 11/27/21.
//

#ifndef KARATSUBA_S_ALGORITHM_POLYNOMIALARITHMETIC_H
#define KARATSUBA_S_ALGORITHM_POLYNOMIALARITHMETIC_H


#include "Polynomial.h"

class PolynomialArithmetic {
    public:
        static Polynomial computeRegularPolynomialMultiplication(Polynomial firstPolynomial,
                                                                 Polynomial secondPolynomial);

        static Polynomial computeSimplePolynomialOperation(Polynomial firstPolynomial,
                                                           Polynomial secondPolynomial,
                                                           char operation);

        static Polynomial computeKaratsubaPolynomialMultiplication(Polynomial firstPolynomial,
                                                                   Polynomial secondPolynomial);

        static Polynomial computeParallelizedRegularPolynomialMultiplication(Polynomial& firstPolynomial,
                                                                             Polynomial& secondPolynomial);

        static Polynomial computeParallelizedKaratsubaPolynomialMultiplication(Polynomial firstPolynomial,
                                                                               Polynomial secondPolynomial,
                                                                               int treeLevel);
};


#endif //KARATSUBA_S_ALGORITHM_POLYNOMIALARITHMETIC_H
