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

#include "Sprites.h"

using namespace puptent;
using namespace cinder;

void Sprite::applyDataToMesh()
{ // set mesh data from current frame
  const auto drawing = currentDrawing().drawing;
  Rectf tex_coord_rect = drawing.texture_bounds;
  Rectf position_rect( Vec2f::zero(), drawing.size );
  position_rect -= drawing.registration_point;

  mesh->vertices[0].position = { position_rect.getX2(), position_rect.getY1() };
  mesh->vertices[1].position = { position_rect.getX1(), position_rect.getY1() };
  mesh->vertices[2].position = { position_rect.getX2(), position_rect.getY2() };
  mesh->vertices[3].position = { position_rect.getX1(), position_rect.getY2() };

  mesh->vertices[0].tex_coord = { tex_coord_rect.getX2(), tex_coord_rect.getY1() };
  mesh->vertices[1].tex_coord = { tex_coord_rect.getX1(), tex_coord_rect.getY1() };
  mesh->vertices[2].tex_coord = { tex_coord_rect.getX2(), tex_coord_rect.getY2() };
  mesh->vertices[3].tex_coord = { tex_coord_rect.getX1(), tex_coord_rect.getY2() };
}

void SpriteSystem::configure( shared_ptr<EventManager> events )
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  events->subscribe<EntityDestroyedEvent>( *this );
  events->subscribe<ComponentAddedEvent<Sprite>>( *this );
}

void SpriteSystem::receive(const entityx::EntityDestroyedEvent &event)
{ // stop tracking the entity
  auto entity = event.entity;
  if( entity.component<Sprite>() )
  {
    vector_remove( &mEntities, entity );
  }
}

void SpriteSystem::receive(const ComponentAddedEvent<puptent::Sprite> &event)
{ // track the sprite
  mEntities.push_back( event.entity );
}

void SpriteSystem::update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt )
{
  for( auto entity : mEntities )
  { // what is the performance of this component casting business?
    // fast enough for most things, no doubt
    auto sprite = entity.component<Sprite>();
    sprite->hold += dt;
    int next_index = sprite->current_index;
    // check timing
    if( sprite->hold > sprite->frame_duration * sprite->currentDrawing().hold )
    { // move to next frame
      next_index += 1;
      sprite->hold = 0.0f;
    }
    else if ( sprite->hold < 0.0f )
    { // step back a frame
      next_index -= 1;
      sprite->hold = sprite->frame_duration * sprite->currentDrawing().hold;
    }
    // handle wrapping around beginning and end
    if( next_index >= static_cast<int>( sprite->drawings.size() ) )
    { // handle wraparound at end
      next_index = sprite->looping ? 0 : sprite->drawings.size() - 1;
    }
    else if( next_index < 0 )
    { // handle wraparound at beginning
      next_index = sprite->looping ? sprite->drawings.size() - 1 : 0;
    }
    // actually change the drawing
    if( next_index != sprite->current_index )
    { // the frame index has changed, update display
      sprite->current_index = next_index;
      sprite->applyDataToMesh();
    }
  }
}
