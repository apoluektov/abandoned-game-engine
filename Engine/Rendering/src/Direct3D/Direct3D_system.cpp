////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/src/Direct3D/Direct3D_system.cpp#6 $
// $DateTime: 2009/08/14 12:07:51 $

// Implementation of Direct3D 9.0 SDK wrappers.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Rendering/Direct3D/Direct3D_system.h"

#include "Engine/Rendering/Logging.h"

#include "Third_party/Platform/Direct3D.h"

#include "boost/format.hpp"

#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_system_ptr::D3D_system_ptr()
   : m_raw_D3D(::Direct3DCreate9(D3D_SDK_VERSION), false)
{
   if (!m_raw_D3D)
   {
      LOG_RENDERER(Logging::critical) << "!!! Direct3D initialization failed; throw exception !!!";
      throw D3D_exception("Direct3DCreate9() failed", E_FAIL);
   }
   LOG_RENDERER(Logging::major) << "Direct3D initialized";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_system_ptr::~D3D_system_ptr()
{
   LOG_RENDERER(Logging::major) << "Direct3D destroyed";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_device_ptr D3D_system_ptr::create_device(HWND window_handle, const D3DPRESENT_PARAMETERS& present_params)
{
   LOG_RENDERER(Logging::major) << "Initializing Direct3D device ...";
   IDirect3DDevice9* raw_device = 0;
   HRESULT hr = m_raw_D3D->CreateDevice( // TODO: comment me!!!
      D3DADAPTER_DEFAULT,
      D3DDEVTYPE_HAL,
      window_handle,
      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
      const_cast<D3DPRESENT_PARAMETERS*>(&present_params), // pray that CreateDevice() doesn't change present_params
      &raw_device);

   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! Direct3D device initialization FAILED !!!\n";
      throw D3D_exception("Can't create D3D device", hr);
   }
   if (!raw_device)
   {
      LOG_RENDERER(Logging::critical) << "!!! Direct3D device initialization FAILED !!!\n";
      throw D3D_exception("Can't create D3D device: null pointer returned", E_FAIL);
   }

   LOG_RENDERER(Logging::major) << "... Direct3D device initialized";
   return raw_device;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_render_state(IDirect3DDevice9* device, D3DRENDERSTATETYPE state, DWORD value);

D3D_device_ptr::D3D_device_ptr(IDirect3DDevice9* raw_device)
   : m_raw_device(raw_device, false)
{
   // TODO: more advanced precondition/postcondition system
   assert(m_raw_device);

   set_render_state(m_raw_device.get(), D3DRS_CULLMODE, D3DCULL_NONE);
   set_render_state(m_raw_device.get(), D3DRS_LIGHTING, FALSE);
   set_render_state(m_raw_device.get(), D3DRS_ZENABLE, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_vertex_buffer_ptr D3D_device_ptr::create_vertex_buffer(UINT size)
{
   IDirect3DVertexBuffer9* raw_vbuf;
   HRESULT hr = m_raw_device->CreateVertexBuffer(size, 0, 0, D3DPOOL_DEFAULT, &raw_vbuf, 0); // TODO: comment me!!!
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! Can't create vertex buffer; throw !!!";
      throw D3D_exception("IDirect3DDevice9::CreateVertexBuffer() failed", hr);
   }
   if (!raw_vbuf)
   {
      LOG_RENDERER(Logging::critical) << "!!! Can't create vertex buffer; throw !!!\n";
      throw D3D_exception("Can't create vertex buffer: null pointer returned", E_FAIL);
   }

   return raw_vbuf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_texture_ptr D3D_device_ptr::create_texture(const std::string& file_name)
{
   IDirect3DTexture9* raw_texture;
   HRESULT hr = D3DXCreateTextureFromFileEx(m_raw_device.get(), file_name.c_str(),
                                            D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
                                            D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &raw_texture);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! Can't create vertex buffer texture from file \""
                                      << file_name << "\"; throw !!!";
      throw D3D_exception("D3DXCreateTextureFromFile() failed", hr);
   }

   return raw_texture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::set_vertex_buffer(D3D_vertex_buffer_ptr buf, uint offset, uint vertex_bytes)
{
   HRESULT hr = m_raw_device->SetStreamSource(0, buf.m_raw_buffer.get(), offset, vertex_bytes);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::SetStreamSource() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::SetStreamSource() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::set_vertex_format(DWORD fvf)
{
   HRESULT hr = m_raw_device->SetFVF(fvf);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::SetFVF() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::SetFVF() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::set_texture(uint nstage, D3D_texture_ptr texture)
{
   HRESULT hr = m_raw_device->SetTexture(nstage, texture.m_raw_texture.get());
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::SetTexture() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::SetTexture() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::set_texture_stage_state(uint nstage, D3DTEXTURESTAGESTATETYPE type, uint value)
{
   HRESULT hr = m_raw_device->SetTextureStageState(nstage, type, value);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::SetTextureStageState() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::SetTextureStageState() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::clear()
{
   HRESULT hr = m_raw_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::Clear() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::Clear() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::draw_primitive(D3DPRIMITIVETYPE type, uint offset, uint nvertexes)
{
   HRESULT hr = m_raw_device->DrawPrimitive(type, offset, nvertexes);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::DrawPrimitive() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9:::DrawPrimitive() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool D3D_device_ptr::present()
{
   HRESULT hr = m_raw_device->Present(0, 0, 0, 0);
   if (hr == D3D_OK)
   {
      return true;
   }
   else if (hr == D3DERR_DEVICELOST)
   {
      LOG_RENDERER(Logging::minor) << "Device lost";

      return false;
   }
   else
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::Present() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::Present() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_device_ptr::State D3D_device_ptr::get_state() const
{
   HRESULT hr = m_raw_device->TestCooperativeLevel();
   switch (hr)
   {
   case D3D_OK:
      return operational;
   case D3DERR_DEVICELOST:
      return lost;
   case D3DERR_DEVICENOTRESET:
      return not_reset;
   case D3DERR_DRIVERINTERNALERROR:
      LOG_RENDERER(Logging::critical)
         << "!!! IDirect3DDevice9::TestCooperativeLevel() return D3DERR_DRIVERINTERNALERROR !!!";
      throw D3D_exception("IDirect3DDevice9::TestCooperativeLevel() return D3DERR_DRIVERINTERNALERROR", hr);
   }
   assert(false && "Should not reach here");
   return lost;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::reset(D3DPRESENT_PARAMETERS& present_params)
{
   if (m_raw_device->Reset(&present_params) != D3D_OK)
   {
      LOG_RENDERER(Logging::minor) << "Reset failed";
   }

   set_render_state(m_raw_device.get(), D3DRS_CULLMODE, D3DCULL_NONE);
   set_render_state(m_raw_device.get(), D3DRS_LIGHTING, FALSE);
   set_render_state(m_raw_device.get(), D3DRS_ZENABLE, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_device_ptr::Scene_guard::Scene_guard(D3D_device_ptr device)
   : m_raw_device(device.m_raw_device.get())
{
   if (m_raw_device)
   {
      HRESULT hr = m_raw_device->BeginScene();
      if (hr != D3D_OK)
      {
         LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::EndScene() failed; throw !!!";
         throw D3D_exception("ISirect3DDevice9::EndScene() failed", hr);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_device_ptr::Scene_guard::~Scene_guard()
{
   reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_device_ptr::Scene_guard::reset()
{
   if (m_raw_device)
   {
      HRESULT hr = m_raw_device->EndScene();
      m_raw_device = 0;

      if (hr != D3D_OK)
      {
         LOG_RENDERER(Logging::critical)
            << "!!! IDirect3DDevice9::EndScene() failed; CANNOT THROW IN CLEANUP FUNCTION - ignore !!!";
      }
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_vertex_buffer_ptr::Lock::Lock(D3D_vertex_buffer_ptr buf)
   : m_vbuf(buf.m_raw_buffer.get())
   , m_raw(0)
{
   if (m_vbuf)
   {
      HRESULT hr = m_vbuf->Lock(0, 0, &m_raw, 0);
      if (hr != D3D_OK)
      {
         LOG_RENDERER(Logging::critical) << "!!! Can't lock vertex buffer; throw !!!";
         throw D3D_exception("IDirect3DVertexBuffer9::Lock() failed", hr);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_vertex_buffer_ptr::Lock::~Lock()
{
   reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D_vertex_buffer_ptr::Lock::reset()
{
   if (m_vbuf)
   {
      HRESULT hr = m_vbuf->Unlock();
      m_vbuf = 0;

      if (hr != D3D_OK)
      {
         LOG_RENDERER(Logging::critical) << "!!! Can't unlock vertex buffer; CANNOT THROW IN CLEANUP FUNCTION - ignore !!!";
      }
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* D3D_vertex_buffer_ptr::Lock::get_raw_memory()
{
   return m_raw;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_render_state(IDirect3DDevice9* device, D3DRENDERSTATETYPE state, DWORD value)
{
   HRESULT hr = device->SetRenderState(state, value);
   if (hr != D3D_OK)
   {
      LOG_RENDERER(Logging::critical) << "!!! IDirect3DDevice9::SetRenderState() failed; throw !!!";
      throw D3D_exception("IDirect3DDevice9::SetRenderState() failed", hr);
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D_exception::D3D_exception(const std::string& msg, HRESULT err_code)
   : std::runtime_error(msg)
   , m_err_code(err_code)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* D3D_exception::what() const
{
   try
   {
      // format message on demand only
      m_formatted_msg = (boost::format("%1%; HRESULT code is %2%") % std::runtime_error::what() % m_err_code).str();
      return m_formatted_msg.c_str();
   }
   catch (...)
   {
      return "<Sorry, exception formatting failed>";
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Rendering
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
