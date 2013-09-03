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

#include "PhysicsSystem2d.h"
#include "pockets/CollectionUtilities.hpp"

#include "puptent/Locus.h"

using namespace puptent;
using namespace cinder;

void PhysicsSystem2d::configure( shared_ptr<EventManager> events )
{
  events->subscribe<ComponentAddedEvent<PhysicsComponent2d>>( *this );
  events->subscribe<ComponentRemovedEvent<PhysicsComponent2d>>( *this );
  events->subscribe<EntityDestroyedEvent>( *this );
}

void PhysicsSystem2d::receive(const ComponentAddedEvent<puptent::PhysicsComponent2d> &event)
{
  mEntities.push_back( event.entity );
}

void PhysicsSystem2d::receive(const ComponentRemovedEvent<puptent::PhysicsComponent2d> &event)
{
  vector_remove( &mEntities, event.entity );
}

void PhysicsSystem2d::receive(const entityx::EntityDestroyedEvent &event)
{ // in case we were tracking it, stop (if it had a physics component we were)
  vector_remove( &mEntities, event.entity );
}

void PhysicsSystem2d::update(shared_ptr<entityx::EntityManager> es, shared_ptr<entityx::EventManager> events, double dt)
{
  for( auto entity : mEntities )
  {
    auto locus = entity.component<Locus>();
    auto physics = entity.component<PhysicsComponent2d>();
//    locus->position = Vec2f{ physics->body->GetPosition().x, physics->body->GetPosition().y } * mScale;
//    locus->rotation = physics->body->GetRotation();
  }
}
