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
#include "puptent/Locus.h"
#include "puptent/RenderMesh.h"

namespace puptent
{
  /**
   BatchRenderSystem2d:
   2d layer-sorted rendering system
   Batch renders RenderMesh2d components by combining them into a single
   triangle strip. This is very efficient for ribbons and other common
   generative geometry and also efficient for rectangles (like sprites).
   If a texture is assigned, it will be bound before rendering begins.
   Alpha blending is performed assuming colors are premultiplied.
   */
  struct BatchRenderSystem2d : public System<BatchRenderSystem2d>, Receiver<BatchRenderSystem2d>
  {
    typedef std::pair<shared_ptr<Locus>, shared_ptr<RenderMesh2d>> MeshPair;

    //! listen for events
    void configure( shared_ptr<EventManager> event_manager ) override;
    //! gather geometry and apply transforms when collecting into list
    void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override;
    //! batch render scene to screen
    void draw() const;
    //! set a texture to be bound for all rendering
    void setTexture( ci::gl::TextureRef texture )
    {
      mTexture = texture;
    }
    void receive( const EntityDestroyedEvent &event );
    void receive( const ComponentAddedEvent<RenderMesh2d> &event );
    void receive( const ComponentRemovedEvent<RenderMesh2d> &event );
  private:
    std::vector<MeshPair> mGeometry;
    std::vector<Vertex2d> mVertices;
    ci::gl::TextureRef    mTexture;
    // maybe add a CameraRef for positioning the scene
    // use a POV2d and Locus component as camera, allowing dynamic switching
  };

} // puptent::
