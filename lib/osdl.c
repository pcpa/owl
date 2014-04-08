/*
 * Copyright (C) 2014  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#include "owl.h"

/*
 * Types
 */
typedef struct {
    oint32_t		 i32;
} nat_i32_t;
typedef struct {
    ouint32_t		 u32;
} nat_u32_t;
typedef struct {
    owindow_t		*win;
} nat_win_t;
typedef struct {
    orenderer_t		*ren;
} nat_ren_t;
typedef struct {
    otexture_t		*tex;
} nat_tex_t;
typedef struct {
    oevent_t		*ev;
} nat_ev_t;
typedef struct {
    otimer_t		*tm;
} nat_tm_t;
typedef struct {
    ouint32_t		 u32;
    oobject_t		 obj;
} nat_u32_obj_t;
typedef struct {
    orenderer_t		*ren;
    ovector_t		*vec;
} nat_ren_vec_t;
typedef struct {
    orenderer_t		*ren;
    osurface_t		*srf;
} nat_ren_srf_t;
typedef struct {
    owindow_t		*win;
    oint32_t		 i32;
    ouint32_t		 u32;
} nat_win_i32_u32_t;
typedef struct {
    orenderer_t		*ren;
    otexture_t		*tex;
    orect_t		*src;
    orect_t		*dst;
} nat_ren_tex_rec_rec_t;
typedef struct {
    orenderer_t		*ren;
    ouint32_t		 u32;
    oint32_t		 si0;
    oint32_t		 si1;
    oint32_t		 si2;
} nat_ren_u32_i32_i32_i32_t;
typedef struct {
    ovector_t		*vec;
    oint32_t		 si0, si1, si2, si3;
    ouint32_t		 u32;
} nat_vec_i32_i32_i32_i32_u32_t;

/*
 * Prototypes
 */
static void native_init(oobject_t list, oint32_t ac);
static void native_get_error(oobject_t list, oint32_t ac);
static void native_quit(oobject_t list, oint32_t ac);
static void native_create_window(oobject_t list, oint32_t ac);
static void native_change_window(oobject_t list, oint32_t ac);
static void native_get_window_renderer(oobject_t list, oint32_t ac);
static void native_destroy_window(oobject_t list, oint32_t ac);
static void query_renderer(orenderer_t *or);
static void native_create_renderer(oobject_t list, oint32_t ac);
static void native_change_renderer(oobject_t list, oint32_t ac);
static void native_get_renderer_window(oobject_t list, oint32_t ac);
static void native_render_clear(oobject_t list, oint32_t ac);
static void native_render_copy(oobject_t list, oint32_t ac);
static void native_render_present(oobject_t list, oint32_t ac);
static void native_destroy_renderer(oobject_t list, oint32_t ac);
static void native_destroy_renderer(oobject_t list, oint32_t ac);
static void query_texture(otexture_t *ot);
static void handle_texture(otexture_t *tex);
static void native_create_texture(oobject_t list, oint32_t ac);
static void native_create_texture_from_surface(oobject_t list, oint32_t ac);
static void native_load_texture(oobject_t list, oint32_t ac);
static void native_change_texture(oobject_t list, oint32_t ac);
static void native_destroy_texture(oobject_t list, oint32_t ac);
static inline void translate_window(oevent_t *ev, ouint32_t id);
static void translate_event(oevent_t *ev);
static void native_poll_event(oobject_t list, oint32_t ac);
static void native_wait_event(oobject_t list, oint32_t ac);
static Uint32 timer_callback(Uint32 ms, void *data);
static void pop_timer(oobject_t *pointer);
static void push_timer(otimer_t *timer);
static void native_add_timer(oobject_t list, oint32_t ac);
static void native_get_ticks(oobject_t list, oint32_t ac);
static void native_delay(oobject_t list, oint32_t ac);
static void native_remove_timer(oobject_t list, oint32_t ac);
#if __WORDSIZE == 32
static void ret_u32(oregister_t *r, ouint32_t v);
#else
#define ret_u32(R, V)							\
    do {								\
	R->t = t_word;							\
	R->v.w = V;							\
    } while (0)
#endif

/*
 * Initialization
 */
orecord_t	*sdl_record;

