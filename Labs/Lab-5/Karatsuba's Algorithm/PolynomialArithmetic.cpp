//
// Created by calandrinon on 11/27/21.
//

#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include "PolynomialArithmetic.h"
#include "Polynomial.h"
#include "ThreadPool.h"


Polynomial PolynomialArithmetic::computeRegularPolynomialMultiplication(Polynomial firstPolynomial,
                                                                      Polynomial secondPolynomial) {
    Polynomial result(firstPolynomial.getDegree() + secondPolynomial.getDegree());

    for (int firstIndex = 0; firstIndex < firstPolynomial.getDegree() + 1; firstIndex++) {
        for (int secondIndex = 0; secondIndex < secondPolynomial.getDegree() + 1; secondIndex++) {
            int previousValue = result.getCoefficientOfDegree(firstIndex + secondIndex);
            result.setCoefficientOfDegree(firstIndex + secondIndex,
                                          previousValue + firstPolynomial.getCoefficientOfDegree(firstIndex)
                                                * secondPolynomial.getCoefficientOfDegree(secondIndex));
        }
    }

    return result;
}


Polynomial PolynomialArithmetic::computeSimplePolynomialOperation(Polynomial firstPolynomial,
                                                                  Polynomial secondPolynomial, char operation) {
    int minimumDegree = std::min(firstPolynomial.getDegree(), secondPolynomial.getDegree());
    int maximumDegree = std::max(firstPolynomial.getDegree(), secondPolynomial.getDegree());
    Polynomial result(maximumDegree);
    int sign = (operation == '-' ? -1 : 1);

    for (int index = 0; index < minimumDegree + 1; index++)
        result.setCoefficientOfDegree(index,
  firstPolynomial.getCoefficientOfDegree(index) + sign * secondPolynomial.getCoefficientOfDegree(index));

    if (maximumDegree != minimumDegree) {
        if (maximumDegree == firstPolynomial.getDegree()) {
            for (int index = minimumDegree + 1; index <= maximumDegree; index++) {
                result.setCoefficientOfDegree(index, sign * firstPolynomial.getCoefficientOfDegree(index));
            }
        } else {
            for (int index = minimumDegree + 1; index <= maximumDegree; index++) {
                result.setCoefficientOfDegree(index, sign * secondPolynomial.getCoefficientOfDegree(index));
            }
        }
    }

    return result;
}

Polynomial PolynomialArithmetic::computeKaratsubaPolynomialMultiplication(Polynomial firstPolynomial,
                                                                          Polynomial secondPolynomial) {
    if (firstPolynomial.getDegree() != secondPolynomial.getDegree())
        throw std::invalid_argument("The polynomials should have the same degree.");

    if (firstPolynomial.getNumberOfCoefficients() <= 1 || secondPolynomial.getNumberOfCoefficients() <= 1)
        return computeRegularPolynomialMultiplication(firstPolynomial, secondPolynomial);

    int halfSize = firstPolynomial.getNumberOfCoefficients() / 2;

    Polynomial a(halfSize - 1), b(halfSize - 1), c(halfSize - 1), d(halfSize - 1);

    for (int index = 0; index < halfSize; index++) {
        a.setCoefficientOfDegree(index, firstPolynomial.getCoefficientOfDegree(index));
        b.setCoefficientOfDegree(index, firstPolynomial.getCoefficientOfDegree(halfSize + index));
    }

    for (int index = 0; index < halfSize; index++) {
        c.setCoefficientOfDegree(index, secondPolynomial.getCoefficientOfDegree(index));
        d.setCoefficientOfDegree(index, secondPolynomial.getCoefficientOfDegree(halfSize + index));
    }

    Polynomial ac = computeKaratsubaPolynomialMultiplication(a, c);
    Polynomial bd = computeKaratsubaPolynomialMultiplication(b, d);
    Polynomial aPlusB = computeSimplePolynomialOperation(a, b, '+');
    Polynomial cPlusD = computeSimplePolynomialOperation(c, d, '+');
    Polynomial aPlusBTimesCPlusD = computeKaratsubaPolynomialMultiplication(aPlusB, cPlusD);

    Polynomial paddedBc = bd.copy();
    paddedBc.padWithZeroes(2*halfSize);
    Polynomial aPlusBTimesCPlusDMinusBd = computeSimplePolynomialOperation(aPlusBTimesCPlusD, bd, '-');
    Polynomial partialResult = computeSimplePolynomialOperation(aPlusBTimesCPlusDMinusBd, ac, '-');
    partialResult.padWithZeroes(halfSize);

    return computeSimplePolynomialOperation(computeSimplePolynomialOperation(paddedBc, partialResult, '+'), ac, '+');
}


