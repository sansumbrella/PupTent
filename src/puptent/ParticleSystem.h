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

namespace puptent
{
  struct ParticleEmitter2d : Component<ParticleEmitter2d>
  {
    int         tag;
    ci::Vec3f   direction;  // direction in which to fire particles
    float       rate;
    float       hold;
  };

  struct Particle2d : Component<Particle2d>
  {
    float life; // life remaining in seconds
    float seed; // seed enabling some random variation from norm
    int   tag;
  };

  /**
   ParticleSystem:
   Simple sprited 2d particle system
   Each "particle" is an entity with Particle2d and Locus components
   This lets us integrate the particles with the BatchRenderSystem and avoid
   writing any new rendering code.
   System manages basic colliders, adding forces, gravity
   */
  struct ParticleSystem2d : public System<ParticleSystem2d>
  {
    void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override;
  private:
    std::vector<Entity>     mEntities;
    ci::Vec3f               mGravity;
  };
} // puptent::