static struct {
    char	*name;
    int		 value;
} consts[] = {
    /* event_t.state */
    { "Pressed",			SDL_PRESSED },
    { "Released",			SDL_RELEASED },
    /* event_t.button */
    { "ButtonLeft",			SDL_BUTTON_LEFT },
    { "ButtonMiddle",			SDL_BUTTON_MIDDLE },
    { "ButtonRight",			SDL_BUTTON_RIGHT },
    { "ButtonX1",			SDL_BUTTON_X1 },
    { "ButtonX2",			SDL_BUTTON_X2 },
    /* event_t.state (for EventMouse*) */
    { "ButtonLMask",			SDL_BUTTON_LMASK },
    { "ButtonMMask",			SDL_BUTTON_MMASK },
    { "ButtonRMask",			SDL_BUTTON_RMASK },
    { "ButtonX1Mask",			SDL_BUTTON_X1MASK },
    { "ButtonX2Mask",			SDL_BUTTON_X2MASK },
    /* event_t.event */
    { "WindowEventShown",		SDL_WINDOWEVENT_SHOWN },
    { "WindowEventHidden",		SDL_WINDOWEVENT_HIDDEN },
    { "WindowEventExposed",		SDL_WINDOWEVENT_EXPOSED },
    { "WindowEventMoved",		SDL_WINDOWEVENT_MOVED },
    { "WindowEventResized",		SDL_WINDOWEVENT_RESIZED },
    { "WindowEventSizeChanged",		SDL_WINDOWEVENT_SIZE_CHANGED },
    { "WindowEventMinimized",		SDL_WINDOWEVENT_MINIMIZED },
    { "WindowEventMaximized",		SDL_WINDOWEVENT_MAXIMIZED },
    { "WindowEventRestored",		SDL_WINDOWEVENT_RESTORED },
    { "WindowEventEnter",		SDL_WINDOWEVENT_ENTER },
    { "WindowEventLeave",		SDL_WINDOWEVENT_LEAVE },
    { "WindowEventFocusGained",		SDL_WINDOWEVENT_FOCUS_GAINED },
    { "WindowEventFocusLost",		SDL_WINDOWEVENT_FOCUS_LOST },
    { "WindowEventClose",		SDL_WINDOWEVENT_CLOSE },
    /* event_t.hat */
    { "HatCentered",			SDL_HAT_CENTERED },
    { "HatUp",				SDL_HAT_UP },
    { "HatRight",			SDL_HAT_RIGHT },
    { "HatDown",			SDL_HAT_DOWN },
    { "HatLeft",			SDL_HAT_LEFT },
    { "HatRightup",			SDL_HAT_RIGHTUP },
    { "HatRightdown",			SDL_HAT_RIGHTDOWN },
    { "HatLeftup",			SDL_HAT_LEFTUP },
    { "HatLeftdown",			SDL_HAT_LEFTDOWN },
    /* event_t.keysym*/
    { "Key_Return",			SDLK_RETURN },
    { "Key_Escape",			SDLK_ESCAPE },
    { "Key_Backspace",			SDLK_BACKSPACE },
    { "Key_Tab",			SDLK_TAB },
    { "Key_Space",			SDLK_SPACE },
    { "Key_Exclaim",			SDLK_EXCLAIM },
    { "Key_Quotedbl",			SDLK_QUOTEDBL },
    { "Key_Hash",			SDLK_HASH },
    { "Key_Percent",			SDLK_PERCENT },
    { "Key_Dollar",			SDLK_DOLLAR },
    { "Key_Ampersand",			SDLK_AMPERSAND },
    { "Key_Quote",			SDLK_QUOTE },
    { "Key_LeftParen",			SDLK_LEFTPAREN },
    { "Key_RightParen",			SDLK_RIGHTPAREN },
    { "Key_Asterisk",			SDLK_ASTERISK },
    { "Key_Plus",			SDLK_PLUS },
    { "Key_Comma",			SDLK_COMMA },
    { "Key_Minus",			SDLK_MINUS },
    { "Key_Period",			SDLK_PERIOD },
    { "Key_Slash",			SDLK_SLASH },
    { "Key_0",				SDLK_0 },
    { "Key_1",				SDLK_1 },
    { "Key_2",				SDLK_2 },
    { "Key_3",				SDLK_3 },
    { "Key_4",				SDLK_4 },
    { "Key_5",				SDLK_5 },
    { "Key_6",				SDLK_6 },
    { "Key_7",				SDLK_7 },
    { "Key_8",				SDLK_8 },
    { "Key_9",				SDLK_9 },
    { "Key_Colon",			SDLK_COLON },
    { "Key_Semicolon",			SDLK_SEMICOLON },
    { "Key_Less",			SDLK_LESS },
    { "Key_Equals",			SDLK_EQUALS },
    { "Key_Greater",			SDLK_GREATER },
    { "Key_Question",			SDLK_QUESTION },
    { "Key_At",				SDLK_AT },
    { "Key_LeftBracket",		SDLK_LEFTBRACKET },
    { "Key_Backslash",			SDLK_BACKSLASH },
    { "Key_RightBracket",		SDLK_RIGHTBRACKET },
    { "Key_Caret",			SDLK_CARET },
    { "Key_Underscore",			SDLK_UNDERSCORE },
    { "Key_Backquote",			SDLK_BACKQUOTE },
    { "Key_a",				SDLK_a },
    { "Key_b",				SDLK_b },
    { "Key_c",				SDLK_c },
    { "Key_d",				SDLK_d },
    { "Key_e",				SDLK_e },
    { "Key_f",				SDLK_f },
    { "Key_g",				SDLK_g },
    { "Key_h",				SDLK_h },
    { "Key_i",				SDLK_i },
    { "Key_j",				SDLK_j },
    { "Key_k",				SDLK_k },
    { "Key_l",				SDLK_l },
    { "Key_m",				SDLK_m },
    { "Key_n",				SDLK_n },
    { "Key_o",				SDLK_o },
    { "Key_p",				SDLK_p },
    { "Key_q",				SDLK_q },
    { "Key_r",				SDLK_r },
    { "Key_s",				SDLK_s },
    { "Key_t",				SDLK_t },
    { "Key_u",				SDLK_u },
    { "Key_v",				SDLK_v },
    { "Key_w",				SDLK_w },
    { "Key_x",				SDLK_x },
    { "Key_y",				SDLK_y },
    { "Key_z",				SDLK_z },
    { "Key_CapsLock",			SDLK_CAPSLOCK },
    { "Key_F1",				SDLK_F1 },
    { "Key_F2",				SDLK_F2 },
    { "Key_F3",				SDLK_F3 },
    { "Key_F4",				SDLK_F4 },
    { "Key_F5",				SDLK_F5 },
    { "Key_F6",				SDLK_F6 },
    { "Key_F7",				SDLK_F7 },
    { "Key_F8",				SDLK_F8 },
    { "Key_F9",				SDLK_F9 },
    { "Key_F10",			SDLK_F10 },
    { "Key_F11",			SDLK_F11 },
    { "Key_F12",			SDLK_F12 },
    { "Key_PrintScreen",		SDLK_PRINTSCREEN },
    { "Key_ScrollLock",			SDLK_SCROLLLOCK },
    { "Key_Pause",			SDLK_PAUSE },
    { "Key_Insert",			SDLK_INSERT },
    { "Key_Home",			SDLK_HOME },
    { "Key_PageUp",			SDLK_PAGEUP },
    { "Key_Delete",			SDLK_DELETE },
    { "Key_End",			SDLK_END },
    { "Key_PageDown",			SDLK_PAGEDOWN },
    { "Key_Right",			SDLK_RIGHT },
    { "Key_Left",			SDLK_LEFT },
    { "Key_Down",			SDLK_DOWN },
    { "Key_Up",				SDLK_UP },
    { "Key_NumlockClear",		SDLK_NUMLOCKCLEAR },
    { "Key_KP_Divide",			SDLK_KP_DIVIDE },
    { "Key_KP_Multiply",		SDLK_KP_MULTIPLY },
    { "Key_KP_Minus",			SDLK_KP_MINUS },
    { "Key_KP_Plus",			SDLK_KP_PLUS },
    { "Key_KP_Enter",			SDLK_KP_ENTER },
    { "Key_KP_1",			SDLK_KP_1 },
    { "Key_KP_2",			SDLK_KP_2 },
    { "Key_KP_3",			SDLK_KP_3 },
    { "Key_KP_4",			SDLK_KP_4 },
    { "Key_KP_5",			SDLK_KP_5 },
    { "Key_KP_6",			SDLK_KP_6 },
    { "Key_KP_7",			SDLK_KP_7 },
    { "Key_KP_8",			SDLK_KP_8 },
    { "Key_KP_9",			SDLK_KP_9 },
    { "Key_KP_0",			SDLK_KP_0 },
    { "Key_KP_Period",			SDLK_KP_PERIOD },
    { "Key_Application",		SDLK_APPLICATION },
    { "Key_Power",			SDLK_POWER },
    { "Key_KP_Equals",			SDLK_KP_EQUALS },
    { "Key_F13",			SDLK_F13 },
    { "Key_F14",			SDLK_F14 },
    { "Key_F15",			SDLK_F15 },
    { "Key_F16",			SDLK_F16 },
    { "Key_F17",			SDLK_F17 },
    { "Key_F18",			SDLK_F18 },
    { "Key_F19",			SDLK_F19 },
    { "Key_F20",			SDLK_F20 },
    { "Key_F21",			SDLK_F21 },
    { "Key_F22",			SDLK_F22 },
    { "Key_F23",			SDLK_F23 },
    { "Key_F24",			SDLK_F24 },
    { "Key_Execute",			SDLK_EXECUTE },
    { "Key_Help",			SDLK_HELP },
    { "Key_Menu",			SDLK_MENU },
    { "Key_Select",			SDLK_SELECT },
    { "Key_Stop",			SDLK_STOP },
    { "Key_Again",			SDLK_AGAIN },
    { "Key_Undo",			SDLK_UNDO },
    { "Key_Cut",			SDLK_CUT },
    { "Key_Copy",			SDLK_COPY },
    { "Key_Paste",			SDLK_PASTE },
    { "Key_Find",			SDLK_FIND },
    { "Key_Mute",			SDLK_MUTE },
    { "Key_VolumeUp",			SDLK_VOLUMEUP },
    { "Key_VolumeDown",			SDLK_VOLUMEDOWN },
    { "Key_KP_Comma",			SDLK_KP_COMMA },
    { "Key_KP_EqualsAs400",		SDLK_KP_EQUALSAS400 },
    { "Key_AltErase",			SDLK_ALTERASE },
    { "Key_SysReq",			SDLK_SYSREQ },
    { "Key_Cancel",			SDLK_CANCEL },
    { "Key_Clear",			SDLK_CLEAR },
    { "Key_Prior",			SDLK_PRIOR },
    { "Key_Return2",			SDLK_RETURN2 },
    { "Key_Separator",			SDLK_SEPARATOR },
    { "Key_Out",			SDLK_OUT },
    { "Key_Oper",			SDLK_OPER },
    { "Key_ClearAgain",			SDLK_CLEARAGAIN },
    { "Key_Crsel",			SDLK_CRSEL },
    { "Key_Exsel",			SDLK_EXSEL },
    { "Key_KP_00",			SDLK_KP_00 },
    { "Key_KP_000",			SDLK_KP_000 },
    { "Key_ThousandsSeparator",		SDLK_THOUSANDSSEPARATOR },
    { "Key_DecimalSeparator",		SDLK_DECIMALSEPARATOR },
    { "Key_CurrencyUnit",		SDLK_CURRENCYUNIT },
    { "Key_CurrencySubUnit",		SDLK_CURRENCYSUBUNIT },
    { "Key_KP_LeftParen",		SDLK_KP_LEFTPAREN },
    { "Key_KP_RightParen",		SDLK_KP_RIGHTPAREN },
    { "Key_KP_LeftBrace",		SDLK_KP_LEFTBRACE },
    { "Key_KP_RightBrace",		SDLK_KP_RIGHTBRACE },
    { "Key_KP_Tab",			SDLK_KP_TAB },
    { "Key_KP_Backspace",		SDLK_KP_BACKSPACE },
    { "Key_KP_A",			SDLK_KP_A },
    { "Key_KP_B",			SDLK_KP_B },
    { "Key_KP_C",			SDLK_KP_C },
    { "Key_KP_D",			SDLK_KP_D },
    { "Key_KP_E",			SDLK_KP_E },
    { "Key_KP_F",			SDLK_KP_F },
    { "Key_KP_Xor",			SDLK_KP_XOR },
    { "Key_KP_Power",			SDLK_KP_POWER },
    { "Key_KP_Percent",			SDLK_KP_PERCENT },
    { "Key_KP_Less",			SDLK_KP_LESS },
    { "Key_KP_Greater",			SDLK_KP_GREATER },
    { "Key_KP_Ampersand",		SDLK_KP_AMPERSAND },
    { "Key_KP_Dblampersand",		SDLK_KP_DBLAMPERSAND },
    { "Key_KP_VerticalBar",		SDLK_KP_VERTICALBAR },
    { "Key_KP_DblverticalBar",		SDLK_KP_DBLVERTICALBAR },
    { "Key_KP_Colon",			SDLK_KP_COLON },
    { "Key_KP_Hash",			SDLK_KP_HASH },
    { "Key_KP_Space",			SDLK_KP_SPACE },
    { "Key_KP_At",			SDLK_KP_AT },
    { "Key_KP_Exclam",			SDLK_KP_EXCLAM },
    { "Key_KP_MemStore",		SDLK_KP_MEMSTORE },
    { "Key_KP_MemRecall",		SDLK_KP_MEMRECALL },
    { "Key_KP_MemClear",		SDLK_KP_MEMCLEAR },
    { "Key_KP_MemAdd",			SDLK_KP_MEMADD },
    { "Key_KP_MemSubtract",		SDLK_KP_MEMSUBTRACT },
    { "Key_KP_MemMultiply",		SDLK_KP_MEMMULTIPLY },
    { "Key_KP_MemDivide",		SDLK_KP_MEMDIVIDE },
    { "Key_KP_PlusMinus",		SDLK_KP_PLUSMINUS },
    { "Key_KP_Clear",			SDLK_KP_CLEAR },
    { "Key_KP_ClearEntry",		SDLK_KP_CLEARENTRY },
    { "Key_KP_Binary",			SDLK_KP_BINARY },
    { "Key_KP_Octal",			SDLK_KP_OCTAL },
    { "Key_KP_Decimal",			SDLK_KP_DECIMAL },
    { "Key_KP_Hexadecimal",		SDLK_KP_HEXADECIMAL },
    { "Key_Lctrl",			SDLK_LCTRL },
    { "Key_Lshift",			SDLK_LSHIFT },
    { "Key_Lalt",			SDLK_LALT },
    { "Key_Lgui",			SDLK_LGUI },
    { "Key_Rctrl",			SDLK_RCTRL },
    { "Key_Rshift",			SDLK_RSHIFT },
    { "Key_Ralt",			SDLK_RALT },
    { "Key_Rgui",			SDLK_RGUI },
    { "Key_Mode",			SDLK_MODE },
    { "Key_AudioNext",			SDLK_AUDIONEXT },
    { "Key_AudioPrev",			SDLK_AUDIOPREV },
    { "Key_AudioStop",			SDLK_AUDIOSTOP },
    { "Key_AudioPlay",			SDLK_AUDIOPLAY },
    { "Key_AudioMute",			SDLK_AUDIOMUTE },
    { "Key_MediaSelect",		SDLK_MEDIASELECT },
    { "Key_WWW",			SDLK_WWW },
    { "Key_Mail",			SDLK_MAIL },
    { "Key_Calculator",			SDLK_CALCULATOR },
    { "Key_Computer",			SDLK_COMPUTER },
    { "Key_AC_Search",			SDLK_AC_SEARCH },
    { "Key_AC_Home",			SDLK_AC_HOME },
    { "Key_AC_Back",			SDLK_AC_BACK },
    { "Key_AC_Forward",			SDLK_AC_FORWARD },
    { "Key_AC_Stop",			SDLK_AC_STOP },
    { "Key_AC_Refresh",			SDLK_AC_REFRESH },
    { "Key_AC_Bookmarks",		SDLK_AC_BOOKMARKS },
    { "Key_BrightnessDown",		SDLK_BRIGHTNESSDOWN },
    { "Key_BrightnessUp",		SDLK_BRIGHTNESSUP },
    { "Key_DisplaySwitch",		SDLK_DISPLAYSWITCH },
    { "Key_KbdillumToggle",		SDLK_KBDILLUMTOGGLE },
    { "Key_KbdillumDown",		SDLK_KBDILLUMDOWN },
    { "Key_KbdillumUp",			SDLK_KBDILLUMUP },
    { "Key_Eject",			SDLK_EJECT },
    { "Key_Sleep",			SDLK_SLEEP },
    /* event_t.type */
    { "EventQuit",			SDL_QUIT },
    { "EventWindow",			SDL_WINDOWEVENT },
    { "EventKeyDown",			SDL_KEYDOWN },
    { "EventKeyUp",			SDL_KEYUP },
    { "EventTextEdit",			SDL_TEXTEDITING },
    { "EventTextInput",			SDL_TEXTINPUT },
    { "EventMouseMotion",		SDL_MOUSEMOTION },
    { "EventMouseButtonDown",		SDL_MOUSEBUTTONDOWN },
    { "EventMouseButtonUp",		SDL_MOUSEBUTTONUP },
    { "EventMouseWheel",		SDL_MOUSEWHEEL },
    { "EventJoyAxisMotion",		SDL_JOYAXISMOTION },
    { "EventJoyBallMotion",		SDL_JOYBALLMOTION },
    { "EventJoyHatMotion",		SDL_JOYHATMOTION },
    { "EventJoyButtonDown",		SDL_JOYBUTTONDOWN },
    { "EventJoyButtonUp",		SDL_JOYBUTTONUP },
    { "EventJoyDeviceAdded",		SDL_JOYDEVICEADDED },
    { "EventJoyDeviceRemoved",		SDL_JOYDEVICEREMOVED },
    { "EventControllerAxisMotion",	SDL_CONTROLLERAXISMOTION },
    { "EventControllerButtonDown",	SDL_CONTROLLERBUTTONDOWN },
    { "EventControllerButtonUp",	SDL_CONTROLLERBUTTONUP },
    { "EventControllerDeviceAdded",	SDL_CONTROLLERDEVICEADDED },
    { "EventControllerDeviceRemoved",	SDL_CONTROLLERDEVICEREMOVED },
    { "EventControllerDeviceRemapped",	SDL_CONTROLLERDEVICEREMAPPED },
    { "EventFingerDown",		SDL_FINGERDOWN },
    { "EventFingerUp",			SDL_FINGERUP },
    { "EventFingerMotion",		SDL_FINGERMOTION },
    { "EventDollarGesture",		SDL_DOLLARGESTURE },
    { "EventDollarRecord",		SDL_DOLLARRECORD },
    { "EventMultiGesture",		SDL_MULTIGESTURE },
    { "EventTimer",			SDL_USEREVENT },
    /* create_window */
    { "WindowFullscreen",		SDL_WINDOW_FULLSCREEN },
    { "WindowOpenGL",			SDL_WINDOW_OPENGL },
    { "WindowShown",			SDL_WINDOW_SHOWN },
    { "WindowHidden",			SDL_WINDOW_HIDDEN },
    { "WindowBorderless",		SDL_WINDOW_BORDERLESS },
    { "WindowResizable",		SDL_WINDOW_RESIZABLE },
    { "WindowMinimized",		SDL_WINDOW_MINIMIZED },
    { "WindowMaximized",		SDL_WINDOW_MAXIMIZED },
    { "WindowInputGrabbed",		SDL_WINDOW_INPUT_GRABBED },
    { "WindowInputFocus",		SDL_WINDOW_INPUT_FOCUS },
    { "WindowMouseFocus",		SDL_WINDOW_MOUSE_FOCUS },
    { "WindowFullscreenDesktop",	SDL_WINDOW_FULLSCREEN_DESKTOP },
    { "WindowForeign",			SDL_WINDOW_FOREIGN },
    { "WindowAllowHighDpi",		SDL_WINDOW_ALLOW_HIGHDPI },
    { "WindowposUndefined",		SDL_WINDOWPOS_UNDEFINED },
    { "WindowposCentered",		SDL_WINDOWPOS_CENTERED },
    /* create_renderer, renderer_t.flags */
    { "RendererSoftware",		SDL_RENDERER_SOFTWARE },
    { "RendererAccelerated",		SDL_RENDERER_ACCELERATED },
    { "RendererPresentVsync",		SDL_RENDERER_PRESENTVSYNC },
    { "RendererTargetTexture",		SDL_RENDERER_TARGETTEXTURE },
    /* texture_t.access */
    { "TextureAccessStatic",		SDL_TEXTUREACCESS_STATIC },
    { "TextureAccessStreaming",		SDL_TEXTUREACCESS_STREAMING },
    { "TextureAccessTarget",		SDL_TEXTUREACCESS_TARGET },
    /* texture_t.blend */
    { "BlendModeNone",			SDL_BLENDMODE_NONE },
    { "BlendModeBlend",			SDL_BLENDMODE_BLEND },
    { "BlendModeAdd",			SDL_BLENDMODE_ADD },
    { "BlendModeMod",			SDL_BLENDMODE_MOD },
};
static ovector_t		*error_vector;
static ovector_t		*timer_vector;
static ohashtable_t		*window_table;
static ohashtable_t		*texture_table;

