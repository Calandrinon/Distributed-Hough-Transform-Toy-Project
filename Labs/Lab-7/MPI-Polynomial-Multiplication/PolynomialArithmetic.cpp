//
// Created by calandrinon on 11/27/21.
//

#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include <mpi.h>
#include "PolynomialArithmetic.h"
#include "Polynomial.h"
#define MPI_POLYNOMIAL_MESSAGE_LENGTH 0
#define MPI_POLYNOMIAL_MESSAGE 1
#define MPI_POLYNOMIAL_RANGE_START 2
#define MPI_POLYNOMIAL_RANGE_END 3

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


void PolynomialArithmetic::computeMasterMultiplicationTask(Polynomial& firstPolynomial, Polynomial& secondPolynomial,
                                            int numberOfProcesses) {
    int chunkSize = firstPolynomial.getNumberOfCoefficients() / (numberOfProcesses - 1);
    int intervalStart = 0, intervalEnd = 0;

    for (int processIndex = 1; processIndex < numberOfProcesses; processIndex++) {
        intervalStart = (processIndex - 1) * chunkSize;
        intervalEnd = (processIndex == numberOfProcesses - 1 ? firstPolynomial.getNumberOfCoefficients() : intervalStart + chunkSize);

        std::string serializedFirstPolynomial = firstPolynomial.serialize();
        std::string serializedSecondPolynomial = secondPolynomial.serialize();
        int lengthOfTheFirstSerializedPolynomial = (int)serializedFirstPolynomial.size();
        int lengthOfTheSecondSerializedPolynomial = (int)serializedSecondPolynomial.size();
        const char* firstPolynomialAsConstChar = serializedFirstPolynomial.c_str();
        const char* secondPolynomialAsConstChar = serializedSecondPolynomial.c_str();

        MPI_Send(&intervalStart, 1, MPI_INT, processIndex, MPI_POLYNOMIAL_RANGE_START, MPI_COMM_WORLD);
        MPI_Send(&intervalEnd, 1, MPI_INT, processIndex, MPI_POLYNOMIAL_RANGE_END, MPI_COMM_WORLD);

        MPI_Send(&lengthOfTheFirstSerializedPolynomial, 1, MPI_INT, processIndex, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD);
        MPI_Send(&lengthOfTheSecondSerializedPolynomial, 1, MPI_INT, processIndex, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD);

        MPI_Send(firstPolynomialAsConstChar, lengthOfTheFirstSerializedPolynomial + 1, MPI_CHAR, processIndex, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD);
        MPI_Send(secondPolynomialAsConstChar, lengthOfTheSecondSerializedPolynomial + 1, MPI_CHAR, processIndex, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD);
    }

    std::vector<std::string> results;
    std::vector<Polynomial> polynomials;
    int responseLength;
    MPI_Status status[2];

    for (int processIndex = 1; processIndex < numberOfProcesses; processIndex++) {
        MPI_Recv(&responseLength, 1, MPI_INT, processIndex, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD, &status[0]);
        char* buffer = (char*)malloc(responseLength*sizeof(char));
        MPI_Recv(buffer, responseLength + 1, MPI_CHAR, processIndex, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD, &status[1]);
        std::string result(buffer);
        results.push_back(result);
        free(buffer);
    }

    for (int processIndex = 1; processIndex < numberOfProcesses; processIndex++) {
        polynomials.push_back(Polynomial::deserialize(results[processIndex - 1].c_str()));
    }

    Polynomial finalResult = reconstructFinalPolynomial(polynomials);
}

Polynomial PolynomialArithmetic::reconstructFinalPolynomial(std::vector<Polynomial> results) {
    int degreeOfThePolynomial = results[0].getDegree();
    Polynomial finalResult(degreeOfThePolynomial);

    for (int coefficientIndex = 0; coefficientIndex < finalResult.getNumberOfCoefficients(); coefficientIndex++) {
        for (const Polynomial& polynomial: results) {
            int newValue = finalResult.getCoefficientOfDegree(coefficientIndex) + polynomial.getCoefficientOfDegree(coefficientIndex);
            finalResult.setCoefficientOfDegree(coefficientIndex, newValue);
        }
    }

    return finalResult;
}


