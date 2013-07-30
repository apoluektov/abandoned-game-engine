////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Logging/test/Logging_set_global_message_level.cpp#3 $
// $DateTime: 2009/07/30 12:22:26 $

// Unit-test for Engine.Logging set_global_message_level().
// Note that due to static (non-object) nature of Logger, each unit-test should be separate application.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Logging/Logging.h"
#include "Dummy_timer.h"

#include "boost/test/unit_test.hpp"

#include "boost/lexical_cast.hpp"

#include <limits>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Engine::Logging;
using namespace Engine::Timing;
using boost::lexical_cast;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Checks how set_global_message_level() works.
void test_message_level()
{
   // perform init with custom output and dummy timer
   ostringstream strout;
   Dummy_timer timer;
   Logger::init(&strout, &timer);

   // check that for all senders message level is "trivial"
   for (uchar sender_id = 0; sender_id < numeric_limits<uchar>::max(); ++sender_id)
   {
      BOOST_CHECK(Logger::get_message_level(sender_id) == trivial);
   }

   // set message level for all senders to "major"
   Logger::set_global_message_level(major);

   // check that for all senders message level is "major"
   for (uchar sender_id = 0; sender_id < numeric_limits<uchar>::max(); ++sender_id)
   {
      BOOST_CHECK(Logger::get_message_level(sender_id) == major);
   }

   // write some test messages to log
   // note, that only messages with level greater than "major" should be logged
   LOG(0, trivial) << "Message0";
   LOG(1, minor) << "Message1";
   LOG(2, major) << "Message2";
   LOG(3, critical) << "Message3";

   const string out_address = lexical_cast<string>(&strout);
   const string timer_address = lexical_cast<string>(&timer);

   // check that output match etalon
   const string etalon =
      "--------------------------------------------------------------------------------\n"
      "|Time(ms) | Sender ID  |Severity|     Message                                   \n"
      "--------------------------------------------------------------------------------\n"
      "(00000000) (sender:000) Major:   Logger output stream set to class std::basic_ostringstream<char,struct std::char_traits<char>,class std::allocator<char> >, object is " + out_address + "\n"
      "(00000000) (sender:000) Major:   Logger timer set to class Dummy_timer, object is " + timer_address + "\n"
      "(00000001) (sender:002) Major:   Message2\n"
      "(00000002) (sender:003) Critical: Message3\n"
      ;

   BOOST_CHECK(etalon == strout.str());

   //clog << strout.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boost::unit_test::test_suite* init_unit_test_suite(int, char** const)
{
   boost::unit_test::test_suite* test = BOOST_TEST_SUITE("Logging_set_global_message_level");
   test->add(BOOST_TEST_CASE(test_message_level));

   return test;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