/*
 * Implementation
 */
void
init_sdl(void)
{
    char			*string;
    oword_t			 offset;
    osymbol_t			*symbol;
    orecord_t			*record;
    ovector_t			*vector;
    obuiltin_t			*builtin;

    oadd_root((oobject_t *)&error_vector);
    oadd_root((oobject_t *)&timer_vector);
    oadd_root((oobject_t *)&window_table);
    oadd_root((oobject_t *)&texture_table);
    onew_hashtable(&window_table, 4);
    onew_hashtable(&texture_table, 4);
    for (offset = 0; offset < osize(consts); ++offset) {
	string = consts[offset].name;
	onew_constant(sdl_record, oget_string((ouint8_t *)string,
					      strlen(string)),
		      consts[offset].value);
    }

#if __WORDSIZE == 32
#  define pointer_string	"uint32_t"
#else
#  define pointer_string	"uint64_t"
#endif
#define define_builtin0(TYPE, NAME, VARARGS)				\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin1(TYPE, NAME, A0, VARARGS)			\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin2(TYPE, NAME, A0, A1, VARARGS)			\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin3(TYPE, NAME, A0, A1, A2, VARARGS)		\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin4(TYPE, NAME, A0, A1, A2, A3, VARARGS)		\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin5(TYPE, NAME, A0, A1, A2, A3, A4, VARARGS)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin6(TYPE, NAME, A0, A1, A2, A3, A4, A5, VARARGS)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	oend_builtin(builtin);						\
    } while (0)
