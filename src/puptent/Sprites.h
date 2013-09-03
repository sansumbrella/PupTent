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
#include "puptent/SpriteData.h"
#include "puptent/Mesh.h"
#include "pockets/CollectionUtilities.hpp"

namespace cinder
{
  class JsonTree;
}

namespace puptent
{
  typedef size_t AnimationId;
  struct SpriteAnimation : Component<SpriteAnimation>
  {
    SpriteAnimation()
    {}
    // build a sprite from a list of drawings
    SpriteAnimation( AnimationId animation ):
    animation( animation )
    {}
    AnimationId               animation = 0;
    bool                      looping = true;
    float                     hold = 0.0f;      // time spent on this frame
    int                       current_index = 0;
    // we create the mesh here and then add that mesh to entities
    // that way, we know we are using the expected mesh in the entity
    shared_ptr<RenderMesh2d>  mesh = shared_ptr<RenderMesh2d>{ new RenderMesh2d{ 4, 0 } };
  };

  typedef std::shared_ptr<class SpriteAnimationSystem> SpriteAnimationSystemRef;
  struct SpriteAnimationSystem : public System<SpriteAnimationSystem>, Receiver<SpriteAnimationSystem>
  {
    struct Drawing
    {
      Drawing( const SpriteData &drawing=SpriteData{}, float hold=1.0f ):
      drawing( drawing ),
      hold( hold )
      {}
      SpriteData      drawing;  // size and texture information
      float           hold;     // frames to hold
    };
    struct Animation
    {
      std::string           name;
      std::vector<Drawing>  drawings;
      float                 frame_duration;
    };
    SpriteAnimationSystem( TextureAtlasRef atlas, const ci::JsonTree &animations );
    static SpriteAnimationSystemRef create( TextureAtlasRef atlas, const ci::JsonTree &animations );
    void configure( shared_ptr<EventManager> events ) override;
    //! remove sprites from our collection when entities are destroyed
    void receive( const EntityDestroyedEvent &event );
    //! Add sprite to our collection on creation
    void receive( const ComponentAddedEvent<SpriteAnimation> &event );
    void receive( const ComponentRemovedEvent<SpriteAnimation> &event );
    void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override;
    SpriteAnimationRef getSpriteAnimation( const std::string &id ) const;
  private:
    //! active sprite components
    std::vector<SpriteAnimationRef>     mSpriteAnimations;
    TextureAtlasRef                     mAtlas;
    // name : index into mAnimations
    std::map<std::string, AnimationId>  mAnimationIds;
    std::vector<Animation>              mAnimations;
  };

} // puptent::

