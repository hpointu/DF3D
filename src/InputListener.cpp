#include "InputListener.hpp"

#include <sstream>

InputListener::InputListener(Ogre::RenderWindow *window, Ogre::Camera *camera):
	mWindow(window),
	mCamera(camera)
{
	initOIS();
}

void InputListener::initOIS()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("-**- Initializing OIS -**-");

	OIS::ParamList params;
	std::stringstream ss;
	size_t windowHND = 0;

	mWindow->getCustomAttribute("WINDOW", &windowHND);
	ss << windowHND;
	params.insert(std::make_pair(std::string("WINDOW"), ss.str()));

	mInputManager = OIS::InputManager::createInputSystem(params);

	mKeyboard = static_cast<OIS::Keyboard*>( mInputManager->createInputObject(OIS::OISKeyboard, false) );

	mMouse = static_cast<OIS::Mouse*>( mInputManager->createInputObject(OIS::OISMouse, false) );

	windowResized(mWindow);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
}

void InputListener::windowResized(Ogre::RenderWindow *rw)
{
	unsigned int width, height, depth;
	int top, left;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void InputListener::windowClosed(Ogre::RenderWindow *rw)
{
	if(rw == mWindow)
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject(mKeyboard);
			mInputManager->destroyInputObject(mMouse);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = NULL;
		}
	}
}

bool InputListener::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	(void)&evt;

	if(mWindow->isClosed())
		return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
		return false;
	}

	Ogre::Vector3 movement(0,0,0);
	float speed = 100*evt.timeSinceLastFrame;

	if(mKeyboard->isKeyDown(OIS::KC_Q))
		movement.x -= speed;
	if(mKeyboard->isKeyDown(OIS::KC_D))
		movement.x += speed;
	if(mKeyboard->isKeyDown(OIS::KC_Z))
		movement.z -= speed;
	if(mKeyboard->isKeyDown(OIS::KC_S))
		movement.z += speed;
	if(mKeyboard->isKeyDown(OIS::KC_SPACE))
		movement.y += speed;
	if(mKeyboard->isKeyDown(OIS::KC_LSHIFT))
		movement.y -= speed;


	const OIS::MouseState &ms = mMouse->getMouseState();
	float mouseSens = .3f;

	Ogre::Degree rx(-ms.Y.rel*mouseSens);
	Ogre::Degree ry(-ms.X.rel*mouseSens);

	mCamera->yaw(ry);
	mCamera->pitch(rx);
	mCamera->moveRelative(movement);

	return true;
}

InputListener::~InputListener()
{
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}