#define add_field(type, name)						\
    do {								\
	vector = oget_string((ouint8_t *)type, strlen(type));		\
	symbol = oget_identifier(vector);				\
	if (!symbol->base) {						\
	    symbol = oget_symbol(sdl_record, vector);			\
	    /*assert(symbol && symbol->type);*/				\
	}								\
	vector = oget_string((ouint8_t *)name, strlen(name));		\
	offset = record->offset;					\
	(void)onew_symbol(record, vector, symbol->tag);			\
    } while (0)
#define add_vec_field(type, name)					\
    do {								\
	vector = oget_string((ouint8_t *)type, strlen(type));		\
	symbol = oget_identifier(vector);				\
	if (!symbol->base) {						\
	    symbol = oget_symbol(sdl_record, vector);			\
	    /*assert(symbol && symbol->type);*/				\
	}								\
	vector = oget_string((ouint8_t *)name, strlen(name));		\
	offset = record->offset;					\
	(void)onew_symbol(record, vector, otag_vector(symbol->tag, 0));	\
    } while (0)
#define add_union(type, name)						\
    do {								\
	vector = oget_string((ouint8_t *)type, strlen(type));		\
	symbol = oget_identifier(vector);				\
	if (!symbol->base) {						\
	    symbol = oget_symbol(sdl_record, vector);			\
	    /*assert(symbol && symbol->type);*/				\
	}								\
	vector = oget_string((ouint8_t *)name, strlen(name));		\
	record->offset = record->length = offset;			\
	(void)onew_symbol(record, vector, symbol->tag);			\
    } while (0)

    record = type_vector->v.ptr[t_point];
    add_field("int32_t",	"x");
    add_field("int32_t",	"y");
    oend_record(record);

    record = type_vector->v.ptr[t_rect];
    add_field("int32_t",	"x");
    add_field("int32_t",	"y");
    add_field("int32_t",	"w");
    add_field("int32_t",	"h");
    oend_record(record);

    record = type_vector->v.ptr[t_window];
    add_field(pointer_string,	"*window*");
    add_field("auto",		"*handle*");	/* gc it */
    add_field("auto",		"*renderer*");	/* gc it */
    add_field("int32_t",	"x");
    add_field("int32_t",	"y");
    add_field("int32_t",	"w");
    add_field("int32_t",	"h");
    add_field("int32_t",	"min_w");
    add_field("int32_t",	"min_h");
    add_field("int32_t",	"max_w");
    add_field("int32_t",	"max_h");
    add_field("uint32_t",	"flags");
    add_field("string_t",	"title");
    add_field("int32_t",	"*x*");
    add_field("int32_t",	"*y*");
    add_field("int32_t",	"*w*");
    add_field("int32_t",	"*h*");
    add_field("int32_t",	"*min_w*");
    add_field("int32_t",	"*min_h*");
    add_field("int32_t",	"*max_w*");
    add_field("int32_t",	"*max_h*");
    add_field("uint32_t",	"*flags*");
    add_field("string_t",	"*title*");
    oend_record(record);

    record = type_vector->v.ptr[t_renderer];
    add_field(pointer_string,	"*renderer*");
    add_field("auto",		"*window*");	/* gc it */
    add_field("texture_t",	"target");
    add_field("int32_t",	"log_w");
    add_field("int32_t",	"log_h");
    add_field("int32_t",	"max_w");
    add_field("int32_t",	"max_h");
    add_field("string_t",	"name");
    add_field("uint32_t",	"flags");
    add_vec_field("uint32_t",	"formats");
    add_field("texture_t",	"*target*");
    add_field("int32_t",	"*log_w*");
    add_field("int32_t",	"*log_h*");
    oend_record(record);

    record = type_vector->v.ptr[t_surface];
    add_field(pointer_string,	"*surface*");
    oend_record(record);

    record = type_vector->v.ptr[t_texture];
    add_field(pointer_string,	"*texture*");
    add_field("auto",		"*handle*");	/* gc it */
    add_field("uint32_t",	"format");
    add_field("int32_t",	"access");
    add_field("int32_t",	"w");
    add_field("int32_t",	"h");
    add_field("uint8_t",	"r");
    add_field("uint8_t",	"g");
    add_field("uint8_t",	"b");
    add_field("uint8_t",	"a");
    add_field("uint8_t",	"blend");
    add_field("uint8_t",	"*r*");
    add_field("uint8_t",	"*g*");
    add_field("uint8_t",	"*b*");
    add_field("uint8_t",	"*a*");
    add_field("uint8_t",	"*blend*");
    oend_record(record);

    record = type_vector->v.ptr[t_timer];
    add_field("uint32_t",	"*timer*");
    add_field("uint32_t",	"msec");
    add_field("auto",		"data");
    oend_record(record);

    record = type_vector->v.ptr[t_event];
    add_field("auto",		"*event*");	/* gc it */
    /**/
    add_field("uint32_t",	"type");
    /**/
    add_field("uint32_t",	"time");
    /**/
    add_field("auto",		"window");	/* gc it */
    add_union(pointer_string,	"gesture");
    add_union(pointer_string,	"finger");
    /**/
    add_field("auto",		"device");	/* gc it */
    /**/
    add_field("int16_t",	"state");
    add_union("int16_t",	"event");
    add_union("int16_t",	"axis");
    add_union("int16_t",	"ball");
    add_union("int16_t",	"hat");
    /**/
    add_field("int16_t",	"repeat");
    add_union("int16_t",	"fingers");
    /**/
    add_field("int32_t",	"button");
    add_union("int32_t",	"keysym");
    add_union("int32_t",	"value");
    add_union("float32_t",	"pressure");
    /**/
    add_field("int32_t",	"x");
    add_union("int32_t",	"w");
    add_union("float32_t",	"fx");
    add_union("int32_t",	"start");
    /**/
    add_field("int32_t",	"y");
    add_union("int32_t",	"h");
    add_union("float32_t",	"fy");
    add_union("int32_t",	"length");
    /**/
    add_field("int32_t",	"dx");
    add_union("float32_t",	"fdx");
    add_union("float32_t",	"theta");
    add_union("float32_t",	"ferror");
    /**/
    add_field("int32_t",	"dy");
    add_union("float32_t",	"fdy");
    add_union("float32_t",	"dist");
    /**/
    add_field("string_t",	"text");
    /* do not "union" with text to avoid gc'ing it if handing timers and text */
    add_field("timer_t",	"timer");
    oend_record(record);

    record = current_record;
    current_record = sdl_record;

    define_builtin0(t_int32,  init, false);
    define_builtin0(t_string, get_error, false);
    define_builtin0(t_void,   quit, false);

    define_builtin6(t_window,   create_window,
		    t_string, t_int32, t_int32, t_int32, t_int32, t_uint32,
		    false);
    define_builtin1(t_void,     change_window, t_window, false);
    define_builtin1(t_renderer, get_window_renderer, t_window, false);
    define_builtin1(t_void,     destroy_window, t_window, false);

    define_builtin3(t_renderer, create_renderer,
		    t_window, t_int32, t_uint32,
		    false);
    define_builtin1(t_int32,    change_renderer, t_renderer, false);
    define_builtin1(t_window,   get_renderer_window, t_renderer, false);
    define_builtin1(t_void,     render_clear, t_renderer, false);
    define_builtin4(t_int32,    render_copy,
		    t_renderer, t_texture, t_rect, t_rect,
		    false);
    define_builtin1(t_void,    render_present, t_renderer, false);
    define_builtin1(t_void,    destroy_renderer, t_renderer, false);

    define_builtin5(t_texture, create_texture,
		    t_renderer, t_uint32, t_int32, t_int32, t_int32,
		    false);
    define_builtin2(t_texture, create_texture_from_surface,
		    t_renderer, t_surface,
		    false);
    define_builtin2(t_texture, load_texture, t_string, t_renderer, false);
    define_builtin1(t_int32,   change_texture, t_texture, false);
    define_builtin1(t_void,    destroy_texture, t_texture, false);

    define_builtin1(t_int32,   poll_event, t_event, false);
    define_builtin1(t_int32,   wait_event, t_event, false);

    define_builtin2(t_timer,   add_timer, t_uint32, t_void, false);
    define_builtin0(t_uint32,  get_ticks, false);
    define_builtin1(t_void,    delay, t_uint32, false);
    define_builtin1(t_int32,   remove_timer, t_timer, false);

    current_record = record;
