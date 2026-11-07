#pragma once

/// @file Task2.hpp
/// @brief Example task that prints a large block of output periodically.

#include "Message.hpp"
#include "TargetPort.hpp"

/// @brief Example task that prints the task list, run time stats and printf buffer high water mark
/// @details Prints whenever no message has been received for 5 seconds. Used to measure the time taken by
///          many printf calls that produce more output than the UART can hold.
class Task2
{
    public:

        /// @brief Create this task's message queue and post the initialize message to it
        /// @note Must be called before Run().
        void Initialize();

        /// @brief Post a message for this task to do processing
        void Process();

        /// @brief Execution of Task2
        /// @details Main loop that waits for messages posted to the queue and processes them. Posts a process
        ///          message to itself when no message is received within 5 seconds. Never returns.
        void Run();

        /// @brief Post a message to shut down this task
        void Shutdown();

    private:

        /// @brief Post a message to this task's queue
        /// @param[in] id - ID of the message to send
        void SendMessage(Message::MsgId id);

        /// @brief Handle the initialize message: print a message
        void HandleInitialize();

        /// @brief Handle the process message: print the task list, run time stats and high water mark
        void HandleProcess();

        /// @brief Handle the shutdown message: print a message and stop processing
        void HandleShutdown();

        static QHandle msgQHandle;  ///< This task's message queue
};
