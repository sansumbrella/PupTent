#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Json.h"
#include "cinder/ip/Premultiply.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"

#include "puptent/BatchRenderSystem2d.h"
#include "puptent/TextureAtlas.h"
#include "puptent/SpriteAnimation.h"

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
      loc->scale = math<float>::sin( 0.25f * time + M_PI * loc->position.x / 640.0f + M_PI * loc->position.y / 480.0f );
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
  mSystemManager->add<MovementSystem>();
  auto renderer = mSystemManager->add<BatchRenderSystem2d>();
  renderer->setTexture( atlas->getTexture() );
  shared_ptr<SpriteAnimationSystem> sprite_system{ new SpriteAnimationSystem{ atlas, animations } };
  mSystemManager->add( sprite_system );
  mSystemManager->configure();


  Rand r;
  Vec2f center = getWindowCenter();
  Entity entity;
  for( int i = 0; i < 10; ++i )
  {
    entity = mEntities->create();
    auto loc = shared_ptr<Locus>{ new Locus };
//    loc->registration_point = { 0.0f, -75.0f };
    // get an animation out of the sprite system
    auto anim = sprite_system->createSpriteAnimation( "deerleg" );
//    anim->setFrameIndex( r.randInt( anim->drawings.size() ) );
    auto mesh = anim->mesh;
    loc->position = { r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) };
    loc->rotation = r.nextFloat( M_PI * 2 );
    float dist = loc->position.distance( center );
    ColorA color{ CM_HSV, 0.0f, 0.0f, lmap( dist, 0.0f, 0.75f * getWindowWidth(), 0.0f, 1.0f ), 1.0f };
    for( auto &v : mesh->vertices )
    {
      v.color = color;
    }
    mesh->render_layer = dist;
    entity.assign( anim );
    entity.assign( loc );
    entity.assign( mesh );
  }

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
        auto mesh = RenderMesh2dRef{ new RenderMesh2d{ 4 } };
        // perhaps have a component to hang on to texturing data
        mesh->setAsTexture( atlas->get( "dl-0001" ) );
        mesh->render_layer = 1000;
        ColorA color{ 1.0f, 1.0f, 1.0f, 1.0f };
        for( auto &v : mesh->vertices )
        {
          v.color = color;
        }
        entity.assign<RenderMesh2d>( mesh );
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
  mSystemManager->update<SpriteAnimationSystem>( dt );
  mSystemManager->update<BatchRenderSystem2d>( dt );
  double end = getElapsedSeconds();
  double ms = (end - start) * 1000;
  mAverageUpdateTime = (mAverageUpdateTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 90 == 0 )
  {
    cout << "Update: " << mAverageUpdateTime << ", " << ms << endl;
  }
}

void PupTentApp::draw()
{
	gl::clear( Color::black() );
  gl::disableDepthRead();
  gl::disableDepthWrite();
  gl::color( Color::white() );
  double start = getElapsedSeconds();
  mSystemManager->system<BatchRenderSystem2d>()->draw();
  double end = getElapsedSeconds();
  double ms = (end - start) * 1000;
  mAverageRenderTime = (mAverageRenderTime * 59.0 + ms) / 60.0;
  if( getElapsedFrames() % 90 == 0 )
  {
    cout << "Render ms: " << mAverageRenderTime << ", " << ms << endl;
  }
}

CINDER_APP_NATIVE( PupTentApp, RendererGl( RendererGl::AA_MSAA_8 ) )