#undef add_field
#undef define_builtin1
#undef define_builtin0
}

void
finish_sdl(void)
{
    orem_root((oobject_t *)&texture_table);
    orem_root((oobject_t *)&window_table);
    orem_root((oobject_t *)&timer_vector);
    orem_root((oobject_t *)&error_vector);
}

void
odestroy_window(owindow_t *window)
{
    if (window->__window) {
	orem_hashentry(window_table, window->__handle);
	SDL_DestroyWindow(window->__window);
	window->__window = null;
    }
}

extern void
odestroy_texture(otexture_t *texture)
{
    if (texture->__texture) {
	orem_hashentry(texture_table, texture->__handle);
	SDL_DestroyTexture(texture->__texture);
	texture->__texture = null;
    }
}

static void
native_init(oobject_t list, oint32_t ac)
/* int32_t init(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = ((SDL_Init(SDL_INIT_EVERYTHING) != 0) |
	       (IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|
			 IMG_INIT_TIF|IMG_INIT_WEBP) == 0) |
	       (TTF_Init() != 0));
}

static void
native_get_error(oobject_t list, oint32_t ac)
/* string_t get_error(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    const char			*error;
    oword_t			 length;

    r0 = &thread_self->r0;
    if ((error = SDL_GetError())) {
	length = strlen(error);
	if (error_vector == null)
	    onew_vector((oobject_t *)&error_vector, t_uint8, length);
	else if (length != error_vector->length)
	    orenew_vector(error_vector, length);
	memcpy(error_vector->v.ptr, error, length);
	r0->t = t_string;
	r0->v.o = error_vector;
    }
    else
	r0->t = t_void;
}

static void
native_quit(oobject_t list, oint32_t ac)
/* void quit(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

static void
native_create_window(oobject_t list, oint32_t ac)
/* window_t create_window(string_t title,
			  int32_t x, int32_t y, int32_t w, int32_t h,
			  uint32_t flags); */
{
    GET_THREAD_SELF()
    SDL_Window				*sw;
    owindow_t				*ow;
    oregister_t				*r0;
    nat_vec_i32_i32_i32_i32_u32_t	*alist;
    const char				*buffer;
    oword_t				 length;
    char				 title[BUFSIZ];

    r0 = &thread_self->r0;
    alist = (nat_vec_i32_i32_i32_i32_u32_t *)list;
    if (alist->vec == null)
	title[0] = '\0';
    else if (otype(alist->vec) != t_string)
	ovm_raise(except_invalid_argument);
    else {
	if ((length = alist->vec->length) > BUFSIZ - 1)
	    length = BUFSIZ - 1;
	memcpy(title, alist->vec->v.ptr, length);
	title[length] = '\0';
    }
    if ((sw = SDL_CreateWindow(title, alist->si0, alist->si1,
			       alist->si2, alist->si3,
			       alist->u32))) {
	/* window */
	onew_object(&thread_self->obj, t_window, sizeof(owindow_t));
	ow = (owindow_t *)thread_self->obj;
	ow->__window = sw;
	/* handle */
	onew_object((oobject_t *)&ow->__handle,
		    t_hashentry, sizeof(ohashentry_t));
	ow->__handle->nt = t_word;
	ow->__handle->nv.w = SDL_GetWindowID(sw);
	ow->__handle->vt = t_window;
	ow->__handle->vv.o = ow;
	okey_hashentry(ow->__handle);
	assert(oget_hashentry(window_table, ow->__handle) == null);
	oput_hashentry(window_table, ow->__handle);
	/* x, y */
	SDL_GetWindowPosition(sw, &ow->__x, &ow->__y);
	ow->x = ow->__x;
	ow->y = ow->__y;
	/* w, h */
	SDL_GetWindowSize(sw, &ow->__w, &ow->__h);
	ow->w = ow->__w;
	ow->h = ow->__h;
	/* min_w, min_h */
	SDL_GetWindowMinimumSize(sw, &ow->__min_w, &ow->__min_h);
	ow->min_w = ow->__min_w;
	ow->min_h = ow->__min_h;
	/* max_w, max_h */
	SDL_GetWindowMaximumSize(sw, &ow->__max_w, &ow->__max_h);
	ow->max_w = ow->__max_w;
	ow->max_h = ow->__max_h;
	/* flags */
	ow->__flags = SDL_GetWindowFlags(sw);
	ow->flags = ow->__flags;
	/* title */
	buffer = SDL_GetWindowTitle(sw);
	if (buffer)
	    length = strlen(buffer);
	else
	    length = 0;
	onew_vector((oobject_t *)&ow->__title, t_uint8, length);
	if (length)
	    memcpy(ow->__title->v.u8, buffer, length);
	ow->title = ow->__title;
	/**/
	r0->v.o = thread_self->obj;
	r0->t = t_window;
    }
    else
	r0->t = t_void;
}

