#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"

#include "pockets/Types.h"
#include "pockets/CollectionUtilities.hpp"

#include "puptent/Rendering.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace puptent;

using namespace entityx;
using pockets::Vertex2d;

struct SpriteData
{
  ci::Vec2f registration_point = ci::Vec2f::zero();
  ci::Vec2i size = ci::Vec2i( 48, 48 );
  const ci::Rectf texture_bounds = ci::Rectf(0,0,1,1);
};

struct Sprite : Component<Sprite>
{
  struct Drawing{
    SpriteData  sprite;
    float       hold; // frames to hold
  };

  Sprite()
  {
    applyDataToMesh();
  }

  void applyDataToMesh()
  { // set mesh data from current frame
    const auto drawing = currentFrame().sprite;
    Rectf tex_coord_rect = drawing.texture_bounds;
    Rectf position_rect( Vec2f::zero(), drawing.size );
    position_rect -= drawing.registration_point;

    mesh->vertices[0].position = { position_rect.getX2(), position_rect.getY1() };
    mesh->vertices[1].position = { position_rect.getX1(), position_rect.getY1() };
    mesh->vertices[2].position = { position_rect.getX2(), position_rect.getY2() };
    mesh->vertices[3].position = { position_rect.getX1(), position_rect.getY2() };

    mesh->vertices[0].tex_coord = { tex_coord_rect.getX2(), tex_coord_rect.getY1() };
    mesh->vertices[1].tex_coord = { tex_coord_rect.getX1(), tex_coord_rect.getY1() };
    mesh->vertices[2].tex_coord = { tex_coord_rect.getX2(), tex_coord_rect.getY2() };
    mesh->vertices[3].tex_coord = { tex_coord_rect.getX1(), tex_coord_rect.getY2() };
  }

  const Drawing &currentFrame() const { return frames.at( current_index ); }
  std::vector<Drawing>  frames;
  int                   current_index = 0; // this limits us to 32k frames per animation...
  float                 hold = 0.0f;      // time spent on this frame
  float                 frame_duration = 1.0f / 24.0f;
  bool                  looping = true;
  // we create the mesh here and then add that mesh to entities
  // that way, we know we are using the expected mesh in the entity
  shared_ptr<RenderMesh2d>  mesh = shared_ptr<RenderMesh2d>{ new RenderMesh2d{ 4, 0 } }; // the mesh we will be updating
};

struct SpriteSystem : public System<SpriteSystem>, Receiver<SpriteSystem>
{
  void configure( shared_ptr<EventManager> events ) override
  {
    events->subscribe<EntityDestroyedEvent>( *this );
    events->subscribe<ComponentAddedEvent<Sprite>>( *this );
  }

  void receive( const EntityDestroyedEvent &event )
  { // stop tracking the entity
    auto entity = event.entity;
    if( entity.component<Sprite>() )
    {
      vector_remove( &mEntities, entity );
    }
  }

  void receive( const ComponentAddedEvent<Sprite> &event )
  { // track the sprite
    mEntities.push_back( event.entity );
  }

  void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
  {
    for( auto entity : mEntities )
    {
      auto sprite = entity.component<Sprite>();
      sprite->hold += dt;
      int next_index = sprite->current_index;
      if( sprite->hold > sprite->frame_duration * sprite->currentFrame().hold )
      { // move to next frame
        next_index += 1;
        sprite->hold = 0.0f;
      }
      else if ( sprite->hold < 0.0f )
      { // step back a frame
        next_index -= 1;
        sprite->hold = sprite->frame_duration * sprite->currentFrame().hold;
      }
      if( next_index >= static_cast<int>( sprite->frames.size() ) )
      { // handle wraparound at end
        next_index = sprite->looping ? 0 : sprite->frames.size() - 1;
      }
      else if( next_index < 0 )
      { // handle wraparound at beginning
        next_index = sprite->looping ? sprite->frames.size() - 1 : 0;
      }
      if( next_index != sprite->current_index )
      { // the frame index has changed, update display
        sprite->current_index = next_index;
        sprite->applyDataToMesh();
      }
    }
  }
private:
  std::vector<Entity>  mEntities;
};

struct Velocity : Component<Velocity>
{
  Velocity( float x = 0.0f, float y = 0.0f ):
  velocity( x, y )
  {}
  ci::Vec2f velocity;
};

struct MovementSystem : public System<MovementSystem>
{
  void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
  {
    if( mElements.empty() )
    {
      for( auto entity : es->entities_with_components<Locus>() )
      {
        mElements.push_back( entity.component<Locus>() );
      }
    }
    for( auto& loc : mElements )
    {
      loc->rotation = fmodf( loc->rotation + M_PI * 0.01f, M_PI * 2 );
    }
  }

  vector<shared_ptr<Locus>> mElements;
};

class PupTentApp : public AppNative
{
public:
  void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
private:
  shared_ptr<EventManager>  mEvents;
  shared_ptr<EntityManager> mEntities;
  shared_ptr<SystemManager> mSystemManager;
  shared_ptr<MovementSystem>  mMovement;
  double mLastUpdate = 0.0;
  double            mAverageRenderTime = 0;
};

void PupTentApp::prepareSettings( Settings *settings )
{
  settings->disableFrameRate();
}

void PupTentApp::setup()
{
  gl::enableVerticalSync();
  mEvents = EventManager::make();
  mEntities = EntityManager::make(mEvents);
  mSystemManager = SystemManager::make( mEntities, mEvents );
  mSystemManager->add<MovementSystem>();
  mSystemManager->add<RenderSystem>();
  mSystemManager->configure();

  Rand r;
  Vec2f center = getWindowCenter();
  for( int i = 0; i < 1000; ++i )
  {
    Entity entity = mEntities->create();
    auto loc = shared_ptr<Locus>{ new Locus };
    auto mesh = shared_ptr<RenderMesh2d>{ new RenderMesh2d };
    ColorA color{ CM_HSV, r.nextFloat( 1.0f ), 0.9f, 0.9f, 1.0f };
    for( auto &v : mesh->vertices )
    {
      v.color = color;
    }
    loc->position = { r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) };
    loc->rotation = r.nextFloat( M_PI * 2 );
    mesh->render_layer = loc->position.distance( center );
    entity.assign<Locus>( loc );
    entity.assign<RenderMesh2d>( mesh );
  }
}

void PupTentApp::update()
{
  double now = getElapsedSeconds();
  double dt = now - mLastUpdate;
  mLastUpdate = now;
  double start = getElapsedSeconds();
  mSystemManager->update<MovementSystem>( dt );
  double end = getElapsedSeconds();
  if( getElapsedFrames() % 120 == 0 )
  {
    cout << "Update: " << (end - start) * 1000 << endl;
  }
}

void PupTentApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  double start = getElapsedSeconds();
  mSystemManager->update<RenderSystem>( 0.0 );
  mSystemManager->system<RenderSystem>()->draw();
  double end = getElapsedSeconds();
  double ms = (end - start) * 1000;
  mAverageRenderTime = (mAverageRenderTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 120 == 0 )
  {
    cout << "Render ms: " << mAverageRenderTime << endl;
  }
}

CINDER_APP_NATIVE( PupTentApp, RendererGl( RendererGl::AA_MSAA_4 ) )

