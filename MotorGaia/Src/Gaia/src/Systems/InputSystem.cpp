#include "InputSystem.h"
#include "DebugUtils.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/System.h>


InputSystem::InputSystem() : MOUSE_POSITION_X(0), MOUSE_POSITION_Y(0), MOUSE_WHEEL(0), keyboardState(nullptr)
{

}

InputSystem::~InputSystem()
{

}

// Initialization
void InputSystem::init()
{
	// MOUSE
	MOUSE_POSITION_X = 0;
	MOUSE_POSITION_Y = 0;

	MOUSE_BUTTON_LEFT.hold = false;
	MOUSE_BUTTON_RIGHT.hold = false;
	MOUSE_BUTTON_MIDDLE.hold = false;

	SDL_GetMouseState(&MOUSE_POSITION_X, &MOUSE_POSITION_Y);

	// CONTROLLER SYSTEM
	for (int i = 0; i < 4; i++) controllers[i].controllerIndex = i;

	SDL_GameControllerEventState(true);

	// Callback definitions
	onKeyDown([this](std::string keyName, int key) { processKeyDown(keyName, key); });
	onKeyUp([this](std::string keyName, int key) { processKeyUp(keyName, key); });
	onMouseMotion([this](int x, int y) { processMouseMotion(x, y); });
	onMouseLeftButtonDown([this]() { processMouseLeftButtonDown(); });
	onMouseRightButtonDown([this]() { processMouseRightButtonDown(); });
	onMouseMiddleButtonDown([this]() { processMouseMiddleButtonDown(); });
	onMouseLeftButtonUp([this]() { processMouseLeftButtonUp(); });
	onMouseRightButtonUp([this]() { processMouseRightButtonUp(); });
	onMouseMiddleButtonUp([this]() { processMouseMiddleButtonUp(); });
	onMouseWheelScrollY([this](int value) { processMouseWheelScrollY(value); });
	onControllerButtonDown([this](int index, int button) { processControllerButtonDown(index, button); });
	onControllerButtonUp([this](int index, int button) { processControllerButtonUp(index, button); });
	onControllerDeviceAdded([this](int index) { processControllerDeviceAdded(index); });
	onControllerDeviceRemoved([this](int index) { processControllerDeviceRemoved(index); });
}

// Closing
void InputSystem::close()
{
	// CONTROLLER SYSTEM
	for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (controllers[ControllerIndex].controller)
		{
			SDL_GameControllerClose(controllers[ControllerIndex].controller);
			if (controllers[ControllerIndex].controllerRumble) SDL_HapticClose(controllers[ControllerIndex].controllerRumble);
			controllers[ControllerIndex].isConected = false;
		}
	}

	destroy();
}

void InputSystem::preUpdate()
{
	// Clear previous frame press/release events
	clearInputs();
}

/// MAIN LOOP

void InputSystem::update()
{
	// Save current keyboard state
	keyboardState = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (controllers[i].isConected)
		{
			// Left joystick
			controllers[i].LeftStickX = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTX)) > controllers[i].JOYSTICK_DEAD_ZONE ?
				SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTX) : 0;
			controllers[i].LeftStickY = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTY)) > controllers[i].JOYSTICK_DEAD_ZONE ?
				SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTY) : 0;

			// Right joystick
			controllers[i].RightStickX = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTX)) > controllers[i].JOYSTICK_DEAD_ZONE ?
				SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTX) : 0;
			controllers[i].RightStickY = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTY)) > controllers[i].JOYSTICK_DEAD_ZONE ?
				SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTY) : 0;

			// Triggers
			controllers[i].LeftTrigger = SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
			controllers[i].RightTrigger = SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
		}
		else
		{
			// TODO: This controller is not plugged in.
		}
	}
}

void InputSystem::setDeadZone(int controller, int zone)
{
	if (controller > 0 && controller < 4)
		controllers[controller].JOYSTICK_DEAD_ZONE = zone;
	if (flags) std::cout << "Controller: " << controller << " | Deadzone: " << zone << "\n";
}

// KEYBOARD