static void
native_change_window(oobject_t list, oint32_t ac)
/* void change_window(window_t window); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    owindow_t			*ow;
    SDL_Window			*sw;
    nat_win_t			*alist;
    const char			*buffer;
    oword_t			 length;
    char			 title[BUFSIZ];

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    if (alist->win == null ||
	otype(alist->win) != t_window || alist->win->__window == null)
	ovm_raise(except_invalid_argument);
    ow = alist->win;
    sw = ow->__window;
    /* x, y */
    if (ow->x != ow->__x || ow->y != ow->__y) {
	SDL_SetWindowPosition(sw, ow->x, ow->y);
	SDL_GetWindowPosition(sw, &ow->__x, &ow->__y);
	ow->x = ow->__x;
	ow->y = ow->__y;
    }
    /* w, h */
    if (ow->w != ow->__w || ow->h != ow->__h) {
	SDL_SetWindowSize(sw, ow->w, ow->h);
	SDL_GetWindowSize(sw, &ow->__w, &ow->__h);
	ow->w = ow->__w;
	ow->h = ow->__h;
    }
    /* min_w, min_h */
    if (ow->min_w != ow->__min_w || ow->min_h != ow->__min_h) {
	SDL_SetWindowMinimumSize(sw, ow->min_w, ow->min_h);
	SDL_GetWindowMinimumSize(sw, &ow->__min_w, &ow->__min_h);
	ow->min_w = ow->__min_w;
	ow->min_h = ow->__min_h;
    }
    /* max_w, max_h */
    if (ow->max_w != ow->__max_w || ow->max_h != ow->__max_h) {
	SDL_SetWindowMaximumSize(sw, ow->max_w, ow->max_h);
	SDL_GetWindowMaximumSize(sw, &ow->__max_w, &ow->__max_h);
	ow->max_w = ow->__max_w;
	ow->max_h = ow->__max_h;
    }
    /* flags */
    if (ow->flags != ow->__flags) {
#define change(FLAG)		((ow->flags & FLAG) ^ (ow->__flags & FLAG))
#define set(FLAG)		(!!(ow->flags & FLAG))
	if (change(SDL_WINDOW_FULLSCREEN))
	    SDL_SetWindowFullscreen(sw, ow->flags &
				    (SDL_WINDOW_FULLSCREEN_DESKTOP));
	if (change(SDL_WINDOW_SHOWN) || change(SDL_WINDOW_HIDDEN)) {
	    if (set(SDL_WINDOW_SHOWN) && !set(SDL_WINDOW_HIDDEN))
		SDL_ShowWindow(sw);
	    else if (!set(SDL_WINDOW_SHOWN) && set(SDL_WINDOW_HIDDEN))
		SDL_HideWindow(sw);
	}
	if (!set(SDL_WINDOW_FULLSCREEN) &&
	    (!change(SDL_WINDOW_MINIMIZED) || change(SDL_WINDOW_MAXIMIZED))) {
	    if (set(SDL_WINDOW_MINIMIZED) && !set(SDL_WINDOW_MAXIMIZED))
		SDL_MinimizeWindow(sw);
	    else if (!set(SDL_WINDOW_MINIMIZED) && set(SDL_WINDOW_MAXIMIZED))
		SDL_MaximizeWindow(sw);
	    else
		SDL_RestoreWindow(sw);
	}
	if (change(SDL_WINDOW_INPUT_GRABBED))
	    SDL_SetWindowGrab(sw, set(SDL_WINDOW_INPUT_GRABBED));
#undef set
#undef unset
#undef mismatch
    }
    /* actual flags may change based on previous changes */
    ow->__flags = SDL_GetWindowFlags(sw);
    ow->flags = ow->__flags;
    /* title */
    if (ow->title != ow->__title) {
	if (ow->title == null)
	    title[0] = '\0';
	else if (otype(ow->title) != t_string)
	    ovm_raise(except_invalid_argument);
	else {
	    if ((length = ow->title->length) > BUFSIZ - 1)
		length = BUFSIZ - 1;
	    memcpy(title, ow->title->v.ptr, length);
	    title[length] = '\0';
	}
	SDL_SetWindowTitle(sw, title);
	buffer = SDL_GetWindowTitle(sw);
	if (buffer)
	    length = strlen(buffer);
	else
	    length = 0;
	orenew_vector(ow->__title, length);
	if (length)
	    memcpy(ow->__title->v.u8, buffer, length);
	ow->title = ow->__title;
    }
    r0->t = t_void;
}

static void
native_get_window_renderer(oobject_t list, oint32_t ac)
/* renderer_t get_window_renderer(window_t window); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_win_t			*alist;

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    if (alist->win == null || otype(alist->win) != t_window)
	ovm_raise(except_invalid_argument);
    if ((r0->v.o = alist->win->__renderer))
	r0->t = t_renderer;
    else
	r0->t = t_void;
}

static void
native_destroy_window(oobject_t list, oint32_t ac)
/* void destroy_window(window_t window); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_win_t			*alist;

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    if (alist->win) {
	if (otype(alist->win) != t_window)
	    ovm_raise(except_invalid_argument);
	odestroy_window(alist->win);
    }
    r0->t = t_void;
}

static void
query_renderer(orenderer_t *or)
{
    SDL_Renderer		*sr;
    SDL_Texture			*tx;
    ohashentry_t		 et;
    ohashentry_t		*ot;

    sr = or->__renderer;
    SDL_RenderGetLogicalSize(sr, &or->__log_w, &or->__log_h);
    or->log_w = or->__log_w;
    or->log_h = or->__log_h;
    if ((tx = SDL_GetRenderTarget(sr))) {
	et.nt = t_word;
	et.nv.w = (oword_t)tx;
	if ((ot = oget_hashentry(texture_table, &et)))
	    or->target = or->__target = (otexture_t *)ot->nv.w;
    }
}

static void
native_create_renderer(oobject_t list, oint32_t ac)
/* renderer_t create_renderer(window_t win, int32_t index, uint32_t flags); */
{
    GET_THREAD_SELF()
    orenderer_t			*or;
    SDL_Renderer		*sr;
    oregister_t			*r0;
    SDL_RendererInfo		 info;
    nat_win_i32_u32_t		*alist;

    alist = (nat_win_i32_u32_t *)list;
    if (alist->win == null || otype(alist->win) != t_window)
	ovm_raise(except_invalid_argument);
    r0 = &thread_self->r0;
    if ((sr = SDL_CreateRenderer(alist->win->__window,
				 alist->i32, alist->u32))) {
	onew_object(&thread_self->obj, t_renderer, sizeof(orenderer_t));
	or = (orenderer_t *)thread_self->obj;
	or->__renderer = sr;
	or->__window = alist->win;
	or->__window->__renderer = or;
	SDL_GetRendererInfo(sr, &info);
	onew_vector((oobject_t *)&or->name, t_uint8,
		    info.name ? strlen(info.name) : 0);
	or->flags = info.flags;
	if (info.name)
	    memcpy(or->name->v.ptr, info.name, strlen(info.name));
	onew_vector((oobject_t *)&or->formats, t_uint32,
		    info.num_texture_formats);
	memcpy(or->formats->v.ptr, info.texture_formats,
	       info.num_texture_formats * sizeof(ouint32_t));
	or->max_w = info.max_texture_width;
	or->max_h = info.max_texture_height;
	query_renderer(or);
	r0->v.o = thread_self->obj;
	r0->t = t_renderer;
    }
    else
	r0->t = t_void;
}

