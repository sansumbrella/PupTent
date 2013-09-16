//
//  Copyright (c) 2013 David Wicks, sansumbrella.com.
//  All rights reserved.
//

#pragma once
#include "pockets/ConnectionManager.h"
#include <set>

/**
 Input:
 Receives user input events and stores them in a format
 useful for the main game update.
 Arrow keys get stored as normalized xy forces.
 TODO: mapping from keycodes to "buttons" for easier script access
 TODO: consider moving into PupTent proper, since it's generically useful
*/
typedef std::shared_ptr<class Input> InputRef;
class Input
{
public:
  Input();
  ~Input();
  void update();
  void connect( ci::app::WindowRef window );
  void pause(){ mConnections.block(); }

  //! returns normalized force along xy axes
  ci::Vec2f	getForce() const { return mForce; }
  //! returns true if the key with code key is down
  bool getKeyDown( int key ) const;
  //! returns true if the key with code key was pressed this frame
  bool getKeyPressed( int key ) const;
  //! returns true if the key with code key was released this frame
  bool getKeyReleased( int key ) const;
  //! creates a new input
  static auto create() -> InputRef;
private:
	pk::ConnectionManager	mConnections;
	ci::Vec2f							mForce = ci::Vec2f::zero();
  std::vector<int>      mHeldKeys;
  std::set<int>         mPressedKeys;
  std::set<int>         mReleasedKeys;
  std::vector<ci::app::KeyEvent>  mDownEvents;
  std::vector<ci::app::KeyEvent>  mUpEvents;

  void keyDown( const ci::app::KeyEvent &event );
  void keyUp( const ci::app::KeyEvent &event );
};
