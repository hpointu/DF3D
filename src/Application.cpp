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

	// adding test light
	Ogre::Light *l = mSceneMgr->createLight("light0");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(0.55, -0.3, 0.75);
	l->setDiffuseColour(1,1,1);
	l->setSpecularColour(.4,.4,.4);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(.2, .2, .2));

	// testing terrain
	mTerrainGlobals = new Ogre::TerrainGlobalOptions();

	mTerrainGroup = new Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
	mTerrainGroup->setFilenameConvention(Ogre::String("TestTerrain"), Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	configureTerrainDefault(l);

	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			defineTerrain(x, y);

	// sync load since we want everything in place when we start
	mTerrainGroup->loadAllTerrains(true);

	if (mTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}

	mTerrainGroup->freeTemporaryResources();
}

void Application::initBlendMaps(Terrain *t)
{

}

void Application::defineTerrain(long x, long y)
{

}

void Application::configureTerrainDefault(Ogre::Light *l)
{
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600; // due terrain.png is 8 bpp
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;

	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
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
