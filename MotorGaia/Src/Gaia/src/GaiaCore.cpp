#include "GaiaCore.h"
//Ogre includes
#include <OgreRoot.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <iostream>

#include "RenderSystem.h"
#include "Window.h"

GaiaCore::GaiaCore()
{

}

GaiaCore::~GaiaCore()
{
	delete mRoot;
}

void GaiaCore::setupResources()
{
	// Ogre configuration loader
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	Ogre::String name, locationType;
	Ogre::ConfigFile::SettingsBySection_ settingsBySection = cf.getSettingsBySection();
	for (const auto& p : settingsBySection) {
		for (const auto& r : p.second) {
			locationType = r.first;
			name = r.second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locationType);
		}
	}
}

void GaiaCore::init()
{
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
	mWindowCfg = "window_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
	mWindowCfg = "window.cfg";
#endif

	// Ogre initialization
	mRoot = new Ogre::Root(mPluginsCfg, mWindowCfg);
	setupResources();
	
	if (!mRoot->restoreConfig());

	// Setup window
	Window* win = new Window(mRoot, "Ventana de prueba");
	
	// Setup input
	/*input = GaiaInput::GetInstance();
	input->init();*/
	
}

void GaiaCore::run()
{
	while (true) {
	};
}

void GaiaCore::update()
{

}