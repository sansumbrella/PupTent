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

#include "puptent/SpriteSystem.h"
#include "puptent/RenderMesh.h"
#include "cinder/Json.h"

using namespace puptent;
using namespace cinder;
using namespace std;

SpriteAnimationSystemRef SpriteAnimationSystem::create( TextureAtlasRef atlas, const ci::JsonTree &animations )
{
  return SpriteAnimationSystemRef{ new SpriteAnimationSystem{ atlas, animations } };
}

SpriteAnimationSystem::SpriteAnimationSystem( TextureAtlasRef atlas, const JsonTree &animations ):
mAtlas( atlas )
{
  try
  {
    for( auto &anim : animations )
    {
      vector<Drawing> drawings;
      float frame_duration = 1.0f / anim.getChild("fps").getValue<float>();
      string key = anim.getKey();
      auto frames = anim.getChild("frames");
      for( auto &child : frames.getChildren() )
      { // stored in json as [ "id", duration ]
        drawings.emplace_back( mAtlas->get(child[0].getValue()), child[1].getValue<float>() );
      }
      mAnimations.emplace_back( Animation{ key, drawings, frame_duration } );
      mAnimationIds[key] = mAnimations.size() - 1;
    }
  }
  catch( JsonTree::Exception &exc )
  {
    std::cout << __FUNCTION__ << " error: " << exc.what() << std::endl;
  }
}

void SpriteAnimationSystem::configure( shared_ptr<EventManager> events )
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  events->subscribe<EntityDestroyedEvent>( *this );
  events->subscribe<ComponentAddedEvent<SpriteAnimation>>( *this );
  events->subscribe<ComponentRemovedEvent<SpriteAnimation>>( *this );
}

void SpriteAnimationSystem::addAnimation(const string &name, const Animation &animation)
{
  mAnimations.emplace_back( animation );
  mAnimationIds[name] = mAnimations.size() - 1;
}

AnimationId SpriteAnimationSystem::getAnimationId( const string &name ) const
{
  AnimationId index = 0;
  auto iter = mAnimationIds.find( name );
  if( iter != mAnimationIds.end() )
  {
    index = iter->second;
  }
  return index;
}

SpriteAnimationRef SpriteAnimationSystem::createSpriteAnimation(const string &name) const
{
  return createSpriteAnimation( getAnimationId( name ) );
}

SpriteAnimationRef SpriteAnimationSystem::createSpriteAnimation(AnimationId animation_id ) const
{
  return SpriteAnimationRef{ new SpriteAnimation{ animation_id } };
}

void SpriteAnimationSystem::receive(const ComponentAddedEvent<SpriteAnimation> &event)
{ // track the sprite
  auto entity = event.entity;
  auto mesh = entity.component<RenderMesh>();
  if( mesh )
  {
    auto sprite = event.component;
    auto drawings = mAnimations.at( sprite->animation ).drawings;
    sprite->current_index = math<int>::clamp( sprite->current_index, 0, drawings.size() - 1 );
    mesh->setAsTexture( drawings.at( sprite->current_index ).drawing );
  }
  mEntities.push_back( entity );
}

void SpriteAnimationSystem::receive(const ComponentRemovedEvent<SpriteAnimation> &event)
{
  vector_remove( &mEntities, event.entity );
}

void SpriteAnimationSystem::receive(const entityx::EntityDestroyedEvent &event)
{ // stop tracking the entity (assuming we are looking at it)
  vector_remove( &mEntities, event.entity );
}

void SpriteAnimationSystem::update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt )
{
  for( auto &entity : mEntities )
  {
    auto sprite = entity.component<SpriteAnimation>();

    const auto &anim = mAnimations.at( sprite->animation );
    const auto &current_drawing = anim.drawings.at( sprite->current_index );
    sprite->hold += dt; // this becomes a problem if many share the same sprite
    int next_index = sprite->current_index;
    // check timing
    if( sprite->hold > anim.frame_duration * current_drawing.hold )
    { // move to next frame
      next_index += 1;
      sprite->hold = 0.0f;
    }
    else if ( sprite->hold < 0.0f )
    { // step back a frame
      next_index -= 1;
      sprite->hold = anim.frame_duration * current_drawing.hold;
    }
    // handle wrapping around beginning and end
    if( next_index >= static_cast<int>( anim.drawings.size() ) )
    { // handle wraparound at end
      next_index = sprite->looping ? 0 : anim.drawings.size() - 1;
    }
    else if( next_index < 0 )
    { // handle wraparound at beginning
      next_index = sprite->looping ? anim.drawings.size() - 1 : 0;
    }
    // actually change the drawing
    if( next_index != sprite->current_index )
    { // the frame index has changed, update display
      sprite->current_index = next_index;
      const auto new_drawing = anim.drawings.at( sprite->current_index ).drawing;
      auto mesh = entity.component<RenderMesh>();
      if( mesh ){ mesh->setAsTexture( new_drawing ); }
    }
  }
}
