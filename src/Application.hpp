#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <Ogre.h>
#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/Terrain/OgreTerrainGroup.h>

#include <OGRE/ExampleApplication.h>

class Application //: public ExampleApplication
{
public:
	Application();

	void run();

	bool initApplication();

	void createScene();

	void initListeners();

	void configureTerrainDefault(Ogre::Light *l);
	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain *t);
private:

	Ogre::Root *mRoot;

	Ogre::RenderWindow *mWindow;

	Ogre::SceneManager *mSceneMgr;

	Ogre::Camera *mCamera;

	Ogre::TerrainGlobalOptions *mTerrainGlobals;
	Ogre::TerrainGroup *mTerrainGroup;
	bool mTerrainsImported;

};

#endif // APPLICATION_HPP
