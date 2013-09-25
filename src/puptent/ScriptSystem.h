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
#include "cinder/app/TouchEvent.h"
#include "cinder/app/KeyEvent.h"

namespace puptent
{
  /**
   ScriptComponent:
   Runs an arbitrary script on every update cycle.
   Use for behavior unique to an entity, like user-controlled entities,
   special items, synching timeline animations, or temporary behaviors.

   Will probably add ability for other scripts to fire when events happen.
   Those scripts are likely to be called from their respective systems.
   */
  // ScriptFn receives self entity, entity manager for world queries, event manager, and timestep
  typedef std::function<void (Entity, EntityManagerRef, EventManagerRef, double)> ScriptFn;
  struct ScriptComponent : Component<ScriptComponent>
  {
    ScriptComponent( ScriptFn fn ):
    script( fn )
    {}
    ScriptFn script;
  };

  /**
   ScriptSystem:
   Executes arbitrary code on a component.
   Passes in entity information.
   Currently runs c++ functions.
   Planning to eventually expand to run Lua scripts.
   */
  struct ScriptSystem : public System<ScriptSystem>, Receiver<ScriptSystem>
  {
    //! gather scripts and execute them
    void update( EntityManagerRef es, EventManagerRef events, double dt ) override;
  };
}