/// Returns "true" while "key" is hold
bool InputSystem::isKeyPressed(std::string key)
{
	std::transform(key.begin(), key.end(), key.begin(), ::toupper);
	const bool is_in = keyHold.find(key) != keyHold.end();
	return is_in;
}

/// Returns "true" if "key" has been pressed on latest frame
bool InputSystem::getKeyPress(std::string key)
{
	std::transform(key.begin(), key.end(), key.begin(), ::toupper);
	const bool is_in = keyPress.find(key) != keyPress.end();
	return is_in;
}

/// Returns "true" if "key" has been released on latest frame
bool InputSystem::getKeyRelease(std::string key)
{
	std::transform(key.begin(), key.end(), key.begin(), ::toupper);
	const bool is_in = keyRelease.find(key) != keyRelease.end();
	return is_in;
}

// MOUSE

void InputSystem::processMouseInputDown(SDL_MouseButtonEvent& e)
{
	switch (e.button) {
	case SDL_BUTTON_LEFT:
		MOUSE_BUTTON_LEFT.hold = true;
		MOUSE_BUTTON_LEFT.pressed = true;

		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MouseButton::LeftButton);
		break;
	case SDL_BUTTON_RIGHT:
		MOUSE_BUTTON_RIGHT.hold = true;
		MOUSE_BUTTON_RIGHT.pressed = true;

		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MouseButton::RightButton);
		break;
	case SDL_BUTTON_MIDDLE:
		MOUSE_BUTTON_MIDDLE.hold = true;
		MOUSE_BUTTON_MIDDLE.pressed = true;
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MouseButton::MiddleButton);
		break;
	default:
		break;
	}
}

void InputSystem::processMouseInputUp(SDL_MouseButtonEvent& e)
{
	switch (e.button) {
	case SDL_BUTTON_LEFT:
		MOUSE_BUTTON_LEFT.hold = false;
		MOUSE_BUTTON_LEFT.released = true;
		break;
	case SDL_BUTTON_RIGHT:
		MOUSE_BUTTON_RIGHT.hold = false;
		MOUSE_BUTTON_RIGHT.released = true;
		break;
	case SDL_BUTTON_MIDDLE:
		MOUSE_BUTTON_MIDDLE.hold = false;
		MOUSE_BUTTON_MIDDLE.released = true;
		break;
	default:
		break;
	}
}

/// 'l' = left button | 'r' = right button | 'm' = middle button
bool InputSystem::getMouseButtonClick(char button)
{
	switch (button) {
	case 'l':
		return MOUSE_BUTTON_LEFT.pressed;
		break;
	case 'r':
		return MOUSE_BUTTON_RIGHT.pressed;
		break;
	case 'm':
		return MOUSE_BUTTON_MIDDLE.pressed;
		break;
	default:
		std::cout << "Button does not exist\n";
		;
	}
	return false;
}

/// 'l' = left button | 'r' = right button | 'm' = middle button
bool InputSystem::getMouseButtonHold(char button)
{
	switch (button) {
	case 'l':
		return MOUSE_BUTTON_LEFT.hold;
		break;
	case 'r':
		return MOUSE_BUTTON_RIGHT.hold;
		break;
	case 'm':
		return MOUSE_BUTTON_MIDDLE.hold;
		break;
	default:
		std::cout << "Button does not exist\n";
		;
	}
	return false;
}

/// 'l' = left button | 'r' = right button | 'm' = middle button
bool InputSystem::getMouseButtonRelease(char button)
{
	switch (button) {
	case 'l':
		return MOUSE_BUTTON_LEFT.released;
		break;
	case 'r':
		return MOUSE_BUTTON_RIGHT.released;
		break;
	case 'm':
		return MOUSE_BUTTON_MIDDLE.released;
		break;
	default:
		std::cout << "Button does not exist\n";
		;
	}
	return false;
}


// CONTROLLER

