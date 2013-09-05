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

#include "puptent/ParticleSystem.h"
#include "puptent/Locus.h"
#include "pockets/CollectionUtilities.hpp"

using namespace puptent;
using namespace cinder;

void ParticleSystem::update( EntityManagerRef es, EventManagerRef events, double dt )
{
  for( auto entity : mEmitters )
  {
    auto emitter = entity.component<ParticleEmitter>();
    auto loc = entity.component<Locus>();
    Entity e = es->create();
    auto p = e.assign<Particle>();
    auto l = e.assign<Locus>();
    if( emitter->build_fn )
    {
      emitter->build_fn( e );
    }
    mParticles.push_back( { p, l, e } );
  }

  for( auto info : mParticles )
  {
    // Perform verlet integration
    ParticleRef p = info.particle;
    p->life -= dt;
    if( p->life > 0.0f )
    {
      Vec3f position = p->position;
      Vec3f velocity = position - p->p_position;
      p->position = position + velocity * p->friction;
      p->p_position = position;

      // Synchronize to Locus
      LocusRef l = info.locus;
      l->position.x = p->position.x;
      l->position.y = p->position.y;
      l->render_layer = p->position.z;
      l->rotation = p->rotation;
      l->scale = p->scale;
    }
    else
    { // destroy associated entity
      info.entity.destroy();
    }
  }
  // stop tracking dead particles
  vector_erase_if( &mParticles, []( const ParticleInfo &particle ){
    return particle.particle->life <= 0.0f;
  });
}
