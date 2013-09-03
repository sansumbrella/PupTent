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
#include "sansumbrella/suBox2D.h"

namespace puptent
{
  typedef std::shared_ptr<class PhysicsComponent2d> PhysicsComponent2dRef;
  struct PhysicsComponent2d : Component<PhysicsComponent2d>
  {
    PhysicsComponent2d( b2::unique_body_ptr &&body ):
    body( std::move( body ) )
    {}
    b2::unique_body_ptr body;
  };
  /**
   PhysicsSystem2d:
   Physics calculation using Box2D
   Updates Entity's Locus component with physics transformations
   */
  typedef std::shared_ptr<class PhysicsSystem2d> PhysicsSystem2dRef;
  struct PhysicsSystem2d : public System<PhysicsSystem2d>, Receiver<PhysicsSystem2d>
  {
    PhysicsSystem2d();
    ~PhysicsSystem2d();
    //! called by SystemManager to register event handlers
    void configure( shared_ptr<EventManager> events ) override;
    //! add/remove components when they are created
    void receive( const ComponentAddedEvent<PhysicsComponent2d> &event );
    void receive( const ComponentRemovedEvent<PhysicsComponent2d> &event );
    void receive( const EntityDestroyedEvent &event );
    //! steps the physics simulation
    void stepPhysics();
    //! synchronizes locus components with physics coords
    void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override;
    void debugDraw();

    void createBoundaryRect( ci::Rectf screen_bounds ) { mSandbox.createBoundaryRect( mScale.toPhysics( screen_bounds ) ); }
    //! Destroy the boundary rectangle
    void destroyBoundaryRect() { mSandbox.destroyBoundaryRect(); }
    //! Create a box using screen coordinates
    PhysicsComponent2dRef createBox( const ci::Vec2f &pos, const ci::Vec2f &size, float rotation );
    //! Create a circle using screen coordinates
    PhysicsComponent2dRef createCircle( const ci::Vec2f &pos, float radius );
  private:
    box2d::Sandbox       mSandbox;
    std::vector<Entity>  mEntities;
    box2d::Scale         mScale;
  };
}
