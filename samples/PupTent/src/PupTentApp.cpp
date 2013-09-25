#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Json.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/Easing.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/tags/TagsComponent.h"

#include "puptent/RenderSystem.h"
#include "puptent/PhysicsSystem.h"
#include "puptent/TextureAtlas.h"
#include "puptent/SpriteSystem.h"
#include "puptent/ParticleSystem.h"
#include "puptent/ExpiresSystem.h"
#include "puptent/ScriptSystem.h"
#include "puptent/ParticleBehaviorSystems.h"
#include "KeyboardInput.h"

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

class PupTentApp : public AppNative
{
public:
  void prepareSettings( Settings *settings );
	void setup();
	void update() override;
	void draw() override;
  Entity createPlayer();
  Entity createTreasure();
  Entity createRibbon();
  Entity createLine();
private:
  shared_ptr<EventManager>  mEvents;
  shared_ptr<EntityManager> mEntities;
  shared_ptr<SystemManager> mSystemManager;
  SpriteAnimationSystemRef  mSpriteSystem;
  double                    mAverageUpdateTime = 1.0;
  double                    mAverageRenderTime = 1.0;
  Timer                     mTimer;
  TextureAtlas              mTextureAtlas;
};

void PupTentApp::prepareSettings( Settings *settings )
{
  settings->disableFrameRate();
  settings->setWindowSize( 1024, 768 );
  settings->setFullScreen();
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

  // Set up entity manager and systems
  mEvents = EventManager::make();
  mEntities = EntityManager::make(mEvents);
  mSystemManager = SystemManager::make( mEntities, mEvents );
  mSystemManager->add<ExpiresSystem>();
  mSystemManager->add<ParticleSystem>();
  mSystemManager->add<ScriptSystem>();
  mSpriteSystem = mSystemManager->add<SpriteAnimationSystem>( atlas, animations );
  auto physics = mSystemManager->add<PhysicsSystem>();
  physics->createBoundaryRect( getWindowBounds() );
  auto renderer = mSystemManager->add<RenderSystem>();
  renderer->setTexture( atlas->getTexture() );
  mSystemManager->configure();

  createPlayer();
  for( int i = 0; i < 1000; ++i )
  {
    createTreasure();
  }
  renderer->checkOrdering();

  createRibbon();
  createLine();
  mTimer.start();
}

Entity PupTentApp::createRibbon()
{
  Entity e = mEntities->create();
  auto loc = e.assign<Locus>();
  auto mesh = e.assign<RenderMesh>( 100 );
  e.assign<RenderData>( mesh, loc );
  vector<Vec2f> positions;
  for( int i = 0; i < 50; ++i )
  {
    positions.push_back( Vec2f{ 50.0f + i * 10.0f, 100.0f + sin( i * 0.2f ) * 50.0f } );
  }
  try {
    mesh->setAsRibbon( positions, 20.0f, false );
  } catch ( exception &exc ) {
    cout << "Error: " << exc.what() << endl;
  }
  return e;
}

Entity PupTentApp::createLine()
{
  Entity e = mEntities->create();
  auto loc = e.assign<Locus>();
  auto mesh = e.assign<RenderMesh>();
  e.assign<RenderData>( mesh, loc, 20 );
  e.assign<ScriptComponent>( [=](Entity self, EntityManagerRef es, EventManagerRef events, double dt){
    mesh->setAsLine( getWindowCenter(), getMousePos(), 8.0f );
  } );
  return e;
}

Entity PupTentApp::createPlayer()
{
  Rand r;
  auto player = mEntities->create();
  auto loc = shared_ptr<Locus>{ new Locus };
  // get an animation out of the sprite system
  auto anim = mSpriteSystem->createSpriteAnimation( "jellyfish" );
  // ping-pong animation
//  anim->looping = false;
//  anim->finish_fn = []( SpriteAnimationRef animation ){
//    animation->rate *= -1.0f;
//  };
  loc->position = getWindowCenter();
  loc->rotation = r.nextFloat( M_PI * 2 );
  loc->registration_point = { 20.0f, 10.0f }; // center of the mesh created below
  auto mesh = player.assign<RenderMesh>( 4 );
  player.assign( anim );
  player.assign( loc );
  player.assign<RenderData>( mesh, loc, 10 );
  auto verlet = player.assign<Particle>( loc );
  verlet->friction = 0.9f;
  verlet->rotation_friction = 0.5f;
  auto input = KeyboardInput::create();
  input->connect( getWindow() );
  // give custom behavior to the player
  player.assign<ScriptComponent>( [=](Entity self, EntityManagerRef es, EventManagerRef events, double dt){
   auto locus = self.component<Locus>();
   locus->position += input->getForce() * dt * 100.0f;
   auto view = tags::TagsComponent::view( es->entities_with_components<Locus>(), "treasure" );
   for( auto entity : view )
   {
     auto other_loc = entity.component<Locus>();
     if( other_loc->position.distance( locus->position ) < 50.0f )
     {
       cout << "Eating treasure" << endl;
       entity.destroy();
     }
   }
  } );
  return player;
}

Entity PupTentApp::createTreasure()
{
  auto entity = mEntities->create();
  auto loc = shared_ptr<Locus>{ new Locus };
  // get an animation out of the sprite system
  auto anim = mSpriteSystem->createSpriteAnimation( "dot" );
  anim->current_index = Rand::randInt( 0, 10 );
  loc->position = { Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) };
  loc->rotation = Rand::randFloat( M_PI * 2 );
  loc->registration_point = { 20.0f, 10.0f }; // center of the mesh created below
  auto color = Color( CM_HSV, Rand::randFloat( 0.4f, 1.0f ), 0.8f, 0.8f );
  auto mesh = entity.assign<RenderMesh>( 4 );
  mesh->setColor( color );
  entity.assign( anim );
  entity.assign( loc );
  entity.assign<RenderData>( mesh, loc, 50, eNormalPass );
  // randomized expire time, weighted toward end
//  entity.assign<Expires>( easeOutQuad( Rand::randFloat() ) * 9.0f + 1.0f );
  entity.assign<tags::TagsComponent>( "treasure" );
//  entity.assign<SeekComponent>( predator_loc, Vec2f{-1.0f, -0.5f} );
  return entity;
}

void PupTentApp::update()
{
  double dt = mTimer.getSeconds();
  mTimer.start();
  Timer up;
  up.start();
  mSystemManager->update<ExpiresSystem>( dt );
  mSystemManager->update<ScriptSystem>( dt );
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
	gl::clear( Color::black() );
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
