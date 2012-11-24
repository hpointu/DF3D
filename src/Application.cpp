#include "Application.hpp"
#include "InputListener.hpp"

Application::Application()
{
	if(initApplication()) {
		createScene();
		initListeners();
	}
}

void Application::initListeners()
{
	mRoot->addFrameListener(new InputListener(mWindow, mCamera));
}

bool Application::initApplication()
{
	mRoot = new Ogre::Root();

	Ogre::ConfigFile cfg;
	cfg.load("resources.cfg");

	Ogre::ConfigFile::SectionIterator it = cfg.getSectionIterator();

	Ogre::String secName, typeName, archName;

	while(it.hasMoreElements())
	{
		secName = it.peekNextKey();

		Ogre::ConfigFile::SettingsMultiMap *settings = it.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
						archName, typeName, secName);
		}
	}

	// trying to load config, or display config prompt
	if( !mRoot->restoreConfig() && !mRoot->showConfigDialog() )
	{
		return false;
	}

	mWindow = mRoot->initialise(true, "Test Title");

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	return true;
}

void Application::createScene()
{
	// creating scene manager
	mSceneMgr = mRoot->createSceneManager("DefaultSceneManager", "mainSceneManager");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));

	// adding camera & viewport
	mCamera = mSceneMgr->createCamera("mainCamera");
	mCamera->setPosition(0,0,100);
	mCamera->lookAt(0,0,0);
	mCamera->setNearClipDistance(5);

	Ogre::Viewport *viewport = mWindow->addViewport(mCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	mCamera->setAspectRatio(
				Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight())
				);

	// adding test mesh
	Ogre::Entity *e = mSceneMgr->createEntity("testMesh", "penguin.mesh");
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(e);

}

void Application::run()
{
	while(true)
	{
		// update window
		Ogre::WindowEventUtilities::messagePump();

		if(mWindow->isClosed())
			return;
		if(!mRoot->renderOneFrame())
			return;
	}
}
