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

namespace puptent
{
  struct Sprite : Component<Sprite>
  {
    struct Drawing
    {
      Drawing( const SpriteData &drawing=SpriteData{}, float hold=1.0f ):
      drawing( drawing ),
      hold( hold )
      {}
      SpriteData      drawing;
      float           hold; // frames to hold
    };

    Sprite()
    {
      drawings.assign( 1, Drawing{} );
      applyDataToMesh();
    }
    // build a sprite from a list of drawings
    Sprite( std::vector<Drawing> &&drawings, float frame_duration = 1.0f / 24.0f ):
    drawings( std::move(drawings) ),
    frame_duration( frame_duration )
    {
      applyDataToMesh();
    }

    //! commits the current frame position and texture coordinates to mesh
    void applyDataToMesh();
    //! returns the current drawing
    inline const Drawing &currentDrawing() const { return drawings.at( current_index ); }
    //! set the frame duration as frame rate
    void setFrameRate( float frame_rate ){ frame_duration = 1.0f / frame_rate; }
    std::vector<Drawing>  drawings;
    int                   current_index = 0; // index of current animation frame
    float                 hold = 0.0f;      // time spent on this frame
    float                 frame_duration = 1.0f / 24.0f;
    bool                  looping = true;
    // we create the mesh here and then add that mesh to entities
    // that way, we know we are using the expected mesh in the entity
    shared_ptr<RenderMesh2d>  mesh = shared_ptr<RenderMesh2d>{ new RenderMesh2d{ 4, 0 } }; // the mesh we will be updating
  };

  struct SpriteSystem : public System<SpriteSystem>, Receiver<SpriteSystem>
  {
    void configure( shared_ptr<EventManager> events ) override;
    //! remove sprites from our collection when entities are destroyed
    void receive( const EntityDestroyedEvent &event );
    //! Add sprite to our collection on creation
    void receive( const ComponentAddedEvent<Sprite> &event );
    void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override;
  private:
    std::vector<Entity>  mEntities;
  };

} // puptent::

