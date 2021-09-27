// ======================================================================
// BFW_LI_Platform_SDL.c
// ======================================================================

// ======================================================================
// includes
// ======================================================================
#include "BFW.h"
#include "BFW_LocalInput.h"
#include "BFW_LI_Platform.h"
#include "BFW_LI_Private.h"
#include "BFW_Console.h"
#include "BFW_LI_Platform_SDL.h"
#include "BFW_Timer.h"
#include "BFW_ScriptLang.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>

// ======================================================================
// globals
// ======================================================================
static UUtWindow			LIgWindow;

// the other platforms specify a factor to multiply the relative mouse movement value by
// probably to scale movement to a common range
//TODO: check what factor we want here
#define SDL_MOUSEMOVE_FACTOR 1.0f
#define SDL_MOUSEWHEEL_FACTOR 1.0f
// SDL doesn't report wheel state with SDL_GetRelativeMouseState()
// so this is filled in via SDL_MouseWheelEvents
static float vertical_wheel_scroll = 0;

struct sdl_oni_key {
	UUtUns8 oni_key;
	Uint32 sdl_key;
};

// for non-ascii keys
static const struct sdl_oni_key sdl_keyboard_translation_table[] =
{
	{LIcKeyCode_F1, SDLK_F1},
	{LIcKeyCode_F2, SDLK_F2},
	{LIcKeyCode_F3, SDLK_F3},
	{LIcKeyCode_F4, SDLK_F4},
	{LIcKeyCode_F5, SDLK_F5},
	{LIcKeyCode_F6, SDLK_F6},
	{LIcKeyCode_F7, SDLK_F7},
	{LIcKeyCode_F8, SDLK_F8},
	{LIcKeyCode_F9, SDLK_F9},
	{LIcKeyCode_F10, SDLK_F10},
	{LIcKeyCode_F11, SDLK_F11},
	{LIcKeyCode_F12, SDLK_F12},
	{LIcKeyCode_F13, SDLK_F13},
	{LIcKeyCode_F14, SDLK_F14},
	{LIcKeyCode_F15, SDLK_F15},
	{LIcKeyCode_CapsLock, SDLK_CAPSLOCK},
	{LIcKeyCode_LeftShift, SDLK_LSHIFT},
	{LIcKeyCode_RightShift, SDLK_RSHIFT},
	{LIcKeyCode_LeftControl, SDLK_LCTRL},
	//{LIcKeyCode_LeftOption, },
	{LIcKeyCode_LeftAlt, SDLK_LALT},
	{LIcKeyCode_RightAlt, SDLK_RALT},
	//{LIcKeyCode_RightOption, },
	{LIcKeyCode_AppMenuKey, SDLK_MENU},
	{LIcKeyCode_RightControl, SDLK_RCTRL},
	{LIcKeyCode_PrintScreen, SDLK_PRINTSCREEN},
	{LIcKeyCode_ScrollLock, SDLK_SCROLLLOCK},
	{LIcKeyCode_Pause, SDLK_PAUSE},
	{LIcKeyCode_Insert, SDLK_INSERT},
	{LIcKeyCode_Home, SDLK_HOME},
	{LIcKeyCode_PageUp, SDLK_PAGEUP},
	{LIcKeyCode_Delete, SDLK_DELETE},
	{LIcKeyCode_End, SDLK_END},
	{LIcKeyCode_PageDown, SDLK_PAGEDOWN},
	
	{LIcKeyCode_UpArrow, SDLK_UP},
	{LIcKeyCode_LeftArrow, SDLK_LEFT},
	{LIcKeyCode_DownArrow, SDLK_DOWN},
	{LIcKeyCode_RightArrow, SDLK_RIGHT},
	
	{LIcKeyCode_NumLock, SDLK_NUMLOCKCLEAR},
	{LIcKeyCode_Divide, SDLK_KP_DIVIDE},
	{LIcKeyCode_Multiply, SDLK_KP_MULTIPLY},
	{LIcKeyCode_Subtract, SDLK_KP_MINUS},
	{LIcKeyCode_Add, SDLK_KP_PLUS},
	{LIcKeyCode_Decimal, SDLK_KP_DECIMAL},
	{LIcKeyCode_NumPadEnter, SDLK_KP_ENTER},
	{LIcKeyCode_NumPadComma, SDLK_KP_COMMA},
	{LIcKeyCode_NumPadEquals, SDLK_KP_EQUALS},
	
	{LIcKeyCode_NumPad0, SDLK_KP_0},
	{LIcKeyCode_NumPad1, SDLK_KP_1},
	{LIcKeyCode_NumPad2, SDLK_KP_2},
	{LIcKeyCode_NumPad3, SDLK_KP_3},
	{LIcKeyCode_NumPad4, SDLK_KP_4},
	{LIcKeyCode_NumPad5, SDLK_KP_5},
	{LIcKeyCode_NumPad6, SDLK_KP_6},
	{LIcKeyCode_NumPad7, SDLK_KP_7},
	{LIcKeyCode_NumPad8, SDLK_KP_8},
	{LIcKeyCode_NumPad9, SDLK_KP_9},

	{LIcKeyCode_LeftWindowsKey, SDLK_LGUI},
	{LIcKeyCode_RightWindowsKey, SDLK_RGUI},
};

