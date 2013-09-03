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

namespace cinder
{
  class JsonTree;
}

namespace puptent
{
  /**
   TextureAtlas:
   A texture and lookup information for named sprites on that texture.
   Not a component, but used by the SpriteAnimationSystem
  */
  typedef std::unique_ptr<class TextureAtlas> TextureAtlasUniqueRef;
  class TextureAtlas
  {
  public:
    TextureAtlas() = default;
    TextureAtlas( const ci::Surface &images, const ci::JsonTree &description );
    inline const SpriteData& get( const std::string &sprite_name ) const
    {
      auto iter = mData.find(sprite_name);
      if( iter != mData.end() )
      {
        return iter->second;
      }
      return mErrorData;
    }
    //! returns SpriteData with string id \a sprite_name or default sprite if none exists
    inline const SpriteData&  operator [] ( const std::string &sprite_name ) const
    {
      return get( sprite_name );
    }
    ci::gl::TextureRef  getTexture() const { return mTexture; }
    static TextureAtlasUniqueRef create( const ci::Surface &images, const ci::JsonTree &description );
  private:
    std::map<std::string, SpriteData>   mData;
    ci::gl::TextureRef                  mTexture;
    SpriteData                          mErrorData;
  };

} // puptent::
