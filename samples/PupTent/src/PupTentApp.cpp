#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"

#include "pockets/Types.h"
#include "pockets/CollectionUtilities.hpp"

#include "puptent/Rendering.h"
#include "puptent/Sprites.h"

/**
 Sample app used to develop features of PupTent.
 Learning about component systems and building my own components.
 While everything is dependent on the underlying component machinery,
 it is wonderfully decoupled from other systems and components.
*/

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace puptent;

using namespace entityx;
using pockets::Vertex2d;

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
    time += dt;
    for( auto& loc : mElements )
    {
      loc->rotation = fmodf( loc->rotation + M_PI * 0.01f, M_PI * 2 );
      loc->scale = math<float>::sin( 0.5f * time + M_PI * loc->position.x / 640.0f + M_PI * loc->position.y / 480.0f );
    }
  }
  double time = 0.0;
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
  double            mAverageRenderTime = 0;
  Timer  mTimer;
};

void PupTentApp::prepareSettings( Settings *settings )
{
  settings->disableFrameRate();
//  settings->setFullScreen();
}

void PupTentApp::setup()
{
  gl::enableVerticalSync();
  mEvents = EventManager::make();
  mEntities = EntityManager::make(mEvents);
  mSystemManager = SystemManager::make( mEntities, mEvents );
  mSystemManager->add<MovementSystem>();
  mSystemManager->add<BatchRenderSystem2d>();
  mSystemManager->configure();

  Rand r;
  Vec2f center = getWindowCenter();
  Entity entity;
  for( int i = 0; i < 1000; ++i )
  {
    entity = mEntities->create();
    auto loc = shared_ptr<Locus>{ new Locus };
    auto mesh = RenderMesh2dRef{ new RenderMesh2d{ 4 } };
    mesh->setAsBox( { -20.0f, -10.0f, 20.0f, 10.0f } );
//    mesh->setAsCircle( Vec2f{ 20.0f, 20.0f }, 0.0f, M_PI * 1.5f );
    ColorA color{ CM_HSV, r.nextFloat( 0.4f, 0.8f ), 0.9f, 0.7f, 1.0f };
    for( auto &v : mesh->vertices )
    {
      v.color = color;
    }
    loc->position = { r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) };
    loc->rotation = r.nextFloat( M_PI * 2 );
    mesh->render_layer = loc->position.distance( center );
    entity.assign( loc );
    entity.assign( mesh );
  }

  getWindow()->getSignalMouseDown().connect( [=]( MouseEvent &event ) mutable
  {
    if( entity.valid() )
    {
      cout << "Removing mesh component: " << entity << endl;
      if( entity.component<RenderMesh2d>() )
      {
        entity.remove<RenderMesh2d>();
      }
      else
      {
        auto mesh = RenderMesh2dRef{ new RenderMesh2d{ 4 } };
        float r = Rand::randFloat( 20.0f, 100.0f );
        mesh->setAsCircle( Vec2f{ r, r }, 0.0f, M_PI * 1.5f );
        mesh->render_layer = 1000;
        ColorA color{ CM_HSV, Rand::randFloat( 0.4f, 0.8f ), 0.9f, 0.7f, 1.0f };
        for( auto &v : mesh->vertices )
        {
          v.color = color;
        }
        entity.assign( mesh );
      }
    }
  });

  mTimer.start();
}

void PupTentApp::update()
{
  double dt = mTimer.getSeconds();
  mTimer.start();
  double start = getElapsedSeconds();
  mSystemManager->update<MovementSystem>( dt );
  mSystemManager->update<BatchRenderSystem2d>( 0.0 );
  double end = getElapsedSeconds();
  if( getElapsedFrames() % 60 == 0 )
  {
    cout << "Update: " << (end - start) * 1000 << endl;
  }
}

void PupTentApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  double start = getElapsedSeconds();
  mSystemManager->system<BatchRenderSystem2d>()->draw();
  double end = getElapsedSeconds();
  double ms = (end - start) * 1000;
  mAverageRenderTime = (mAverageRenderTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 60 == 0 )
  {
    cout << "Render ms: " << mAverageRenderTime << ", " << ms << endl;
  }
}

CINDER_APP_NATIVE( PupTentApp, RendererGl( RendererGl::AA_MSAA_8 ) )

