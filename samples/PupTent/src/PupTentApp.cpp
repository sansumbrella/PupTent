#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"

#include "pockets/Types.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace entityx;
using pockets::Vertex2d;

struct Locus : Component<Locus>
{
  Locus() = default;
  Locus( const ci::Vec2f &pos, const ci::Vec2f &registration, float rot, shared_ptr<Locus> parent=nullptr ):
  position( pos ),
  registration_point( registration ),
  rotation( rot ),
  parent( parent )
  {}
  ci::Vec2f         position = ci::Vec2f::zero();
  ci::Vec2f         registration_point = ci::Vec2f::zero();
  float             rotation = 0.0f;
  shared_ptr<Locus> parent = nullptr;

  ci::MatrixAffine2f  toMatrix() const{
    MatrixAffine2f mat;
    mat.translate( position + registration_point );
    mat.rotate( rotation );
    mat.translate( -registration_point );
    if( parent ){ mat = parent->toMatrix() * mat; }
    return mat;
  }
};

struct Mesh : Component<Mesh>
{
  Mesh( const ci::Rectf &bounds={ -20.0f, -10.0f, 20.0f, 10.0f } )
  {
    vertices.assign( 4, Vertex2d{} );
    vertices[0].position = bounds.getUpperRight();
    vertices[1].position = bounds.getUpperLeft();
    vertices[2].position = bounds.getLowerRight();
    vertices[3].position = bounds.getLowerLeft();

    ColorA color{ CM_HSV, Rand::randFloat(), 0.9f, 0.9f, 1.0f };
    for( auto &v : vertices )
    {
      v.color = color;
    }
  }
  std::vector<Vertex2d> vertices;
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
    for( auto entity : es->entities_with_components<Locus>() )
    {
      auto loc = entity.component<Locus>();
      loc->rotation = fmodf( loc->rotation + M_PI * 0.01f, M_PI * 2 );
    }
  }
};

struct RenderSystem : public System<RenderSystem>
{
  void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
  {
    mVertices.clear();
    // assemble all vertices
    for( auto entity : es->entities_with_components<Locus, Mesh>() )
    {
      auto loc = entity.component<Locus>();
      auto mesh = entity.component<Mesh>();
      auto mat = loc->toMatrix();
      if( !mVertices.empty() )
      { // create degenerate triangle between previous and current shape
        mVertices.emplace_back( mVertices.back() );
        auto vert = mesh->vertices.front();
        mVertices.emplace_back( Vertex2d{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
      for( auto &vert : mesh->vertices )
      {
        mVertices.emplace_back( Vertex2d{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
    }
  }

  void draw()
  {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].position.x );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].tex_coord.x );
    glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex2d ), &mVertices[0].color.r );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  }
private:
//  struct RenderData{
//    Locus2dRef  locus;
//    MeshRef     mesh;
//    int         layer;
//  };
  std::vector<Vertex2d> mVertices;
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
  double            mAverageRenderTime = 0;
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
    auto loc = shared_ptr<Locus>{ new Locus };
    loc->position = { r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) };
    loc->rotation = Rand::randFloat( M_PI * 2 );
    entity.assign<Locus>( loc );
    entity.assign<Mesh>();
//    entity.assign<Velocity>( r.nextFloat( -5.0f, 5.0f ), r.nextFloat( -5.0f, 5.0f ) );
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
//  double start = getElapsedSeconds();
  mSystemManager->update<MovementSystem>( dt );
//  double end = getElapsedSeconds();
//  cout << "Update: " << (end - start) * 1000 << endl;
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

CINDER_APP_NATIVE( PupTentApp, RendererGl )

