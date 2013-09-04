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

#include "puptent/RenderSystem.h"
#include "cinder/gl/Texture.h"

using namespace cinder;
using namespace puptent;

void RenderSystem::configure( shared_ptr<EventManager> event_manager )
{
  event_manager->subscribe<EntityDestroyedEvent>( *this );
  event_manager->subscribe<ComponentAddedEvent<RenderMesh>>( *this );
  event_manager->subscribe<ComponentRemovedEvent<RenderMesh>>( *this );
}

void RenderSystem::receive(const ComponentAddedEvent<puptent::RenderMesh> &event)
{
  std::cout << "Render component added: " << event.component << std::endl;
  mGeometry.clear();
}

void RenderSystem::receive(const ComponentRemovedEvent<puptent::RenderMesh> &event)
{
  std::cout << "Render component removed: " << event.component << std::endl;
  mGeometry.clear();
}

void RenderSystem::receive(const EntityDestroyedEvent &event)
{
  std::cout << "Entity destroyed" << std::endl;
  auto entity = event.entity;
  if( entity.component<RenderMesh>() )
  { // if a mesh was destroyed, we will update our render list this frame
    mGeometry.clear();
  }
}

void RenderSystem::update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt )
{
  // build our sorted geometry list from query if the layers/components have changed
  // will rebuild if any meshes are removed or added
  if( mGeometry.empty() )
  {
    for( auto entity : es->entities_with_components<Locus, RenderMesh>() )
    {
      mGeometry.emplace_back( entity.component<Locus>(), entity.component<RenderMesh>() );
    }
  }
  stable_sort( mGeometry.begin(), mGeometry.end(), []( const RenderMeshPair &lhs, const RenderMeshPair &rhs ) -> bool {
    return lhs.first->render_layer < rhs.first->render_layer;
  } );
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
      mVertices.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
    }
    for( auto &vert : mesh->vertices )
    {
      mVertices.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
    }
  }
}

void RenderSystem::draw() const
{
  if( mTexture )
  {
    gl::enableAlphaBlending( true );
    gl::enable( GL_TEXTURE_2D );
    mTexture->bind();
  }
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_COLOR_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mVertices[0].color.r );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );

  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );

  if( mTexture )
  {
    mTexture->unbind();
    gl::disable( GL_TEXTURE_2D );
    gl::disableAlphaBlending();
  }
}
