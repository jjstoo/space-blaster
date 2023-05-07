#include "rng.h"

double random_float_in_range(double lo, double hi, Distribution distribution) {
    if (distribution == Distribution::NORMAL) {
        std::normal_distribution<double> distr((lo + hi) / 2, (hi - lo) / 4);
        return distr(gen);
    } else if (distribution == Distribution::UNIFORM) {
        std::uniform_real_distribution<double> distr(lo, hi);
        return distr(gen);
    }

    return 0.0;
}

double random_normal_float(double mean, double stddev) {
    std::normal_distribution<double> distr(mean, stddev);
    return distr(gen);
}

