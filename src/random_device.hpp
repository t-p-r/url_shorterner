#ifndef RANDOM_DEVICE_HPP
#define RANDOM_DEVICE_HPP

#include <chrono>
#include <random>

/**
 * Device that generates a sequence of pseudo-randomized unsigned integers.
 */
class random_device {
   public:
    random_device()
        : rng(std::chrono::high_resolution_clock::now()
                  .time_since_epoch()
                  .count()) {}

    unsigned int next_uint() { return rng(); }

   private:
    std::mt19937 rng;
};

#endif  // RANDOM_DEVICE_HPP