#if 0 //TODO: assert match
    SDLK_AMPERSAND = '&',
    SDLK_ASTERISK = '*',
    SDLK_AT = '@',
    SDLK_BACKQUOTE = '`',
    SDLK_BACKSLASH = '\\',
    SDLK_BACKSPACE = '\b',
    SDLK_CARET = '^',
    SDLK_COLON = ':',
    SDLK_COMMA = ',',
    SDLK_DOLLAR = '$',
    SDLK_EQUALS = '=',
    SDLK_ESCAPE = '\x1B',
    SDLK_EXCLAIM = '!',
    SDLK_GREATER = '>',
    SDLK_HASH = '#',
    SDLK_LEFTBRACKET = '[',
    SDLK_LEFTPAREN = '(',
    SDLK_LESS = '<',
    SDLK_MINUS = '-',
    SDLK_PERCENT = '%',
    SDLK_PERIOD = '.',
    SDLK_PLUS = '+',
    SDLK_QUESTION = '?',
    SDLK_QUOTE = '\'',
    SDLK_QUOTEDBL = '"',
    SDLK_RETURN = '\r',
    SDLK_RIGHTBRACKET = ']',
    SDLK_RIGHTPAREN = ')',
    SDLK_SEMICOLON = ';',
    SDLK_SLASH = '/',
    SDLK_SPACE = ' ',
    SDLK_TAB = '\t',
    SDLK_UNDERSCORE = '_',

	LIcKeyCode_Ampersand		= '&',
	LIcKeyCode_Apostrophe		= '\'',
	LIcKeyCode_At				= '@',
	LIcKeyCode_BackSlash		= '\\',
	LIcKeyCode_BackSpace		= 0x08,
	LIcKeyCode_Colon			= ':',
	LIcKeyCode_Comma			= ',',
	LIcKeyCode_Dollar			= '$',
	LIcKeyCode_Equals			= '=',
	LIcKeyCode_Escape			= 0x1B,
	LIcKeyCode_Exclamation		= '!',
	LIcKeyCode_Grave			= '`',
	LIcKeyCode_GreaterThan		= '>',
	LIcKeyCode_Hat				= '^',
	LIcKeyCode_LeftBracket		= '[',
	LIcKeyCode_LeftCurlyBracket	= '{',
	LIcKeyCode_LeftParen		= '(',
	LIcKeyCode_LessThan			= '<',
	LIcKeyCode_Minus			= '-',
	LIcKeyCode_Percent			= '%',
	LIcKeyCode_Period			= '.',
	LIcKeyCode_Plus				= '+',
	LIcKeyCode_Pound			= '#',
	LIcKeyCode_Question			= '?',
	LIcKeyCode_Quote			= '\"',
	LIcKeyCode_Return			= 0x0D,
	LIcKeyCode_RightBracket		= ']',
	LIcKeyCode_RightCurlyBracket	= '}',
	LIcKeyCode_RightParen		= ')',
	LIcKeyCode_Semicolon		= ';',
	LIcKeyCode_Slash			= '/',
	LIcKeyCode_Star				= '*',
	LIcKeyCode_Tab				= 0x09,
	LIcKeyCode_Tilde			= '~',
	LIcKeyCode_Underscore		= '_',
	LIcKeyCode_VerticalLine		= '|',
	LIcKeyCode_Space			= 0x20,
#endif

static const unsigned num_sdl_keys = sizeof(sdl_keyboard_translation_table)/sizeof(struct sdl_oni_key);

