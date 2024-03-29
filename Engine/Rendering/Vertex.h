////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Engine/Rendering/Vertex.h#1 $
// $DateTime: 2009/08/05 15:00:45 $

// Vertex.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_RENDERING_VERTEX_H_INCLUDED
#define ENGINE_RENDERING_VERTEX_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Rendering/Primitives.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Vertex primary template.
/// Specializations typically hold graphical data that depend on Vertex_format.
template <Vertex_format>
struct Vertex;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Vertex that holds position and diffuse color.
template <>
struct Vertex<Vertex_format(position | diffuse_color)>
{
   Position      position;
   Diffuse_color color;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Vertex that holds position, diffuse color and texture coordinate.
template <>
struct Vertex<Vertex_format(position | diffuse_color | texture_coord0)>
{
   Position       position;
   Diffuse_color  color;
   Texture_coord  texture_coord;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Vertex that holds position, diffuse color and two texture coordinates (for multitexturing).
template <>
struct Vertex<Vertex_format(position | diffuse_color | texture_coord0 | texture_coord1)>
{
   Position       position;
   Diffuse_color  color;
   Texture_coord  texture_coord0;
   Texture_coord  texture_coord1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Rendering
} // namespace Engine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENGINE_RENDERING_VERTEX_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