void InputSystem::controllerInputDown(int index)
{
	if (!controllers[index].isConected)return;

	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && !controllers[index].Up) {
		controllers[index].Up = true;
		controllers[index].buttonPress.emplace("UP");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && !controllers[index].Down) {
		controllers[index].Down = true;
		controllers[index].buttonPress.emplace("DOWN");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && !controllers[index].Left) {
		controllers[index].Left = true;
		controllers[index].buttonPress.emplace("LEFT");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && !controllers[index].Right) {
		controllers[index].Right = true;
		controllers[index].buttonPress.emplace("RIGHT");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_START) && !controllers[index].Start) {
		controllers[index].Start = true;
		controllers[index].buttonPress.emplace("START");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_BACK) && !controllers[index].Back) {
		controllers[index].Back = true;
		controllers[index].buttonPress.emplace("BACK");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) && !controllers[index].LeftShoulder) {
		controllers[index].LeftShoulder = true;
		controllers[index].buttonPress.emplace("LB");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) && !controllers[index].RightShoulder) {
		controllers[index].RightShoulder = true;
		controllers[index].buttonPress.emplace("RB");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_A) && !controllers[index].AButton) {
		controllers[index].AButton = true;
		controllers[index].buttonPress.emplace("A");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_B) && !controllers[index].BButton) {
		controllers[index].BButton = true;
		controllers[index].buttonPress.emplace("B");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_X) && !controllers[index].XButton) {
		controllers[index].XButton = true;
		controllers[index].buttonPress.emplace("X");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_Y) && !controllers[index].YButton) {
		controllers[index].YButton = true;
		controllers[index].buttonPress.emplace("Y");
	}
}

void InputSystem::controllerInputUp(int index)
{
	if (!controllers[index].isConected)return;

	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && !controllers[index].Up) {
		controllers[index].Up = false;
		controllers[index].buttonRelease.emplace("UP");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && !controllers[index].Down) {
		controllers[index].Down = false;
		controllers[index].buttonRelease.emplace("DOWN");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && !controllers[index].Left) {
		controllers[index].Left = false;
		controllers[index].buttonRelease.emplace("LEFT");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && !controllers[index].Right) {
		controllers[index].Right = false;
		controllers[index].buttonRelease.emplace("RIGHT");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_START) && !controllers[index].Start) {
		controllers[index].Start = false;
		controllers[index].buttonRelease.emplace("START");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_BACK) && !controllers[index].Back) {
		controllers[index].Back = false;
		controllers[index].buttonRelease.emplace("BACK");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) && !controllers[index].LeftShoulder) {
		controllers[index].LeftShoulder = false;
		controllers[index].buttonRelease.emplace("LB");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) && !controllers[index].RightShoulder) {
		controllers[index].RightShoulder = false;
		controllers[index].buttonRelease.emplace("RB");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_A) && !controllers[index].AButton) {
		controllers[index].AButton = false;
		controllers[index].buttonRelease.emplace("A");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_B) && !controllers[index].BButton) {
		controllers[index].BButton = false;
		controllers[index].buttonRelease.emplace("B");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_X) && !controllers[index].XButton) {
		controllers[index].XButton = false;
		controllers[index].buttonRelease.emplace("X");
	}
	if (SDL_GameControllerGetButton(controllers[index].controller, SDL_CONTROLLER_BUTTON_Y) && !controllers[index].YButton) {
		controllers[index].YButton = false;
		controllers[index].buttonRelease.emplace("Y");
	}
}

void InputSystem::processKeyDown(std::string keyName, int key)
{
	std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);
	if (!isKeyPressed(keyName)) {
		keyPress.emplace(keyName);
		keyHold.emplace(keyName);
		if (flags) std::cout << "Key down: " << keyName << "\n";
	}
}

void InputSystem::processKeyUp(std::string keyName, int key)
{
	std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);
	keyRelease.emplace(keyName);
	keyHold.erase(keyName);
	if (flags) std::cout << "Key up: " << keyName << "\n";
}

void InputSystem::processMouseMotion(int x, int y)
{
	// SDL_GetMouseState(&MOUSE_POSITION_X, &MOUSE_POSITION_Y);
	MOUSE_POSITION_X = x;
	MOUSE_POSITION_Y = y;
}

void InputSystem::processMouseLeftButtonDown()
{
	MOUSE_BUTTON_LEFT.hold = true;
	MOUSE_BUTTON_LEFT.pressed = true;
	if (flags) LOG("Mouse X: %i | Mouse Y: %i", MOUSE_POSITION_X, MOUSE_POSITION_Y);
}

