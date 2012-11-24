#ifndef INPUTLISTENER_HPP
#define INPUTLISTENER_HPP

#include <Ogre.h>
#include <OIS/OIS.h>

class InputListener :
		public Ogre::FrameListener,
		public Ogre::WindowEventListener
{
public:
	InputListener(Ogre::RenderWindow *window, Ogre::Camera *camera);
	~InputListener();

	bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	void windowResized(Ogre::RenderWindow *rw);
	void windowClosed(Ogre::RenderWindow *rw);

private:
	void initOIS();

	Ogre::RenderWindow *mWindow;
	Ogre::Camera *mCamera;

	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
	OIS::InputManager *mInputManager;
};

#endif // INPUTLISTENER_HPP