static SDL_Keycode oni_to_sdl_keycode(LItKeyCode key)
{
	int i;
	
	if (key < 0x80)
	{
		// ascii range maps directly
		// except...
		switch (key)
		{
			case LIcKeyCode_Tilde:
			case LIcKeyCode_LeftCurlyBracket:
			case LIcKeyCode_RightCurlyBracket:
			case LIcKeyCode_VerticalLine:
				// not available as key codes in SDL
				return SDLK_UNKNOWN;
		}
		
		return key;
	}
	
	for (i = 0; i<num_sdl_keys; ++i)
	{
		const struct sdl_oni_key *map = &sdl_keyboard_translation_table[i];
		if (map->oni_key == key)
		{
			return map->sdl_key;
		}
	}
	
	return SDLK_UNKNOWN;
}

static LItKeyCode sdl_to_oni_keycode(SDL_Keycode key)
{
	int i;
	
	if (key < 0x80)
	{
		// ascii range maps directly
		return key;
	}
	
	for (i = 0; i<num_sdl_keys; ++i)
	{
		const struct sdl_oni_key *map = &sdl_keyboard_translation_table[i];
		if (map->sdl_key == key)
		{
			return map->oni_key;
		}
	}
	
	return LIcKeyCode_None;
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
static void
LIiPlatform_Mouse_GetData(
	LItAction				*outAction)
{
	LItDeviceInput			deviceInput;
	
	int x = 0, y = 0, i;
	Uint32 buttonsstate = SDL_GetRelativeMouseState(&x, &y);
	
	for (i = 0; i < 4; ++i)
	{
		if (buttonsstate & SDL_BUTTON(i + 1))
		{
			deviceInput.input = LIcMouseCode_Button1 + i;
			deviceInput.analogValue = 1.0f;
			LIrActionBuffer_Add(outAction, &deviceInput);
		}
	}
	
	deviceInput.input = LIcMouseCode_XAxis;
	deviceInput.analogValue = (float)x * SDL_MOUSEMOVE_FACTOR;
	LIrActionBuffer_Add(outAction, &deviceInput);

	deviceInput.input = LIcMouseCode_YAxis;
	deviceInput.analogValue = -(float)y * SDL_MOUSEMOVE_FACTOR;
	if (LIgMouse_Invert)
	{
		deviceInput.analogValue = -deviceInput.analogValue;
	}
	LIrActionBuffer_Add(outAction, &deviceInput);

	deviceInput.input = LIcMouseCode_ZAxis;
	deviceInput.analogValue = vertical_wheel_scroll * SDL_MOUSEWHEEL_FACTOR;
	vertical_wheel_scroll = 0.0f;
	LIrActionBuffer_Add(outAction, &deviceInput);
}

#if UUmCompiler == UUmCompiler_MWerks
#pragma profile reset
#endif

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================

// ----------------------------------------------------------------------
static void
LIiPlatform_Keyboard_GetData(
	LItAction				*outAction)
{
	int i;
	int numkeys = 0;
	const Uint8 *keyState = SDL_GetKeyboardState(&numkeys);
	for (i = 0; i < numkeys; ++i)
	{
		if (keyState[i])
		{
			LItDeviceInput			deviceInput;
			
			deviceInput.input = sdl_to_oni_keycode(SDL_GetKeyFromScancode(i));
			deviceInput.analogValue = 1.0f;
			
			if (deviceInput.input != LIcKeyCode_None) 
			{
				LIrActionBuffer_Add(outAction, &deviceInput);
			}
		}
	}
}

#if UUmCompiler == UUmCompiler_MWerks
#pragma profile reset
#endif

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
#if UUmCompiler == UUmCompiler_MWerks
#pragma profile off
#endif

static void
LIiPlatform_Devices_GetData(
	LItAction				*outAction)
{
	// get mouse data
	if (LIcMode_Game == LIgMode_Internal)
	{
		LIiPlatform_Mouse_GetData(outAction);
	}
	
	// get the keyboard data
	LIiPlatform_Keyboard_GetData(outAction);
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
static UUtUns32
sdl_to_oni_mouse_modifiers(SDL_Keymod sdl_modifiers)
{
	UUtUns32 oni_modifiers = 0;
	if (sdl_modifiers & KMOD_LSHIFT)
		oni_modifiers |= LIcMouseState_LShiftDown;
	if (sdl_modifiers & KMOD_RSHIFT)
		oni_modifiers |= LIcMouseState_RShiftDown;
	if (sdl_modifiers & KMOD_LCTRL)
		oni_modifiers |= LIcMouseState_LControlDown;
	if (sdl_modifiers & KMOD_RCTRL)
		oni_modifiers |= LIcMouseState_RControlDown;
	return oni_modifiers;
}

static UUtUns32
sdl_to_oni_mouse_button_modifiers(Uint32 buttonsstate)
{
	UUtUns32 oni_modifiers = 0;
	if (buttonsstate & SDL_BUTTON_LMASK)
		oni_modifiers |= LIcMouseState_LButtonDown;
	if (buttonsstate & SDL_BUTTON_MMASK)
		oni_modifiers |= LIcMouseState_MButtonDown;
	if (buttonsstate & SDL_BUTTON_RMASK)
		oni_modifiers |= LIcMouseState_RButtonDown;
	return oni_modifiers;
}

static UUtUns32
sdl_to_oni_key_modifiers(SDL_Keymod sdl_modifiers)
{
	UUtUns32 oni_modifiers = 0;
	if (sdl_modifiers & KMOD_LSHIFT)
		oni_modifiers |= LIcKeyState_LShiftDown;
	if (sdl_modifiers & KMOD_RSHIFT)
		oni_modifiers |= LIcKeyState_RShiftDown;
	if (sdl_modifiers & KMOD_LCTRL)
		oni_modifiers |= LIcKeyState_LCommandDown;
	if (sdl_modifiers & KMOD_RCTRL)
		oni_modifiers |= LIcKeyState_RCommandDown;
	if (sdl_modifiers & KMOD_LALT)
		oni_modifiers |= LIcKeyState_LAltDown;
	if (sdl_modifiers & KMOD_RALT)
		oni_modifiers |= LIcKeyState_RAltDown;
	return oni_modifiers;
}

static SDL_Keymod
sdl_to_kmod(SDL_Keycode sym)
{
	switch (sym)
	{
		case SDLK_LCTRL:  return KMOD_LCTRL;
		case SDLK_RCTRL:  return KMOD_RCTRL;
		case SDLK_LSHIFT: return KMOD_LSHIFT;
		case SDLK_RSHIFT: return KMOD_RSHIFT;
		case SDLK_LALT:   return KMOD_LALT;
		case SDLK_RALT:   return KMOD_RALT;
		default:          return KMOD_NONE;
	}
}

UUtUns32
LIrPlatform_InputEvent_InterpretModifiers(
	LItInputEventType	inEventType,
	UUtUns32			inModifiers)
{
	(void)inEventType;
	// already translated in LIrPlatform_Update()
	return inModifiers;
}

// ----------------------------------------------------------------------
void
LIrPlatform_InputEvent_GetMouse(
	LItMode				inMode,
	LItInputEvent		*outInputEvent)
{
	// set up the outInputEvent
	outInputEvent->type			= LIcInputEvent_None;
	outInputEvent->where.x		= 0;
	outInputEvent->where.y		= 0;
	outInputEvent->key			= 0;
	outInputEvent->modifiers	= 0;

	if (inMode == LIcMode_Normal)
	{
		int x, y;
		
		Uint32 buttons = SDL_GetMouseState(&x, &y);
		outInputEvent->where.x = (UUtInt16)x;
		outInputEvent->where.y = (UUtInt16)y;
		// Mac checks Shift, left and right mouse buttons
		if (LIrPlatform_TestKey(LIcKeyCode_LeftShift, inMode))
			outInputEvent->modifiers |= LIcMouseState_LShiftDown;
		if (buttons & SDL_BUTTON_LMASK)
			outInputEvent->modifiers |= LIcMouseState_LButtonDown;
		if (buttons & SDL_BUTTON_RMASK)
			outInputEvent->modifiers |= LIcMouseState_RButtonDown;
	}
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
void
LIrPlatform_Mode_Set(
	LItMode				inMode)
{
	int w = 0, h = 0;
	if (inMode == LIcMode_Normal)
	{
		SDL_SetWindowGrab(LIgWindow, SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_GetWindowSize(LIgWindow, &w, &h);
		SDL_WarpMouseInWindow(LIgWindow, h / 2,  w / 2);
	}
	else
	{
		SDL_SetWindowGrab(LIgWindow, SDL_TRUE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
UUtError
LIrPlatform_Initialize(
	UUtAppInstance		inInstance,
	UUtWindow			inWindow)
{
	//UUmAssert(inInstance);
	UUmAssert(inWindow);
	
	// ------------------------------
	// initialize the globals
	// ------------------------------
	LIgWindow					= inWindow;

	// ------------------------------
	// console variables
	// ------------------------------

	return UUcError_None;
}

// ----------------------------------------------------------------------
#if UUmCompiler == UUmCompiler_MWerks
#pragma profile off
#endif

void
LIrPlatform_PollInputForAction(
	LItAction				*outAction)
{
	UUtUns16				i;
	
	// clear the action
	outAction->buttonBits = 0;
	for (i = 0; i < LIcNumAnalogValues; i++)
		outAction->analogValues[i] = 0.0f;
	
	// get the action data
	LIiPlatform_Devices_GetData(outAction);
}

#if UUmCompiler == UUmCompiler_MWerks
#pragma profile reset
#endif

// ----------------------------------------------------------------------
void
LIrPlatform_Terminate(
	void)
{
}

// ----------------------------------------------------------------------
UUtBool
LIrPlatform_Update(
	LItMode					inMode)
{
	SDL_Event event;
	LItInputEventType eventType = LIcInputEvent_None;
	IMtPoint2D where = {0, 0};
	static Uint32 current_mouse_buttons = 0;
	static Uint32 current_modifiers = 0;
	
	//FIXME: Mac and Win32 only poll for a single event
	//       however, input is laggy if that is done.
	while (1)
	{
		if (!SDL_PollEvent(&event)) {
			return UUcFalse;
		}
		
		switch (event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				eventType = (event.key.state == SDL_PRESSED) ? LIcInputEvent_KeyDown : LIcInputEvent_KeyUp;
				
				current_modifiers |= sdl_to_kmod(event.key.keysym.sym);
				if (current_modifiers != event.key.keysym.mod)
				{
					UUmAssert(UUcFalse);
					current_modifiers = event.key.keysym.mod;
				}
				
				LIrInputEvent_Add(
					eventType,
					NULL,
					sdl_to_oni_keycode(event.key.keysym.sym),
					sdl_to_oni_key_modifiers(current_modifiers)
				);
			break;
			case SDL_MOUSEMOTION:
				where.x = event.motion.x;
				where.y = event.motion.y;
				
				if (current_mouse_buttons != event.motion.state)
				{
					UUmAssert(UUcFalse);
					current_mouse_buttons = event.motion.state;
				}
				
				LIrInputEvent_Add(
					LIcInputEvent_MouseMove,
					&where,
					sdl_to_oni_keycode(event.key.keysym.sym),
					sdl_to_oni_mouse_modifiers(current_modifiers) | sdl_to_oni_mouse_button_modifiers(current_mouse_buttons)
				);
			break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						eventType = (event.button.state == SDL_PRESSED) ? LIcInputEvent_LMouseDown : LIcInputEvent_LMouseUp;
					break;
					case SDL_BUTTON_RIGHT:
						eventType = (event.button.state == SDL_PRESSED) ? LIcInputEvent_RMouseDown : LIcInputEvent_RMouseUp;
					break;
					case SDL_BUTTON_MIDDLE:
						eventType = (event.button.state == SDL_PRESSED) ? LIcInputEvent_MMouseDown : LIcInputEvent_MMouseUp;
					break;
					default:
						return UUcTrue;
				}
				
				where.x = event.button.x;
				where.y = event.button.y;
				
				if (event.button.state == SDL_PRESSED)
				{
					current_mouse_buttons |= sdl_to_oni_mouse_modifiers(SDL_BUTTON(event.button.button));
				}
				else
				{
					current_mouse_buttons &= ~sdl_to_oni_mouse_modifiers(SDL_BUTTON(event.button.button));
				}
				
				LIrInputEvent_Add(
					eventType,
					&where,
					0,
					sdl_to_oni_mouse_modifiers(current_modifiers) | sdl_to_oni_mouse_button_modifiers(current_mouse_buttons)
				);
			break;
			case SDL_MOUSEWHEEL:
				//TODO: inputEvent
				//TODO: regard event.wheel.direction?
				vertical_wheel_scroll += (float)event.wheel.y;
			break;
			//FIXME: not input - should be Oni_Platform_SDL.c
			case SDL_APP_DIDENTERFOREGROUND:
				LIrGameIsActive(UUcTrue);
			break;
			case SDL_APP_WILLENTERBACKGROUND:
				LIrGameIsActive(UUcFalse);
			break;
		}
	}
	
	return UUcTrue;
}

// ----------------------------------------------------------------------
UUtBool
LIrPlatform_TestKey(
	LItKeyCode			inKeyCode,
	LItMode				inMode)
{
	(void)inMode;
	UUtBool keyDown = UUcFalse;
	
	int numkeys = 0;
	//TODO: capture in LIrPlatform_Update()?
	const Uint8 *keyState = SDL_GetKeyboardState(&numkeys);
	SDL_Scancode scancode = SDL_GetScancodeFromKey(oni_to_sdl_keycode(inKeyCode));
	if (scancode > 0 && scancode < numkeys) {
		keyDown = keyState[scancode];
	}
	
	return keyDown;
}