Polynomial PolynomialArithmetic::computeParallelizedRegularPolynomialMultiplication(Polynomial& firstPolynomial,
                                                                                    Polynomial& secondPolynomial) {
    const int resultSize = firstPolynomial.getDegree() + secondPolynomial.getDegree();
    std::shared_ptr<Polynomial> result = std::make_shared<Polynomial>(resultSize);
    ThreadPool threadPool;
    int taskWorkload = !(result->getNumberOfCoefficients() / threadPool.getNumberOfThreads()) ? 1 : result->getNumberOfCoefficients() / threadPool.getNumberOfThreads();


    for (int resultIndex = 0; resultIndex < resultSize + 1; resultIndex += taskWorkload) {
        threadPool.enqueue([=](){
            int intervalStart = resultIndex, intervalEnd = resultIndex + taskWorkload;
            for (int firstIndex = intervalStart; firstIndex < intervalEnd; firstIndex++) {
                if (firstIndex > result->getNumberOfCoefficients())
                    return;

                for (int secondIndex = 0; secondIndex <= firstIndex; secondIndex++) {
                    if (secondIndex < firstPolynomial.getNumberOfCoefficients() && (firstIndex - secondIndex) < secondPolynomial.getNumberOfCoefficients()) {
                        int previousValue = result->getCoefficientOfDegree(firstIndex);
                        result->setCoefficientOfDegree(firstIndex, previousValue +
                                                                   firstPolynomial.getCoefficientOfDegree(secondIndex) *
                                                                   secondPolynomial.getCoefficientOfDegree(
                                                                           firstIndex - secondIndex));
                    }
                }
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    threadPool.close();

    return *result;
}

Polynomial PolynomialArithmetic::computeParallelizedKaratsubaPolynomialMultiplication(Polynomial firstPolynomial,
                                                                                      Polynomial secondPolynomial, int treeLevel) {
    if (treeLevel > 4 || (firstPolynomial.getDegree() <= 1 || secondPolynomial.getDegree() <= 1))
        return computeKaratsubaPolynomialMultiplication(firstPolynomial, secondPolynomial);

    int halfSize = firstPolynomial.getNumberOfCoefficients() / 2;

    Polynomial a(halfSize - 1), b(halfSize - 1), c(halfSize - 1), d(halfSize - 1);

    for (int index = 0; index < halfSize; index++) {
        a.setCoefficientOfDegree(index, firstPolynomial.getCoefficientOfDegree(index));
        b.setCoefficientOfDegree(index, firstPolynomial.getCoefficientOfDegree(halfSize + index));
    }

    for (int index = 0; index < halfSize; index++) {
        c.setCoefficientOfDegree(index, secondPolynomial.getCoefficientOfDegree(index));
        d.setCoefficientOfDegree(index, secondPolynomial.getCoefficientOfDegree(halfSize + index));
    }

    std::future<Polynomial> acFuture, bdFuture, aPlusBTimesCPlusDFuture;
    acFuture = std::async(std::launch::async, [=]() {
        return computeParallelizedKaratsubaPolynomialMultiplication(a, c, treeLevel + 1);
    });

    bdFuture = std::async(std::launch::async, [=]() {
        return computeParallelizedKaratsubaPolynomialMultiplication(b, d, treeLevel + 1);
    });

    Polynomial aPlusB = computeSimplePolynomialOperation(a, b, '+');
    Polynomial cPlusD = computeSimplePolynomialOperation(c, d, '+');

    aPlusBTimesCPlusDFuture = std::async(std::launch::async, ([=]() {
        return computeParallelizedKaratsubaPolynomialMultiplication(aPlusB, cPlusD, treeLevel);
    }));

    Polynomial ac = acFuture.get();
    Polynomial bd = bdFuture.get();
    Polynomial aPlusBTimesCPlusD = aPlusBTimesCPlusDFuture.get();
    Polynomial paddedBc = bd.copy();
    paddedBc.padWithZeroes(2*halfSize);
    Polynomial aPlusBTimesCPlusDMinusBd = computeSimplePolynomialOperation(aPlusBTimesCPlusD, bd, '-');
    Polynomial partialResult = computeSimplePolynomialOperation(aPlusBTimesCPlusDMinusBd, ac, '-');
    partialResult.padWithZeroes(halfSize);

    return computeSimplePolynomialOperation(computeSimplePolynomialOperation(paddedBc, partialResult, '+'), ac, '+');
}
