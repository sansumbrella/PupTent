# PupTent - a 2D game engine built on Cinder and entityx

PupTent is meant for use in the development of small games with big hearts.
It is especially supportive of mixing procedural and hand-drawn 2d art.

By providing basic systems, PupTent lets you get start coding your gameplay
systems without a lot of set up. If you want to change how something works later,
feel free to swap in your own System. The component architecture means that
it's easy to replace one system with another.
Systems only know about components, and components only know about themselves.
Locus and RenderMesh are the only components used by multiple systems.

## Features (Existing and Planned):
### Simple 2d spatial transformations
- Attach a Locus component for access to
  - Position
  - Rotation
  - Scale
  - Registration point

### Quick batch renderer for 2d geometry
- Attach a RenderMesh component to store triangle_strip vertices
- RenderSystem batches all meshes into a single draw call
  - Each entity's RenderMesh verticies are transformed by its Locus
  - A single render pass makes the GPU driver happy
  - Note that means all sprites should packed into one texture
  - Cinder provides great support for any additional drawing you might want to do

### Texture Packing (and atlasing)
- TextureAtlas loads and stores sprite information
- TexturePacker (in pockets) automates sprite sheet building and allows for offline and runtime asset packing

### Sprite animation, loading from disk
- Add a SpriteAnimation component to update RenderMesh with animation frames
- Simple JSON description format for both sprites and animations

### Script system
- ScriptComponent executes arbitrary c++ functions on Entities
- (TODO eventually) Lua scripting support

### (Incomplete) Physics system
- PhysicsComponent updates the Locus based on Box2D physics simulation
- (TODO) callbacks on collision
- (TODO) sensors

### TODO
- Background layer rendering with parallax (BackgroundSystem)
- Panning/zooming camera(s)
- Sound effect and music playback (SoundSystem, hopefully atop Cinder...)
- Locus & POV component, way of promoting one as "main" view
- (Longer term) Entity editor
- (Longer term) World editor
- Line and path expansion
  - Line between e.g. two Locii
  - LineComponent modifies Mesh to draw a line

## Dependencies:
- [Cinder 0.8.5](http://libcinder.org/download)
- [entityx](http://github.com/alecthomas/entityx) (required parts included in src/)
- [suBox2d](http://github.com/sansumbrella/suBox2D) (If using the PhysicsComponent)

Conventions:
```c++
typedef std::shared_ptr<T>  TRef;
typedef std::unique_ptr<T>  TUniqueRef;
typedef std::weak_ptr<T>    TWeakRef;
```

PupTent has only been tested on Mac OSX 10.8.
Support is planned for iOS 6.0+.
Windows 7 will be next, assuming VS2012 has the C++11 support needed.
Android is the final target, pending Cinder support
(I’m looking at https://github.com/safetydank/Cinder/tree/android-dev/android)
