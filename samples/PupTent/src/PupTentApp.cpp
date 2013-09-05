#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Json.h"
#include "cinder/ip/Premultiply.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"

#include "puptent/RenderSystem.h"
#include "puptent/PhysicsSystem.h"
#include "puptent/TextureAtlas.h"
#include "puptent/SpriteSystem.h"
#include "puptent/ParticleSystem.h"
#include "puptent/ExpiresSystem.h"

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
    Vec2f center = getWindowCenter();
    float max_dist = center.length();
    for( auto& loc : mElements )
    {
      loc->rotation = fmodf( loc->rotation - M_PI * 0.01f, M_PI * 2 );
      float theta = loc->position.distance( center );
      loc->scale = cos( -time * 0.66f + M_PI * theta / max_dist );
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
  double                    mAverageUpdateTime = 1.0;
  double                    mAverageRenderTime = 1.0;
  Timer                     mTimer;
  TextureAtlas              mTextureAtlas;
};

void PupTentApp::prepareSettings( Settings *settings )
{
  settings->disableFrameRate();
  settings->setWindowSize( 1024, 768 );
//  settings->setFullScreen();
}

void PupTentApp::setup()
{
  gl::enableVerticalSync();
  Surface sprite_surf{ loadImage( loadAsset( "spritesheet.png" ) ) };
  if( !sprite_surf.isPremultiplied() )
  {
    ip::premultiply( &sprite_surf );
  }
  TextureAtlasRef atlas = TextureAtlas::create( sprite_surf, JsonTree( loadAsset( "spritesheet.json" ) ) );
  JsonTree animations{ loadAsset( "animations.json" ) };

  mEvents = EventManager::make();
  mEntities = EntityManager::make(mEvents);
  mSystemManager = SystemManager::make( mEntities, mEvents );
  mSystemManager->add<ExpiresSystem>();
  mSystemManager->add<MovementSystem>();
  mSystemManager->add<ParticleSystem>();
  auto physics = mSystemManager->add<PhysicsSystem>();
  physics->createBoundaryRect( getWindowBounds() );
  auto renderer = mSystemManager->add<RenderSystem>();
  renderer->setTexture( atlas->getTexture() );
  shared_ptr<SpriteAnimationSystem> sprite_system{ new SpriteAnimationSystem{ atlas, animations } };
  mSystemManager->add( sprite_system );
  mSystemManager->configure();


  Rand r;
  Vec2f center = getWindowCenter();
  float max_dist = center.length();
  Entity entity;
  for( int i = 0; i < 5000; ++i )
  {
    entity = mEntities->create();
    auto loc = shared_ptr<Locus>{ new Locus };
    // get an animation out of the sprite system
    auto anim = sprite_system->createSpriteAnimation( "dot" );
    anim->current_index = r.nextInt( 0, 10 );
    loc->position = { r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) };
    loc->rotation = r.nextFloat( M_PI * 2 );
    loc->registration_point = { 20.0f, 10.0f }; // center of the mesh created below
    float dist = loc->position.distance( center );
    loc->render_layer = dist;
//    auto color = ColorA::gray( math<float>::clamp( lmap( dist, 0.0f, max_dist - 10.0f, 0.0f, 1.0f ) ) );
    auto color = Color( CM_HSV, r.nextFloat( 0.4f, 1.0f ), 0.8f, 0.8f );
//    entity.assign( physics->createCircle( loc->position, atlas->get( "d-0001" ).size.x / 16.0f ) );
    auto mesh = entity.assign<RenderMesh>( 4 );
    mesh->setAsBox( { 0.0f, 0.0f, 40.0f, 20.0f } );
    for( auto &v : mesh->vertices )
    {
      v.color = color;
    }
//    entity.assign( anim );
    entity.assign( loc );
    RenderPass pass = eMultiplyPass; // r.nextFloat() < 0.9f ? eNormalPass : eMultiplyPass;
    entity.assign<RenderData>( mesh, loc, pass );
    entity.assign<Expires>( r.nextFloat( 1.0f, 20.0f ) );
  }

  renderer->checkOrdering();

  getWindow()->getSignalMouseDown().connect( [=]( MouseEvent &event ) mutable
  {
    if( entity.valid() )
    {
      if( entity.component<SpriteAnimation>() )
      {
        cout << "Removing Sprite Animation component: " << entity << endl;
        entity.remove<SpriteAnimation>();
      }
      else
      {
        cout << "Adding Mesh component: " << entity << endl;
        auto loc = entity.component<Locus>();
        if( loc )
        {
          loc->render_layer = 1000;
        }
        auto mesh = RenderMeshRef{ new RenderMesh{ 4 } };
        // perhaps have a component to hang on to texturing data
        mesh->matchTexture( atlas->get( "dl-0001" ) );
        ColorA color{ 1.0f, 1.0f, 1.0f, 1.0f };
        for( auto &v : mesh->vertices )
        {
          v.color = color;
        }
        entity.assign<RenderMesh>( mesh );
        entity.component<RenderData>()->mesh = mesh;
      }
    }
  });

  mTimer.start();
}

void PupTentApp::update()
{
  double dt = mTimer.getSeconds();
  mTimer.start();
  Timer up;
  up.start();
//  mSystemManager->system<PhysicsSystem>()->stepPhysics(); // could parallelize this with sprite animation and some other things...
//  mSystemManager->update<PhysicsSystem>( dt );
  mSystemManager->update<ExpiresSystem>( dt );
  mSystemManager->update<MovementSystem>( dt );
  mSystemManager->update<SpriteAnimationSystem>( dt );
  mSystemManager->update<ParticleSystem>( dt );
  mSystemManager->update<RenderSystem>( dt );
  double ms = up.getSeconds() * 1000;
  mAverageUpdateTime = (mAverageUpdateTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 90 == 0 )
  {
    cout << "Update: " << mAverageUpdateTime << ", " << ms << endl;
  }
}

void PupTentApp::draw()
{
	gl::clear( Color::white() );
  gl::color( Color::white() );
  Timer dr;
  dr.start();
//  mSystemManager->system<PhysicsSystem>()->debugDraw();
  mSystemManager->system<RenderSystem>()->draw();
  double ms = dr.getSeconds() * 1000;
  mAverageRenderTime = (mAverageRenderTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 90 == 0 )
  {
    cout << "Render: " << mAverageRenderTime << ", " << ms << endl;
  }
}

CINDER_APP_NATIVE( PupTentApp, RendererGl( RendererGl::AA_MSAA_4 ) )