void InputSystem::processMouseRightButtonDown()
{
	MOUSE_BUTTON_RIGHT.hold = true;
	MOUSE_BUTTON_RIGHT.pressed = true;
	if (flags) LOG("Mouse X: %i | Mouse Y: %i", MOUSE_POSITION_X, MOUSE_POSITION_Y);
}

void InputSystem::processMouseMiddleButtonDown()
{
	MOUSE_BUTTON_MIDDLE.hold = true;
	MOUSE_BUTTON_MIDDLE.pressed = true;
	if (flags) LOG("Mouse X: %i | Mouse Y: %i", MOUSE_POSITION_X, MOUSE_POSITION_Y);
}

void InputSystem::processMouseLeftButtonUp()
{
	MOUSE_BUTTON_LEFT.hold = false;
	MOUSE_BUTTON_LEFT.released = true;
}

void InputSystem::processMouseRightButtonUp()
{
	MOUSE_BUTTON_RIGHT.hold = false;
	MOUSE_BUTTON_RIGHT.released = true;
}

void InputSystem::processMouseMiddleButtonUp()
{
	MOUSE_BUTTON_MIDDLE.hold = false;
	MOUSE_BUTTON_MIDDLE.released = true;
}

void InputSystem::processMouseWheelScrollY(int value)
{
	if (value > 0) {
		MOUSE_WHEEL = 1;
		if (flags) LOG("Mouse wheel up");
	}
	else if (value < 0) {
		MOUSE_WHEEL = -1;
		if (flags) LOG("Mouse wheel down");
	}
}

void InputSystem::processControllerButtonDown(int index, int button)
{
	int controllerIndex = -1;
	if ((controllerIndex = getControllerFromEvent(index)) == -1) return;

	if (controllers[controllerIndex].isConected) controllerInputDown(controllerIndex);

	if (flags) LOG("Controller: %i Button Down", controllerIndex);
}

void InputSystem::processControllerButtonUp(int index, int button)
{
	int controllerIndex = -1;
	if ((controllerIndex = getControllerFromEvent(index)) == -1) return;

	if (controllers[controllerIndex].isConected) controllerInputUp(controllerIndex);

	if (flags) LOG("Controller: %i Button Up", controllerIndex);
}

void InputSystem::processControllerDeviceAdded(int index)
{
	int controllerIndex = -1;
	if (currentControllers < MAX_CONTROLLERS && SDL_IsGameController(index)) {

		if ((controllerIndex = getFirstFreeController()) == -1) return;

		controllers[controllerIndex].controller = SDL_GameControllerOpen(index);
		controllers[controllerIndex].isConected = true;
		controllers[controllerIndex].ID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[controllerIndex].controller));

		// Checks if controller has rumble device & asigns it
		SDL_Joystick* JoystickHandle = SDL_GameControllerGetJoystick(controllers[controllerIndex].controller);
		controllers[controllerIndex].controllerRumble = SDL_HapticOpenFromJoystick(JoystickHandle);

		if (SDL_HapticRumbleInit(controllers[controllerIndex].controllerRumble) != 0)
		{
			SDL_HapticClose(controllers[controllerIndex].controllerRumble);
		}

		currentControllers++;

		if (flags) LOG("Controller: %i added", controllerIndex);
	}
}

void InputSystem::processControllerDeviceRemoved(int index)
{
	int controllerIndex = -1;
	if (index < 0) return; // unknown controller?

	if ((controllerIndex = getControllerRemovedIndex(index)) == -1) return;

	SDL_GameControllerClose(controllers[controllerIndex].controller);

	if (controllers[controllerIndex].controllerRumble) {
		SDL_HapticClose(controllers[controllerIndex].controllerRumble);
		controllers[controllerIndex].controllerRumble = nullptr;
	}

	controllers[controllerIndex].controller = nullptr;
	controllers[controllerIndex].isConected = false;

	currentControllers--;

	if (flags) LOG("Controller: %i removed", controllerIndex);
}

