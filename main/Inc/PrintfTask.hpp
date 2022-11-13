#pragma once

#include "TargetPort.hpp"

/// @brief Printf task
/// @details This class is is responsible for sending all printf buffered up data to the configured stdout.
///
/// @note This should be the one of the lowest priority tasks.
class PrintfTask
{
    public:

        /// @brief Initialize this task and configure stdout rerouting
        void Initialize();

        /// @brief execution of printf task
        /// @details Main loop that output all data buffered up from printf
        void Run();

    private:

        //None

};
