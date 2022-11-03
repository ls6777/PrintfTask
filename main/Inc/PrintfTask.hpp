#pragma once

#include "TargetPort.hpp"

/// @brief printf task
class PrintfTask
{
    public:

//        /// @brief Post initialize message to this task
//        void Initialize();

        /// @brief task for the default task
        /// @details Main loop that waits for messages to get posted to the queue and process them
        void Run();

//        /// @brief send msg to shutdown this task
//        void Shutdown();

//        static void Printf(const char *format, ...);

    private:

//        /// @brief Initialize the default task
//        void HandleInitialize();
//
//        /// @brief Shutdown this task
//        void HandleShutdown();

//        /// @brief Transmit the data in the buffer to stdout
//        void TransmitData();

//        static QHandle msgQHandle;
};
