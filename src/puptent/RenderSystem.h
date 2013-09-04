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
   Composite component.
   Lets us store information needed for RenderSystem in one fast-to-access place.
   Requires an extra step when defining element components
   */
  typedef std::shared_ptr<class RenderData> RenderDataRef;
  struct RenderData : Component<RenderData>
  {
    RenderData( RenderMeshRef mesh, LocusRef locus ):
    mesh( mesh ),
    locus( locus )
    {}
    RenderMeshRef mesh;
    LocusRef      locus;
  };

  /**
   RenderSystem:
    layer-sorted rendering system
   Batch renders RenderMesh components by combining them into a single
   triangle strip. This is very efficient for ribbons and other common
   generative geometry and also efficient for rectangles (like sprites).
   If a texture is assigned, it will be bound before rendering begins.
   Alpha blending is performed assuming colors are premultiplied.
   */
  struct RenderSystem : public System<RenderSystem>, Receiver<RenderSystem>
  {
    //! listen for events
    void        configure( shared_ptr<EventManager> event_manager ) override;
    //! sort the render data by render layer
    inline void sort()
    { stable_sort( mGeometry.begin(), mGeometry.end(), &RenderSystem::layerSort ); }
    //! generate vertex list by transforming meshes by locii
    void        update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override;
    //! batch render scene to screen
    void        draw() const;
    //! set a texture to be bound for all rendering
    inline void setTexture( ci::gl::TextureRef texture )
    { mTexture = texture; }
    void        receive( const EntityDestroyedEvent &event );
    void        receive( const ComponentAddedEvent<RenderData> &event );
    void        receive( const ComponentRemovedEvent<RenderData> &event );
    void        checkOrdering() const;
  private:
    std::vector<RenderDataRef>  mGeometry;
    std::vector<Vertex>         mVertices;
    ci::gl::TextureRef          mTexture;
    static bool                 layerSort( const RenderDataRef &lhs, const RenderDataRef &rhs )
    { return lhs->locus->render_layer < rhs->locus->render_layer; }
    // maybe add a CameraRef for positioning the scene
    // use a POV and Locus component as camera, allowing dynamic switching
  };

} // puptent::
