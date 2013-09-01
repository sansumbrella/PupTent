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
  Mesh( int render_layer=0, const ci::Rectf &bounds={ -20.0f, -10.0f, 20.0f, 10.0f } ):
  render_layer( render_layer )
  {
    vertices.assign( 4, Vertex2d{} );
    vertices[0].position = bounds.getUpperRight();
    vertices[1].position = bounds.getUpperLeft();
    vertices[2].position = bounds.getLowerRight();
    vertices[3].position = bounds.getLowerLeft();
  }
  std::vector<Vertex2d> vertices;
  int                   render_layer = 0;
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

/**
 Basic layer-sorted rendering system
*/
struct RenderSystem : public System<RenderSystem>, Receiver<RenderSystem>
{
  typedef pair<shared_ptr<Locus>, shared_ptr<Mesh>> MeshPair;

  void configure( EventManager &event_manager )
  {
    event_manager.subscribe<EntityDestroyedEvent>( *this );
    event_manager.subscribe<ComponentAddedEvent<Mesh>>( *this );
  }

  void update( shared_ptr<EntityManager> es, shared_ptr<EventManager> events, double dt ) override
  {
    // build our sorted geometry list from query if the layers/components have changed
    // need to rebuild if any meshes are removed or added
    if( mGeometry.empty() )
    {
      for( auto entity : es->entities_with_components<Locus, Mesh>() )
      {
        mGeometry.emplace_back( entity.component<Locus>(), entity.component<Mesh>() );
      }
      stable_sort( mGeometry.begin(), mGeometry.end(), []( const MeshPair &lhs, const MeshPair &rhs ) -> bool {
        return lhs.second->render_layer < rhs.second->render_layer;
      } );
    }
    // assemble all vertices
    mVertices.clear();
    for( auto pair : mGeometry )
    {
      auto loc = pair.first;
      auto mesh = pair.second;
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

  void receive( const EntityDestroyedEvent &event )
  {
    auto entity = event.entity;
    if( entity.component<Mesh>() )
    { // if a mesh was destroyed, we will update our render list this frame
      mGeometry.clear();
    }
  }

  void receive( const ComponentAddedEvent<Mesh> &event )
  { // empty our geometry
    mGeometry.clear();
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
  std::vector<MeshPair> mGeometry;
  std::vector<Vertex2d> mVertices;
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
    auto mesh = shared_ptr<Mesh>{ new Mesh };
    ColorA color{ CM_HSV, r.nextFloat( 1.0f ), 0.9f, 0.9f, 1.0f };
    for( auto &v : mesh->vertices )
    {
      v.color = color;
    }
    loc->position = { r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ) };
    loc->rotation = r.nextFloat( M_PI * 2 );
    mesh->render_layer = loc->position.distance( center );
    entity.assign<Locus>( loc );
    entity.assign<Mesh>( mesh );
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

