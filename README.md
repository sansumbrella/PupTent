PupTent

A 2D entity-system game engine built on top of Cinder and entityx.

PupTent is meant for use in the development of small games with big hearts.
It is especially supportive of mixing procedural and hand-drawn 2d art.

Features (Existing and Planned):
+ Locus component for flexible 2d spatial transformations
  - Move things around with intuitive properties
      - Position
      - Rotation
      - Scale
      - Registration point
+ Fast batch renderer for 2d geometry
  - Put your geometry into triangle_strip order and you're set
  - Every mesh is rendered in a single pass to make the GPU driver happy
  - Note that means all sprites should packed into one texture
+ Sprite animation, loading from disk
  - JSON description format for sprites and animations
+ Texture Packing
  - Offline texture packing for prepping hand-drawn assets
  - (todo: make example) Runtime texture packing for generative sprites or asset subsets
+ Script system
  - executes arbitrary c++ functions on Entities
  - (TODO) Lua scripting support
+ (Incomplete) Physics system
  - Wraps suBox2d for easy physics-based interaction
  - (TODO) callbacks on collision
  - (TODO) sensors
+ (TODO) Line and path expansion
  - Line between e.g. two Locii
+ (TODO) Background layer rendering with parallax
+ (TODO) Panning/zooming camera(s)
  - Locus + POV component, way of promoting one as "main" view
+ (Would Like TODO) Entity editor
+ (Would Like TODO) World editor

Requires:
Cinder 0.8.5
entityx (necessary bits included in src/)

Conventions:
```c++
typedef std::shared_ptr<T>  TRef;
typedef std::unique_ptr<T>  TUniqueRef;
typedef std::weak_ptr<T>    TWeakRef;
```

/*
 * Copyright (C) 2013 David Wicks <david@sansumbrella.com>
 * All rights reserved.
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution.
 *
 * Author: David Wicks <david@sansumbrella.com>
 */
