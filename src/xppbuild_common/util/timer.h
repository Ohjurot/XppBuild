#pragma once

#include <chrono>

namespace xpp::util
{
    class Timer
    {
        public:
            // Constructor
            Timer() noexcept
            {
                timer_end_time = timer_begin_time = std::chrono::high_resolution_clock::now();
            }
            Timer(const Timer&) noexcept = default;

            // Copy
            Timer& operator=(const Timer&) noexcept = default;

            // Stop the timer
            void stop()
            {
                timer_end_time = std::chrono::high_resolution_clock::now();
            }

            // Get result as float
            float duration_float_seconds()
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>(timer_end_time - timer_begin_time).count() / 1000.f;
            }

        private:
            // Timer begin and end time
            std::chrono::time_point<std::chrono::high_resolution_clock> timer_begin_time;
            std::chrono::time_point<std::chrono::high_resolution_clock> timer_end_time;
    };
}