void PolynomialArithmetic::runWorkerComputingRegularMultiplication(int processId) {

    std::string firstPolynomialAsString, secondPolynomialAsString;
    int intervalStart, intervalEnd, lengthOfTheFirstSerializedPolynomial, lengthOfTheSecondSerializedPolynomial;
    MPI_Status status[6];

    MPI_Recv(&intervalStart, 1, MPI_INT, 0, MPI_POLYNOMIAL_RANGE_START, MPI_COMM_WORLD, &status[0]);
    MPI_Recv(&intervalEnd, 1, MPI_INT, 0, MPI_POLYNOMIAL_RANGE_END, MPI_COMM_WORLD, &status[1]);

    MPI_Recv(&lengthOfTheFirstSerializedPolynomial, 1, MPI_INT, 0, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD, &status[2]);
    MPI_Recv(&lengthOfTheSecondSerializedPolynomial, 1, MPI_INT, 0, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD, &status[3]);

    char* firstPolynomialAsConstChar = (char*) malloc(lengthOfTheFirstSerializedPolynomial * sizeof(char));
    char* secondPolynomialAsConstChar = (char*) malloc(lengthOfTheSecondSerializedPolynomial * sizeof(char));

    MPI_Recv(firstPolynomialAsConstChar, lengthOfTheFirstSerializedPolynomial + 1, MPI_CHAR, 0, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD, &status[4]);
    MPI_Recv(secondPolynomialAsConstChar, lengthOfTheSecondSerializedPolynomial + 1, MPI_CHAR, 0, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD, &status[5]);

    Polynomial firstPolynomial = Polynomial::deserialize(firstPolynomialAsConstChar);
    Polynomial secondPolynomial = Polynomial::deserialize(secondPolynomialAsConstChar);
    Polynomial result = computeRegularPolynomialMultiplicationBetweenXAndY(firstPolynomial, secondPolynomial, intervalStart, intervalEnd);

    free(firstPolynomialAsConstChar);
    free(secondPolynomialAsConstChar);

    std::string responsePolynomial = result.serialize();
    const char* responsePolynomialAsString = responsePolynomial.c_str();
    int responseLength = (int)responsePolynomial.size();

    MPI_Send(&responseLength, 1, MPI_INT, 0, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD);
    MPI_Send(responsePolynomialAsString, responseLength + 1, MPI_CHAR, 0, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD);
}


Polynomial PolynomialArithmetic::computeRegularPolynomialMultiplicationBetweenXAndY(Polynomial& firstPolynomial,
                                                                              Polynomial& secondPolynomial,
                                                                              int intervalStart, int intervalEnd) {
    Polynomial result(firstPolynomial.getDegree() + secondPolynomial.getDegree());

    for (int firstIndex = intervalStart; firstIndex < intervalEnd; firstIndex++) {
        for (int secondIndex = 0; secondIndex < secondPolynomial.getNumberOfCoefficients(); secondIndex++) {
            int previousValue = result.getCoefficientOfDegree(firstIndex + secondIndex);
            result.setCoefficientOfDegree(firstIndex + secondIndex,
                                          previousValue + firstPolynomial.getCoefficientOfDegree(firstIndex)
                                                          * secondPolynomial.getCoefficientOfDegree(secondIndex));
        }
    }

    return result;
}

void PolynomialArithmetic::runWorkerComputingKaratsubaMultiplication(int processId) {

    std::string firstPolynomialAsString, secondPolynomialAsString;
    int intervalStart, intervalEnd, lengthOfTheFirstSerializedPolynomial, lengthOfTheSecondSerializedPolynomial;
    MPI_Status status[6];

    MPI_Recv(&intervalStart, 1, MPI_INT, 0, MPI_POLYNOMIAL_RANGE_START, MPI_COMM_WORLD, &status[0]);
    MPI_Recv(&intervalEnd, 1, MPI_INT, 0, MPI_POLYNOMIAL_RANGE_END, MPI_COMM_WORLD, &status[1]);

    MPI_Recv(&lengthOfTheFirstSerializedPolynomial, 1, MPI_INT, 0, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD, &status[2]);
    MPI_Recv(&lengthOfTheSecondSerializedPolynomial, 1, MPI_INT, 0, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD, &status[3]);

    char* firstPolynomialAsConstChar = (char*) malloc(lengthOfTheFirstSerializedPolynomial * sizeof(char));
    char* secondPolynomialAsConstChar = (char*) malloc(lengthOfTheSecondSerializedPolynomial * sizeof(char));

    MPI_Recv(firstPolynomialAsConstChar, lengthOfTheFirstSerializedPolynomial + 1, MPI_CHAR, 0, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD, &status[4]);
    MPI_Recv(secondPolynomialAsConstChar, lengthOfTheSecondSerializedPolynomial + 1, MPI_CHAR, 0, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD, &status[5]);

    Polynomial firstPolynomial = Polynomial::deserialize(firstPolynomialAsConstChar);
    Polynomial secondPolynomial = Polynomial::deserialize(secondPolynomialAsConstChar);

    for (int index = 0; index < firstPolynomial.getNumberOfCoefficients(); index++) {
        if (index < intervalStart || index >= intervalEnd)
            firstPolynomial.setCoefficientOfDegree(index, 0);
    }

    Polynomial result = computeKaratsubaPolynomialMultiplication(firstPolynomial, secondPolynomial);


    free(firstPolynomialAsConstChar);
    free(secondPolynomialAsConstChar);

    std::string responsePolynomial = result.serialize();
    const char* responsePolynomialAsString = responsePolynomial.c_str();
    int responseLength = (int)responsePolynomial.size();

    MPI_Send(&responseLength, 1, MPI_INT, 0, MPI_POLYNOMIAL_MESSAGE_LENGTH, MPI_COMM_WORLD);
    MPI_Send(responsePolynomialAsString, responseLength + 1, MPI_CHAR, 0, MPI_POLYNOMIAL_MESSAGE, MPI_COMM_WORLD);
}
