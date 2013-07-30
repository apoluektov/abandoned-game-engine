////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Window/Window.cpp#8 $
// $DateTime: 2009/08/14 12:07:51 $

// Window implementation.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Window/Window.h"

#include "Engine/Logging/Logging.h"

#include "boost/format.hpp"      // for exception formatting

#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Logging
{

const uchar sender_window = 2;

#define LOG_WINDOW(message_level) LOG(Logging::sender_window, message_level)

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND create_window(const std::string& caption, int x, int y, int width, int height);
void log_then_throw(const Win32_exception& ex, Logging::Message_level message_level);
void set_window_long_ptr(HWND window_handle, int index, LONG_PTR new_long);
LONG_PTR get_window_long_ptr(HWND window_handle, int index);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window::Window(const std::string& caption, int x, int y, int width, int height)
   : m_window_class(caption, msg_proc)
   , m_window_handle(create_window(caption, x, y, width, height))
   , m_closing(false)
{
   set_window_long_ptr(m_window_handle, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));

   set_message_handler(WM_CLOSE,   handle_close);
   set_message_handler(WM_DESTROY, handle_destroy);
   set_message_handler(WM_SYSKEYDOWN, handle_system_key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window::~Window()
{
   LOG_WINDOW(Logging::major) << "Window is destroying ...";
   BOOL status = ::DestroyWindow(m_window_handle);

   // can't handle failure in destructor; just log it
   if (status)
   {
      LOG_WINDOW(Logging::major) << "... Window destroyed";
   }
   else
   {
      LOG_WINDOW(Logging::critical) << "!!! DestroyWindow() failed; GetLastError() returns "
                                    << ::GetLastError();
   }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Window::set_message_handler(UINT msg, handler_func func)
{
   LOG_WINDOW(Logging::minor) << "Handler set to " << msg << " OS message";
   m_msg_handlers[msg] = func;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Window::handle_messages()
{
   LOG_WINDOW(Logging::trivial) << "(Handling window messages)";

   MSG msg = { };
   if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
   {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      // not interesting in return values
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT WINAPI Window::msg_proc(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param)
{
   Window* window = reinterpret_cast<Window*>(get_window_long_ptr(window_handle, GWL_USERDATA));
   if (!window)
   {
      return ::DefWindowProc(window_handle, msg, w_param, l_param);
   }

   std::map<UINT, handler_func>::const_iterator it = window->m_msg_handlers.find(msg);
   if (it != window->m_msg_handlers.end())
   {
      return (it->second)(window, w_param, l_param);
   }
   else
   {
      return ::DefWindowProc(window_handle, msg, w_param, l_param);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT Window::handle_close(Window* window, WPARAM, LPARAM)
{
   LOG_WINDOW(Logging::trivial) << "(Handling closing window)";
   window->m_closing = true;
   // and let window decide what to do
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT Window::handle_destroy(Window* window, WPARAM, LPARAM)
{
   LOG_WINDOW(Logging::trivial) << "(Handling destroying window)";
   ::PostQuitMessage(0); // no return value
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT Window::handle_system_key(Window* window, WPARAM w_param, LPARAM)
{
   LOG_WINDOW(Logging::trivial) << "(Handling system key pressed ...)";
   if (w_param == VK_F4)
   {
      LOG_WINDOW(Logging::trivial) << "(... Alt+F4 pressed; should close)";
      window->m_closing = true;
   }
   else
   {
      LOG_WINDOW(Logging::trivial) << "(... Ahh, nothing interesting)";
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND create_window(const std::string& caption, int x, int y, int width, int height)
{
   LOG_WINDOW(Logging::major) << "Window is creating ...";
   HWND window_handle = ::CreateWindow(
      caption.c_str(),       // window class name
      caption.c_str(),       // caption
      WS_OVERLAPPEDWINDOW,   // window style
      x, y,                  // initial position
      width, height,         // width and height
      NULL,                  // handle to parent window (FIXME: should indeed store handle to parent window)
      NULL,                  // menu: no menu
      ::GetModuleHandle(NULL), // instance handle
      NULL);                 // lpParam; unused

   if (window_handle == 0)
   {
      log_then_throw(Win32_exception("CreateWindow() failed", ::GetLastError()), Logging::critical);
   }

   ::ShowWindow(window_handle, SW_SHOWDEFAULT);
   // return value means previos visibility; ignore

   if (::UpdateWindow(window_handle) == FALSE)
   {
      log_then_throw(Win32_exception("UpdateWindow() failed", ::GetLastError()), Logging::critical);
   }

   LOG_WINDOW(Logging::major) << "... Window created succesfully";
   return window_handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window::Window_class::Window_class(const std::string& name, WNDPROC msg_proc)
   : m_class_name(name)
{
   // http://msdn.microsoft.com/en-us/library/ms633577(VS.85).aspx

   LOG_WINDOW(Logging::minor) << "Registering window class \"" << m_class_name << "\"...";
   WNDCLASSEX wc =
      {
         sizeof(WNDCLASSEX),
         CS_CLASSDC,            // style: one device context to be shared by all windows in the class
         msg_proc,              // window procedure
         0L,                    // extra bytes to allocate following window-class structure: unused
         0L,                    // extra bytes to allocate following window instance: unused
         ::GetModuleHandle(NULL), // handle to the instance
         NULL,                  // icon: default icon
         NULL,                  // cursor: no cursor
         NULL,                  // handle to background brush: app paint its own background
         NULL,                  // menu: no default menu
         m_class_name.c_str(),    // class name: same as window caption
         NULL                   // small icon: same as icon
      };

   if (::RegisterClassEx(&wc) == 0)
   {
      log_then_throw(Win32_exception("RegisterClassEx() fails", ::GetLastError()), Logging::critical);
   }
   LOG_WINDOW(Logging::minor) << "... window class registered";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window::Window_class::~Window_class()
{
   LOG_WINDOW(Logging::minor) << "Unregistering window class ...";
   BOOL success = ::UnregisterClass(m_class_name.c_str(), ::GetModuleHandle(NULL));

   // by design, (1) window class with such name exists and (2) no open window remained (ensured by Window class)
   // so UnregisterClass() shouldn't fail.

   if (!success)
   {
      LOG_WINDOW(Logging::critical) << "!!! Unregistering window class FAILED (but can't be handled)";
   }
   else
   {
      LOG_WINDOW(Logging::minor) << "... window class unregistered";
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Win32_exception::Win32_exception(const std::string& msg, DWORD last_err)
   : std::runtime_error(msg)
   , m_last_err(last_err)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Win32_exception::what() const
{
   try
   {
      // format message on demand only
      m_formatted_msg = (boost::format("%1%; GetLastError() returns %2%") % std::runtime_error::what() % m_last_err).str();
      return m_formatted_msg.c_str();
   }
   catch (...)
   {
      return "<Sorry, exception formatting failed>";
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void log_then_throw(const Win32_exception& ex, Logging::Message_level message_level)
{
   LOG_WINDOW(message_level) << "!!! Throw exception !!!";
   LOG_WINDOW(message_level) << "!!!   Type:    " << typeid(ex).name() << " !!!";
   LOG_WINDOW(message_level) << "!!!   Message: " << ex.what() << "!!!";

   throw ex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper to handle weird behavior of SetWindowLongPtr.
void set_window_long_ptr(HWND window_handle, int index, LONG_PTR new_long)
{
   // see http://msdn.microsoft.com/en-us/library/ms644898(VS.85).aspx "Return Value" for details

   ::SetLastError(0);
   if (::SetWindowLongPtr(window_handle, index, new_long) == 0 && ::GetLastError() != 0)
   {
      log_then_throw(Win32_exception("SetWindowLongPtr() failed", ::GetLastError()), Logging::critical);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \see set_window_long_ptr for details.
LONG_PTR get_window_long_ptr(HWND window_handle, int index)
{
   ::SetLastError(0);
   LONG_PTR val = ::GetWindowLongPtr(window_handle, index);
   if (val == 0 && ::GetLastError() != 0)
   {
      log_then_throw(Win32_exception("GetWindowLongPtr() failed", ::GetLastError()), Logging::critical);
   }
   return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
