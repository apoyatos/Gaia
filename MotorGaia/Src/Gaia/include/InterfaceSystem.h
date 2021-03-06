#pragma once
#ifndef INTERFACE_SYSTEM_H
#define INTERFACE_SYSTEM_H

#include "Singleton.h"
#include "MouseEventListener.h"
#include "KeyboardEventListener.h"
#include "WindowEventListener.h"
#include "ControllerEventListener.h"
#include "UIElement.h"
#include "Window.h"

#include <CEGUI\CEGUI.h>
#include <string>
#include <map>

namespace CEGUI
{
	class Window;
	class OgreRenderer;
	class String;
	class EventArgs;
}

class Window;

typedef std::pair<std::string, std::function<bool()>> UIEvent;

class GAIA_API InterfaceSystem : public Singleton<InterfaceSystem>, public MouseEventListener, public KeyboardEventListener, public WindowEventListener, public ControllerEventListener
{
	friend class GaiaCore;
	friend class ResourcesManager;
	friend class UILayout;
private:
	CEGUI::OgreRenderer* renderer;
	UIElement* root;

	// For controller input
	CEGUI::Window* currentLayout;
	std::map<std::string, CEGUI::Window*> buttons;
	std::string firstButton;
	std::string currentButton;
	float scrollAmount;

	std::string ceguiResourceFile;

	bool controllerNavigation;
	bool keyboardNavigation;

	void layoutButtonSearch(UIElement* parent);
	void searchNextVisibleButton(std::string direction);
	/*------------------------------*/

	double deltaX, deltaY;
#ifdef _DEBUG
	UIElement* fpsText;
#endif

	static std::map<std::string, UIEvent> events;

	std::map<std::string, CEGUI::String> eventTypes;

	void setupResources();
	CEGUI::Key::Scan SDLKeyToCEGUIKey(int key);

	void processControllerButtonDown(int index, int button);
	void processControllerButtonUp(int index, int button);
	void moveScrollBar(CEGUI::Window* scrollBar, float amount);

	void moveControllerToButton();
	bool checkFirstControllerInput();

	void processMouseMotion(int x, int y);

	void processKeyPress(std::string keyName, int key);
	void processKeyUp(std::string keyName, int key);

public:
	InterfaceSystem();
	~InterfaceSystem();

private:
	void init(Window* window);
	void render();
	void update(float deltaTime);
	void close();

	void createRoot();
	void initDefaultResources(const std::string& filename);
	CEGUI::String getEventType(std::string eventType);
	static UIEvent getEvent(const std::string& eventName);

public:
	static void registerEvent(const std::string& eventName, UIEvent event);
	void unregisterEvent(const std::string& eventName);

	UIElement* getRoot();
	UIElement* loadLayout(const std::string& filename);

	void initControllerMenuInput(UIElement* newRoot = nullptr);
	void clearControllerMenuInput();

	/// Percentage of scrollbar that changes when moved by controller or keyboard
	void setScrollbarControllerAmount(float percent);

	void setControllerNavigation(bool enable);
	void setKeyboardNavigation(bool enable);

	bool isControllerNavigationEnabled() const;
	bool isKeyboardNavigationEnabled() const;

};

#endif