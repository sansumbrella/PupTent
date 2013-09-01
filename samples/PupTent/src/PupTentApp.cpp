#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace entityx;

struct Position : Component<Position>
{
  Position( float x = 0.0f, float y = 0.0f ):
  x( x ),
  y( y )
  {}
  float x, y;
};

struct Velocity : Component<Velocity>
{
  Velocity( float x = 0.0f, float y = 0.0f ):
  x( x ),
  y( y )
  {}
  float x, y;
};

struct Radius : Component<Radius>
{
  Radius( float r = 10.0f ):
  r( r )
  {}
  float r;
};

struct MovementSystem : public System<MovementSystem>
{
  void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
  {
    for( auto entity : es->entities_with_components<Position, Velocity>() )
    {
      auto pos = entity.component<Position>();
      auto vel = entity.component<Velocity>();
      pos->x += vel->x * dt;
      pos->y += vel->y * dt;
    }
  }
};

struct RenderSystem : public System<RenderSystem>
{
  void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
  {
    for( auto entity : es->entities_with_components<Position, Radius>() )
    {
      auto pos = entity.component<Position>();
      auto radius = entity.component<Radius>();
      gl::color( 1.0f, 0.0f, 0.0f );
      gl::drawSolidCircle( { pos->x, pos->y }, radius->r );
    }
  }
};

class PupTentApp : public AppNative
{
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
private:
  shared_ptr<EventManager>  mEvents;
  shared_ptr<EntityManager> mEntities;
  shared_ptr<SystemManager> mSystemManager;
  shared_ptr<MovementSystem>  mMovement;
  double mLastUpdate = 0.0;
};

void PupTentApp::setup()
{
  mEvents = EventManager::make();
  mEntities = EntityManager::make(mEvents);
  mSystemManager = SystemManager::make( mEntities, mEvents );
  mSystemManager->add<MovementSystem>();
  mSystemManager->add<RenderSystem>();
  mSystemManager->configure();

  Rand r;
  for( int i = 0; i < 1000; ++i )
  {
    Entity entity = mEntities->create();
    entity.assign<Position>( r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) );
    entity.assign<Velocity>( r.nextFloat( -5.0f, 5.0f ), r.nextFloat( -5.0f, 5.0f ) );
    entity.assign<Radius>( r.nextFloat( 8.0f, 24.0f ) );
  }
}

void PupTentApp::mouseDown( MouseEvent event )
{
}

void PupTentApp::update()
{
  double now = getElapsedSeconds();
  double dt = now - mLastUpdate;
  mLastUpdate = now;
  mSystemManager->update<MovementSystem>( dt );
}

void PupTentApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  mSystemManager->update<RenderSystem>( 0.0 );
}

CINDER_APP_NATIVE( PupTentApp, RendererGl )

