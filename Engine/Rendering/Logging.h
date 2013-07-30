////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/Logging.h#1 $
// $DateTime: 2009/08/05 15:00:45 $

// Logging of rendering stuff.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_RENDERING_LOGGING_H_INCLUDED
#define ENGINE_RENDERING_LOGGING_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Logging/Logging.h"

#include "Common/Typedefs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Logging
{

const uchar sender_renderer = 3;

#define LOG_RENDERER(message_level) LOG(Engine::Logging::sender_renderer, message_level)

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_RENDERING_LOGGING_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////