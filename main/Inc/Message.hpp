#pragma once

/// @file Message.hpp
/// @brief Message posted to a task's message queue.

/// @brief Message posted to a task's message queue to tell the task what to do
class Message
{
    public:

        /// @brief Type of message
        enum MsgId
        {
            INVALID,        ///< Not a valid message (default)
            INITIALIZE,     ///< Initialize the task
            PROCESS,        ///< Do some processing
            SHUTDOWN,       ///< Shut down the task
            NUM_MSG_IDS     ///< Number of message IDs (not a valid message)
        };

        /// @brief Constructor
        /// @param[in] id - ID of this message
        Message(MsgId id = INVALID) : msgId{id} {}

        MsgId msgId;    ///< ID of this message
};
