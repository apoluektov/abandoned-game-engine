////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Logging/src/Logging.cpp#3 $
// $DateTime: 2009/07/30 12:22:26 $

// Provides logging services to application.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Logging/Logging.h"

#include <iomanip>              // for std::setw and std::setfill
#include <typeinfo>
#include <algorithm>            // for std::fill

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Timing
{

/// Fallback timer stub.
class No_timer : public Timing::Timer
{
public:

   Timing::Milliseconds get_app_time() const { return 0; }
};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Logging
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_heading(std::ostream& out)
{
   // print heading
   out << "--------------------------------------------------------------------------------\n";
   out << "|Time(ms) | Sender ID  |Severity|     Message                                   \n" ;
   out << "--------------------------------------------------------------------------------\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Logger::init(std::ostream* out, Timing::Timer* timer)
{
   // log all messages unless application won't change it
   set_global_message_level(trivial);

   // names for message levels
   m_lvl_names[trivial]    = "Trivial:";
   m_lvl_names[minor]      = "Minor:  ";
   m_lvl_names[major]      = "Major:  ";
   m_lvl_names[critical]   = "Critical:";

   static Timing::No_timer dummy;
   m_timer = &dummy;

   // init output stream
   if (out)
   {
      m_out = out;
      print_heading(*m_out);
      get_stream(0, major) << "Logger output stream set to " << typeid(*out).name() << ", object is " << m_out;
   }
   else
   {
      // fallback
      m_out = &std::clog;
      print_heading(*m_out);
      get_stream(0, major) << "Logger output stream set to std::clog";
   }


   // init timer
   if (timer)
   {
      m_timer = timer;
      get_stream(0, major) << "Logger timer set to " << typeid(*timer).name() << ", object is " << m_timer;
   }
   else
   {
      // fallback: using No_timer
      get_stream(0, critical) << "!!! No timer provided; use " << typeid(*m_timer).name() << " as fallback !!!";
      get_stream(0, critical) << "!!!   Messages would not be timestamped !!!";
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Logger::set_global_message_level(Message_level lvl)
{
   std::fill(m_msg_lvls, m_msg_lvls + m_senders_num, lvl);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Common::Decorated_stream Logger::get_stream(uchar sender_id, Message_level lvl)
{
   // prepend message with "(<timestamp>) (sender:<id>) <level>: " info
   *m_out << '(' << std::setw(8) << std::setfill('0') << m_timer->get_app_time() << ')'
          << " (sender:" << std::setw(3) << std::setfill('0') << static_cast<int>(sender_id) << ") " << to_tag(lvl) << " ";

   return *m_out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Logger::to_tag(Message_level lvl)
{
   return m_lvl_names[lvl].c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// definition of static members
Message_level Logger::m_msg_lvls[m_senders_num];
std::ostream* Logger::m_out;
std::string Logger::m_lvl_names[4];
Timing::Timer* Logger::m_timer;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Logging

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
