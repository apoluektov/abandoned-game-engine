////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Input/Input_handler.h#1 $
// $DateTime: 2009/08/12 00:03:18 $

// Input handler.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_INPUT_INPUT_HANDLER_H_INCLUDED
#define ENGINE_INPUT_INPUT_HANDLER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Input/Key_code.h"

#include "boost/noncopyable.hpp"

#include <utility>             // for std::pair

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Input
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Mouse coordinates.
typedef std::pair<int, int> Mouse_coords;

/// Mouse wheel delta.
typedef int                 Mouse_wheel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Handler of user input (keyboard and mouse).
class Input_handler : private boost::noncopyable
{
public:

   virtual ~Input_handler() { }
   // copying is disallowed

   virtual bool is_key_pressed(Key_code key)    const          = 0;
   virtual bool is_left_mouse_button_pressed()  const          = 0;
   virtual bool is_right_mouse_button_pressed() const          = 0;
   virtual Mouse_coords get_mouse_coords()      const          = 0;
   virtual Mouse_wheel get_mouse_wheel_delta()  const          = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Input
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_INPUT_INPUT_HANDLER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
