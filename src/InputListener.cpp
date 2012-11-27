#include "InputListener.hpp"

#include <sstream>

InputListener::InputListener(Ogre::RenderWindow *window, Ogre::Camera *camera,
									  Ogre::Plane *groundPlane, Ogre::SceneManager *sceneMgr) :
	mWindow(window),
	mCamera(camera)
{
	initOIS();
	mGroundPlane = groundPlane;
	mSceneMgr = sceneMgr;
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

	mMouse = static_cast<OIS::Mouse*>( mInputManager->createInputObject(OIS::OISMouse, true) );
	mMouse->setEventCallback(this);

	windowResized(mWindow);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
}

CEGUI::MouseButton InputListener::convertButton(OIS::MouseButtonID buttonID)
{
	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}

bool InputListener::mouseMoved(const OIS::MouseEvent &arg)
{
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	return true;
}

bool InputListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));

	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width),
																		  mousePos.d_y/float(arg.state.height));

	std::pair<bool, Ogre::Real> result = mouseRay.intersects(*mGroundPlane);
	if(result.first)
	{
		Ogre::Vector3 pos = mouseRay.getPoint(result.second);
		Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject( mSceneMgr->createEntity("Plane.mesh") );
		node->setPosition(pos);
		node->pitch(Ogre::Degree(90));
	}

	return true;
}

bool InputListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	return true;
}

void InputListener::windowResized(Ogre::RenderWindow *rw)
{
	unsigned int width, height, depth;
	int top, left;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;

	mCamera->setAspectRatio( Ogre::Real(width) / Ogre::Real(height) );

	CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(width, height));
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


	//	const OIS::MouseState &ms = mMouse->getMouseState();
	//	float mouseSens = .1f;

	//	Ogre::Degree rx(-ms.Y.rel*mouseSens);
	//	Ogre::Degree ry(-ms.X.rel*mouseSens);

	//	mCamera->yaw(ry);
	//	mCamera->pitch(rx);


	//	mCamera->moveRelative(movement);
	mCamera->move(movement);

	return true;
}

InputListener::~InputListener()
{
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}
