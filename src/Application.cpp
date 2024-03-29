#include "Application.hpp"
#include "InputListener.hpp"

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();

}

Application::Application()
{
	if(initApplication()) {
		createScene();
		initListeners();
	}
}

void Application::initListeners()
{
	mRoot->addFrameListener(new InputListener(mWindow, mCamera, &mGroundPlane, mSceneMgr));
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
	// init CEGUI
	mCeguiRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*mWindow);

	// chargement resources cegui
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().create("OgreTray.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("OgreTrayImages", "MouseArrow");
	CEGUI::MouseCursor::getSingleton().setImage(CEGUI::System::getSingleton().getDefaultMouseCursor());

//	CEGUI::System::getSingleton().getDe

	// creating scene manager
	mSceneMgr = mRoot->createSceneManager("DefaultSceneManager", "mainSceneManager");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));

	// adding camera & viewport
	mCamera = mSceneMgr->createCamera("mainCamera");
	mCamera->setPosition(0,100,100);
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

	/*
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
	*/


	// testing plane
	mGroundPlane = Ogre::Plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
																 mGroundPlane, 200, 200, 10, 10, true, 1, 2, 2, Ogre::Vector3::UNIT_Z);

	Ogre::Entity *groundEntity = mSceneMgr->createEntity("groundEntity", "ground");
	Ogre::SceneNode *groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	groundNode->attachObject(groundEntity);

	Ogre::Material *matPtr = static_cast<Ogre::Material *>(Ogre::MaterialManager::getSingleton().create("test_mat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true).getPointer());
	Ogre::Pass *pass = matPtr->getTechnique(0)->getPass(0);
	Ogre::TextureUnitState *tex = pass->createTextureUnitState("terrain.png");

	tex->setTextureFiltering(Ogre::TFO_ANISOTROPIC);
	tex->setTextureAnisotropy(8);
	matPtr->load(); // important !

	groundEntity->setMaterialName("test_mat");



}

void Application::initBlendMaps(Terrain *terrain)
{
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

void Application::defineTerrain(long x, long y)
{
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
	{
		mTerrainGroup->defineTerrain(x, y);
	}
	else
	{
		Ogre::Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);
		mTerrainsImported = true;
	}
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
