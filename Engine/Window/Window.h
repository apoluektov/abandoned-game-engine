////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Window/Window.h#7 $
// $DateTime: 2009/08/12 20:22:39 $

// Window interface.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_WINDOW_WINDOW_H_INCLUDED
#define ENGINE_WINDOW_WINDOW_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Third_party/Platform/Win32.h"

#include "boost/noncopyable.hpp"

#include "boost/function.hpp"

#include <map>
#include <string>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Represents Windows(tm) window concept.
class Window : boost::noncopyable
{
public:

   /// Type of message handler callback.
   typedef boost::function<LRESULT (Window*, WPARAM, LPARAM)> handler_func;

public:

   Window(const std::string& caption, int x, int y, int width, int height);
   ~Window();
   // copying is disallowed

   /// \return System handle.
   /// \note Non-const as Window could be changed through this handle.
   HWND get_handle() { return m_window_handle; }

   /// \return Whether windows closing was requested.
   bool is_closing() const { return m_closing; }

   /// Sets handler callback.
   /// \param msg Windows message code to be handled.
   /// \param func Function to handle this message.
   /// \note If message handler was already set for this message, it will be silently overriden.
   void set_message_handler(UINT msg, handler_func func);

   /// Usual Windows(tm) message handling.
   void handle_messages();

private:

   class Window_class : private boost::noncopyable
   {
   public:
      Window_class(const std::string& name, WNDPROC msg_proc);
      ~Window_class();
      // copying is disallowed
   private:
      std::string m_class_name;
   };

private:

   static LRESULT WINAPI msg_proc(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param);
   static LRESULT handle_close(Window*, WPARAM, LPARAM);
   static LRESULT handle_destroy(Window*, WPARAM, LPARAM);
   static LRESULT handle_system_key(Window*, WPARAM, LPARAM);

private:

   Window_class m_window_class;
   HWND         m_window_handle;
   bool         m_closing;
   std::map<UINT, handler_func> m_msg_handlers;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Thrown if any problems with window occur.
class Win32_exception : public std::runtime_error
{
public:

   /// \param msg Error message.
   /// \param last_err Code of last OS error.
   Win32_exception(const std::string& msg, DWORD last_err);

   // std::exception interface
   virtual const char* what() const;

private:

   mutable std::string m_formatted_msg;
   DWORD m_last_err;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_WINDOW_WINDOW_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
