/*
 * Copyright (c) 2013 David Wicks, sansumbrella.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#include "puptent/PupTent.h"

namespace puptent
{
  struct Vertex
  {
    Vertex( const ci::Vec2f &position = ci::Vec2f::zero(),
            const ci::ColorA8u &color = ci::ColorA8u::white(),
            const ci::Vec2f &uv = ci::Vec2f::zero() ):
    position( position ),
    color( color ),
    tex_coord( uv )
    {}
    ci::Vec2f     position;
    ci::ColorA8u  color;
    ci::Vec2f     tex_coord;
  };
  /**
   RenderMesh:
   Collection of vertices suitable for rendering as a triangle strip.
   Drawn by the RenderSystem
   Updated by various systems that want content to be visible
  */
  typedef std::shared_ptr<class RenderMesh> RenderMeshRef;
  class SpriteData;
  struct RenderMesh : Component<RenderMesh>
  {
    RenderMesh( int vertex_count=3 )
    {
      vertices.assign( vertex_count, Vertex{} );
    }
    //! vertices in triangle_strip order
    std::vector<Vertex> vertices;
    //! Convenience method for making circular shapes
    //! If you aren't dynamically changing the circle, consider using a Sprite
    void setAsCircle( const ci::Vec2f &radius, float start_radians=0, float end_radians=M_PI * 2, size_t segments=0 );
    //! Set the mesh bounds to a box shape
    void setAsBox( const ci::Rectf &bounds );
    //! Set the texture coords to those specified by the sprite data
    //! Does not affect shape of mesh
    void setBoxTextureCoords( const SpriteData &sprite_data );
    //! Set the mesh as a box of sprite's size with correct texture coordinates
    void matchTexture( const SpriteData &sprite_data );
    void setColor( const ci::ColorA8u &color );
  };
} // puptent::
