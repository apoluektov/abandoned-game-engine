////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/Direct3D/Direct3D_renderer.h#4 $
// $DateTime: 2009/08/12 19:12:54 $

// Direct3D implementation of renderer.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_RENDERING_DIRECT3D_DIRECT3D_RENDERER_H_INCLUDED
#define ENGINE_RENDERING_DIRECT3D_DIRECT3D_RENDERER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Rendering/Renderer.h"

#include "Engine/Rendering/Direct3D/Direct3D_system.h"

#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Renderer that uses Direct3D 9.0.
class Direct3D_renderer : public Renderer
{
public:

   /// Constructs renderer attached to window with given handle.
   /// \param winow_handle Handle of window that will be used for rendering.
   Direct3D_renderer(HWND window_handle, bool fullscreen);
   // copying is disallowed via base class

// Renderer interface
public:

   /// \see base class for details.
   virtual void add_to_scene(const Colored_sprite&);

   /// \see base class for details.
   virtual void add_to_scene(const Textured_sprite&);

   // \see base class for details.
   virtual void add_to_scene(const Multitextured_2_sprite&);

   /// \see base class for details.
   virtual void render_scene();

   /// \see base class for details.
   virtual void clear_scene();

   /// \see base class for details.
   virtual bool is_focused() const;

   /// \see base class for details.
   virtual void try_restore();

private:

   void set_texture(uint nstage, const Texture_ID& id);
   void set_blending(uint nstage, Blending_mode mode);

   void copy_scene_to_vbuf();
   void draw_to_back_buffer();

private:

   std::vector<Colored_sprite>         m_sprites_colored;
   std::vector<Textured_sprite>        m_sprites_textured;
   std::vector<Multitextured_2_sprite> m_sprites_multitextured;

   HWND                     m_window_handle;
   D3D_system_ptr           m_D3D;
   D3DPRESENT_PARAMETERS    m_present_params;
   D3D_device_ptr           m_device;
   D3D_vertex_buffer_ptr    m_vbuf;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Rendering
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_RENDERING_DIRECT3D_DIRECT3D_RENDERER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
