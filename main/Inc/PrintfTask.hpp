#pragma once

#include "TargetPort.hpp"

/// @brief printf task
class PrintfTask
{
    public:

        /// @brief task for the default task
        /// @details Main loop that waits for messages to get posted to the queue and process them
        void Run();

    private:

};
