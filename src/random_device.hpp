#include <chrono>
#include <random>

class random_device {
   private:
    std::mt19937 rng;

   public:
    random_device()
        : rng(std::chrono::high_resolution_clock::now()
                  .time_since_epoch()
                  .count()) {}

    unsigned int get() { return rng(); }
};