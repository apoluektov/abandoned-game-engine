////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Input/src/Win32_input_handler.cpp#6 $
// $DateTime: 2009/08/12 20:22:39 $

// Win32_input_handler implementation.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Input/Win32_input_handler.h"

#include "Engine/Window/Window.h"

#include "Engine/Logging/Logging.h"

#include "boost/bind.hpp"
#include "boost/ref.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Logging
{

const uchar sender_input_handler = 4;

#define LOG_INPUT(message_level) LOG(Engine::Logging::sender_input_handler, message_level)

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Input
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT update_kbd_state  (Engine::Window*, WPARAM w_param, LPARAM l_param, Input_state&);
LRESULT update_mouse_state(Engine::Window*, WPARAM w_param, LPARAM l_param, Input_state&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Stores retrieved input state.
struct Input_state
{
   BYTE key_buf[256];
   int mouse_buttons_pressed;
   int mouse_x;
   int mouse_y;
   int wheel_delta;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Win32_input_handler::Win32_input_handler(Engine::Window& window)
   : m_input_state(new Input_state())
{
   LOG_INPUT(Logging::major) << "Win32 input hanlder initializing ...";

   Engine::Window::handler_func update_kbd_func
      =  boost::bind(update_kbd_state, _1, _2, _3, boost::ref(*m_input_state));
   Engine::Window::handler_func update_mouse_func
      =  boost::bind(update_mouse_state, _1, _2, _3, boost::ref(*m_input_state));

   // add keyboard and mouse specific message handling

   window.set_message_handler(WM_KEYDOWN,         update_kbd_func);
   window.set_message_handler(WM_KEYUP,           update_kbd_func);

   // TODO: probably different handlers for up/down because of SetCapture()/ReleaseCapture()
   window.set_message_handler(WM_LBUTTONDOWN,     update_mouse_func);
   window.set_message_handler(WM_LBUTTONUP,       update_mouse_func);
   window.set_message_handler(WM_RBUTTONUP,       update_mouse_func);
   window.set_message_handler(WM_RBUTTONDOWN,     update_mouse_func);
   window.set_message_handler(WM_MOUSEMOVE,       update_mouse_func);
   window.set_message_handler(WM_MOUSEWHEEL,      update_mouse_func);

   LOG_INPUT(Logging::major) << "... Win32 input hanlder initialized";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Win32_input_handler::~Win32_input_handler()
{
   LOG_INPUT(Logging::major) << "Win32 input hanlder destroyed";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Win32_input_handler::is_key_pressed(Key_code key) const
{
   return (m_input_state->key_buf[key] & 0x80) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Win32_input_handler::is_left_mouse_button_pressed() const
{
   return (m_input_state->mouse_buttons_pressed & MK_LBUTTON) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Win32_input_handler::is_right_mouse_button_pressed() const
{
   return (m_input_state->mouse_buttons_pressed & MK_RBUTTON) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mouse_coords Win32_input_handler::get_mouse_coords() const
{
   return Mouse_coords(m_input_state->mouse_x, m_input_state->mouse_y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mouse_wheel Win32_input_handler::get_mouse_wheel_delta() const
{
   return m_input_state->wheel_delta;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT update_kbd_state(Engine::Window*, WPARAM w_param, LPARAM l_param, Input_state& state)
{
   LOG_INPUT(Logging::trivial) << "(Handling keyboard state updating)";

   if (!::GetKeyboardState(state.key_buf))
   {
      LOG_INPUT(Logging::critical) << "!!! GetKeyboardState() failed; throw";
      throw Engine::Win32_exception("GetKeyboardState() failed", ::GetLastError());
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT update_mouse_state(Engine::Window*, WPARAM w_param, LPARAM l_param, Input_state& state)
{
   LOG_INPUT(Logging::trivial) << "(Handling mouse state updating)";

   state.mouse_buttons_pressed = GET_KEYSTATE_WPARAM(w_param);
   state.wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param);
   state.mouse_x = GET_X_LPARAM(l_param);
   state.mouse_y = GET_Y_LPARAM(l_param);

   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Input
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
