//
//  Copyright (c) 2013 David Wicks, sansumbrella.com.
//  All rights reserved.
//

#pragma once

#include "puptent/PupTent.h"

class PlayerController
{
public:
  PlayerController();
  ~PlayerController();
  void connect( ci::app::WindowRef window );
  void disconnect(){};
  void keyDown( const ci::app::KeyEvent &event );
  void setPlayer( pt::Entity entity ){ mPlayer = entity; }
private:
  pt::Entity  mPlayer;
};
