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
#include "pockets/Types.h"

namespace puptent
{
  /**
   Collection of 2d vertices suitable for rendering as a triangle strip.
  */
  typedef std::shared_ptr<class RenderMesh2d> RenderMesh2dRef;
  struct RenderMesh2d : Component<RenderMesh2d>
  {
    RenderMesh2d( int vertex_count=3, int render_layer=0 ):
    render_layer( render_layer )
    {
      vertices.assign( vertex_count, Vertex2d{} );
    }

    static RenderMesh2dRef createCircle( float radius, float start_radians, float completion_radians, size_t segments, int render_layer=0 )
    { // triangle strips are unfortunately inefficient at rendering circular shapes
      RenderMesh2dRef mesh{ new RenderMesh2d{ segments * 5, render_layer } };
      ci::Vec2f a{ 0.0f, 0.0f };
      for( int i = 0; i < segments; ++i )
      {
        float t1 = ci::lmap<float>( i, 0, segments, start_radians, completion_radians );
        float t2 = ci::lmap<float>( i + 1, 0, segments, start_radians, completion_radians );
        ci::Vec2f b = { ci::math<float>::cos( t1 ) * radius, ci::math<float>::sin( t1 ) * radius };
        ci::Vec2f c = { ci::math<float>::cos( t2 ) * radius, ci::math<float>::sin( t2 ) * radius };
        mesh->vertices.at(i * 5 + 0).position = a;
        mesh->vertices.at(i * 5 + 1).position = b;
        mesh->vertices.at(i * 5 + 2).position = c;
        mesh->vertices.at(i * 5 + 3).position = c;
        mesh->vertices.at(i * 5 + 4).position = a;
      }
      return mesh;
    }

    static RenderMesh2dRef createBox( const ci::Rectf &bounds, int render_layer=0 )
    {
      RenderMesh2dRef mesh{ new RenderMesh2d{ 4, render_layer } };
      mesh->vertices[0].position = bounds.getUpperRight();
      mesh->vertices[1].position = bounds.getUpperLeft();
      mesh->vertices[2].position = bounds.getLowerRight();
      mesh->vertices[3].position = bounds.getLowerLeft();
      return mesh;
    }
    std::vector<Vertex2d> vertices;
    int                   render_layer = 0;
  };

  struct RenderMesh3d : Component<RenderMesh3d>
  {
  	std::vector<Vertex3d> vertices;
  };
} // puptent::
