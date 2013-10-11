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
  typedef std::shared_ptr<class PhysicsComponent> PhysicsComponentRef;
  struct PhysicsComponent : Component<PhysicsComponent>
  {
    PhysicsComponent( b2::unique_body_ptr &&body ):
    body( std::move( body ) )
    {}
    b2::unique_body_ptr body;
    Entity              entity;
  };

  /**
   Stuff caught in a point test
   */
  class QueryCallback : public b2QueryCallback {
  public:
    bool ReportFixture(b2Fixture* fixture) override;
    b2Body *getBody() const { return mBody; }
    b2Fixture *getFixture() const { return mFixture; }
  private:
    b2Body    *mBody = nullptr;
    b2Fixture *mFixture = nullptr;
  };
  /**
   PhysicsSystem:
   Physics calculation using Box2D
   Updates Entity's Locus component with physics transformations
   */
  typedef std::shared_ptr<class PhysicsSystem> PhysicsSystemRef;
  struct PhysicsSystem : public System<PhysicsSystem>, Receiver<PhysicsSystem>
  {
    PhysicsSystem();
    ~PhysicsSystem();
    //! called by SystemManager to register event handlers
    void configure( EventManagerRef events ) override;
    //! add/remove components when they are created
    void receive( const ComponentAddedEvent<PhysicsComponent> &event );
    void receive( const ComponentRemovedEvent<PhysicsComponent> &event );
    void receive( const EntityDestroyedEvent &event );
    //! steps the physics simulation
    void stepPhysics();
    //! synchronizes locus components with physics coords
    void update( EntityManagerRef es, EventManagerRef events, double dt ) override;
    //! draw physics debug information to screen (using box2d's debugDraw facility)
    void debugDraw();
    //! return an entity from the box2d body or joint's userdata
    Entity getBodyEntity( void *b2UserData );
    //! return collection of objects that contain pos
    QueryCallback getIntersectedObjects( const ci::Vec2f &pos, const float epsilon = 0.5f );

    void createBoundaryRect( ci::Rectf screen_bounds ) { mSandbox.createBoundaryRect( mScale.toPhysics( screen_bounds ) ); }
    //! Destroy the boundary rectangle
    void destroyBoundaryRect() { mSandbox.destroyBoundaryRect(); }
    //! Create a box using screen coordinates
    PhysicsComponentRef createBox( const ci::Vec2f &pos, const ci::Vec2f &size, float rotation );
    //! Create a circle using screen coordinates
    PhysicsComponentRef createCircle( const ci::Vec2f &pos, float radius );
    box2d::Sandbox& getSandbox() { return mSandbox; }
    box2d::Scale&   getScale() { return mScale; }
  private:
    box2d::Sandbox       mSandbox;
    std::vector<Entity>  mEntities;
    box2d::Scale         mScale;
  };
}
