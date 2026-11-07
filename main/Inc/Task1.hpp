#pragma once

/// @file Task1.hpp
/// @brief Example task that prints a single line periodically.

#include "Message.hpp"
#include "TargetPort.hpp"

/// @brief Example task that prints a counter every 10 ms
/// @details Used to measure the time taken by a single, short printf call.
class Task1
{
    public:

        /// @brief Create this task's message queue and post the initialize message to it
        /// @note Must be called before Run().
        void Initialize();

        /// @brief Post a message for this task to do processing
        void Process();

        /// @brief Execution of Task1
        /// @details Main loop that waits for messages posted to the queue and processes them. Never returns.
        void Run();

        /// @brief Post a message to shut down this task
        void Shutdown();

    private:

        /// @brief Post a message to this task's queue
        /// @param[in] id - ID of the message to send
        void SendMessage(Message::MsgId id);

        /// @brief Handle the initialize message: print a message and start processing
        void HandleInitialize();

        /// @brief Handle the process message: print the counter, delay, then post the next process message
        void HandleProcess();

        /// @brief Handle the shutdown message: print a message and stop processing
        void HandleShutdown();

        static QHandle msgQHandle;  ///< This task's message queue
};
