#pragma once

#include "expression_traits.h"
#include "fraction.h"

#include <random>

template <>
struct ExpressionTraits<Fraction> : BaseExpressionTraits<Fraction>
{
private:
    static bool checkAdditive(const Fraction & left, const Fraction & right)
    {
        double a = right.denominator();
        double b = left.denominator();
        double c = left.numerator() * a;
        double d = right.numerator() * b;
        return checkBounds(a * b) && checkBounds(c) && checkBounds(d) && checkBounds(c + d) && checkBounds(c - d);
    }

    static bool checkMultiplicative(const Fraction & left, const Fraction & right)
    {
        double a = left.numerator();
        double b = left.denominator();
        double c = right.numerator();
        double d = right.denominator();
        return checkBounds(a * c) && checkBounds(a * d) && checkBounds(b * c) && checkBounds(b * d);
    }

public:
    static Fraction randomNumber(std::mt19937 & rng)
    {
        std::uniform_int_distribution<int> numDist(-100, 100);
        std::uniform_int_distribution<int> denomDist(-99, 100);
        auto num = numDist(rng);
        auto denom = denomDist(rng);
        return Fraction(num, denom > 0 ? denom : denom - 1);
    }

    static inline const UnaryOperation UNARY_OPERATIONS[]{
            UnaryOperation(std::negate{}, operator-),
    };

    static inline const BinaryOperation BINARY_OPERATIONS[]{
            BinaryOperation(std::plus{}, operator+, checkAdditive),
            BinaryOperation(std::minus{}, operator-, checkAdditive),
            BinaryOperation(std::multiplies{}, operator*, checkMultiplicative),
            BinaryOperation(std::divides{}, operator/, [](const Fraction & left, const Fraction & right) {
                return right != 0 && checkMultiplicative(left, right);
            }),
    };
};

using Number = Fraction;
