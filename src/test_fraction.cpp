#include "fraction.h"

#include <cmath>
#include <gtest/gtest.h>
#include <limits>
#include <sstream>
#include <type_traits>

static constexpr int64_t MIN_INT64 = std::numeric_limits<int64_t>::min();
static constexpr int64_t MAX_INT64 = std::numeric_limits<int64_t>::max();

TEST(FractionTest, traits)
{
    EXPECT_EQ(sizeof(Fraction), sizeof(double) * 2);
    EXPECT_TRUE(std::is_trivially_copyable_v<Fraction>);
}

TEST(FractionTest, construct)
{
    const Fraction zero;
    EXPECT_EQ(zero.numerator(), 0);
    EXPECT_EQ(zero.denominator(), 1);

    const Fraction one = 1;
    EXPECT_EQ(one.numerator(), 1);
    EXPECT_EQ(one.denominator(), 1);

    const Fraction five = 5;
    EXPECT_EQ(five.numerator(), 5);
    EXPECT_EQ(five.denominator(), 1);

    const Fraction twoThirds(2, 3);
    EXPECT_EQ(twoThirds.numerator(), 2);
    EXPECT_EQ(twoThirds.denominator(), 3);
}

TEST(FractionTest, construct_negative)
{
    const Fraction negOne = -1;
    EXPECT_EQ(negOne.numerator(), -1);
    EXPECT_EQ(negOne.denominator(), 1);

    const Fraction negFour(-4, 1);
    EXPECT_EQ(negFour.numerator(), -4);
    EXPECT_EQ(negFour.denominator(), 1);

    const Fraction negFive(5, -1);
    EXPECT_EQ(negFive.numerator(), -5);
    EXPECT_EQ(negFive.denominator(), 1);

    const Fraction negTwoThirds(2, -3);
    EXPECT_EQ(negTwoThirds.numerator(), -2);
    EXPECT_EQ(negTwoThirds.denominator(), 3);
}

TEST(FractionTest, reduce)
{
    const Fraction one(-1, -1);
    EXPECT_EQ(one.numerator(), 1);
    EXPECT_EQ(one.denominator(), 1);

    const Fraction alsoOne(42, 42);
    EXPECT_EQ(alsoOne.numerator(), 1);
    EXPECT_EQ(alsoOne.denominator(), 1);

    const Fraction two(42, 21);
    EXPECT_EQ(two.numerator(), 2);
    EXPECT_EQ(two.denominator(), 1);

    const Fraction negTwo(42, -21);
    EXPECT_EQ(negTwo.numerator(), -2);
    EXPECT_EQ(negTwo.denominator(), 1);

    const Fraction half(21, 42);
    EXPECT_EQ(half.numerator(), 1);
    EXPECT_EQ(half.denominator(), 2);

    const Fraction big(9214746018752493454, 507826684400187515);
    EXPECT_EQ(big.numerator(), 998);
    EXPECT_EQ(big.denominator(), 55);
}

TEST(FractionTest, reduce_zero)
{
    const Fraction zero(0, -5);
    EXPECT_EQ(zero.numerator(), 0);
    EXPECT_EQ(zero.denominator(), 1);

    const Fraction inf(5, 0);
    EXPECT_EQ(inf.numerator(), 1);
    EXPECT_EQ(inf.denominator(), 0);

    const Fraction negInf(-5, 0);
    EXPECT_EQ(negInf.numerator(), -1);
    EXPECT_EQ(negInf.denominator(), 0);

    const Fraction nan(0, 0);
    EXPECT_EQ(nan.numerator(), 0);
    EXPECT_EQ(nan.denominator(), 0);
}

TEST(FractionTest, double_cast)
{
    EXPECT_EQ(static_cast<double>(Fraction(42)), 42);
    EXPECT_EQ(static_cast<double>(Fraction(-42, 10)), -4.2);
    EXPECT_EQ(static_cast<double>(Fraction(100000000000000002, 3)), 33333333333333334);
}

