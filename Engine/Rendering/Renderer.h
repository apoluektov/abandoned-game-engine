////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/Renderer.h#2 $
// $DateTime: 2009/08/12 18:29:41 $

// Sprite renderer.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_RENDERING_RENDERER_H_INCLUDED
#define ENGINE_RENDERING_RENDERER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Rendering/Sprite.h"

#include "boost/noncopyable.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Rendering device for sprites.
/// Accumulates sprites into scene and render that scene afterwards.
class Renderer : private boost::noncopyable
{
public:

   virtual ~Renderer() { }
   // copying is disallowed

   /// Adds sprite contained color information to scene.
   virtual void add_to_scene(const Colored_sprite&)             = 0;

   /// Adds textured sprite to scene.
   virtual void add_to_scene(const Textured_sprite&)            = 0;

   // Adds sprite with 2 textures associated to scene.
   virtual void add_to_scene(const Multitextured_2_sprite&)     = 0;

   /// Render scene on screen.
   virtual void render_scene() = 0;

   /// Clear scene: remove all sprites.
   virtual void clear_scene() = 0;

   /// Is renderer in focus? (So user sees scene rendered etc.)
   virtual bool is_focused() const = 0;

   /// Try to restore device if it's not focused.
   /// It is not quaranteed that it would succeed.
   virtual void try_restore() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Rendering
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_RENDERING_RENDERER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
