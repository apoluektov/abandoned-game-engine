////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/src/Direct3D/Direct3D_renderer.cpp#4 $
// $DateTime: 2009/08/12 19:12:54 $

// Direct3D_renderer implementation.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Rendering/Direct3D/Direct3D_renderer.h"

#include "Engine/Logging/Logging.h"

#include "Common/Typedefs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void add_to_vbuf(const Colored_sprite& s, struct D3D_colored_vertex* where);
void add_to_vbuf(const Textured_sprite& s, struct D3D_textured_vertex* where);
void add_to_vbuf(const Multitextured_2_sprite& s, struct D3D_multitextured_2_vertex* where);
D3DPRESENT_PARAMETERS default_present_params(HWND, bool fullscreen);
D3DTEXTUREOP get_direct3d_texture_op(Blending_mode mode);
void init_direct3d_texture_stages();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Direct3D friendly rendering structures.
// 'w' is rhw param for D3DFVF_XYZRHW (see DirectX documentation for details).

struct D3D_colored_vertex
{
   float x, y, z, w;
   D3DCOLOR color;
};

struct D3D_textured_vertex
{
   float x, y, z, w;
   D3DCOLOR color;
   float tu, tv;
};

struct D3D_multitextured_2_vertex
{
   float x, y, z, w;
   D3DCOLOR color;
   float tu0, tv0;
   float tu1, tv1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Direct3D_renderer::Direct3D_renderer(HWND window_handle, bool fullscreen)
   : m_window_handle(window_handle)
   , m_present_params(default_present_params(window_handle, fullscreen))
   , m_device(m_D3D.create_device(window_handle, m_present_params))
   , m_vbuf(m_device.create_vertex_buffer(1 << 20)) // TODO: expose parameter to config
{
   init_direct3d_texture_stages();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::add_to_scene(const Colored_sprite& s)
{
   m_sprites_colored.push_back(s);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::add_to_scene(const Textured_sprite& s)
{
   m_sprites_textured.push_back(s);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::add_to_scene(const Multitextured_2_sprite& s)
{
   m_sprites_multitextured.push_back(s);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::render_scene()
{
   m_device.clear();

   copy_scene_to_vbuf();
   draw_to_back_buffer();
   m_device.present();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::clear_scene()
{
   // clear scene
   m_sprites_colored.clear();
   m_sprites_textured.clear();
   m_sprites_multitextured.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Direct3D_renderer::is_focused() const
{
   return m_device.get_state() == D3D_device_ptr::operational;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::try_restore()
{
   if (is_focused())
   {
      // nothing to do
      LOG_RENDERER(Logging::minor) << "Renderer restoring requested, but it is nothing to restore";
      return;
   }

   if (m_device.get_state() == D3D_device_ptr::lost)
   {
      // can't restore
      LOG_RENDERER(Logging::minor) << "Renderer restoring requested, but can't restore at the moment";
      return;
   }
   else if (m_device.get_state() == D3D_device_ptr::not_reset)
   {
      LOG_RENDERER(Logging::minor) << "Restore renderer";

      // reset device and associated resources
      m_vbuf.reset();
      m_device.reset(m_present_params);
      m_vbuf = m_device.create_vertex_buffer(1 << 20);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::set_texture(uint nstage, const Texture_ID& id)
{
   // reset texture if empty Texture_ID is given; set valid texture otherwise
   if (id == Texture_ID())
   {
      D3D_texture_ptr texture;
      m_device.set_texture(nstage, texture);
   }
   else
   {
      D3D_texture_ptr texture = m_device.create_texture(id.file_name.c_str());
      m_device.set_texture(nstage, texture);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::set_blending(uint nstage, Blending_mode mode)
{
   // set both color and alpha blending mode
   m_device.set_texture_stage_state(nstage, D3DTSS_COLOROP, get_direct3d_texture_op(mode));
   m_device.set_texture_stage_state(nstage, D3DTSS_ALPHAOP, get_direct3d_texture_op(mode));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::copy_scene_to_vbuf()
{
   D3D_vertex_buffer_ptr::Lock lock(m_vbuf);

   // add colored sprites
   void* raw = lock.get_raw_memory();
   D3D_colored_vertex* coords = reinterpret_cast<D3D_colored_vertex*>(raw);
   for (size_t i = 0; i < m_sprites_colored.size(); ++i)
   {
      add_to_vbuf(m_sprites_colored[i], coords + 4*i);
   }

   // add textured sprites
   raw = coords + 4*m_sprites_colored.size();
   D3D_textured_vertex* coords2 = reinterpret_cast<D3D_textured_vertex*>(raw);
   for (size_t i = 0; i < m_sprites_textured.size(); ++i)
   {
      add_to_vbuf(m_sprites_textured[i], coords2 + 4*i);
   }

   // add multitextured sprites
   raw = coords2 + 4*m_sprites_textured.size();
   D3D_multitextured_2_vertex* coords3 = reinterpret_cast<D3D_multitextured_2_vertex*>(raw);
   for (size_t i = 0; i < m_sprites_multitextured.size(); ++i)
   {
      add_to_vbuf(m_sprites_multitextured[i], coords3 + 4*i);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D_renderer::draw_to_back_buffer()
{
   D3D_device_ptr::Scene_guard guard(m_device);

   // reset state
   set_texture(0, Texture_ID());
   set_blending(0, blending_mode_select_arg1);
   set_texture(1, Texture_ID());
   set_blending(1, blending_mode_disable);

   // draw colored sprites
   m_device.set_vertex_buffer(m_vbuf, 0, sizeof(D3D_colored_vertex));
   m_device.set_vertex_format(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
   for (size_t i = 0; i < m_sprites_colored.size(); ++i)
   {
      m_device.draw_primitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
   }

   // draw textured sprites
   // TODO: clean me!!!
   m_device.set_vertex_buffer(m_vbuf, 4*sizeof(D3D_colored_vertex)*m_sprites_colored.size(), sizeof(D3D_textured_vertex));
   m_device.set_vertex_format(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
   for (size_t i = 0; i < m_sprites_textured.size(); ++i)
   {
      set_texture(0, m_sprites_textured[i].texture);
      set_blending(0, m_sprites_textured[i].blending);
      m_device.draw_primitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
   }
   // draw multi-textured sprites
   // TODO: clean me!!!
   m_device.set_vertex_buffer(m_vbuf, 4*sizeof(D3D_colored_vertex)*m_sprites_colored.size()
                                    + 4*sizeof(D3D_textured_vertex)*m_sprites_textured.size(), sizeof(D3D_multitextured_2_vertex));
   m_device.set_vertex_format(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);
   for (size_t i = 0; i < m_sprites_multitextured.size(); ++i)
   {
      set_texture(0, m_sprites_multitextured[i].texture0);
      set_blending(0, m_sprites_multitextured[i].blending0);
      set_texture(1, m_sprites_multitextured[i].texture1);
      set_blending(1, m_sprites_multitextured[i].blending1);
      m_device.draw_primitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3DPRESENT_PARAMETERS default_present_params(HWND window_handle, bool fullscreen)
{
   D3DPRESENT_PARAMETERS present_params;
   present_params.BackBufferWidth            = 1280;                        // TODO: retrieve from hardware!
   present_params.BackBufferHeight           = 1024;
   present_params.BackBufferFormat           = D3DFMT_A8R8G8B8;             // TODO: retrieve from hardware!
   present_params.BackBufferCount            = 1;
   present_params.MultiSampleType            = D3DMULTISAMPLE_NONE;         // no multisampling
   present_params.MultiSampleQuality         = 0;                           // no multisampling
   present_params.SwapEffect                 = D3DSWAPEFFECT_DISCARD;       // discard back-buffer content after present
   present_params.hDeviceWindow              = window_handle;
   present_params.Windowed                   = !fullscreen;
   present_params.EnableAutoDepthStencil     = TRUE;                        // enable auto management of depth buffers
   present_params.AutoDepthStencilFormat     = D3DFMT_D16;                  // 16-bit z-buffer bit depth
   present_params.Flags                      = 0;                           // no flags
   present_params.FullScreen_RefreshRateInHz = 0;                           // use current display settings
   present_params.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE; // the max buffers swap rate

   return present_params;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void add_to_vbuf(const Colored_sprite& s, D3D_colored_vertex* where)
{
   const Vertex<Vertex_format(position | diffuse_color)>* const v = s.vertexes;

   where[0].x = v[0].position.x;
   where[0].y = v[0].position.y;
   where[0].z = v[0].position.z;
   where[0].w = 1;
   where[0].color = D3DCOLOR_ARGB(v[0].color.a, v[0].color.r, v[0].color.g, v[0].color.b);

   where[1].x = v[1].position.x;
   where[1].y = v[1].position.y;
   where[1].z = v[1].position.z;
   where[1].w = 1;
   where[1].color = D3DCOLOR_ARGB(v[1].color.a, v[1].color.r, v[1].color.g, v[1].color.b);

   // note: index mismatch because of drawing triangle strip
   where[2].x = v[3].position.x;
   where[2].y = v[3].position.y;
   where[2].z = v[3].position.z;
   where[2].w = 1;
   where[2].color = D3DCOLOR_ARGB(v[3].color.a, v[3].color.r, v[3].color.g, v[3].color.b);

   where[3].x = v[2].position.x;
   where[3].y = v[2].position.y;
   where[3].z = v[2].position.z;
   where[3].w = 1;
   where[3].color = D3DCOLOR_ARGB(v[2].color.a, v[2].color.r, v[2].color.g, v[2].color.b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void add_to_vbuf(const Textured_sprite& s, D3D_textured_vertex* where)
{
   const Vertex<Vertex_format(position | diffuse_color | texture_coord0)>* const v = s.vertexes;

   where[0].x = v[0].position.x;
   where[0].y = v[0].position.y;
   where[0].z = v[0].position.z;
   where[0].w = 1;
   where[0].color = D3DCOLOR_ARGB(v[0].color.a, v[0].color.r, v[0].color.g, v[0].color.b);
   where[0].tu = v[0].texture_coord.tu;
   where[0].tv = v[0].texture_coord.tv;

   where[1].x = v[1].position.x;
   where[1].y = v[1].position.y;
   where[1].z = v[1].position.z;
   where[1].w = 1;
   where[1].color = D3DCOLOR_ARGB(v[1].color.a, v[1].color.r, v[1].color.g, v[1].color.b);
   where[1].tu = v[1].texture_coord.tu;
   where[1].tv = v[1].texture_coord.tv;

   // note: index mismatch because of drawing triangle strip
   where[2].x = v[3].position.x;
   where[2].y = v[3].position.y;
   where[2].z = v[3].position.z;
   where[2].w = 1;
   where[2].color = D3DCOLOR_ARGB(v[3].color.a, v[3].color.r, v[3].color.g, v[3].color.b);
   where[2].tu = v[3].texture_coord.tu;
   where[2].tv = v[3].texture_coord.tv;

   where[3].x = v[2].position.x;
   where[3].y = v[2].position.y;
   where[3].z = v[2].position.z;
   where[3].w = 1;
   where[3].color = D3DCOLOR_ARGB(v[2].color.a, v[2].color.r, v[2].color.g, v[2].color.b);
   where[3].tu = v[2].texture_coord.tu;
   where[3].tv = v[2].texture_coord.tv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void add_to_vbuf(const Multitextured_2_sprite& s, struct D3D_multitextured_2_vertex* where)
{
   const Vertex<Vertex_format(position | diffuse_color | texture_coord0 | texture_coord1)>* const v = s.vertexes;

   where[0].x = v[0].position.x;
   where[0].y = v[0].position.y;
   where[0].z = v[0].position.z;
   where[0].w = 1;
   where[0].color = D3DCOLOR_ARGB(v[0].color.a, v[0].color.r, v[0].color.g, v[0].color.b);
   where[0].tu0 = v[0].texture_coord0.tu;
   where[0].tv0 = v[0].texture_coord0.tv;
   where[0].tu1 = v[0].texture_coord1.tu;
   where[0].tv1 = v[0].texture_coord1.tv;

   where[1].x = v[1].position.x;
   where[1].y = v[1].position.y;
   where[1].z = v[1].position.z;
   where[1].w = 1;
   where[1].color = D3DCOLOR_ARGB(v[1].color.a, v[1].color.r, v[1].color.g, v[1].color.b);
   where[1].tu0 = v[1].texture_coord0.tu;
   where[1].tv0 = v[1].texture_coord0.tv;
   where[1].tu1 = v[1].texture_coord1.tu;
   where[1].tv1 = v[1].texture_coord1.tv;

   // note: index mismatch because of drawing triangle strip
   where[2].x = v[3].position.x;
   where[2].y = v[3].position.y;
   where[2].z = v[3].position.z;
   where[2].w = 1;
   where[2].color = D3DCOLOR_ARGB(v[3].color.a, v[3].color.r, v[3].color.g, v[3].color.b);
   where[2].tu0 = v[3].texture_coord0.tu;
   where[2].tv0 = v[3].texture_coord0.tv;
   where[2].tu1 = v[3].texture_coord1.tu;
   where[2].tv1 = v[3].texture_coord1.tv;

   where[3].x = v[2].position.x;
   where[3].y = v[2].position.y;
   where[3].z = v[2].position.z;
   where[3].w = 1;
   where[3].color = D3DCOLOR_ARGB(v[2].color.a, v[2].color.r, v[2].color.g, v[2].color.b);
   where[3].tu0 = v[2].texture_coord0.tu;
   where[3].tv0 = v[2].texture_coord0.tv;
   where[3].tu1 = v[2].texture_coord1.tu;
   where[3].tv1 = v[2].texture_coord1.tv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3DTEXTUREOP direct3d_texture_stages[blending_mode_number_of_elements];
// Yeah, global data. But it's used for table methods only (see below), so why bother?

void init_direct3d_texture_stages()
{
   direct3d_texture_stages[blending_mode_modulate]    = D3DTOP_MODULATE;
   direct3d_texture_stages[blending_mode_add]         = D3DTOP_ADD;
   direct3d_texture_stages[blending_mode_select_arg1] = D3DTOP_SELECTARG1;
   direct3d_texture_stages[blending_mode_select_arg2] = D3DTOP_SELECTARG2;
   direct3d_texture_stages[blending_mode_disable]     = D3DTOP_DISABLE;
}

D3DTEXTUREOP get_direct3d_texture_op(Blending_mode mode)
{
   return direct3d_texture_stages[mode];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Rendering
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
