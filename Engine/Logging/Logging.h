////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Logging/Logging.h#5 $
// $DateTime: 2009/08/12 20:21:49 $

// Provides logging services to application.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_LOGGING_LOGGING_H_INCLUDED
#define ENGINE_LOGGING_LOGGING_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Timing/Timer.h"
#include "Common/Decorated_stream.h"

#include "Common/Typedefs.h"

#include <string>
#include <iostream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Logging
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Level assigned to each message to be logged.
/// Logger assign message level, too.
/// If message's level is less than one of Logger, message is ignored; otherwise message is logged.
enum Message_level
{
   trivial         = 0
   , minor         = 1
   , major         = 2
   , critical      = 3
   , no_log
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Provides logging services to application.
/// Allows filter messages by level and by sender.
/// Note that application responsible for any clashes between sender identifiers.
/// Note that application responsible for correct initializing of Logger.
class Logger
{
public:

   /// Initialize logger with output stream and timer.
   /// Must be called before Logger usage (application responsible for).
   /// \param out Output stream that Logger writes into. If null, std::clog is used.
   /// \param timer Timer for tagging each log entry with timestamp. If null, no timestamp provided.
   static void          init(std::ostream* out, Timing::Timer* timer);

   /// Sets level for all messages to given value.
   static void          set_global_message_level(Message_level lvl);

   /// Sets level for messages logged by sender with given id to given level.
   static void          set_message_level(uchar sender_id, Message_level lvl)     { m_msg_lvls[sender_id] = lvl;  }

   /// Gets message level for sender identified by given id.
   static Message_level get_message_level(uchar sender_id)                        { return m_msg_lvls[sender_id]; }

   /// Puts message info into output stream and returns stream that is automatically flushed.
   static Common::Decorated_stream get_stream(uchar message_id, Message_level lvl);

private:

   static const char*   to_tag(Message_level lvl);

private:

   // no need to create, copy or destroy objects
   Logger();
   Logger(const Logger&);
   Logger& operator=(const Logger&);
   ~Logger();

private:
   static const size_t   m_senders_num = 256;
   static Message_level  m_msg_lvls[m_senders_num];
   static std::string    m_lvl_names[4];
   static std::ostream*  m_out;
   static Timing::Timer* m_timer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Logging
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG(sender_id, msg_lvl) \
   if (msg_lvl < Engine::Logging::Logger::get_message_level(sender_id)) \
   { } \
   else Engine::Logging::Logger::get_stream(sender_id, msg_lvl)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_LOGGING_LOGGING_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