TEST(FractionTest, zero_denominator)
{
    if (!std::numeric_limits<double>::is_iec559) {
        GTEST_SKIP();
    }

    double infty = std::numeric_limits<double>::infinity();

    EXPECT_EQ(static_cast<double>(Fraction(5, 0)), infty);
    EXPECT_EQ(static_cast<double>(Fraction(-5, 0)), -infty);
    EXPECT_TRUE(std::isnan(static_cast<double>(Fraction(0, 0))));
}

TEST(FractionTest, equals)
{
    const Fraction x(1, 2);
    const Fraction y(20, 40);
    const Fraction z = x;
    const Fraction other(1000000000000000000, 2000000000000000001);

    EXPECT_EQ(x, x);

    EXPECT_EQ(x, y);
    EXPECT_EQ(y, x);

    EXPECT_EQ(x, z);
    EXPECT_EQ(y, z);

    EXPECT_NE(x, other);
    EXPECT_NE(other, x);
}

TEST(FractionTest, add)
{
    Fraction x(3, 5);
    const Fraction y(-42, 43);

    EXPECT_EQ(x + 0, x);
    EXPECT_EQ(x + 1, Fraction(8, 5));

    EXPECT_EQ(x + y, Fraction(-81, 215));

    x += y;
    EXPECT_EQ(x, Fraction(-81, 215));
    EXPECT_EQ(y, Fraction(-42, 43));
}

TEST(FractionTest, add_big)
{
    EXPECT_EQ(Fraction(1, MAX_INT64) + Fraction(2, MAX_INT64), Fraction(3, MAX_INT64));
}

TEST(FractionTest, subtract)
{
    Fraction x(3, 5);
    const Fraction y(-42, 43);

    EXPECT_EQ(x - 0, x);
    EXPECT_EQ(x - 1, Fraction(-2, 5));

    EXPECT_EQ(x - y, Fraction(339, 215));

    x -= y;
    EXPECT_EQ(x, Fraction(339, 215));
    EXPECT_EQ(y, Fraction(-42, 43));
}

TEST(FractionTest, subtract_big)
{
    EXPECT_EQ(Fraction(3, MAX_INT64) - Fraction(2, MAX_INT64), Fraction(1, MAX_INT64));
}

TEST(FractionTest, multiply)
{
    EXPECT_EQ(Fraction(0) * 0, 0);
    EXPECT_EQ(Fraction(0) * 1, 0);
    EXPECT_EQ(Fraction(1) * 0, 0);
    EXPECT_EQ(Fraction(1) * 1, 1);

    Fraction x(-2, 3);

    EXPECT_EQ(x * 0, 0);
    EXPECT_EQ(x * 1, x);
    EXPECT_EQ(0 * x, 0);
    EXPECT_EQ(1 * x, x);

    const Fraction y(11, 20);

    EXPECT_EQ(x * y, Fraction(-11, 30));
    EXPECT_EQ(y * x, Fraction(-11, 30));

    const Fraction z(-42, 43);

    EXPECT_EQ((x * y) * z, Fraction(77, 215));
    EXPECT_EQ(x * (y * z), Fraction(77, 215));

    x *= -1;
    EXPECT_EQ(x, Fraction(2, 3));
}

TEST(FractionTest, multiply_big)
{
    EXPECT_EQ(Fraction(3, MAX_INT64) * Fraction(MAX_INT64, 5), Fraction(3, 5));
}

TEST(FractionTest, divide)
{
    EXPECT_EQ(Fraction(0) / 1, 0);
    EXPECT_EQ(Fraction(1) / 1, 1);

    EXPECT_EQ(Fraction(0) / MAX_INT64, 0);
    EXPECT_EQ(Fraction(1) / MAX_INT64, Fraction(1, MAX_INT64));

    Fraction x(-2, 3);

    EXPECT_EQ(x / 1, x);
    EXPECT_EQ(0 / x, 0);
    EXPECT_EQ(1 / x, Fraction(-3, 2));

    const Fraction y(42, 123);

    EXPECT_EQ(x / y, Fraction(-41, 21));
    EXPECT_EQ(y / x, Fraction(-21, 41));
    EXPECT_EQ(x / y, 1 / (y / x));
    EXPECT_EQ(y / x, 1 / (x / y));

    x /= -1;
    EXPECT_EQ(x, Fraction(2, 3));
}

