#include "GaiaInput.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/System.h>

// Initialization
void GaiaInput::init()
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

    if (!SDL_WasInit(SDL_INIT_JOYSTICK)) SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

    SDL_GameControllerEventState(true);

    int MaxJoysticks = SDL_NumJoysticks();
    int ControllerIndex = 0;
    for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
    {
        if (SDL_IsGameController(JoystickIndex) && ControllerIndex < MAX_CONTROLLERS)
        {
            ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);

            // Data pass to internal controller struct
            controllers[currentControllers].controller = ControllerHandles[ControllerIndex];
            controllers[currentControllers].isConected = true;

            // Checks if controller has rumble device & asigns it
            SDL_Joystick* JoystickHandle = SDL_GameControllerGetJoystick(ControllerHandles[ControllerIndex]);
            if ((RumbleHandles[ControllerIndex] = SDL_HapticOpenFromJoystick(JoystickHandle)) != NULL) {
                controllers[currentControllers].controllerRumble = RumbleHandles[ControllerIndex];
            }
            std::cout << "ANADIDO MANDO " << currentControllers << "\n";
            ControllerIndex++;
            currentControllers++;
        }
    }
}

// Closing
void GaiaInput::close()
{
    // CONTROLLER SYSTEM
    for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
    {
        if (ControllerHandles[ControllerIndex])
        {
            SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
            if (RumbleHandles[ControllerIndex]) SDL_HapticClose(RumbleHandles[ControllerIndex]);
            controllers[ControllerIndex].isConected = false;
        }
    }
}

/// MAIN LOOP

void GaiaInput::update()
{
    // Clear previous frame press/release events
    clearInputs();

    // Get new input events
    SDL_Event event;
    int index; int controllerIndex;
    std::string key;

    while (SDL_PollEvent(&event))
    {
        /*SDL_JoystickUpdate();*/
        switch (event.type)
        {
            // Keyboard events
        case SDL_KEYDOWN:
            key = SDL_GetKeyName(event.key.keysym.sym);
            std::transform(key.begin(), key.end(), key.begin(), ::toupper);
            keyPress.emplace(key);
            keyHold.emplace(key);
            break;
        case SDL_KEYUP:
            key = SDL_GetKeyName(event.key.keysym.sym);
            std::transform(key.begin(), key.end(), key.begin(), ::toupper);
            keyRelease.emplace(key);
            keyHold.erase(key);
            break;
        case SDL_TEXTINPUT:
            break;

            // Mouse events
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&MOUSE_POSITION_X, &MOUSE_POSITION_Y);
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(
                static_cast<float>(MOUSE_POSITION_X),
                static_cast<float>(MOUSE_POSITION_Y)
            );
            break;
        case SDL_MOUSEBUTTONDOWN:
            processMouseInputDown(event.button);
            std::cout << MOUSE_POSITION_X << " " << MOUSE_POSITION_Y << "\n";
            break;
        case SDL_MOUSEBUTTONUP:
            processMouseInputUp(event.button);
            break;

            // Controller events
        case SDL_CONTROLLERBUTTONDOWN:

            if ((controllerIndex = getControllerByReference(ControllerHandles[event.cdevice.which])) == -1) break;

            if (controllers[controllerIndex].isConected) controllerInputDown(controllerIndex);

            std::cout << "MANDO DOWN | Index = ";
            std::cout << event.cdevice.which << "\n";

            break;
        case SDL_CONTROLLERBUTTONUP:

            if ((controllerIndex = getControllerByReference(ControllerHandles[event.cdevice.which])) == -1) break;

            if (controllers[controllerIndex].isConected) controllerInputUp(controllerIndex);
            break;
        
        case SDL_CONTROLLERAXISMOTION:
            break;

        case SDL_JOYAXISMOTION:
            break;

        case SDL_CONTROLLERDEVICEADDED:
            if (currentControllers < MAX_CONTROLLERS && SDL_IsGameController(event.cdevice.which)) {

                index = event.cdevice.which;

                ControllerHandles[index] = SDL_GameControllerOpen(index);

                if ((controllerIndex = getFirstFreeController()) == -1) break;

                controllers[controllerIndex].controller = ControllerHandles[index];
                controllers[controllerIndex].isConected = true;

                // Checks if controller has rumble device & asigns it

                SDL_Joystick* JoystickHandle = SDL_GameControllerGetJoystick(ControllerHandles[index]);
                if ((RumbleHandles[index] = SDL_HapticOpenFromJoystick(JoystickHandle)) != NULL) {
                    controllers[controllerIndex].controllerRumble = RumbleHandles[index];
                }

                // Checks if rumble device is compatible with basic rumble features
                if (SDL_HapticRumbleInit(RumbleHandles[index]) != 0)
                {
                    SDL_HapticClose(RumbleHandles[index]);
                    RumbleHandles[index] = 0;
                }

                currentControllers++;

                std::cout << "MANDO ADDED | Index = ";
                std::cout << controllerIndex << "\n";
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            index = event.cdevice.which;

            if (index < 0) return; // unknown controller?

            if ((controllerIndex = getControllerByReference(ControllerHandles[index])) == -1) break;

            SDL_GameControllerClose(ControllerHandles[index]);

            if (RumbleHandles[index]) {
                controllers[controllerIndex].controllerRumble = nullptr;
                SDL_HapticClose(RumbleHandles[index]);
            }

            ControllerHandles[index] = NULL;
           
            controllers[controllerIndex].controller = nullptr;
            controllers[controllerIndex].isConected = false;

            currentControllers--;

            std::cout << "MANDO REMOVED | Index = ";
            std::cout << controllerIndex << "\n";

            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            break;

            // System events
        case SDL_QUIT:
            exit = true;
            break;

        case SDL_WINDOWEVENT:
            break;

        default:
            std::cout << "Unknown event\n";
            std::cout << event.type << "\n";
        }

        // Save current keyboard state
        keyboardState = SDL_GetKeyboardState(NULL);

        for (int i = 0; i < MAX_CONTROLLERS; i++)
        {
            if (controllers[i].isConected)
            {
                // Left joystick
                controllers[i].LeftStickX = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTX)) > JOYSTICK_DEAD_ZONE ?
                    SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTX) : 0;
                controllers[i].LeftStickY = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTY)) > JOYSTICK_DEAD_ZONE ?
                    SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_LEFTY) : 0;

                // Right joystick
                controllers[i].RightStickX = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTX)) > JOYSTICK_DEAD_ZONE ?
                    SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTX) : 0;
                controllers[i].RightStickY = abs(SDL_GameControllerGetAxis(controllers[i].controller, SDL_CONTROLLER_AXIS_RIGHTY)) > JOYSTICK_DEAD_ZONE ?
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
}

