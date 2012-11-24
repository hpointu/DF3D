#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <Ogre.h>


class Application
{
public:
	Application();

	void run();

	bool initApplication();

	void createScene();

	void initListeners();

private:

	Ogre::Root *mRoot;

	Ogre::RenderWindow *mWindow;

	Ogre::SceneManager *mSceneMgr;

	Ogre::Camera *mCamera;

};

#endif // APPLICATION_HPP