TEST(FractionTest, divide_big)
{
    EXPECT_EQ(Fraction(3, MAX_INT64) / Fraction(5, MAX_INT64), Fraction(3, 5));
}

TEST(FractionTest, negate)
{
    const Fraction x(-1, 2);

    EXPECT_EQ(-x, 0 - x);
    EXPECT_EQ(-(-x), x);
}

TEST(FractionTest, compare_strict)
{
    constexpr auto expectLess = [](const Fraction & left, const Fraction & right) {
        EXPECT_TRUE(left < right);
        EXPECT_FALSE(left > right);

        EXPECT_TRUE(right > left);
        EXPECT_FALSE(right < left);
    };

    expectLess(Fraction(0), Fraction(1));
    expectLess(Fraction(-1), Fraction(0));
    expectLess(Fraction(2), Fraction(42));
    expectLess(Fraction(-42), Fraction(42));

    expectLess(0, Fraction(MAX_INT64));
    expectLess(Fraction(MIN_INT64), 0);
    expectLess(Fraction(MIN_INT64), Fraction(MAX_INT64));

    expectLess(Fraction(1, 3), Fraction(1, 2));
    expectLess(Fraction(1, 2), Fraction(2, 3));
    expectLess(Fraction(1, MAX_INT64), Fraction(2, MAX_INT64));
    expectLess(Fraction(-2, MAX_INT64), Fraction(-1, MAX_INT64));

    EXPECT_FALSE(Fraction() < Fraction());
    EXPECT_FALSE(Fraction(-5, 41) < Fraction(-5, 41));
}

TEST(FractionTest, compare_non_strict)
{
    constexpr auto expectLessEq = [](const Fraction & left, const Fraction & right) {
        EXPECT_TRUE(left <= right);
        EXPECT_EQ(left >= right, left == right);

        EXPECT_TRUE(right >= left);
        EXPECT_EQ(right <= left, left == right);
    };

    expectLessEq(Fraction(0), Fraction(1));
    expectLessEq(Fraction(-1), Fraction(0));
    expectLessEq(Fraction(2), Fraction(42));
    expectLessEq(Fraction(-42), Fraction(42));

    expectLessEq(0, Fraction(MAX_INT64));
    expectLessEq(Fraction(MIN_INT64), 0);
    expectLessEq(Fraction(MIN_INT64), Fraction(MAX_INT64));
    expectLessEq(Fraction(MIN_INT64), Fraction(MIN_INT64));
    expectLessEq(Fraction(MAX_INT64), Fraction(MAX_INT64));

    expectLessEq(Fraction(1, 3), Fraction(1, 2));
    expectLessEq(Fraction(1, 2), Fraction(2, 3));
    expectLessEq(Fraction(1, MAX_INT64), Fraction(2, MAX_INT64));
    expectLessEq(Fraction(-2, MAX_INT64), Fraction(-1, MAX_INT64));

    expectLessEq(Fraction(), Fraction());
    expectLessEq(Fraction(-5, 41), Fraction(-5, 41));
}

TEST(FractionTest, string)
{
    static constexpr auto str = [](const Fraction & n) {
        return n.str();
    };

    EXPECT_EQ(str(Fraction()), "0/1");
    EXPECT_EQ(str(Fraction(5)), "5/1");
    EXPECT_EQ(str(Fraction(-42, 5)), "-42/5");
    EXPECT_EQ(str(Fraction(5, -10)), "-1/2");
}

TEST(FractionTest, print)
{
    static constexpr auto str = [](const Fraction & n) {
        std::stringstream ss;
        ss << n;
        return ss.str();
    };

    EXPECT_EQ(str(Fraction()), "0/1");
    EXPECT_EQ(str(Fraction(5)), "5/1");
    EXPECT_EQ(str(Fraction(-42, 5)), "-42/5");
    EXPECT_EQ(str(Fraction(5, -10)), "-1/2");
}
