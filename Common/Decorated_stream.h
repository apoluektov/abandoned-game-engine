////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Common/Decorated_stream.h#3 $
// $DateTime: 2009/08/11 18:52:15 $

// Decorated stream that add new-line and flushes its output.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_DECORATED_STREAM_H_INCLUDED
#define COMMON_DECORATED_STREAM_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Common
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Future directions: code could be generalized to prefix/suffix output in user-provided manner.

/// Automagically flush output after end of chain of output statement.
/// That is, following code
///   decorated_stream << 1 << 2 << 3;
///   decorated_stream << 4 << 5 << 6;
/// produces output:
/// "123"
/// "456"
/// Implementation idea is to construct temporary object (guard) that will be destroyed at the end of statement.
class Decorated_stream
{
public:
   class Guard;
public:

   Decorated_stream(std::ostream& out) : m_out(out) { }
   // default copying is ok

   template <class T>
   Guard operator<<(const T& value) { return Guard(&m_out, value); }

public:

   /// This object is created as a temporary wrapper around std::ostream.
   /// At the end of full expression it destroyed and flush output.
   class Guard
   {
   public:

      // Need two functions: for first output and for all of the next ones.
      // Thus, Decorated_stream::operator<< creates Guard, but all next outputs
      //  are catched by Guard and correspond just usual output to std::ostream.

      template <class T>
      Guard(std::ostream* out, const T& first) : m_out(out)     { (*m_out) << first;  }

      template <class T>
      std::ostream& operator <<(const T& next) const            { return (*m_out) << next; }

      ~Guard()                                                  { (*m_out) << std::endl; }

   private:

      std::ostream* m_out;
   };

private:

   std::ostream& m_out;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Common

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMON_DECORATED_STREAM_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
