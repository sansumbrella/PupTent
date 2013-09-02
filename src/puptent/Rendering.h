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
#include "cinder/Matrix.h"
#include "Locus.h"
#include "Mesh.h"

namespace puptent
{
  /**
   Simple 2d layer-sorted rendering system
   */
  struct RenderSystem : public System<RenderSystem>, Receiver<RenderSystem>
  {
    typedef std::pair<shared_ptr<Locus>, shared_ptr<RenderMesh2d>> MeshPair;

    void configure( shared_ptr<EventManager> event_manager ) override
    {
      std::cout << __PRETTY_FUNCTION__ << std::endl;
      event_manager->subscribe<EntityDestroyedEvent>( *this );
      event_manager->subscribe<ComponentAddedEvent<RenderMesh2d>>( *this );
      event_manager->subscribe<ComponentRemovedEvent<RenderMesh2d>>( *this );
    }

    void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
    {
    // build our sorted geometry list from query if the layers/components have changed
    // need to rebuild if any meshes are removed or added
    if( mGeometry.empty() )
    {
      for( auto entity : es->entities_with_components<Locus, RenderMesh2d>() )
      {
        mGeometry.emplace_back( entity.component<Locus>(), entity.component<RenderMesh2d>() );
      }
      stable_sort( mGeometry.begin(), mGeometry.end(), []( const MeshPair &lhs, const MeshPair &rhs ) -> bool {
        return lhs.second->render_layer < rhs.second->render_layer;
      } );
    }
    // assemble all vertices
    mVertices.clear();
    for( auto pair : mGeometry )
    {
      auto loc = pair.first;
      auto mesh = pair.second;
      auto mat = loc->toMatrix();
      if( !mVertices.empty() )
      { // create degenerate triangle between previous and current shape
        mVertices.emplace_back( mVertices.back() );
        auto vert = mesh->vertices.front();
        mVertices.emplace_back( Vertex2d{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
      for( auto &vert : mesh->vertices )
      {
        mVertices.emplace_back( Vertex2d{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
    }
  }

  void receive( const EntityDestroyedEvent &event )
  {
    std::cout << "Entity destroyed" << std::endl;
    auto entity = event.entity;
    if( entity.component<RenderMesh2d>() )
    { // if a mesh was destroyed, we will update our render list this frame
      mGeometry.clear();
    }
  }

  void receive( const ComponentAddedEvent<RenderMesh2d> &event )
  { // empty our geometry
    std::cout << "Render component added: " << event.component << std::endl;
    mGeometry.clear();
  }

  void receive( const ComponentRemovedEvent<RenderMesh2d> &event )
  {
    std::cout << "Render component removed: " << event.component << std::endl;
    mGeometry.clear();
  }

  void draw()
  {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].position.x );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].tex_coord.x );
    glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex2d ), &mVertices[0].color.r );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  }
private:
  std::vector<MeshPair> mGeometry;
  std::vector<Vertex2d> mVertices;
  // maybe add a TextureRef for whatever texturing is needed (e.g. spriting)
  // then if( tex ){ tex->enableAndBind(); }
  // maybe add a CameraRef for positioning the scene
  // use a POV2d and Locus component as camera, allowing dynamic switching
};

}