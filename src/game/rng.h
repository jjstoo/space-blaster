#ifndef RNG_H
#define RNG_H
#include <random>
static std::random_device rd;
static std::mt19937 gen(rd());

enum Distribution { NORMAL, UNIFORM };

///
/// \brief random_float_in_range returns a uniformly distributed random double
/// in the given range
/// \param lo lower limit
/// \param hi upper limit
/// \param distribution distribution type, default=UNIFORM
/// \return random double, 0 if distribution not supported
///
extern double random_float_in_range(double lo, double hi,
                                    Distribution distribution = UNIFORM);

///
/// \brief random_normal_float returns a normally distributed double from a
/// distribution with the given mean and standard deviation
/// \param mean distribution mean
/// \param stddev distribution stddev
/// \return random double
///
extern double random_normal_float(double mean, double stddev);

///
/// \brief random_int_in_range returns a random integer from a specified
/// distribution
/// \param l lower limit
/// \param h upper limit
/// \param distribution distribution type, default=UNIFORM
/// \return random integer, 0 if distribution not supported
///
template<typename T>
static T random_int_in_range(int lo, int hi,
                             Distribution distribution=UNIFORM) {
    if (distribution == Distribution::NORMAL) {
        return static_cast<T>(random_float_in_range(lo, hi, distribution));
    } else if (distribution == Distribution::UNIFORM) {
        std::uniform_int_distribution<> distr(lo, hi);
        return static_cast<T>(distr(gen));
    }
    return static_cast<T>(0);
}

#endif // RNG_H