// KEYBOARD

/// Returns "true" while "key" is hold
bool GaiaInput::isKeyPressed(std::string key)
{
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    const bool is_in = keyHold.find(key) != keyHold.end();
    return is_in;
}

/// Returns "true" if "key" has been pressed on latest frame
bool GaiaInput::getKeyPress(std::string key)
{
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    const bool is_in = keyPress.find(key) != keyPress.end();
    return is_in;
}

/// Returns "true" if "key" has been released on latest frame
bool GaiaInput::getKeyRelease(std::string key)
{
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    const bool is_in = keyRelease.find(key) != keyRelease.end();
    return is_in;
}

// MOUSE
void GaiaInput::processMouseInputDown(SDL_MouseButtonEvent& e)
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

void GaiaInput::processMouseInputUp(SDL_MouseButtonEvent& e)
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
bool GaiaInput::getMouseButtonClick(char button)
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
bool GaiaInput::getMouseButtonHold(char button)
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
bool GaiaInput::getMouseButtonRelease(char button)
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

void GaiaInput::controllerInputDown(int index)
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

void GaiaInput::controllerInputUp(int index)
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

bool GaiaInput::isButtonPressed(int controllerIndex, std::string button)
{
    if (!controllers[controllerIndex].isConected) return false;

    std::transform(button.begin(), button.end(), button.begin(), ::toupper);

    if (button == "UP") {
        return controllers[controllerIndex].Up;
    }else if (button == "DOWN") {
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

bool GaiaInput::getButtonPress(int controllerIndex, std::string button)
{
    if (controllerIndex >= currentControllers) return false;

    std::transform(button.begin(), button.end(), button.begin(), ::toupper);
    const bool is_in = controllers[controllerIndex].buttonPress.find(button) != controllers[controllerIndex].buttonPress.end();
    return is_in;
}

bool GaiaInput::getButtonRelease(int controllerIndex, std::string button)
{
    if (controllerIndex >= currentControllers) return false;

    std::transform(button.begin(), button.end(), button.begin(), ::toupper);
    const bool is_in = controllers[controllerIndex].buttonRelease.find(button) != controllers[controllerIndex].buttonRelease.end();
    return is_in;
}

/// strength = rumble strength percentage [0.0 , 1.0] | length = duration of effect in miliseconds
void GaiaInput::controllerRumble(int controllerIndex, float strength, int length)
{
    if (strength > 1.0f) strength = 1.0f;
    if (length > 10000) length = 10000;
    if (RumbleHandles[controllerIndex])
    {
        SDL_HapticRumblePlay(RumbleHandles[controllerIndex], strength, length);
    }
}
#pragma region UTILS
void GaiaInput::clearInputs()
{
    keyPress.clear();
    keyRelease.clear();

    if (MOUSE_BUTTON_LEFT.pressed) MOUSE_BUTTON_LEFT.pressed = false;
    else if (MOUSE_BUTTON_LEFT.released)MOUSE_BUTTON_LEFT.released = false;

    if (MOUSE_BUTTON_RIGHT.pressed) MOUSE_BUTTON_RIGHT.pressed = false;
    else if (MOUSE_BUTTON_RIGHT.released)MOUSE_BUTTON_RIGHT.released = false;

    if (MOUSE_BUTTON_MIDDLE.pressed) MOUSE_BUTTON_MIDDLE.pressed = false;
    else if (MOUSE_BUTTON_MIDDLE.released)MOUSE_BUTTON_MIDDLE.released = false;

    for (int i = 0; i < currentControllers; i++) {
        controllers[i].buttonPress.clear();
        controllers[i].buttonRelease.clear();
    }
}
int GaiaInput::getFirstFreeController()
{
    for (int i = 0; i < 4; i++) {
        if (!controllers[i].isConected) return i;
    }
    return -1;
}
int GaiaInput::getControllerByReference(SDL_GameController* handle)
{
    for (int i = 0; i < 4; i++) {
        if (controllers[i].controller == handle) return i;
    }
    return -1;
}
#pragma endregion