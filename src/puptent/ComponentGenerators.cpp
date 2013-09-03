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

#include "ComponentGenerators.h"

#include "puptent/Sprites.h"
#include "puptent/TextureAtlas.h"
#include "cinder/Json.h"

using namespace puptent;
using namespace cinder;
using namespace std;
//
//SpriteAnimationRef pt::createSpriteAnimationFromJson( const JsonTree &anim, const TextureAtlas &atlas )
//{
//  try
//  {
//    vector<SpriteAnimation::Drawing> drawings;
//    float frame_duration = 1.0f / anim.getChild("fps").getValue<float>();
//    auto frames = anim.getChild("frames");
//    for( auto &child : frames.getChildren() )
//    { // stored in json as [ "id", duration ]
//      drawings.emplace_back( atlas[child[0].getValue()], child[1].getValue<float>() );
//    }
//    return SpriteAnimationRef{ new SpriteAnimation{ move(drawings), frame_duration } };
//  }
//  catch( JsonTree::Exception &exc )
//  {
//    std::cout << __FUNCTION__ << " error: " << exc.what() << std::endl;
//  }
//  return SpriteAnimationRef{ new SpriteAnimation{} };
//}

