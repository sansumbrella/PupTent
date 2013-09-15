//
//  Copyright (c) 2013 David Wicks, sansumbrella.com.
//  All rights reserved.
//

#include "PlayerController.h"
#include "puptent/Locus.h"

using namespace puptent;
using namespace cinder;
using namespace cinder::app;

PlayerController::PlayerController()
{}

PlayerController::~PlayerController()
{}

void PlayerController::connect(ci::app::WindowRef window)
{
  window->getSignalKeyDown().connect( [this]( const KeyEvent &event ){ keyDown( event ); } );
}

void PlayerController::keyDown(const ci::app::KeyEvent &event)
{
  // do something with the key info
  Vec2f force{ 0.0f, 0.0f };
  switch ( event.getCode() )
  {
    case KeyEvent::KEY_LEFT:
      force.x = -10.0f;
      break;
    case KeyEvent::KEY_RIGHT:
      force.x = 10.0f;
      break;
    case KeyEvent::KEY_DOWN:
      force.y = 10.0f;
      break;
    case KeyEvent::KEY_UP:
      force.y = -10.0f;
    default:
      break;
  }
  mPlayer.component<Locus>()->position += force;
}