////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/Direct3D/Direct3D_system.h#5 $
// $DateTime: 2009/08/12 20:22:39 $

// Wrappers around Direct3D 9.0 SDK primitives.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_RENDERING_DIRECT3D_DIRECT3D_SYSTEM_H_INCLUDED
#define ENGINE_RENDERING_DIRECT3D_DIRECT3D_SYSTEM_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Common/Typedefs.h"

#include "Engine/Rendering/Logging.h"

#include "Third_party/Platform/Win32.h"
#include "Third_party/Platform/Direct3D.h"

#include "boost/intrusive_ptr.hpp"
#include "boost/noncopyable.hpp"

#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// boost::intrusive_ptr callbacks

inline void intrusive_ptr_add_ref(IUnknown* ptr)
{
   ptr->AddRef();
}

inline void intrusive_ptr_release(IUnknown* ptr)
{
   ptr->Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D_system_ptr;
class D3D_device_ptr;
class D3D_vertex_buffer_ptr;
class D3D_texture_ptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Thrown if problems with Direct3D stuff occur.
class D3D_exception : public std::runtime_error
{
public:
   D3D_exception(const std::string& msg, HRESULT err_code);

   // std::exception interface
   virtual const char* what() const;

private:
   mutable std::string m_formatted_msg;
   HRESULT             m_err_code;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// IDirect3DVertexBuffer9 wrapper.
class D3D_vertex_buffer_ptr
{
public:
   friend class Lock;
   friend class D3D_device_ptr;
public:

   // default copying is ok

   // TODO: upgrade to boost 1.39 and use boost::intrusive_ptr<>::reset() instead
   /// Resets underlying resource.
   /// It is needed as resource should be manually released on device reset.
   void reset() { m_raw_buffer.swap(boost::intrusive_ptr<IDirect3DVertexBuffer9>()); }

public:

   /// RAII wrapper for IDirect3DVertexBuffer9::Lock()/Unlock() operations.
   class Lock : boost::noncopyable
   {
   public:

      /// Locks vertex buffer.
      Lock(D3D_vertex_buffer_ptr);

      /// Unlocks vertex buffer.
      ~Lock();

      // copying is disallowed

      /// Unlocks vertex buffer.
      /// \note it is safe to call reset() multiple times.
      void reset();

      /// Obtaines pointer to buffer's raw memory.
      void* get_raw_memory();

   private:
      IDirect3DVertexBuffer9* m_vbuf;
      void* m_raw;
   };

private:
   D3D_vertex_buffer_ptr(IDirect3DVertexBuffer9* raw_buffer) : m_raw_buffer(raw_buffer, false) { }
private:
   boost::intrusive_ptr<IDirect3DVertexBuffer9> m_raw_buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// IDirect3DTexture9 wrapper.
class D3D_texture_ptr
{
public:
   friend class D3D_device_ptr;
public:

   D3D_texture_ptr() : m_raw_texture(0) { }
   // default copying is ok

   // TODO: upgrade to boost 1.39 and use boost::intrusive_ptr<>::reset() instead
   /// Resets underlying resource.
   /// It is needed as resource should be manually released on device reset.
   void reset() { m_raw_texture.swap(boost::intrusive_ptr<IDirect3DTexture9>()); }

private:
   D3D_texture_ptr(IDirect3DTexture9* raw_texture) : m_raw_texture(raw_texture, false) { }
private:
   boost::intrusive_ptr<IDirect3DTexture9> m_raw_texture;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// IDirect3DDevice9 wrapper.
class D3D_device_ptr
{
public:
   friend class Scene_guard;
   friend class D3D_system_ptr;

   enum State { operational, lost, not_reset };

public:

   // default copying is ok

   /// Constructs vertex buffer with given capacity.
   /// \param Buffer capacity in bytes.
   D3D_vertex_buffer_ptr create_vertex_buffer(uint bytes);

   /// Loads texture by file_name.
   D3D_texture_ptr       create_texture(const std::string& file_name);

   /// Binds vertex buffer to device data stream.
   /// Wrapper for IDirect3DDevice9::SetStreamSource()
   void set_vertex_buffer(D3D_vertex_buffer_ptr, uint offset, uint vertex_bytes);

   /// Sets current vertex stream declaration.
   /// Wrapper for IDirect3DDevice9::SetFVF()
   void set_vertex_format(DWORD);

   /// Assigns texture to stage for device.
   void set_texture(uint nstage, D3D_texture_ptr);

   /// Sets state value for the currecntly assigned texture.
   void set_texture_stage_state(uint nstage, D3DTEXTURESTAGESTATETYPE type, uint value);

   /// Clears screen.
   void clear();

   /// Renders sequence of primitives from current vertex buffer.
   /// Wrapper for IDirect3DDevice9::DrawPrimitive()
   void draw_primitive(D3DPRIMITIVETYPE, uint offset, uint nvertexes);

   /// Presents back buffer on screen.
   bool present();

   /// Which state is device in?
   State get_state() const;

   /// Resets device.
   /// \note All created resources (buffers, textures) should be reseted as well.
   void reset(D3DPRESENT_PARAMETERS& present_params);

public:

   /// RAII wrapper for IDirect3DDevice9::BeginScene()/EndScene() operations.
   class Scene_guard : boost::noncopyable
   {
   public:

      /// Calls IDirect3DDevice9::BeginScene().
      Scene_guard(D3D_device_ptr);

      /// Calls IDirect3DDevice9::EndScene().
      ~Scene_guard();

      // copying is disallowed

      /// Calls IDirect3DDevice9::EndScene().
      /// \note it is safe to call reset() multiple times.
      void reset();

   private:
      IDirect3DDevice9* m_raw_device;
   };
private:
   D3D_device_ptr(IDirect3DDevice9*);
private:
   boost::intrusive_ptr<IDirect3DDevice9> m_raw_device;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// IDirect3D9 wrapper.
class D3D_system_ptr : boost::noncopyable
{
public:
   D3D_system_ptr();
   ~D3D_system_ptr();

   D3D_device_ptr create_device(HWND window_handle, const D3DPRESENT_PARAMETERS& present_params);

private:
   boost::intrusive_ptr<IDirect3D9> m_raw_D3D;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Rendering
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_RENDERING_DIRECT3D_DIRECT3D_SYSTEM_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