static void
native_change_renderer(oobject_t list, oint32_t ac)
/* int32_t change_renderer(renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    orenderer_t			*ren;
    otexture_t			*tex;
    ohashentry_t		 check;
    ohashentry_t		*entry;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    ren = alist->ren;
    if (ren == null || otype(ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = 0;
    if (ren->log_w != ren->__log_w || ren->log_h != ren->__log_h)
	r0->v.w |= SDL_RenderSetLogicalSize(ren->__renderer,
					    ren->log_w, ren->log_h);
    if (ren->target != ren->__target) {
	if (ren->target && otype(ren->target) != t_texture)
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = (oword_t)ren->target;
	if ((entry = oget_hashentry(texture_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	tex = (otexture_t *)entry->nv.w;
	r0->v.w |= SDL_SetRenderTarget(ren->__renderer, tex->__texture);
    }
    query_renderer(ren);
}

static void
native_get_renderer_window(oobject_t list, oint32_t ac)
/* window_t get_renderer_window(renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    if ((r0->v.o = alist->ren->__window))
	r0->t = t_window;
    else
	r0->t = t_void;
}

static void
native_render_clear(oobject_t list, oint32_t ac)
/* void render_clear(renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderClear(alist->ren->__renderer);
}

static void
native_render_copy(oobject_t list, oint32_t ac)
/* void render_present(renderer_t ren, texture_t tex,
		       rect_t src, rect_t dst); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_tex_rec_rec_t	*alist;

    alist = (nat_ren_tex_rec_rec_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->tex == null || otype(alist->tex) != t_texture ||
	(alist->src && otype(alist->src) != t_rect) ||
	(alist->dst && otype(alist->dst) != t_rect))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderCopy(alist->ren->__renderer, alist->tex->__texture,
			     (SDL_Rect *)alist->src, (SDL_Rect *)alist->dst);
}

static void
native_render_present(oobject_t list, oint32_t ac)
/* void render_present(renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    SDL_RenderPresent(alist->ren->__renderer);
    r0->t = t_void;
}

static void
native_destroy_renderer(oobject_t list, oint32_t ac)
/* void destroy_renderer(renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren && alist->ren->__renderer) {
	if (otype(alist->ren) != t_renderer)
	    ovm_raise(except_invalid_argument);
	SDL_DestroyRenderer(alist->ren->__renderer);
	alist->ren->__renderer = null;
    }
    r0->t = t_void;
}

static void
query_texture(otexture_t *ot)
{
    SDL_Texture			*st;
    SDL_BlendMode		 blend;

    st = ot->__texture;
    SDL_QueryTexture(st, &ot->format, &ot->access, &ot->w, &ot->h);
    SDL_GetTextureColorMod(st, &ot->__r, &ot->__g, &ot->__b);
    ot->r = ot->__r;
    ot->g = ot->__g;
    ot->b = ot->__b;
    SDL_GetTextureAlphaMod(st, &ot->__a);
    ot->a = ot->__a;
    SDL_GetTextureBlendMode(st, &blend);
    ot->blend = ot->__blend = blend;
}

static void
handle_texture(otexture_t *tex)
{
    onew_object((oobject_t *)&tex->__handle,
		t_hashentry, sizeof(ohashentry_t));
    tex->__handle->nt = t_word;
    tex->__handle->nv.w = (oword_t)tex->__texture;
    tex->__handle->vt = t_word;
    tex->__handle->vv.w = (oword_t)tex;
    okey_hashentry(tex->__handle);
    assert(oget_hashentry(texture_table, tex->__handle) == null);
    oput_hashentry(texture_table, tex->__handle);
}

static void
native_create_texture(oobject_t list, oint32_t ac)
/* texture_t create_texture(renderer_t ren, uint32_t format,
			    int32_t access, int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    SDL_Texture			*st;
    otexture_t			*ot;
    oregister_t			*r0;
    nat_ren_u32_i32_i32_i32_t	*alist;

    alist = (nat_ren_u32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    if ((st = SDL_CreateTexture(alist->ren->__renderer, alist->u32,
				alist->si0, alist->si1, alist->si2))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(ot);
	query_texture(ot);
	r0->v.o = thread_self->obj;
	r0->t = t_texture;
    }
    else
	r0->t = t_void;
}

static void
native_create_texture_from_surface(oobject_t list, oint32_t ac)
/* texture_t create_texture_from_surface(renderer_t ren, surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Texture			*st;
    otexture_t			*ot;
    oregister_t			*r0;
    nat_ren_srf_t		*alist;

    alist = (nat_ren_srf_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->srf == null || otype(alist->srf) != t_surface)
	ovm_raise(except_invalid_argument);
    if ((st = SDL_CreateTextureFromSurface(alist->ren->__renderer,
					   alist->srf->__surface))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(ot);
	query_texture(ot);
	r0->v.o = thread_self->obj;
	r0->t = t_texture;
    }
    else
	r0->t = t_void;
}

static void
native_load_texture(oobject_t list, oint32_t ac)
/* texture_t load_texture(renderer_t ren, string_t path); */
{
    GET_THREAD_SELF()
    SDL_Texture			*st;
    otexture_t			*ot;
    oregister_t			*r0;
    nat_ren_vec_t		*alist;
    char			 path[BUFSIZ];

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->vec == null || otype(alist->vec) != t_string)
	ovm_raise(except_invalid_argument);
    if (alist->vec->length >= BUFSIZ)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->vec->v.ptr, alist->vec->length);
    path[alist->vec->length] = '\0';
    if ((st = IMG_LoadTexture(alist->ren->__renderer, path))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(ot);
	query_texture(ot);
	r0->v.o = thread_self->obj;
	r0->t = t_texture;
    }
    else
	r0->t = t_void;
}

