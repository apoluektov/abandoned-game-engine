////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Logging/test/Logging_default_init.cpp#3 $
// $DateTime: 2009/07/30 12:22:26 $

// Unit-test for Engine.Logging default initialization.
// Note that due to static (non-object) nature of Logger, each unit-test should be separate application.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Logging/Logging.h"

#include "boost/test/unit_test.hpp"

#include <limits>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Engine::Logging;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Checks how default initialization (no custom out and timer) works.
void test_init()
{
   // redirect clog to string stream
   ostringstream strout;
   streambuf* old_buf = clog.rdbuf();
   clog.rdbuf(strout.rdbuf());

   // perform default init
   Logger::init(0, 0);

   // check that for all senders message level is "trivial"
   for (uchar sender_id = 0; sender_id < numeric_limits<uchar>::max(); ++sender_id)
   {
      BOOST_CHECK(Logger::get_message_level(sender_id) == trivial);
   }

   // write some test messages to log
   LOG(0, trivial) << "Message0";
   LOG(1, minor) << "Message1";
   LOG(2, major) << "Message2";
   LOG(3, critical) << "Message3";

   // check that output match etalon
   const string etalon =
      "--------------------------------------------------------------------------------\n"
      "|Time(ms) | Sender ID  |Severity|     Message                                   \n"
      "--------------------------------------------------------------------------------\n"
      "(00000000) (sender:000) Major:   Logger output stream set to std::clog\n"
      "(00000000) (sender:000) Critical: !!! No timer provided; use class Engine::Timing::No_timer as fallback !!!\n"
      "(00000000) (sender:000) Critical: !!!   Messages would not be timestamped !!!\n"
      "(00000000) (sender:000) Trivial: Message0\n"
      "(00000000) (sender:001) Minor:   Message1\n"
      "(00000000) (sender:002) Major:   Message2\n"
      "(00000000) (sender:003) Critical: Message3\n"
      ;

   BOOST_CHECK(etalon == strout.str());

   // restore old clog buffer
   clog.rdbuf(old_buf);

   //clog << strout.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boost::unit_test::test_suite* init_unit_test_suite(int, char** const)
{
   boost::unit_test::test_suite* test = BOOST_TEST_SUITE("Engine.Logging default_init_test");
   test->add(BOOST_TEST_CASE(test_init));

   return test;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