bool InputSystem::isButtonPressed(int controllerIndex, std::string button)
{
	if (!controllers[controllerIndex].isConected) return false;

	std::transform(button.begin(), button.end(), button.begin(), ::toupper);

	if (button == "UP") {
		return controllers[controllerIndex].Up;
	}
	else if (button == "DOWN") {
		return controllers[controllerIndex].Down;
	}
	else if (button == "RIGHT") {
		return controllers[controllerIndex].Right;
	}
	else if (button == "LEFT") {
		return controllers[controllerIndex].Left;
	}
	else if (button == "START") {
		return controllers[controllerIndex].Start;
	}
	else if (button == "BACK") {
		return controllers[controllerIndex].Back;
	}
	else if (button == "LB") {
		return controllers[controllerIndex].LeftShoulder;
	}
	else if (button == "RB") {
		return controllers[controllerIndex].RightShoulder;
	}
	else if (button == "A") {
		return controllers[controllerIndex].AButton;
	}
	else if (button == "B") {
		return controllers[controllerIndex].BButton;
	}
	else if (button == "Y") {
		return controllers[controllerIndex].YButton;
	}
	else if (button == "X") {
		return controllers[controllerIndex].XButton;
	}
	// TODO
	else return false;
}

bool InputSystem::getButtonPress(int controllerIndex, std::string button)
{
	if (!controllers[controllerIndex].isConected) return false;

	std::transform(button.begin(), button.end(), button.begin(), ::toupper);
	const bool is_in = controllers[controllerIndex].buttonPress.find(button) != controllers[controllerIndex].buttonPress.end();
	return is_in;
}

bool InputSystem::getButtonRelease(int controllerIndex, std::string button)
{
	if (!controllers[controllerIndex].isConected) return false;

	std::transform(button.begin(), button.end(), button.begin(), ::toupper);
	const bool is_in = controllers[controllerIndex].buttonRelease.find(button) != controllers[controllerIndex].buttonRelease.end();
	return is_in;
}

/// strength = rumble strength percentage [0.0 , 1.0] | length = duration of effect in miliseconds
void InputSystem::controllerRumble(int controllerIndex, float strength, int length)
{
	if (strength > 1.0f) strength = 1.0f;
	if (length > 10000) length = 10000;
	if (controllers[controllerIndex].isConected && controllers[controllerIndex].controllerRumble)
	{
		SDL_HapticRumblePlay(controllers[controllerIndex].controllerRumble, strength, length);
	}
}
#pragma region UTILS
void InputSystem::clearInputs()
{
	keyPress.clear();
	keyRelease.clear();

	if (MOUSE_BUTTON_LEFT.pressed) MOUSE_BUTTON_LEFT.pressed = false;
	else if (MOUSE_BUTTON_LEFT.released)MOUSE_BUTTON_LEFT.released = false;

	if (MOUSE_BUTTON_RIGHT.pressed) MOUSE_BUTTON_RIGHT.pressed = false;
	else if (MOUSE_BUTTON_RIGHT.released)MOUSE_BUTTON_RIGHT.released = false;

	if (MOUSE_BUTTON_MIDDLE.pressed) MOUSE_BUTTON_MIDDLE.pressed = false;
	else if (MOUSE_BUTTON_MIDDLE.released)MOUSE_BUTTON_MIDDLE.released = false;

	MOUSE_WHEEL = 0;

	for (int i = 0; i < currentControllers; i++) {
		controllers[i].buttonPress.clear();
		controllers[i].buttonRelease.clear();
	}
}
int InputSystem::getFirstFreeController()
{
	for (int i = 0; i < 4; i++) {
		if (!controllers[i].isConected) return i;
	}
	return -1;
}
int InputSystem::getControllerByReference(SDL_GameController* handle)
{
	for (int i = 0; i < 4; i++) {
		if (controllers[i].controller == handle) return i;
	}
	return -1;
}
int InputSystem::getControllerFromEvent(int index)
{
	for (int i = 0; i < 4; i++) {
		if (controllers[i].ID == index) return i;
	}
	return -1;
}
int InputSystem::getControllerRemovedIndex(int index)
{
	for (int i = 0; i < 4; i++) {
		if (controllers[i].ID == index) return i;
	}
	return -1;
}
#pragma endregion