static void
native_change_texture(oobject_t list, oint32_t ac)
/* int32_t change_texture(texture_t tex); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    otexture_t			*ot;
    nat_tex_t			*alist;

    alist = (nat_tex_t *)list;
    r0 = &thread_self->r0;
    if (alist->tex == null && otype(alist->tex) != t_texture)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = 0;
    ot = alist->tex;
    if (ot->r != ot->__r || ot->g != ot->__g || ot->b != ot->__b)
	r0->v.w |= SDL_SetTextureColorMod(ot->__texture, ot->r, ot->g, ot->b);
    if (ot->a != ot->__a)
	r0->v.w |= SDL_SetTextureAlphaMod(ot->__texture, ot->a);
    if (ot->blend != ot->__blend)
	r0->v.w |= SDL_SetTextureBlendMode(ot->__texture, ot->blend);
    query_texture(ot);
}

static void
native_destroy_texture(oobject_t list, oint32_t ac)
/* void destroy_texture(texture_t tex); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_tex_t			*alist;

    alist = (nat_tex_t *)list;
    r0 = &thread_self->r0;
    if (alist->tex && alist->tex->__texture) {
	if (otype(alist->tex) != t_texture)
	    ovm_raise(except_invalid_argument);
	odestroy_texture(alist->tex);
    }
    r0->t = t_void;
}

static inline void
translate_window(oevent_t *ev, ouint32_t id)
{
    ohashentry_t		 e;
    ohashentry_t		*v;

    e.nt = t_word;
    e.nv.w = id;
    okey_hashentry(&e);
    if ((v = oget_hashentry(window_table, &e)))
	ev->window = v->vv.o;
    else
	ev->window = null;
}

static void
translate_event(oevent_t *ev)
{
    SDL_Event			*sv;

    sv = ev->__event;
    ev->time = sv->common.timestamp;
    switch ((ev->type = sv->type)) {
	case SDL_QUIT:
	    break;
	case SDL_WINDOWEVENT:
	    translate_window(ev, sv->window.windowID);
	    switch ((ev->event = sv->window.event)) {
		case SDL_WINDOWEVENT_MOVED:
		    ev->x	= sv->window.data1;
		    ev->y	= sv->window.data2;
		    break;
		case SDL_WINDOWEVENT_RESIZED:
		    ev->w	= sv->window.data1;
		    ev->h	= sv->window.data2;
		    break;
		default:
		    break;
	    }
	    break;
	case SDL_KEYDOWN:	case SDL_KEYUP:
	    translate_window(ev, sv->key.windowID);
	    ev->state		= sv->key.state;
	    ev->repeat		= sv->key.repeat;
	    ev->keysym		= sv->key.keysym.sym;
	    break;
	case SDL_TEXTEDITING:
	    translate_window(ev, sv->edit.windowID);
	    if (ev->text == null)
		onew_vector((oobject_t *)&ev->text, t_uint8,
			    SDL_TEXTEDITINGEVENT_TEXT_SIZE);
	    else if (otype(ev->text) != t_string ||
		     ev->text->length != SDL_TEXTEDITINGEVENT_TEXT_SIZE)
		ovm_raise(except_type_mismatch);
	    memcpy(ev->text->v.ptr, sv->edit.text,
		   SDL_TEXTEDITINGEVENT_TEXT_SIZE);
	    ev->start		= sv->edit.start;
	    ev->length		= sv->edit.length;
	    break;
	case SDL_TEXTINPUT:
	    translate_window(ev, sv->text.windowID);
	    if (ev->text == null)
		onew_vector((oobject_t *)&ev->text, t_uint8,
			    SDL_TEXTINPUTEVENT_TEXT_SIZE);
	    else if (otype(ev->text) != t_string ||
		     ev->text->length != SDL_TEXTINPUTEVENT_TEXT_SIZE)
		ovm_raise(except_type_mismatch);
	    memcpy(ev->text->v.ptr, sv->text.text,
		   SDL_TEXTINPUTEVENT_TEXT_SIZE);
	    break;
	case SDL_MOUSEMOTION:
	    translate_window(ev, sv->motion.windowID);
#if 0
	    ev->device		= sv->motion.which;
#endif
	    ev->state		= sv->motion.state;
	    ev->x		= sv->motion.x;
	    ev->y		= sv->motion.y;
	    ev->dx		= sv->motion.xrel;
	    ev->dy		= sv->motion.yrel;
	    break;
	case SDL_MOUSEBUTTONDOWN:	case SDL_MOUSEBUTTONUP:
	    translate_window(ev, sv->button.windowID);
#if 0
	    ev->device		= sv->button.which;
#endif
	    ev->button		= sv->button.button;
	    ev->state		= sv->button.state;
	    ev->repeat		= sv->button.clicks;
	    ev->x		= sv->button.x;
	    ev->y		= sv->button.y;
	    break;
	case SDL_MOUSEWHEEL:
	    translate_window(ev, sv->wheel.windowID);
#if 0
	    ev->device		= sv->wheel.which;
#endif
	    ev->x		= sv->wheel.x;
	    ev->y		= sv->wheel.y;
	    break;
	case SDL_JOYAXISMOTION:
#if 0
	    ev->device		= sv->jaxis.which;
#endif
	    ev->axis		= sv->jaxis.axis;
	    ev->value		= sv->jaxis.value;
	    break;
	case SDL_JOYBALLMOTION:
#if 0
	    ev->device		= sv->jball.which;
#endif
	    ev->ball		= sv->jball.ball;
	    ev->dx		= sv->jball.xrel;
	    ev->dy		= sv->jball.yrel;
	    break;
	case SDL_JOYHATMOTION:
#if 0
	    ev->device		= sv->jhat.which;
#endif
	    ev->hat		= sv->jhat.hat;
	    ev->value		= sv->jhat.value;
	    break;
	case SDL_JOYBUTTONDOWN:		case SDL_JOYBUTTONUP:
#if 0
	    ev->device		= sv->jbutton.which;
#endif
	    ev->button		= sv->jbutton.button;
	    ev->state		= sv->jbutton.state;
	    break;
	case SDL_JOYDEVICEADDED:	case SDL_JOYDEVICEREMOVED:
#if 0
	    ev->device		= sv->jdevice.which;
#endif
	    break;
	case SDL_CONTROLLERAXISMOTION:
#if 0
	    ev->device		= sv->caxis.which;
#endif
	    ev->axis		= sv->caxis.axis;
	    ev->value		= sv->caxis.value;
	    break;
	case SDL_CONTROLLERBUTTONDOWN:	case SDL_CONTROLLERBUTTONUP:
#if 0
	    ev->device		= sv->cbutton.which;
#endif
	    ev->button		= sv->cbutton.button;
	    ev->state		= sv->cbutton.state;
	    break;
	case SDL_CONTROLLERDEVICEADDED:	case SDL_CONTROLLERDEVICEREMOVED:
#if 0
	    ev->device		= sv->cdevice.which;
#endif
	    break;
	case SDL_FINGERMOTION:		case SDL_FINGERDOWN:
	case SDL_FINGERUP:
#if 0
	    ev->device		= sv->tfinger.touchId;
	    ev->finger		= sv->tfinger.fingerId;
#endif
	    ev->fx		= sv->tfinger.x;
	    ev->fy		= sv->tfinger.y;
	    ev->fdx		= sv->tfinger.dx;
	    ev->fdy		= sv->tfinger.dy;
	    ev->pressure	= sv->tfinger.pressure;
	    break;
	case SDL_MULTIGESTURE:
#if 0
	    ev->device		= sv->mgesture.touchId;
#endif
	    ev->fx		= sv->mgesture.x;
	    ev->fy		= sv->mgesture.y;
	    ev->theta		= sv->mgesture.dTheta;
	    ev->dist		= sv->mgesture.dDist;
	    ev->fingers		= sv->mgesture.numFingers;
	    break;
	case SDL_DOLLARGESTURE:
#if 0
	    ev->gesture		= sv->dgesture.gestureId;
	    ev->device		= sv->dgesture.touchId;
#endif
	    ev->fingers		= sv->dgesture.numFingers;
	    ev->ferror		= sv->dgesture.error;
	    ev->fx		= sv->dgesture.x;
	    ev->fy		= sv->dgesture.y;
	    break;
	case SDL_USEREVENT:
	    ev->timer		= sv->user.data1;
	    break;
	default:
	    abort();
    }
}

static void
native_poll_event(oobject_t list, oint32_t ac)
/* void poll_event(event_t ev); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ev_t			*alist;

    alist = (nat_ev_t *)list;
    r0 = &thread_self->r0;
    if (alist->ev == null || otype(alist->ev) != t_event)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    if (alist->ev->__event == null)
	onew_object((oobject_t *)&alist->ev->__event,
		    t_void, sizeof(SDL_Event));
    if ((r0->v.w = SDL_PollEvent(alist->ev->__event)))
	translate_event(alist->ev);
}

static void
native_wait_event(oobject_t list, oint32_t ac)
/* void poll_event(event_t ev); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ev_t			*alist;

    alist = (nat_ev_t *)list;
    r0 = &thread_self->r0;
    if (alist->ev == null || otype(alist->ev) != t_event)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    if (alist->ev->__event == null)
	onew_object((oobject_t *)&alist->ev->__event,
		    t_void, sizeof(SDL_Event));
    if ((r0->v.w = SDL_WaitEvent(alist->ev->__event)))
	translate_event(alist->ev);
}

static Uint32
timer_callback(Uint32 ms, void *data)
{
    SDL_Event			 ev;
    otimer_t			*ot;

    ot = (otimer_t *)data;
    if (ot->msec) {
	ev.user.type = SDL_USEREVENT;
	ev.user.code = 0;
	ev.user.data1 = data;
	ev.user.data2 = null;
	SDL_PushEvent(&ev);
    }
    else {
	SDL_RemoveTimer(ot->__timer);
	ot->__timer = 0;
	push_timer(ot);
    }

    return (ot->msec);
}

/* Helper functions to avoid gc in case user properly calls add_timer and
 * then remove_timer (or sets timer.ms to 0) if the timer is no longer used.
 * If the timer object looses all references before a remove_timer call, it
 * will eventually be garbage collected. */
static void
pop_timer(oobject_t *pointer)
{
    if (timer_vector && timer_vector->offset)
	*pointer = timer_vector->v.ptr[--timer_vector->offset];
    else
	onew_object(pointer, t_timer, sizeof(otimer_t));
}

static void
push_timer(otimer_t *timer)
{
    timer->data = null;
    if (timer_vector == null)
	onew_vector((oobject_t *)&timer_vector, t_void, 4);
    else if (timer_vector->offset >= timer_vector->length)
	orenew_vector(timer_vector, timer_vector->length + 4);
    timer_vector->v.ptr[timer_vector->offset++] = timer;
}

static void
native_add_timer(oobject_t list, oint32_t ac)
/* timer_t add_timer(uint32_t ms, auto data); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    SDL_TimerID			 st;
    otimer_t			*ot;
    nat_u32_obj_t		*alist;

    alist = (nat_u32_obj_t *)list;
    r0 = &thread_self->r0;

    pop_timer(&thread_self->obj);
    ot = (otimer_t *)thread_self->obj;
    ot->msec = alist->u32;
    ot->data = alist->obj;
    if ((st = SDL_AddTimer(alist->u32, timer_callback, ot))) {
	thread_self->obj = ot;
	ot->__timer = st;
	r0->v.o = ot;
	r0->t = t_timer;
    }
    else {
	r0->t = t_void;
	push_timer(ot);
	thread_self->obj = null;
    }
}

static void
native_get_ticks(oobject_t list, oint32_t ac)
/* uint32_t get_ticks(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    ret_u32(r0, SDL_GetTicks());
}

static void
native_delay(oobject_t list, oint32_t ac)
/* void delay(uint32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_u32_t			*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    SDL_Delay(alist->u32);
    r0->t = t_void;
}

static void
native_remove_timer(oobject_t list, oint32_t ac)
/* int32_t remove_timer(otimer_t); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_tm_t			*alist;

    alist = (nat_tm_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    if (alist->tm == null || otype(alist->tm) != t_timer)
	ovm_raise(except_invalid_argument);
    if (alist->tm->__timer) {
	r0->v.w = SDL_RemoveTimer(alist->tm->__timer);
	alist->tm->__timer = 0;
	alist->tm->msec = 0;
	push_timer(alist->tm);
    }
    else
	r0->v.w = false;
}

#if __WORDSIZE == 32
static void
ret_u32(oregister_t *r, ouint32_t v)
{
    if ((oint32_t)v < 0) {
	mpz_set_ui(ozr(r), v);
	r->t = t_mpz;
    }
    else {
	r->t = t_word;
	r->v.w = v;
    }
}
#endif