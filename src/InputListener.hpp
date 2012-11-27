#ifndef INPUTLISTENER_HPP
#define INPUTLISTENER_HPP

#include <CEGUI.h>
#include <Ogre.h>
#include <OIS/OIS.h>

class InputListener :
		public Ogre::FrameListener,
		public Ogre::WindowEventListener,
		public OIS::MouseListener
{
public:
	InputListener(Ogre::RenderWindow *window, Ogre::Camera *camera, Ogre::Plane *groundPlane, Ogre::SceneManager *sceneMgr);
	~InputListener();

	bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	void windowResized(Ogre::RenderWindow *rw);
	void windowClosed(Ogre::RenderWindow *rw);

	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

private:
	void initOIS();

	Ogre::RenderWindow *mWindow;
	Ogre::Camera *mCamera;

	Ogre::Plane *mGroundPlane;

	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
	OIS::InputManager *mInputManager;

	Ogre::SceneManager *mSceneMgr;
};

#endif // INPUTLISTENER_HPP
