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
#include "obuiltin.h"

#if __BYTE_ORDER == __LITTLE_ENDIAN
#  define R_MASK		0x000000ff
#  define G_MASK		0x0000ff00
#  define B_MASK		0x00ff0000
#  define A_MASK		0xff000000
#else
#  define R_MASK		0xff000000
#  define G_MASK		0x00ff0000
#  define B_MASK		0x0000ff00
#  define A_MASK		0x000000ff
#endif

#define check_mult(x, y)						\
    do {								\
	if (0x7fffffff / (y) < (x))					\
	    ovm_raise(except_not_a_32_bits_integer);			\
    } while (0)

/*
 * Prototypes
 */
/* sdl */
static void native_Init(oobject_t list, oint32_t ac);
static void native_GetError(oobject_t list, oint32_t ac);
static void native_ClearError(oobject_t list, oint32_t ac);
static void native_Quit(oobject_t list, oint32_t ac);
static void native_CreateWindow(oobject_t list, oint32_t ac);
static void native_ChangeWindow(oobject_t list, oint32_t ac);
static void native_GetWindowRenderer(oobject_t list, oint32_t ac);
static void native_DestroyWindow(oobject_t list, oint32_t ac);
static void query_renderer(orenderer_t *or);
static void native_CreateRenderer(oobject_t list, oint32_t ac);
static void native_ChangeRenderer(oobject_t list, oint32_t ac);
static void native_GetRendererWindow(oobject_t list, oint32_t ac);
static void native_RenderClear(oobject_t list, oint32_t ac);
static void native_RenderDrawPoint(oobject_t list, oint32_t ac);
static void native_RenderDrawPoints(oobject_t list, oint32_t ac);
static void native_RenderDrawLine(oobject_t list, oint32_t ac);
static void native_RenderDrawLines(oobject_t list, oint32_t ac);
static void native_RenderDrawRect(oobject_t list, oint32_t ac);
static void native_RenderDrawRects(oobject_t list, oint32_t ac);
static void native_RenderFillRect(oobject_t list, oint32_t ac);
static void native_RenderFillRects(oobject_t list, oint32_t ac);
static void native_RenderCopy(oobject_t list, oint32_t ac);
static void native_RenderCopyEx(oobject_t list, oint32_t ac);
static void native_RenderPresent(oobject_t list, oint32_t ac);
static void native_DestroyRenderer(oobject_t list, oint32_t ac);
static void native_DestroyRenderer(oobject_t list, oint32_t ac);
static void query_surface(osurface_t *surface);
static void native_LoadSurface(oobject_t list, oint32_t ac);
static void native_CreateRGBSurface(oobject_t list, oint32_t ac);
static void native_CreateRGBASurface(oobject_t list, oint32_t ac);
static void native_ConvertSurface(oobject_t list, oint32_t ac);
static void native_ChangeSurface(oobject_t list, oint32_t ac);
static void native_PullSurface(oobject_t list, oint32_t ac);
static void native_PushSurface(oobject_t list, oint32_t ac);
static void native_BlitSurface(oobject_t list, oint32_t ac);
static void native_ScaleSurface(oobject_t list, oint32_t ac);
static void native_FreeSurface(oobject_t list, oint32_t ac);
static void query_texture(otexture_t *ot);
static void handle_texture(orenderer_t *ren, otexture_t *tex);
static void native_CreateTexture(oobject_t list, oint32_t ac);
static void native_CreateTextureFromSurface(oobject_t list, oint32_t ac);
static void native_LoadTexture(oobject_t list, oint32_t ac);
static void native_ChangeTexture(oobject_t list, oint32_t ac);
static void native_DestroyTexture(oobject_t list, oint32_t ac);
static inline void translate_window(oevent_t *ev, ouint32_t id);
static void translate_event(oevent_t *ev);
static void native_PollEvent(oobject_t list, oint32_t ac);
static void native_WaitEvent(oobject_t list, oint32_t ac);
static void native_GetModState(oobject_t list, oint32_t ac);
static Uint32 timer_callback(Uint32 ms, void *data);
static void pop_timer(oobject_t *pointer);
static void push_timer(otimer_t *timer);
static void native_AddTimer(oobject_t list, oint32_t ac);
static void native_GetTicks(oobject_t list, oint32_t ac);
static void native_Delay(oobject_t list, oint32_t ac);
static void native_RemoveTimer(oobject_t list, oint32_t ac);
static void native_ShowCursor(oobject_t list, oint32_t ac);
/* ttf */
static void query_font(ofont_t *of);
static void native_OpenFont(oobject_t list, oint32_t ac);
static void native_ChangeFont(oobject_t list, oint32_t ac);
static void native_GlyphIsProvided(oobject_t list, oint32_t ac);
static void native_GlyphMetrics(oobject_t list, oint32_t ac);
static void native_SizeText(oobject_t list, oint32_t ac);
static void native_SizeUTF8(oobject_t list, oint32_t ac);
static void native_SizeUNICODE(oobject_t list, oint32_t ac);
static void native_RenderText_Solid(oobject_t list, oint32_t ac);
static void native_RenderUTF8_Solid(oobject_t list, oint32_t ac);
static void native_RenderUNICODE_Solid(oobject_t list, oint32_t ac);
static void native_RenderGlyph_Solid(oobject_t list, oint32_t ac);
static void native_RenderText_Shaded(oobject_t list, oint32_t ac);
static void native_RenderUTF8_Shaded(oobject_t list, oint32_t ac);
static void native_RenderUNICODE_Shaded(oobject_t list, oint32_t ac);
static void native_RenderGlyph_Shaded(oobject_t list, oint32_t ac);
static void native_RenderText_Blended(oobject_t list, oint32_t ac);
static void native_RenderUTF8_Blended(oobject_t list, oint32_t ac);
static void native_RenderUNICODE_Blended(oobject_t list, oint32_t ac);
static void native_RenderGlyph_Blended(oobject_t list, oint32_t ac);
static void native_RenderText_Blended_Wrapped(oobject_t list, oint32_t ac);
static void native_RenderUTF8_Blended_Wrapped(oobject_t list, oint32_t ac);
static void native_RenderUNICODE_Blended_Wrapped(oobject_t list, oint32_t ac);
static void native_GetFontKerningSize(oobject_t list, oint32_t ac);
static void native_CloseFont(oobject_t list, oint32_t ac);
/* mix */
static void native_OpenAudio(oobject_t list, oint32_t ac);
static void native_AllocateChannels(oobject_t list, oint32_t ac);
static void native_LoadChunk(oobject_t list, oint32_t ac);
static void channel_callback(int channel);
static void native_PlayChannel(oobject_t list, oint32_t ac);
static void native_FadeInChannel(oobject_t list, oint32_t ac);
static void native_VolumeChunk(oobject_t list, oint32_t ac);
static void native_SetPanning(oobject_t list, oint32_t ac);
static void native_SetPosition(oobject_t list, oint32_t ac);
static void native_SetDistance(oobject_t list, oint32_t ac);
static void native_SetReverseStereo(oobject_t list, oint32_t ac);
static void native_FadeOutChannel(oobject_t list, oint32_t ac);
static void native_Playing(oobject_t list, oint32_t ac);
static void native_FadingChannel(oobject_t list, oint32_t ac);
static void native_ExpireChannel(oobject_t list, oint32_t ac);
static void native_Pause(oobject_t list, oint32_t ac);
static void native_Resume(oobject_t list, oint32_t ac);
static void native_Paused(oobject_t list, oint32_t ac);
static void native_HaltChannel(oobject_t list, oint32_t ac);
static void native_FreeChunk(oobject_t list, oint32_t ac);
static void native_GroupChannel(oobject_t list, oint32_t ac);
static void native_GroupChannels(oobject_t list, oint32_t ac);
static void native_GroupAvailable(oobject_t list, oint32_t ac);
static void native_GroupCount(oobject_t list, oint32_t ac);
static void native_GroupOldest(oobject_t list, oint32_t ac);
static void native_GroupNewer(oobject_t list, oint32_t ac);
static void native_FadeOutGroup(oobject_t list, oint32_t ac);
static void native_HaltGroup(oobject_t list, oint32_t ac);
static void native_LoadMusic(oobject_t list, oint32_t ac);
static void music_callback(void);
static void native_PlayMusic(oobject_t list, oint32_t ac);
static void native_FadeInMusic(oobject_t list, oint32_t ac);
static void native_VolumeMusic(oobject_t list, oint32_t ac);
static void native_PlayingMusic(oobject_t list, oint32_t ac);
static void native_FadeOutMusic(oobject_t list, oint32_t ac);
static void native_FadingMusic(oobject_t list, oint32_t ac);
static void native_PauseMusic(oobject_t list, oint32_t ac);
static void native_ResumeMusic(oobject_t list, oint32_t ac);
static void native_RewindMusic(oobject_t list, oint32_t ac);
static void native_PausedMusic(oobject_t list, oint32_t ac);
static void native_SetMusicPosition(oobject_t list, oint32_t ac);
static void native_HaltMusic(oobject_t list, oint32_t ac);
static void native_FreeMusic(oobject_t list, oint32_t ac);
static void native_CloseAudio(oobject_t list, oint32_t ac);
/* net */
static void native_ResolveHost(oobject_t list, oint32_t ac);
static void native_ResolveIP(oobject_t list, oint32_t ac);
static void native_GetLocalAddresses(oobject_t list, oint32_t ac);
static void native_AllocSocketSet(oobject_t list, oint32_t ac);
static void native_AddSocket(oobject_t list, oint32_t ac);
static void native_DelSocket(oobject_t list, oint32_t ac);
static void native_CheckSockets(oobject_t list, oint32_t ac);
static void native_SocketReady(oobject_t list, oint32_t ac);
static void native_FreeSocketSet(oobject_t list, oint32_t ac);
/* net.tcp */
static void native_tcp_Open(oobject_t list, oint32_t ac);
static void native_tcp_Accept(oobject_t list, oint32_t ac);
static void native_tcp_GetPeerAddress(oobject_t list, oint32_t ac);
static void native_tcp_Send(oobject_t list, oint32_t ac);
static void native_tcp_Recv(oobject_t list, oint32_t ac);
static void native_tcp_Close(oobject_t list, oint32_t ac);
static void native_tcp_AddSocket(oobject_t list, oint32_t ac);
static void native_tcp_DelSocket(oobject_t list, oint32_t ac);
/* net.udp */
static void native_udp_Open(oobject_t list, oint32_t ac);
static void native_udp_SetPacketLoss(oobject_t list, oint32_t ac);
static void native_udp_Bind(oobject_t list, oint32_t ac);
static void native_udp_Unbind(oobject_t list, oint32_t ac);
static void native_udp_GetPeerAddress(oobject_t list, oint32_t ac);
static void native_udp_Send(oobject_t list, oint32_t ac);
static void native_udp_Recv(oobject_t list, oint32_t ac);
static void native_udp_Close(oobject_t list, oint32_t ac);
static void native_udp_AddSocket(oobject_t list, oint32_t ac);
static void native_udp_DelSocket(oobject_t list, oint32_t ac);
/* sdl.gl */
static void native_ResetAttributes(oobject_t list, oint32_t ac);
static void native_SetAttribute(oobject_t list, oint32_t ac);
static void native_GetAttribute(oobject_t list, oint32_t ac);
static void native_CreateContext(oobject_t list, oint32_t ac);
static void native_MakeCurrent(oobject_t list, oint32_t ac);
static void native_GetCurrentWindow(oobject_t list, oint32_t ac);
static void native_GetCurrentContext(oobject_t list, oint32_t ac);
static void native_SetSwapInterval(oobject_t list, oint32_t ac);
static void native_GetSwapInterval(oobject_t list, oint32_t ac);
static void native_SwapWindow(oobject_t list, oint32_t ac);
static void native_DeleteContext(oobject_t list, oint32_t ac);
static void query_format(SDL_Surface*, uint32_t*, uint32_t*);
static void native_ReadPixels(oobject_t list, oint32_t ac);
static void native_DrawPixels(oobject_t list, oint32_t ac);
static void native_TexImage1D(oobject_t list, oint32_t ac);
static void native_TexImage2D(oobject_t list, oint32_t ac);
static void native_TexSubImage1D(oobject_t list, oint32_t ac);
static void native_TexSubImage2D(oobject_t list, oint32_t ac);

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
orecord_t	*ttf_record;
orecord_t	*mix_record;
orecord_t	*net_record;
orecord_t	*net_tcp_record;
orecord_t	*net_udp_record;
orecord_t	*sdl_gl_record;

static struct {
    char	*name;
    int		 value;
} sdl[] = {
    /* event_t.state */
    { "PRESSED",			SDL_PRESSED },
    { "RELEASED",			SDL_RELEASED },
    /* event_t.button */
    { "BUTTON_LEFT",			SDL_BUTTON_LEFT },
    { "BUTTON_MIDDLE",			SDL_BUTTON_MIDDLE },
    { "BUTTON_RIGHT",			SDL_BUTTON_RIGHT },
    { "BUTTON_X1",			SDL_BUTTON_X1 },
    { "BUTTON_X2",			SDL_BUTTON_X2 },
    /* event_t.state (for EventMouse*) */
    { "BUTTON_LMASK",			SDL_BUTTON_LMASK },
    { "BUTTON_MMASK",			SDL_BUTTON_MMASK },
    { "BUTTON_RMASK",			SDL_BUTTON_RMASK },
    { "BUTTON_X1MASK",			SDL_BUTTON_X1MASK },
    { "BUTTON_X2MASK",			SDL_BUTTON_X2MASK },
    /* event_t.event */
    { "WINDOWEVENT_SHOWN",		SDL_WINDOWEVENT_SHOWN },
    { "WINDOWEVENT_HIDDEN",		SDL_WINDOWEVENT_HIDDEN },
    { "WINDOWEVENT_EXPOSED",		SDL_WINDOWEVENT_EXPOSED },
    { "WINDOWEVENT_MOVED",		SDL_WINDOWEVENT_MOVED },
    { "WINDOWEVENT_RESIZED",		SDL_WINDOWEVENT_RESIZED },
    { "WINDOWEVENT_SIZE_CHANGED",	SDL_WINDOWEVENT_SIZE_CHANGED },
    { "WINDOWEVENT_MINIMIZED",		SDL_WINDOWEVENT_MINIMIZED },
    { "WINDOWEVENT_MAXIMIZED",		SDL_WINDOWEVENT_MAXIMIZED },
    { "WINDOWEVENT_RESTORED",		SDL_WINDOWEVENT_RESTORED },
    { "WINDOWEVENT_ENTER",		SDL_WINDOWEVENT_ENTER },
    { "WINDOWEVENT_LEAVE",		SDL_WINDOWEVENT_LEAVE },
    { "WINDOWEVENT_FOCUS_GAINED",	SDL_WINDOWEVENT_FOCUS_GAINED },
    { "WINDOWEVENT_FOCUS_LOST",		SDL_WINDOWEVENT_FOCUS_LOST },
    { "WINDOWEVENT_CLOSE",		SDL_WINDOWEVENT_CLOSE },
    /* event_t.hat */
    { "HAT_CENTERED",			SDL_HAT_CENTERED },
    { "HAT_UP",				SDL_HAT_UP },
    { "HAT_RIGHT",			SDL_HAT_RIGHT },
    { "HAT_DOWN",			SDL_HAT_DOWN },
    { "HAT_LEFT",			SDL_HAT_LEFT },
    { "HAT_RIGHTUP",			SDL_HAT_RIGHTUP },
    { "HAT_RIGHTDOWN",			SDL_HAT_RIGHTDOWN },
    { "HAT_LEFTUP",			SDL_HAT_LEFTUP },
    { "HAT_LEFTDOWN",			SDL_HAT_LEFTDOWN },
    /* event_t.keysym*/
    { "K_RETURN",			SDLK_RETURN },
    { "K_ESCAPE",			SDLK_ESCAPE },
    { "K_BACKSPACE",			SDLK_BACKSPACE },
    { "K_TAB",				SDLK_TAB },
    { "K_SPACE",			SDLK_SPACE },
    { "K_EXCLAIM",			SDLK_EXCLAIM },
    { "K_QUOTEDBL",			SDLK_QUOTEDBL },
    { "K_HASH",				SDLK_HASH },
    { "K_PERCENT",			SDLK_PERCENT },
    { "K_DOLLAR",			SDLK_DOLLAR },
    { "K_AMPERSAND",			SDLK_AMPERSAND },
    { "K_QUOTE",			SDLK_QUOTE },
    { "K_LEFTPAREN",			SDLK_LEFTPAREN },
    { "K_RIGHTPAREN",			SDLK_RIGHTPAREN },
    { "K_ASTERISK",			SDLK_ASTERISK },
    { "K_PLUS",				SDLK_PLUS },
    { "K_COMMA",			SDLK_COMMA },
    { "K_MINUS",			SDLK_MINUS },
    { "K_PERIOD",			SDLK_PERIOD },
    { "K_SLASH",			SDLK_SLASH },
    { "K_0",				SDLK_0 },
    { "K_1",				SDLK_1 },
    { "K_2",				SDLK_2 },
    { "K_3",				SDLK_3 },
    { "K_4",				SDLK_4 },
    { "K_5",				SDLK_5 },
    { "K_6",				SDLK_6 },
    { "K_7",				SDLK_7 },
    { "K_8",				SDLK_8 },
    { "K_9",				SDLK_9 },
    { "K_COLON",			SDLK_COLON },
    { "K_SEMICOLON",			SDLK_SEMICOLON },
    { "K_LESS",				SDLK_LESS },
    { "K_EQUALS",			SDLK_EQUALS },
    { "K_GREATER",			SDLK_GREATER },
    { "K_QUESTION",			SDLK_QUESTION },
    { "K_AT",				SDLK_AT },
    { "K_LEFTBRACKET",			SDLK_LEFTBRACKET },
    { "K_BACKSLASH",			SDLK_BACKSLASH },
    { "K_RIGHTBRACKET",			SDLK_RIGHTBRACKET },
    { "K_CARET",			SDLK_CARET },
    { "K_UNDERSCORE",			SDLK_UNDERSCORE },
    { "K_BACKQUOTE",			SDLK_BACKQUOTE },
    { "K_a",				SDLK_a },
    { "K_b",				SDLK_b },
    { "K_c",				SDLK_c },
    { "K_d",				SDLK_d },
    { "K_e",				SDLK_e },
    { "K_f",				SDLK_f },
    { "K_g",				SDLK_g },
    { "K_h",				SDLK_h },
    { "K_i",				SDLK_i },
    { "K_j",				SDLK_j },
    { "K_k",				SDLK_k },
    { "K_l",				SDLK_l },
    { "K_m",				SDLK_m },
    { "K_n",				SDLK_n },
    { "K_o",				SDLK_o },
    { "K_p",				SDLK_p },
    { "K_q",				SDLK_q },
    { "K_r",				SDLK_r },
    { "K_s",				SDLK_s },
    { "K_t",				SDLK_t },
    { "K_u",				SDLK_u },
    { "K_v",				SDLK_v },
    { "K_w",				SDLK_w },
    { "K_x",				SDLK_x },
    { "K_y",				SDLK_y },
    { "K_z",				SDLK_z },
    { "K_CAPSLOCK",			SDLK_CAPSLOCK },
    { "K_F1",				SDLK_F1 },
    { "K_F2",				SDLK_F2 },
    { "K_F3",				SDLK_F3 },
    { "K_F4",				SDLK_F4 },
    { "K_F5",				SDLK_F5 },
    { "K_F6",				SDLK_F6 },
    { "K_F7",				SDLK_F7 },
    { "K_F8",				SDLK_F8 },
    { "K_F9",				SDLK_F9 },
    { "K_F10",				SDLK_F10 },
    { "K_F11",				SDLK_F11 },
    { "K_F12",				SDLK_F12 },
    { "K_PRINTSCREEN",			SDLK_PRINTSCREEN },
    { "K_SCROLLLOCK",			SDLK_SCROLLLOCK },
    { "K_PAUSE",			SDLK_PAUSE },
    { "K_INSERT",			SDLK_INSERT },
    { "K_HOME",				SDLK_HOME },
    { "K_PAGEUP",			SDLK_PAGEUP },
    { "K_DELETE",			SDLK_DELETE },
    { "K_END",				SDLK_END },
    { "K_PAGEDOWN",			SDLK_PAGEDOWN },
    { "K_RIGHT",			SDLK_RIGHT },
    { "K_LEFT",				SDLK_LEFT },
    { "K_DOWN",				SDLK_DOWN },
    { "K_UP",				SDLK_UP },
    { "K_NUMLOCKCLEAR",			SDLK_NUMLOCKCLEAR },
    { "K_KP_DIVIDE",			SDLK_KP_DIVIDE },
    { "K_KP_MULTIPLY",			SDLK_KP_MULTIPLY },
    { "K_KP_MINUS",			SDLK_KP_MINUS },
    { "K_KP_PLUS",			SDLK_KP_PLUS },
    { "K_KP_ENTER",			SDLK_KP_ENTER },
    { "K_KP_1",				SDLK_KP_1 },
    { "K_KP_2",				SDLK_KP_2 },
    { "K_KP_3",				SDLK_KP_3 },
    { "K_KP_4",				SDLK_KP_4 },
    { "K_KP_5",				SDLK_KP_5 },
    { "K_KP_6",				SDLK_KP_6 },
    { "K_KP_7",				SDLK_KP_7 },
    { "K_KP_8",				SDLK_KP_8 },
    { "K_KP_9",				SDLK_KP_9 },
    { "K_KP_0",				SDLK_KP_0 },
    { "K_KP_PERIOD",			SDLK_KP_PERIOD },
    { "K_APPLICATION",			SDLK_APPLICATION },
    { "K_POWER",			SDLK_POWER },
    { "K_KP_EQUALS",			SDLK_KP_EQUALS },
    { "K_F13",				SDLK_F13 },
    { "K_F14",				SDLK_F14 },
    { "K_F15",				SDLK_F15 },
    { "K_F16",				SDLK_F16 },
    { "K_F17",				SDLK_F17 },
    { "K_F18",				SDLK_F18 },
    { "K_F19",				SDLK_F19 },
    { "K_F20",				SDLK_F20 },
    { "K_F21",				SDLK_F21 },
    { "K_F22",				SDLK_F22 },
    { "K_F23",				SDLK_F23 },
    { "K_F24",				SDLK_F24 },
    { "K_EXECUTE",			SDLK_EXECUTE },
    { "K_HELP",				SDLK_HELP },
    { "K_MENU",				SDLK_MENU },
    { "K_SELECT",			SDLK_SELECT },
    { "K_STOP",				SDLK_STOP },
    { "K_AGAIN",			SDLK_AGAIN },
    { "K_UNDO",				SDLK_UNDO },
    { "K_CUT",				SDLK_CUT },
    { "K_COPY",				SDLK_COPY },
    { "K_PASTE",			SDLK_PASTE },
    { "K_FIND",				SDLK_FIND },
    { "K_MUTE",				SDLK_MUTE },
    { "K_VOLUMEUP",			SDLK_VOLUMEUP },
    { "K_VOLUMEDOWN",			SDLK_VOLUMEDOWN },
    { "K_KP_COMMA",			SDLK_KP_COMMA },
    { "K_KP_EQUALSAS400",		SDLK_KP_EQUALSAS400 },
    { "K_ALTERASE",			SDLK_ALTERASE },
    { "K_SYSREQ",			SDLK_SYSREQ },
    { "K_CANCEL",			SDLK_CANCEL },
    { "K_CLEAR",			SDLK_CLEAR },
    { "K_PRIOR",			SDLK_PRIOR },
    { "K_RETURN2",			SDLK_RETURN2 },
    { "K_SEPARATOR",			SDLK_SEPARATOR },
    { "K_OUT",				SDLK_OUT },
    { "K_OPER",				SDLK_OPER },
    { "K_CLEARAGAIN",			SDLK_CLEARAGAIN },
    { "K_CRSEL",			SDLK_CRSEL },
    { "K_EXSEL",			SDLK_EXSEL },
    { "K_KP_00",			SDLK_KP_00 },
    { "K_KP_000",			SDLK_KP_000 },
    { "K_THOUSANDSSEPARATOR",		SDLK_THOUSANDSSEPARATOR },
    { "K_DECIMALSEPARATOR",		SDLK_DECIMALSEPARATOR },
    { "K_CURRENCYUNIT",			SDLK_CURRENCYUNIT },
    { "K_CURRENCYSUBUNIT",		SDLK_CURRENCYSUBUNIT },
    { "K_KP_LEFTPAREN",			SDLK_KP_LEFTPAREN },
    { "K_KP_RIGHTPAREN",		SDLK_KP_RIGHTPAREN },
    { "K_KP_LEFTBRACE",			SDLK_KP_LEFTBRACE },
    { "K_KP_RIGHTBRACE",		SDLK_KP_RIGHTBRACE },
    { "K_KP_TAB",			SDLK_KP_TAB },
    { "K_KP_BACKSPACE",			SDLK_KP_BACKSPACE },
    { "K_KP_A",				SDLK_KP_A },
    { "K_KP_B",				SDLK_KP_B },
    { "K_KP_C",				SDLK_KP_C },
    { "K_KP_D",				SDLK_KP_D },
    { "K_KP_E",				SDLK_KP_E },
    { "K_KP_F",				SDLK_KP_F },
    { "K_KP_XOR",			SDLK_KP_XOR },
    { "K_KP_POWER",			SDLK_KP_POWER },
    { "K_KP_PERCENT",			SDLK_KP_PERCENT },
    { "K_KP_LESS",			SDLK_KP_LESS },
    { "K_KP_GREATER",			SDLK_KP_GREATER },
    { "K_KP_AMPERSAND",			SDLK_KP_AMPERSAND },
    { "K_KP_DBLAMPERSAND",		SDLK_KP_DBLAMPERSAND },
    { "K_KP_VERTICALBAR",		SDLK_KP_VERTICALBAR },
    { "K_KP_DBLVERTICALBAR",		SDLK_KP_DBLVERTICALBAR },
    { "K_KP_COLON",			SDLK_KP_COLON },
    { "K_KP_HASH",			SDLK_KP_HASH },
    { "K_KP_SPACE",			SDLK_KP_SPACE },
    { "K_KP_AT",			SDLK_KP_AT },
    { "K_KP_EXCLAM",			SDLK_KP_EXCLAM },
    { "K_KP_MEMSTORE",			SDLK_KP_MEMSTORE },
    { "K_KP_MEMRECALL",			SDLK_KP_MEMRECALL },
    { "K_KP_MEMCLEAR",			SDLK_KP_MEMCLEAR },
    { "K_KP_MEMADD",			SDLK_KP_MEMADD },
    { "K_KP_MEMSUBTRACT",		SDLK_KP_MEMSUBTRACT },
    { "K_KP_MEMMULTIPLY",		SDLK_KP_MEMMULTIPLY },
    { "K_KP_MEMDIVIDE",			SDLK_KP_MEMDIVIDE },
    { "K_KP_PLUSMINUS",			SDLK_KP_PLUSMINUS },
    { "K_KP_CLEAR",			SDLK_KP_CLEAR },
    { "K_KP_CLEARENTRY",		SDLK_KP_CLEARENTRY },
    { "K_KP_BINARY",			SDLK_KP_BINARY },
    { "K_KP_OCTAL",			SDLK_KP_OCTAL },
    { "K_KP_DECIMAL",			SDLK_KP_DECIMAL },
    { "K_KP_HEXADECIMAL",		SDLK_KP_HEXADECIMAL },
    { "K_LCTRL",			SDLK_LCTRL },
    { "K_LSHIFT",			SDLK_LSHIFT },
    { "K_LALT",				SDLK_LALT },
    { "K_LGUI",				SDLK_LGUI },
    { "K_RCTRL",			SDLK_RCTRL },
    { "K_RSHIFT",			SDLK_RSHIFT },
    { "K_RALT",				SDLK_RALT },
    { "K_RGUI",				SDLK_RGUI },
    { "K_MODE",				SDLK_MODE },
    { "K_AUDIONEXT",			SDLK_AUDIONEXT },
    { "K_AUDIOPREV",			SDLK_AUDIOPREV },
    { "K_AUDIOSTOP",			SDLK_AUDIOSTOP },
    { "K_AUDIOPLAY",			SDLK_AUDIOPLAY },
    { "K_AUDIOMUTE",			SDLK_AUDIOMUTE },
    { "K_MEDIASELECT",			SDLK_MEDIASELECT },
    { "K_WWW",				SDLK_WWW },
    { "K_MAIL",				SDLK_MAIL },
    { "K_CALCULATOR",			SDLK_CALCULATOR },
    { "K_COMPUTER",			SDLK_COMPUTER },
    { "K_AC_SEARCH",			SDLK_AC_SEARCH },
    { "K_AC_HOME",			SDLK_AC_HOME },
    { "K_AC_BACK",			SDLK_AC_BACK },
    { "K_AC_FORWARD",			SDLK_AC_FORWARD },
    { "K_AC_STOP",			SDLK_AC_STOP },
    { "K_AC_REFRESH",			SDLK_AC_REFRESH },
    { "K_AC_BOOKMARKS",			SDLK_AC_BOOKMARKS },
    { "K_BRIGHTNESSDOWN",		SDLK_BRIGHTNESSDOWN },
    { "K_BRIGHTNESSUP",			SDLK_BRIGHTNESSUP },
    { "K_DISPLAYSWITCH",		SDLK_DISPLAYSWITCH },
    { "K_KBDILLUMTOGGLE",		SDLK_KBDILLUMTOGGLE },
    { "K_KBDILLUMDOWN",			SDLK_KBDILLUMDOWN },
    { "K_KBDILLUMUP",			SDLK_KBDILLUMUP },
    { "K_EJECT",			SDLK_EJECT },
    { "K_SLEEP",			SDLK_SLEEP },
    /* event_t.type */
    { "QUIT",				SDL_QUIT },
    { "WINDOWEVENT",			SDL_WINDOWEVENT },
    { "KEYDOWN",			SDL_KEYDOWN },
    { "KEYUP",				SDL_KEYUP },
    { "TEXTEDITING",			SDL_TEXTEDITING },
    { "TEXTINPUT",			SDL_TEXTINPUT },
    { "MOUSEMOTION",			SDL_MOUSEMOTION },
    { "MOUSEBUTTONDOWN",		SDL_MOUSEBUTTONDOWN },
    { "MOUSEBUTTONUP",			SDL_MOUSEBUTTONUP },
    { "MOUSEWHEEL",			SDL_MOUSEWHEEL },
    { "JOYAXISMOTION",			SDL_JOYAXISMOTION },
    { "JOYBALLMOTION",			SDL_JOYBALLMOTION },
    { "JOYHATMOTION",			SDL_JOYHATMOTION },
    { "JOYBUTTONDOWN",			SDL_JOYBUTTONDOWN },
    { "JOYBUTTONUP",			SDL_JOYBUTTONUP },
    { "JOYDEVICEADDED",			SDL_JOYDEVICEADDED },
    { "JOYDEVICEREMOVED",		SDL_JOYDEVICEREMOVED },
    { "CONTROLLERAXISMOTION",		SDL_CONTROLLERAXISMOTION },
    { "CONTROLLERBUTTONDOWN",		SDL_CONTROLLERBUTTONDOWN },
    { "CONTROLLERBUTTONUP",		SDL_CONTROLLERBUTTONUP },
    { "CONTROLLERDEVICEADDED",		SDL_CONTROLLERDEVICEADDED },
    { "CONTROLLERDEVICEREMOVED",	SDL_CONTROLLERDEVICEREMOVED },
    { "CONTROLLERDEVICEREMAPPED",	SDL_CONTROLLERDEVICEREMAPPED },
    { "FINGERDOWN",			SDL_FINGERDOWN },
    { "FINGERUP",			SDL_FINGERUP },
    { "FINGERMOTION",			SDL_FINGERMOTION },
    { "DOLLARGESTURE",			SDL_DOLLARGESTURE },
    { "DOLLARRECORD",			SDL_DOLLARRECORD },
    { "MULTIGESTURE",			SDL_MULTIGESTURE },
    { "TIMEREVENT",			SDL_USEREVENT },
    { "MUSICFINISHEDEVENT",		SDL_USEREVENT + 1 },
    { "CHANNELFINISHEDEVENT",		SDL_USEREVENT + 2 },
    /* create_window */
    { "WINDOW_FULLSCREEN",		SDL_WINDOW_FULLSCREEN },
    { "WINDOW_OPENGL",			SDL_WINDOW_OPENGL },
    { "WINDOW_SHOWN",			SDL_WINDOW_SHOWN },
    { "WINDOW_HIDDEN",			SDL_WINDOW_HIDDEN },
    { "WINDOW_BORDERLESS",		SDL_WINDOW_BORDERLESS },
    { "WINDOW_RESIZABLE",		SDL_WINDOW_RESIZABLE },
    { "WINDOW_MINIMIZED",		SDL_WINDOW_MINIMIZED },
    { "WINDOW_MAXIMIZED",		SDL_WINDOW_MAXIMIZED },
    { "WINDOW_INPUT_GRABBED",		SDL_WINDOW_INPUT_GRABBED },
    { "WINDOW_INPUT_FOCUS",		SDL_WINDOW_INPUT_FOCUS },
    { "WINDOW_MOUSE_FOCUS",		SDL_WINDOW_MOUSE_FOCUS },
    { "WINDOW_FULLSCREEN_DESKTOP",	SDL_WINDOW_FULLSCREEN_DESKTOP },
    { "WINDOW_FOREIGN",			SDL_WINDOW_FOREIGN },
    { "WINDOW_ALLOW_HIGHDPI",		SDL_WINDOW_ALLOW_HIGHDPI },
    { "WINDOWPOS_UNDEFINED",		SDL_WINDOWPOS_UNDEFINED },
    { "WINDOWPOS_CENTERED",		SDL_WINDOWPOS_CENTERED },
    /* create_renderer, renderer_t.flags */
    { "RENDERER_SOFTWARE",		SDL_RENDERER_SOFTWARE },
    { "RENDERER_ACCELERATED",		SDL_RENDERER_ACCELERATED },
    { "RENDERER_PRESENTVSYNC",		SDL_RENDERER_PRESENTVSYNC },
    { "RENDERER_TARGETTEXTURE",		SDL_RENDERER_TARGETTEXTURE },
    /* texture_t.access */
    { "TEXTUREACCESS_STATIC",		SDL_TEXTUREACCESS_STATIC },
    { "TEXTUREACCESS_STREAMING",	SDL_TEXTUREACCESS_STREAMING },
    { "TEXTUREACCESS_TARGET",		SDL_TEXTUREACCESS_TARGET },
    /* surface_t.blend,texture_t.blend */
    { "BLENDMODE_NONE",			SDL_BLENDMODE_NONE },
    { "BLENDMODE_BLEND",		SDL_BLENDMODE_BLEND },
    { "BLENDMODE_ADD",			SDL_BLENDMODE_ADD },
    { "BLENDMODE_MOD",			SDL_BLENDMODE_MOD },
    /* render_copy_ex */
    { "FLIP_NONE",			SDL_FLIP_NONE },
    { "FLIP_HORIZONTAL",		SDL_FLIP_HORIZONTAL },
    { "FLIP_VERTICAL",			SDL_FLIP_VERTICAL },
    /* SDL_KeyMod */
    { "KMOD_NONE",			KMOD_NONE },
    { "KMOD_LSHIFT",			KMOD_LSHIFT },
    { "KMOD_RSHIFT",			KMOD_RSHIFT },
    { "KMOD_LCTRL",			KMOD_LCTRL },
    { "KMOD_RCTRL",			KMOD_RCTRL },
    { "KMOD_LALT",			KMOD_LALT },
    { "KMOD_RALT",			KMOD_RALT },
    { "KMOD_LGUI",			KMOD_LGUI },
    { "KMOD_RGUI",			KMOD_RGUI },
    { "KMOD_NUM",			KMOD_NUM },
    { "KMOD_CAPS",			KMOD_CAPS },
    { "KMOD_MODE",			KMOD_MODE },
    { "KMOD_CTRL",			KMOD_CTRL },
    { "KMOD_SHIFT",			KMOD_SHIFT },
    { "KMOD_ALT",			KMOD_ALT },
    { "KMOD_GUI",			KMOD_GUI },
    /* Pixel format */
    { "PIXELFORMAT_BITMAP",		SDL_PIXELFORMAT_INDEX1LSB },
    { "PIXELFORMAT_RGB",		SDL_PIXELFORMAT_RGB24 },
    { "PIXELFORMAT_RGBA",		SDL_PIXELFORMAT_RGBA8888 },
};
static struct {
    char	*name;
    int		 value;
} ttf[] = {
    /* font_t.style */
    { "STYLE_NORMAL",			TTF_STYLE_NORMAL },
    { "STYLE_BOLD",			TTF_STYLE_BOLD },
    { "STYLE_ITALIC",			TTF_STYLE_ITALIC },
    { "STYLE_UNDERLINE",		TTF_STYLE_UNDERLINE },
    { "STYLE_STRIKETHROUGH",		TTF_STYLE_STRIKETHROUGH },
    /* font_t.hinting */
    { "HINTING_NORMAL",			TTF_HINTING_NORMAL },
    { "HINTING_LIGHT",			TTF_HINTING_LIGHT },
    { "HINTING_MONO",			TTF_HINTING_MONO },
    { "HINTING_NONE",			TTF_HINTING_NONE },
};
static struct {
    char	*name;
    int		 value;
} mix[] = {
    /* open_audio flags */
    { "DEFAULT_FREQUENCY",		MIX_DEFAULT_FREQUENCY },
    { "DEFAULT_FORMAT",			MIX_DEFAULT_FORMAT },
    { "DEFAULT_CHANNELS",		MIX_DEFAULT_CHANNELS },
    /* music_t.type */
    { "MUS_NONE",			MUS_NONE },
    { "MUS_CMD",			MUS_CMD },
    { "MUS_WAV",			MUS_WAV },
    { "MUS_MOD",			MUS_MOD },
    { "MUS_MID",			MUS_MID },
    { "MUS_OGG",			MUS_OGG },
    { "MUS_MP3",			MUS_MP3 },
    { "MUS_MP3_MAD",			MUS_MP3_MAD },
    { "MUS_FLAC",			MUS_FLAC },
    { "MUS_MODPLUG",			MUS_MODPLUG },
    /* Special channel effects identifier */
    { "CHANNEL_POST",			MIX_CHANNEL_POST },
    /* Mix_Fadding */
    { "NO_FADING",			MIX_NO_FADING },
    { "FADING_OUT",			MIX_FADING_OUT },
    { "FADING_IN",			MIX_FADING_IN },
    /* Audio format */
    { "AUDIO_U8",			AUDIO_U8 },
    { "AUDIO_S8",			AUDIO_S8 },
    { "AUDIO_U16LSB",			AUDIO_U16LSB },
    { "AUDIO_S16LSB",			AUDIO_S16LSB },
    { "AUDIO_U16MSB",			AUDIO_U16MSB },
    { "AUDIO_S16MSB",			AUDIO_S16MSB },
    { "AUDIO_U16",			AUDIO_U16 },
    { "AUDIO_S16",			AUDIO_S16 },
    { "AUDIO_S32LSB",			AUDIO_S32LSB },
    { "AUDIO_S32MSB",			AUDIO_S32MSB },
    { "AUDIO_S32",			AUDIO_S32 },
    { "AUDIO_F32LSB",			AUDIO_F32LSB },
    { "AUDIO_F32MSB",			AUDIO_F32MSB },
    { "AUDIO_F32",			AUDIO_F32 },
    { "AUDIO_U16SYS",			AUDIO_U16SYS },
    { "AUDIO_S16SYS",			AUDIO_S16SYS },
    { "AUDIO_S32SYS",			AUDIO_S32SYS },
    { "AUDIO_F32SYS",			AUDIO_F32SYS },
};
static struct {
    char	*name;
    int		 value;
} gl[] = {
    /* SDL_GLattr */
    { "RED_SIZE",			SDL_GL_RED_SIZE },
    { "GREEN_SIZE",			SDL_GL_GREEN_SIZE },
    { "BLUE_SIZE",			SDL_GL_BLUE_SIZE },
    { "ALPHA_SIZE",			SDL_GL_ALPHA_SIZE },
    { "BUFFER_SIZE",			SDL_GL_BUFFER_SIZE },
    { "DOUBLE_BUFFER",			SDL_GL_DOUBLEBUFFER },
    { "DEPTH_SIZE",			SDL_GL_DEPTH_SIZE },
    { "STENCIL_SIZE",			SDL_GL_STENCIL_SIZE },
    { "ACCUM_RED_SIZE",			SDL_GL_ACCUM_RED_SIZE },
    { "ACCUM_GREEN_SIZE",		SDL_GL_ACCUM_GREEN_SIZE },
    { "ACCUM_BLUE_SIZE",		SDL_GL_ACCUM_BLUE_SIZE },
    { "ACCUM_ALPHA_SIZE",		SDL_GL_ACCUM_ALPHA_SIZE },
    { "STEREO",				SDL_GL_STEREO },
    { "MULTISAMPLEBUFFERS",		SDL_GL_MULTISAMPLEBUFFERS },
    { "MULTISAMPLESAMPLES",		SDL_GL_MULTISAMPLESAMPLES },
    { "ACCELERATED_VISUAL",		SDL_GL_ACCELERATED_VISUAL },
    { "RETAINED_BACKING",		SDL_GL_RETAINED_BACKING },
    { "CONTEXT_MAJOR_VERSION",		SDL_GL_CONTEXT_MAJOR_VERSION },
    { "CONTEXT_MINOR_VERSION",		SDL_GL_CONTEXT_MINOR_VERSION },
    { "CONTEXT_EGL",			SDL_GL_CONTEXT_EGL },
    { "CONTEXT_FLAGS",			SDL_GL_CONTEXT_FLAGS },
    { "CONTEXT_PROFILE_MASK",		SDL_GL_CONTEXT_PROFILE_MASK },
    { "SHARE_WITH_CURRENT_CONTEXT",	SDL_GL_SHARE_WITH_CURRENT_CONTEXT },
    { "FRAMEBUFFER_SRGB_CAPABLE",	SDL_GL_FRAMEBUFFER_SRGB_CAPABLE },
    /* SDL_GLprofile */
    { "CONTEXT_PROFILE_CORE",		SDL_GL_CONTEXT_PROFILE_CORE },
    { "CONTEXT_PROFILE_COMPATIBILITY",	SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
    { "CONTEXT_PROFILE_ES",		SDL_GL_CONTEXT_PROFILE_ES },
    /* SDL_GLcontextFlag */
    { "CONTEXT_DEBUG_FLAG",		SDL_GL_CONTEXT_DEBUG_FLAG },
    { "CONTEXT_FORWARD_COMPATIBLE_FLAG",SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG },
    { "CONTEXT_ROBUST_ACCESS_FLAG",	SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG },
    { "CONTEXT_RESET_ISOLATION_FLAG",	SDL_GL_CONTEXT_RESET_ISOLATION_FLAG },
};
static ovector_t		*error_vector;
static ovector_t		*timer_vector;
static ovector_t		*address_vector;
static ohashtable_t		*window_table;
/* No need to create hash tables for queries */
static owindow_t		*current_window;
static ocontext_t		*current_context;
static obool_t			 sdl_active;

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
    orecord_t			*this_record;

    oadd_root((oobject_t *)&error_vector);
    oadd_root((oobject_t *)&timer_vector);
    oadd_root((oobject_t *)&address_vector);
    oadd_root((oobject_t *)&window_table);
    onew_hashtable(&window_table, 4);
    for (offset = 0; offset < osize(sdl); ++offset) {
	string = sdl[offset].name;
	onew_constant(sdl_record, oget_string((ouint8_t *)string,
					      strlen(string)),
		      sdl[offset].value);
    }
    for (offset = 0; offset < osize(ttf); ++offset) {
	string = ttf[offset].name;
	onew_constant(ttf_record, oget_string((ouint8_t *)string,
					      strlen(string)),
		      ttf[offset].value);
    }
    for (offset = 0; offset < osize(mix); ++offset) {
	string = mix[offset].name;
	onew_constant(mix_record, oget_string((ouint8_t *)string,
					      strlen(string)),
		      mix[offset].value);
    }
    for (offset = 0; offset < osize(gl); ++offset) {
	string = gl[offset].name;
	onew_constant(sdl_gl_record, oget_string((ouint8_t *)string,
						 strlen(string)),
		      gl[offset].value);
    }

#if __WORDSIZE == 32
#  define word_string		"int32_t"
#  define pointer_string	"uint32_t"
#else
#  define word_string		"int64_t"
#  define pointer_string	"uint64_t"
#endif

#define add_field(type, name)						\
    do {								\
	vector = oget_string((ouint8_t *)type, strlen(type));		\
	symbol = oget_identifier(vector);				\
	if (!symbol->base) {						\
	    symbol = oget_symbol(this_record, vector);			\
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
	    symbol = oget_symbol(this_record, vector);			\
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
	    symbol = oget_symbol(this_record, vector);			\
	    /*assert(symbol && symbol->type);*/				\
	}								\
	vector = oget_string((ouint8_t *)name, strlen(name));		\
	record->offset = record->length = offset;			\
	(void)onew_symbol(record, vector, symbol->tag);			\
    } while (0)

    /* sdl */
    this_record = sdl_record;
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

    record = type_vector->v.ptr[t_color];
    add_field("uint8_t",	"r");
    add_field("uint8_t",	"g");
    add_field("uint8_t",	"b");
    add_field("uint8_t",	"a");
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
    add_field("surface_t",	"icon");
    add_field("float32_t",	"brightness");
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
    add_field("surface_t",	"*icon*");
    add_field("float32_t",	"*brightness*");
    oend_record(record);

    record = type_vector->v.ptr[t_renderer];
    add_field(pointer_string,	"*renderer*");
    add_field("auto",		"*textures*");	/* gc it */
    add_field("auto",		"*window*");	/* gc it */
    add_field("texture_t",	"target");
    add_field("int32_t",	"log_w");
    add_field("int32_t",	"log_h");
    add_field("int32_t",	"max_w");
    add_field("int32_t",	"max_h");
    add_field("string_t",	"name");
    add_field("uint32_t",	"flags");
    add_vec_field("uint32_t",	"formats");
    add_field("int32_t",	"view_x");
    add_field("int32_t",	"view_y");
    add_field("int32_t",	"view_w");
    add_field("int32_t",	"view_h");
    add_field("int32_t",	"clip_x");
    add_field("int32_t",	"clip_y");
    add_field("int32_t",	"clip_w");
    add_field("int32_t",	"clip_h");
    add_field("float32_t",	"scale_x");
    add_field("float32_t",	"scale_y");
    add_field("uint8_t",	"r");
    add_field("uint8_t",	"g");
    add_field("uint8_t",	"b");
    add_field("uint8_t",	"a");
    add_field("uint8_t",	"blend");
    add_field("texture_t",	"*target*");
    add_field("int32_t",	"*log_w*");
    add_field("int32_t",	"*log_h*");
    add_field("int32_t",	"*view_x*");
    add_field("int32_t",	"*view_y*");
    add_field("int32_t",	"*view_w*");
    add_field("int32_t",	"*view_h*");
    add_field("int32_t",	"*clip_x*");
    add_field("int32_t",	"*clip_y*");
    add_field("int32_t",	"*clip_w*");
    add_field("int32_t",	"*clip_h*");
    add_field("float32_t",	"*scale_x*");
    add_field("float32_t",	"*scale_y*");
    add_field("uint8_t",	"*r*");
    add_field("uint8_t",	"*g*");
    add_field("uint8_t",	"*b*");
    add_field("uint8_t",	"*a*");
    add_field("uint8_t",	"*blend*");
    oend_record(record);

    record = type_vector->v.ptr[t_surface];
    add_field(pointer_string,	"*surface*");
    add_field("string_t",	"pixels");
    add_field("int32_t",	"w");
    add_field("int32_t",	"h");
    add_field("int32_t",	"pitch");
    add_field("uint8_t",	"bpp");
    add_field("uint32_t",	"r_mask");
    add_field("uint32_t",	"g_mask");
    add_field("uint32_t",	"b_mask");
    add_field("uint32_t",	"a_mask");
    add_field("int32_t",	"clip_x");
    add_field("int32_t",	"clip_y");
    add_field("int32_t",	"clip_w");
    add_field("int32_t",	"clip_h");
    add_field("uint8_t",	"r");
    add_field("uint8_t",	"g");
    add_field("uint8_t",	"b");
    add_field("uint8_t",	"a");
    add_field("uint8_t",	"blend");
    add_field("int32_t",	"key");
    add_field("int32_t",	"*clip_x*");
    add_field("int32_t",	"*clip_y*");
    add_field("int32_t",	"*clip_w*");
    add_field("int32_t",	"*clip_h*");
    add_field("uint8_t",	"*r*");
    add_field("uint8_t",	"*g*");
    add_field("uint8_t",	"*b*");
    add_field("uint8_t",	"*a*");
    add_field("uint8_t",	"*blend*");
    add_field("int32_t",	"*key*");
    oend_record(record);

    record = type_vector->v.ptr[t_texture];
    add_field(pointer_string,	"*texture*");
    add_field("auto",		"*handle*");	/* gc it */
    add_field("auto",		"*renderer*");	/* gc it */
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
    add_union("int32_t",	"channel");
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

    /* ttf */
    this_record = ttf_record;
    record = type_vector->v.ptr[t_font];
    add_field(pointer_string,	"*font*");
    add_field("int8_t",		"style");
    add_field("int8_t",		"hinting");
    add_field("int8_t",		"kerning");
    add_field("int32_t",	"outline");
    add_field("int32_t",	"height");
    add_field("int32_t",	"ascent");
    add_field("int32_t",	"descent");
    add_field("int32_t",	"skip");
    add_field(word_string,	"faces");
    add_field("string_t",	"style_name");
    add_field("string_t",	"family_name");
    add_field("int8_t",		"fixed");
    add_field("int8_t",		"*style*");
    add_field("int8_t",		"*hinting*");
    add_field("int8_t",		"*kerning*");
    add_field("int32_t",	"*outline*");
    oend_record(record);

    record = type_vector->v.ptr[t_glyph];
    add_field("int32_t",	"min_x");
    add_field("int32_t",	"max_x");
    add_field("int32_t",	"min_y");
    add_field("int32_t",	"max_y");
    add_field("int32_t",	"advance");
    oend_record(record);

    /* net */
    this_record = net_record;
    record = type_vector->v.ptr[t_net_address];
    add_field("uint32_t",	"host");
    add_field("uint16_t",	"port");
    oend_record(record);

    record = type_vector->v.ptr[t_socket_set];
    add_field(pointer_string,	"*set*");
    oend_record(record);

    /* net.tcp */
    this_record = net_tcp_record;
    record = type_vector->v.ptr[t_tcp_socket];
    add_field(pointer_string,	"*socket*");
    oend_record(record);

    /* net.udp */
    this_record = net_udp_record;
    record = type_vector->v.ptr[t_udp_socket];
    add_field(pointer_string,	"*socket*");
    oend_record(record);

    record = type_vector->v.ptr[t_udp_packet];
    add_field(pointer_string,	"*packet*");
    add_field("int32_t",	"channel");
    add_field("string_t",	"data");
    add_field("int32_t",	"status");
    add_field("uint32_t",	"host");
    add_field("uint16_t",	"port");
    oend_record(record);

    /* mix */
    this_record = mix_record;
    record = type_vector->v.ptr[t_chunk];
    add_field(pointer_string,	"*chunk*");
    oend_record(record);

    record = type_vector->v.ptr[t_music];
    add_field(pointer_string,	"*music*");
    add_field("uint8_t",	"type");
    oend_record(record);

    record = type_vector->v.ptr[t_audio];
    add_field("int32_t",	"frequency");
    add_field("uint16_t",	"format");
    add_field("int32_t",	"channels");
    oend_record(record);

    /* sdl.gl */
    this_record = sdl_gl_record;
    record = type_vector->v.ptr[t_context];
    add_field(pointer_string,	"*context*");
    oend_record(record);

    record = current_record;
    current_record = sdl_record;

    define_builtin0(t_int32,  Init);
    define_builtin0(t_string, GetError);
    define_builtin0(t_void,   ClearError);
    define_builtin0(t_void,   Quit);

    define_builtin6(t_window,   CreateWindow,
		    t_string, t_int32, t_int32, t_int32, t_int32, t_uint32);
    define_builtin1(t_void,     ChangeWindow, t_window);
    define_builtin1(t_renderer, GetWindowRenderer, t_window);
    define_builtin1(t_void,     DestroyWindow, t_window);

    define_builtin3(t_renderer, CreateRenderer,
		    t_window, t_int32, t_uint32);
    define_builtin1(t_int32,    ChangeRenderer, t_renderer);
    define_builtin1(t_window,   GetRendererWindow, t_renderer);
    define_builtin1(t_void,     RenderClear, t_renderer);
    define_builtin3(t_int32,    RenderDrawPoint, t_renderer, t_int32, t_int32);
    define_builtin2(t_int32,    RenderDrawPoints, t_renderer, t_vector|t_int32);
    define_builtin5(t_int32,    RenderDrawLine,
		    t_renderer, t_int32, t_int32, t_int32, t_int32);
    define_builtin2(t_int32,    RenderDrawLines, t_renderer, t_vector|t_int32);
    define_builtin5(t_int32,    RenderDrawRect,
		    t_renderer, t_int32, t_int32, t_int32, t_int32);
    define_builtin2(t_int32,    RenderDrawRects, t_renderer, t_vector|t_int32);
    define_builtin5(t_int32,    RenderFillRect,
		    t_renderer, t_int32, t_int32, t_int32, t_int32);
    define_builtin2(t_int32,    RenderFillRects, t_renderer, t_vector|t_int32);
    define_builtin4(t_int32,    RenderCopy,
		    t_renderer, t_texture, t_rect, t_rect);
    define_builtin7(t_int32,    RenderCopyEx,
		    t_renderer, t_texture, t_rect, t_rect,
		    t_float64, t_point, t_int32);
    define_builtin1(t_void,    RenderPresent, t_renderer);
    define_builtin1(t_void,    DestroyRenderer, t_renderer);

    define_builtin1(t_surface, LoadSurface, t_string);
    define_builtin1(t_int32,   ChangeSurface, t_surface);
    define_builtin2(t_surface, ConvertSurface, t_surface, t_uint32);
    define_builtin2(t_surface, CreateRGBSurface, t_int32, t_int32);
    define_builtin2(t_surface, CreateRGBASurface, t_int32, t_int32);
    define_builtin1(t_int32,   PullSurface, t_surface);
    define_builtin1(t_int32,   PushSurface, t_surface);
    define_builtin4(t_int32,   BlitSurface,
		    t_surface, t_rect, t_surface, t_rect);
    define_builtin4(t_int32,   ScaleSurface,
		    t_surface, t_rect, t_surface, t_rect);
    define_builtin1(t_void,    FreeSurface, t_surface);

    define_builtin5(t_texture, CreateTexture,
		    t_renderer, t_uint32, t_int32, t_int32, t_int32);
    define_builtin2(t_texture, CreateTextureFromSurface,
		    t_renderer, t_surface);
    define_builtin2(t_texture, LoadTexture, t_string, t_renderer);
    define_builtin1(t_int32,   ChangeTexture, t_texture);
    define_builtin1(t_void,    DestroyTexture, t_texture);

    define_builtin1(t_int32,   PollEvent, t_event);
    define_builtin1(t_int32,   WaitEvent, t_event);

    define_builtin0(t_uint32, GetModState);

    define_builtin2(t_timer,   AddTimer, t_uint32, t_void);
    define_builtin0(t_uint32,  GetTicks);
    define_builtin1(t_void,    Delay, t_uint32);
    define_builtin1(t_int32,   RemoveTimer, t_timer);

    define_builtin1(t_int32,  ShowCursor, t_int32);

    current_record = ttf_record;
    define_builtin2(t_font,  OpenFont, t_string, t_int32);
    define_builtin1(t_int32, ChangeFont, t_font);
    define_builtin2(t_int32, GlyphIsProvided, t_font, t_uint16);
    define_builtin2(t_glyph, GlyphMetrics, t_font, t_uint16);
    define_builtin3(t_int32, SizeText, t_font, t_string, t_point);
    define_builtin3(t_int32, SizeUTF8, t_font, t_string, t_point);
    define_builtin3(t_int32, SizeUNICODE, t_font, t_vector|t_uint16, t_point);
    define_builtin3(t_surface, RenderText_Solid, t_font, t_string, t_color);
    define_builtin3(t_surface, RenderUTF8_Solid, t_font, t_string, t_color);
    define_builtin3(t_surface, RenderUNICODE_Solid,
		    t_font, t_vector|t_uint16, t_color);
    define_builtin3(t_surface, RenderGlyph_Solid, t_font, t_uint16, t_color);
    define_builtin4(t_surface, RenderText_Shaded,
		    t_font, t_string, t_color, t_color);
    define_builtin4(t_surface, RenderUTF8_Shaded,
		    t_font, t_string, t_color, t_color);
    define_builtin4(t_surface, RenderUNICODE_Shaded,
		    t_font, t_vector|t_uint16, t_color, t_color);
    define_builtin4(t_surface, RenderGlyph_Shaded,
		    t_font, t_uint16, t_color, t_color);
    define_builtin3(t_surface, RenderText_Blended, t_font, t_string, t_color);
    define_builtin3(t_surface, RenderUTF8_Blended, t_font, t_string, t_color);
    define_builtin3(t_surface, RenderUNICODE_Blended,
		    t_font, t_vector|t_uint16, t_color);
    define_builtin3(t_surface, RenderGlyph_Blended, t_font, t_uint16, t_color);
    define_builtin4(t_surface, RenderText_Blended_Wrapped,
		    t_font, t_string, t_color, t_uint32);
    define_builtin4(t_surface, RenderUTF8_Blended_Wrapped,
		    t_font, t_string, t_color, t_uint32);
    define_builtin4(t_surface, RenderUNICODE_Blended_Wrapped,
		    t_font, t_vector|t_uint16, t_color, t_uint32);
    define_builtin3(t_int32, GetFontKerningSize, t_font, t_uint16, t_uint16);
    define_builtin1(t_void,  CloseFont, t_font);

    current_record = mix_record;
    define_builtin4(t_audio, OpenAudio,
		    t_int32, t_uint16, t_int32, t_int32);
    define_builtin1(t_int32, AllocateChannels, t_int32);
    define_builtin1(t_chunk, LoadChunk, t_string);
    define_builtin3(t_int32, PlayChannel, t_int32, t_chunk, t_int32);
    define_builtin4(t_int32, FadeInChannel,
		    t_int32, t_chunk, t_int32, t_int32);
    define_builtin2(t_int32, VolumeChunk, t_chunk, t_int32);
    define_builtin3(t_int32, SetPanning, t_int32, t_uint8, t_uint8);
    define_builtin3(t_int32, SetPosition, t_int32, t_int16, t_uint8);
    define_builtin2(t_int32, SetDistance, t_int32, t_uint8);
    define_builtin2(t_int32, SetReverseStereo, t_int32, t_int8);
    define_builtin2(t_int32, FadeOutChannel, t_int32, t_int32);
    define_builtin1(t_int32, Playing, t_int32);
    define_builtin1(t_int32, FadingChannel, t_int32);
    define_builtin2(t_int32, ExpireChannel, t_int32, t_int32);
    define_builtin1(t_void, Pause, t_int32);
    define_builtin1(t_void, Resume, t_int32);
    define_builtin1(t_int32, Paused, t_int32);
    define_builtin1(t_int32, HaltChannel, t_int32);
    define_builtin1(t_void,  FreeChunk, t_chunk);
    define_builtin2(t_int32, GroupChannel, t_int32, t_int32);
    define_builtin3(t_int32, GroupChannels, t_int32, t_int32, t_int32);
    define_builtin1(t_int32, GroupAvailable, t_int32);
    define_builtin1(t_int32, GroupCount, t_int32);
    define_builtin1(t_int32, GroupOldest, t_int32);
    define_builtin1(t_int32, GroupNewer, t_int32);
    define_builtin1(t_int32, FadeOutGroup, t_int32);
    define_builtin1(t_int32, HaltGroup, t_int32);
    define_builtin1(t_music, LoadMusic, t_string);
    define_builtin2(t_int32, PlayMusic, t_music, t_int32);
    define_builtin3(t_int32, FadeInMusic, t_music, t_int32, t_int32);
    define_builtin1(t_int32, VolumeMusic, t_uint8);
    define_builtin0(t_int32, PlayingMusic);
    define_builtin1(t_int32,  FadeOutMusic, t_int32);
    define_builtin0(t_int32,  FadingMusic);
    define_builtin0(t_void,  PauseMusic);
    define_builtin0(t_void,  ResumeMusic);
    define_builtin0(t_void,  RewindMusic);
    define_builtin0(t_int32,  PausedMusic);
    define_builtin1(t_int32,  SetMusicPosition, t_float64);
    define_builtin0(t_int32,  HaltMusic);
    define_builtin1(t_void,  FreeMusic, t_music);
    define_builtin0(t_void,  CloseAudio);

    current_record = net_record;
    define_builtin3(t_int32,   ResolveHost, t_net_address, t_string, t_uint16);
    define_builtin1(t_string,  ResolveIP, t_net_address);
    define_builtin0(t_vector|t_net_address, GetLocalAddresses);
    define_builtin1(t_socket_set, AllocSocketSet, t_int32);
    define_builtin2(t_int32, AddSocket, t_socket_set, t_void);
    define_builtin2(t_int32, DelSocket, t_socket_set, t_void);
    define_builtin2(t_int32, CheckSockets, t_socket_set, t_uint32);
    define_builtin1(t_int32, SocketReady, t_void);
    define_builtin1(t_void, FreeSocketSet, t_void);

    current_record = net_tcp_record;
    define_nsbuiltin1(t_tcp_socket, tcp_, Open, t_net_address);
    define_nsbuiltin1(t_tcp_socket, tcp_, Accept, t_net_address);
    define_nsbuiltin1(t_net_address, tcp_, GetPeerAddress, t_tcp_socket);
    define_nsbuiltin2(t_int32, tcp_, Send, t_tcp_socket, t_vector);
    define_nsbuiltin3(t_int32, tcp_, Recv, t_tcp_socket, t_vector, t_int32);
    define_nsbuiltin1(t_void,  tcp_, Close, t_tcp_socket);
    define_nsbuiltin2(t_int32, tcp_, AddSocket, t_socket_set, t_tcp_socket);
    define_nsbuiltin2(t_int32, tcp_, DelSocket, t_socket_set, t_tcp_socket);

    current_record = net_udp_record;
    define_nsbuiltin1(t_udp_socket, udp_, Open, t_uint16);
    define_nsbuiltin2(t_void,  udp_, SetPacketLoss, t_udp_socket, t_int32);
    define_nsbuiltin3(t_int32, udp_, Bind,
		      t_udp_socket, t_int32, t_net_address);
    define_nsbuiltin2(t_int32, udp_, Unbind, t_udp_socket, t_int32);
    define_nsbuiltin2(t_net_address, udp_, GetPeerAddress,
		      t_udp_socket, t_int32);
    define_nsbuiltin3(t_int32, udp_, Send,
		      t_udp_socket, t_int32, t_udp_packet);
    define_nsbuiltin3(t_int32, udp_, Recv, t_udp_socket, t_udp_packet, t_int32);
    define_nsbuiltin1(t_void, udp_, Close, t_udp_socket);
    define_nsbuiltin2(t_int32, udp_, AddSocket, t_socket_set, t_udp_socket);
    define_nsbuiltin2(t_int32, udp_, DelSocket, t_socket_set, t_udp_socket);

    current_record = sdl_gl_record;

    define_builtin0(t_void,    ResetAttributes);
    define_builtin2(t_int32,   SetAttribute, t_int32, t_int32);
    define_builtin2(t_int32,   GetAttribute, t_int32, t_vector|t_int32);
    define_builtin1(t_context, CreateContext, t_window);
    define_builtin2(t_int32,   MakeCurrent, t_window, t_context);
    define_builtin0(t_window,  GetCurrentWindow);
    define_builtin0(t_context, GetCurrentContext);
    define_builtin1(t_int32,   SetSwapInterval, t_int8);
    define_builtin0(t_int8,    GetSwapInterval);
    define_builtin1(t_void,    SwapWindow, t_window);
    define_builtin1(t_void,    DeleteContext, t_context);

    define_builtin4(t_surface, ReadPixels, t_int32, t_int32, t_int32, t_int32);
    define_builtin1(t_void,    DrawPixels, t_surface);
    define_builtin1(t_void,    TexImage1D, t_surface);
    define_builtin1(t_void,    TexImage2D, t_surface);
    define_builtin3(t_void,    TexSubImage1D, t_surface, t_int32, t_int32);
    define_builtin5(t_void,    TexSubImage2D,
		    t_surface, t_int32, t_int32, t_int32, t_int32);

    current_record = record;
#undef add_vec_field
#undef add_union
#undef add_field
}

void
finish_sdl(void)
{
    orem_root((oobject_t *)&window_table);
    window_table = null;
    orem_root((oobject_t *)&timer_vector);
    orem_root((oobject_t *)&error_vector);
    orem_root((oobject_t *)&address_vector);
}

void
odestroy_window(owindow_t *window)
{
    if (window->__window) {
	if (window_table)
	    orem_hashentry(window_table, window->__handle);
	if (sdl_active)
	    SDL_DestroyWindow(window->__window);
	window->__window = null;
	if (current_window == window)
	    current_window = null;
    }
}

void
odestroy_renderer(orenderer_t *renderer)
{
    ohashtable_t	*table;
    ohashentry_t	*entry;
    oword_t		 offset;

    if (renderer->__renderer) {
	if (renderer->__target) {
	    odestroy_texture(renderer->__target);
	    renderer->__target = renderer->target = null;
	}
	table = renderer->__textures;
	for (offset = 0; offset < table->size; offset++) {
	    for (entry = table->entries[offset]; entry; entry = entry->next)
		odestroy_texture((otexture_t *)entry->vv.w);
	}
	if (sdl_active)
	    SDL_DestroyRenderer(renderer->__renderer);
	renderer->__renderer = null;
    }
}

void
odestroy_texture(otexture_t *texture)
{
    if (texture->__texture) {
	if (sdl_active)
	    SDL_DestroyTexture(texture->__texture);
	texture->__texture = null;
    }
}

void
odestroy_font(ofont_t *font)
{
    if (font->__font) {
	if (sdl_active)
	    TTF_CloseFont(font->__font);
	font->__font = null;
    }
}

void
odestroy_context(ocontext_t *context)
{
    if (context->__context) {
	if (sdl_active)
	    SDL_GL_DeleteContext(context->__context);
	context->__context = null;
	if (current_context == context)
	    current_context = null;
    }
}

void
odestroy_surface(osurface_t *surface)
{
    if (surface->__surface) {
	if (sdl_active)
	    SDL_FreeSurface(surface->__surface);
	surface->__surface = null;
    }
}

void
odestroy_chunk(ochunk_t *chunk)
{
    if (chunk->__chunk) {
	if (sdl_active)
	    Mix_FreeChunk(chunk->__chunk);
	chunk->__chunk = null;
    }
}

void
odestroy_music(omusic_t *music)
{
    if (music->__music) {
	if (sdl_active)
	    Mix_FreeMusic(music->__music);
	music->__music = null;
    }
}

oint32_t
odestroy_timer(otimer_t *timer)
{
    oint32_t			result;

    result = -1;
    if (timer->__timer) {
	if (sdl_active)
	    result = SDL_RemoveTimer(timer->__timer);
	timer->__timer = 0;
    }

    return (result);
}

void
odestroy_socket_set(osocket_set_t *set)
{
    if (set->__set) {
	if (sdl_active)
	    SDLNet_FreeSocketSet(set->__set);
	set->__set = null;
    }
}

void
odestroy_tcp_socket(otcp_socket_t *socket)
{
    if (socket->__socket) {
	if (sdl_active)
	    SDLNet_TCP_Close(socket->__socket);
	socket->__socket = null;
    }
}

void
odestroy_udp_socket(oudp_socket_t *socket)
{
    if (socket->__socket) {
	if (sdl_active)
	    SDLNet_UDP_Close(socket->__socket);
	socket->__socket = null;
    }
}

void
odestroy_udp_packet(oudp_packet_t *packet)
{
    if (packet->__packet) {
	if (sdl_active)
	    SDLNet_FreePacket(packet->__packet);
	packet->__packet = null;
    }
}

static void
native_Init(oobject_t list, oint32_t ac)
/* int32_t Init(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    if (!sdl_active) {
	r0->v.w = ((SDL_Init(SDL_INIT_EVERYTHING) != 0) |
		   (IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|
			     IMG_INIT_TIF|IMG_INIT_WEBP) == 0) |
		   (TTF_Init() != 0) |
		   (Mix_Init(MIX_INIT_FLAC|MIX_INIT_MOD|
			     MIX_INIT_MODPLUG|MIX_INIT_MP3|
			     MIX_INIT_OGG|MIX_INIT_FLUIDSYNTH) == 0));
	Mix_HookMusicFinished(music_callback);
	Mix_ChannelFinished(channel_callback);
	r0->v.w |= SDLNet_Init();
	sdl_active = true;
    }
    else
	r0->v.w = -1;
}

static void
native_GetError(oobject_t list, oint32_t ac)
/* string_t sdl.GetError(); */
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
native_ClearError(oobject_t list, oint32_t ac)
/* void sdl.ClearError(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    SDL_ClearError();
    r0->t = t_void;
}

static void
native_Quit(oobject_t list, oint32_t ac)
/* void sdl.Quit(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    if (sdl_active) {
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	Mix_Quit();
	SDLNet_Quit();
	sdl_active = false;
    }
}

static void
native_CreateWindow(oobject_t list, oint32_t ac)
/* window_t sdl.CreateWindow(string_t title,
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
    if (alist->a0 == null)
	title[0] = '\0';
    else if (otype(alist->a0) != t_string)
	ovm_raise(except_invalid_argument);
    else {
	if ((length = alist->a0->length) > BUFSIZ - 1)
	    length = BUFSIZ - 1;
	memcpy(title, alist->a0->v.ptr, length);
	title[length] = '\0';
    }
    if ((sw = SDL_CreateWindow(title, alist->a1, alist->a2,
			       alist->a3, alist->a4,
			       alist->a5))) {
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
	/* brightness */
	ow->__brightness = SDL_GetWindowBrightness(sw);
	ow->brightness = ow->__brightness;
	/**/
	r0->v.o = thread_self->obj;
	r0->t = t_window;
    }
    else
	r0->t = t_void;
}

static void
native_ChangeWindow(oobject_t list, oint32_t ac)
/* int32_t sdl.ChangeWindow(sdl.window_t window); */
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
    r0->t = t_word;
    r0->v.w = 0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_window);
    CHECK_NULL(alist->a0->__window);
    ow = alist->a0;
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
	    r0->v.w |= SDL_SetWindowFullscreen(sw, ow->flags &
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
	if (ow->__title->length != length)
	    orenew_vector(ow->__title, length);
	if (length)
	    memcpy(ow->__title->v.u8, buffer, length);
	ow->title = ow->__title;
    }
    /* icon */
    if (ow->icon != ow->__icon) {
	if (ow->icon && otype(ow->icon) != t_surface)
	    ovm_raise(except_invalid_argument);
	SDL_SetWindowIcon(ow->__window, ow->icon->__surface);
	ow->__icon = ow->icon;
    }
    /* brightness */
    if (ow->brightness != ow->__brightness) {
	r0->v.w |= SDL_SetWindowBrightness(sw, ow->brightness);
	ow->__brightness = SDL_GetWindowBrightness(sw);
	ow->brightness = ow->__brightness;
    }
    r0->t = t_void;
}

static void
native_GetWindowRenderer(oobject_t list, oint32_t ac)
/* sdl.renderer_t sdl.GetWindowRenderer(sdl.window_t window); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_win_t			*alist;

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_window);
    CHECK_NULL(alist->a0->__window);
    if ((r0->v.o = alist->a0->__renderer))
	r0->t = t_renderer;
    else
	r0->t = t_void;
}

static void
native_DestroyWindow(oobject_t list, oint32_t ac)
/* void sdl.DestroyWindow(sdl.window_t window); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_win_t			*alist;

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_window);
	odestroy_window(alist->a0);
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
    SDL_BlendMode		 blend;

    sr = or->__renderer;
    SDL_RenderGetLogicalSize(sr, &or->__log_w, &or->__log_h);
    or->log_w = or->__log_w;
    or->log_h = or->__log_h;
    if ((tx = SDL_GetRenderTarget(sr))) {
	et.nt = t_word;
	et.nv.w = (oword_t)tx;
	if ((ot = oget_hashentry(or->__textures, &et)))
	    or->target = or->__target = (otexture_t *)ot->nv.w;
    }
    SDL_RenderGetViewport(sr, (SDL_Rect *)&or->__view_x);
    or->view_x = or->__view_x;
    or->view_y = or->__view_y;
    or->view_w = or->__view_w;
    or->view_h = or->__view_h;
    SDL_RenderGetClipRect(sr, (SDL_Rect *)&or->__clip_x);
    or->clip_x = or->__clip_x;
    or->clip_y = or->__clip_y;
    or->clip_w = or->__clip_w;
    or->clip_h = or->__clip_h;
    SDL_RenderGetScale(sr, &or->__scale_x, &or->__scale_y);
    or->scale_x = or->__scale_x;
    or->scale_y = or->__scale_y;
    SDL_GetRenderDrawColor(sr, &or->__r, &or->__g, &or->__b, &or->__a);
    or->r = or->__r;
    or->g = or->__g;
    or->b = or->__b;
    or->a = or->__a;
    SDL_GetRenderDrawBlendMode(sr, &blend);
    or->blend = or->__blend = blend;
}

static void
native_CreateRenderer(oobject_t list, oint32_t ac)
/* sdl.renderer_t CreateRenderer(sdl.window_t win,
				 int32_t index, uint32_t flags); */
{
    GET_THREAD_SELF()
    orenderer_t			*or;
    SDL_Renderer		*sr;
    oregister_t			*r0;
    SDL_RendererInfo		 info;
    nat_win_i32_u32_t		*alist;

    alist = (nat_win_i32_u32_t *)list;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_window);
    CHECK_NULL(alist->a0->__window);
    r0 = &thread_self->r0;
    if ((sr = SDL_CreateRenderer(alist->a0->__window,
				 alist->a1, alist->a2))) {
	onew_object(&thread_self->obj, t_renderer, sizeof(orenderer_t));
	or = (orenderer_t *)thread_self->obj;
	or->__renderer = sr;
	onew_hashtable(&or->__textures, 4);
	or->__window = alist->a0;
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

	/* FIXME not initialized by SDL 2.0.3 at least (initialized to zero
	 * but first draw call would use all white) */
	SDL_SetRenderDrawColor(sr, 0xff, 0xff, 0xff, 0xff);

	query_renderer(or);
	r0->v.o = thread_self->obj;
	r0->t = t_renderer;
    }
    else
	r0->t = t_void;
}

static void
native_ChangeRenderer(oobject_t list, oint32_t ac)
/* int32_t sdl.ChangeRenderer(sdl.renderer_t ren); */
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
    ren = alist->a0;
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
	if ((entry = oget_hashentry(alist->a0->__textures, &check)) == null)
	    ovm_raise(except_invalid_argument);
	tex = (otexture_t *)entry->nv.w;
	r0->v.w |= SDL_SetRenderTarget(ren->__renderer, tex->__texture);
    }
    if (ren->view_x != ren->__view_x || ren->view_y != ren->__view_y ||
	ren->view_w != ren->__view_w || ren->view_h != ren->__view_h)
	r0->v.w |= SDL_RenderSetViewport(ren->__renderer,
					 (SDL_Rect *)&ren->view_x);
    if (ren->clip_x != ren->__clip_x || ren->clip_y != ren->__clip_y ||
	ren->clip_w != ren->__clip_w || ren->clip_h != ren->__clip_h)
	r0->v.w |= SDL_RenderSetClipRect(ren->__renderer,
					 (SDL_Rect *)&ren->clip_x);
    if (ren->scale_x != ren->__scale_x || ren->scale_y != ren->__scale_y)
	r0->v.w |= SDL_RenderSetScale(ren->__renderer,
				      ren->scale_x, ren->scale_y);
    if (ren->r != ren->__r || ren->g != ren->__g ||
	ren->g != ren->__g || ren->a != ren->__a)
	r0->v.w |= SDL_SetRenderDrawColor(ren->__renderer,
					  ren->r, ren->g, ren->b, ren->a);
    if (ren->blend != ren->__blend)
	r0->v.w |= SDL_SetRenderDrawBlendMode(ren->__renderer, ren->__blend);
    query_renderer(ren);
}

static void
native_GetRendererWindow(oobject_t list, oint32_t ac)
/* sdl.window_t sdl.GetRendererWindow(sdl.renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    if ((r0->v.o = alist->a0->__window))
	r0->t = t_window;
    else
	r0->t = t_void;
}

static void
native_RenderClear(oobject_t list, oint32_t ac)
/* void sdl.RenderClear(sdl.renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    r0->t = t_word;
    r0->v.w = SDL_RenderClear(alist->a0->__renderer);
}

static void
native_RenderDrawPoint(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderDrawPoint(sdl.renderer_t ren, int32_t, int32_t y); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_t		*alist;

    alist = (nat_ren_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawPoint(alist->a0->__renderer,
				  alist->a1, alist->a2);
}

static void
native_RenderDrawPoints(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderDrawPoints(sdl.renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_int32);
    if (!alist->a1->length || (alist->a1->length & 1))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawPoints(alist->a0->__renderer,
				   (SDL_Point *)alist->a1->v.i32,
				   alist->a1->length >> 1);
}

static void
native_RenderDrawLine(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderDrawPoint(sdl.renderer_t ren, int32_t x1, int32_t y1,
			       int32_t x2, int32_t y2); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_i32_i32_t	*alist;

    alist = (nat_ren_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawLine(alist->a0->__renderer,
				 alist->a1, alist->a2,
				 alist->a3, alist->a4);
}

static void
native_RenderDrawLines(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderDrawLines(sdl.renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_int32);
    if (alist->a1->length < 4 || (alist->a1->length & 1))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawLines(alist->a0->__renderer,
				  (SDL_Point *)alist->a1->v.i32,
				  alist->a1->length >> 1);
}

static void
native_RenderDrawRect(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderDrawRect(sdl.renderer_t ren, int32_t x, int32_t y,
			      int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_i32_i32_t	*alist;

    alist = (nat_ren_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawRect(alist->a0->__renderer,
				 (SDL_Rect *)&alist->a1);
}

static void
native_RenderDrawRects(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderDrawRects(sdl.renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_int32);
    if (alist->a1->length & 3)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawRects(alist->a0->__renderer,
				  (SDL_Rect *)alist->a1->v.i32,
				  alist->a1->length >> 2);
}

static void
native_RenderFillRect(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderFillRect(sdl.renderer_t ren, int32_t x, int32_t y,
			      int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_i32_i32_t	*alist;

    alist = (nat_ren_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    r0->t = t_word;
    r0->v.w = SDL_RenderFillRect(alist->a0->__renderer,
				 (SDL_Rect *)&alist->a1);
}

static void
native_RenderFillRects(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderFillRects(sdl.renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_int32);
    if (alist->a1->length & 3)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderFillRects(alist->a0->__renderer,
				  (SDL_Rect *)alist->a1->v.i32,
				  alist->a1->length >> 2);
}

static void
native_RenderCopy(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderCopy(sdl.renderer_t ren, sdl.texture_t tex,
			  sdl.rect_t src, sdl.rect_t dst); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_tex_rec_rec_t	*alist;

    alist = (nat_ren_tex_rec_rec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_texture);
    CHECK_NULL(alist->a1->__texture);
    CHECK_NULL_OR_TYPE(alist->a2, t_rect);
    CHECK_NULL_OR_TYPE(alist->a3, t_rect);
    r0->t = t_word;
    r0->v.w = SDL_RenderCopy(alist->a0->__renderer, alist->a1->__texture,
			     (SDL_Rect *)alist->a2, (SDL_Rect *)alist->a3);
}

static void
native_RenderCopyEx(oobject_t list, oint32_t ac)
/* int32_t sdl.RenderCopyEx(sdl.renderer_t ren, sdl.texture_t tex,
			    sdl.rect_t src, sdl.rect_t dst,
			    float64_t angle, sdl.point_t center,
			    int32_t flip); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_ren_tex_rec_rec_f64_pnt_i32_t	*alist;

    alist = (nat_ren_tex_rec_rec_f64_pnt_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_texture);
    CHECK_NULL(alist->a1->__texture);
    CHECK_NULL_OR_TYPE(alist->a2, t_rect);
    CHECK_NULL_OR_TYPE(alist->a3, t_rect);
    CHECK_NULL_OR_TYPE(alist->a5, t_point);
    r0->t = t_word;
    r0->v.w = SDL_RenderCopyEx(alist->a0->__renderer, alist->a1->__texture,
			       (SDL_Rect *)alist->a2, (SDL_Rect *)alist->a3,
			       alist->a4, (SDL_Point *)alist->a5, alist->a6);
}

static void
native_RenderPresent(oobject_t list, oint32_t ac)
/* void sdl.RenderPresent(sdl.renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    SDL_RenderPresent(alist->a0->__renderer);
    r0->t = t_void;
}

static void
native_DestroyRenderer(oobject_t list, oint32_t ac)
/* void sdl.DestroyRenderer(sdl.renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_t			*alist;

    alist = (nat_ren_t *)list;
    r0 = &thread_self->r0;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_renderer);
	odestroy_renderer(alist->a0);
    }
    r0->t = t_void;
}

void
query_surface(osurface_t *os)
{
    SDL_Surface			*ss;
    SDL_BlendMode		 blend;

    ss = os->__surface;
    os->w = ss->w;
    os->h = ss->h;
    os->pitch = ss->pitch;
    os->bpp = ss->format->BytesPerPixel;
    os->r_mask = ss->format->Rmask;
    os->g_mask = ss->format->Gmask;
    os->b_mask = ss->format->Bmask;
    os->a_mask = ss->format->Amask;
    SDL_GetClipRect(ss, (SDL_Rect *)&os->__clip_x);
    os->clip_x = os->__clip_x;
    os->clip_y = os->__clip_y;
    os->clip_w = os->__clip_w;
    os->clip_h = os->__clip_h;
    SDL_GetSurfaceColorMod(ss, &os->__r, &os->__g, &os->__b);
    os->r = os->__r;
    os->g = os->__g;
    os->b = os->__b;
    SDL_GetSurfaceAlphaMod(ss, &os->__a);
    os->a = os->__a;
    SDL_GetSurfaceBlendMode(ss, &blend);
    os->__blend = os->blend = blend;
    SDL_GetColorKey(ss, &os->__key);
    os->key = os->__key;
}

static void
native_LoadSurface(oobject_t list, oint32_t ac)
/* sdl.surface_t sdl.LoadSurface(string_t path); */
{
    GET_THREAD_SELF()
    SDL_Surface			*ss;
    osurface_t			*os;
    oregister_t			*r0;
    nat_vec_t			*alist;
    char			 path[BUFSIZ];

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_string);
    if (alist->a0->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->a0->v.ptr, alist->a0->length);
    path[alist->a0->length] = '\0';
    if ((ss = IMG_Load(path))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_CreateRGBSurface(oobject_t list, oint32_t ac)
/* sdl.surface_t sdl.CreateRGBSurface(int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    SDL_Surface			*ss;
    osurface_t			*os;
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if ((ss = SDL_CreateRGBSurface(0, alist->a0, alist->a1,
				   24, R_MASK, G_MASK, B_MASK, 0))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_CreateRGBASurface(oobject_t list, oint32_t ac)
/* sdl.surface_t sdl.CreateRGBASurface(int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    SDL_Surface			*ss;
    osurface_t			*os;
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if ((ss = SDL_CreateRGBSurface(0, alist->a0, alist->a1,
				   32, R_MASK, G_MASK, B_MASK, A_MASK))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_ConvertSurface(oobject_t list, oint32_t ac)
/* sdl.surface_t sdl.ConvertSurface(sdl.surface_t surf, uint32_t format); */
{
    GET_THREAD_SELF()
    SDL_Surface			*ss;
    osurface_t			*os;
    oregister_t			*r0;
    nat_srf_u32_t		*alist;

    alist = (nat_srf_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    if ((ss = SDL_ConvertSurfaceFormat(alist->a0->__surface,
				       alist->a1, SDL_SWSURFACE))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_ChangeSurface(oobject_t list, oint32_t ac)
/* int32_t sdl.ChangeSurface(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    osurface_t			*os;
    nat_srf_t			*alist;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    r0->t = t_word;
    r0->v.w = 0;
    os = alist->a0;
    if (os->clip_x != os->__clip_x || os->clip_y != os->__clip_y ||
	os->clip_w != os->__clip_w || os->clip_h != os->__clip_h)
	r0->v.w |= !SDL_SetClipRect(os->__surface, (SDL_Rect *)&os->clip_x);
    if (os->r != os->__r || os->g != os->__g || os->b != os->__b)
	r0->v.w |= SDL_SetSurfaceColorMod(os->__surface, os->r, os->g, os->b);
    if (os->a != os->__a)
	r0->v.w |= SDL_SetSurfaceAlphaMod(os->__surface, os->a);
    if (os->blend != os->__blend)
	r0->v.w |= SDL_SetSurfaceBlendMode(os->__surface, os->blend);
    query_surface(os);
}

static void
native_PullSurface(oobject_t list, oint32_t ac)
/* int32_t sdl.PullSurface(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Surface			*ss;
    osurface_t			*os;
    oregister_t			*r0;
    nat_srf_t			*alist;
    oword_t			 length;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    os = alist->a0;
    ss = os->__surface;
    if (os->pixels && otype(os->pixels) != t_vector_uint8)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    if (!SDL_MUSTLOCK(ss) || !SDL_LockSurface(ss)) {
	r0->v.w = 0;
	check_mult(ss->pitch, ss->h);
	length = ss->pitch * ss->h;
	if (os->pixels == null)
	    onew_vector((oobject_t *)&os->pixels, t_uint8, length);
	else if (os->pixels->length != length)
	    orenew_vector(os->pixels, length);
	memcpy(os->pixels->v.ptr, ss->pixels, length);
	if (SDL_MUSTLOCK(ss))
	    SDL_UnlockSurface(ss);
    }
    else
	r0->v.w = -1;
}

static void
native_PushSurface(oobject_t list, oint32_t ac)
/* int32_t sdl.PushSurface(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Surface			*ss;
    osurface_t			*os;
    oregister_t			*r0;
    nat_srf_t			*alist;
    oword_t			 length;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    os = alist->a0;
    ss = alist->a0->__surface;
    check_mult(ss->pitch, ss->h);
    length = ss->pitch * ss->h;
    if (os->pixels == null ||
	otype(os->pixels) != t_vector_uint8 || os->pixels->length != length)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    if (!SDL_MUSTLOCK(ss) || !SDL_LockSurface(ss)) {
	r0->v.w = 0;
	memcpy(ss->pixels, os->pixels->v.ptr, length);
	if (SDL_MUSTLOCK(ss))
	    SDL_UnlockSurface(ss);
    }
    else
	r0->v.w = -1;
}

static void
native_BlitSurface(oobject_t list, oint32_t ac)
/* int32_t sdl.BlitSurface(sdl.surface_t src, sdl.rect_t src_rect,
			   sdl.surface_t dst, sdl.rect_t dst_rect); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_srf_rec_srf_rec_t	*alist;

    alist = (nat_srf_rec_srf_rec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    CHECK_NULL_OR_TYPE(alist->a1, t_rect);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_surface);
    CHECK_NULL(alist->a2->__surface);
    CHECK_NULL_OR_TYPE(alist->a3, t_rect);
    r0->t = t_word;
    r0->v.w = SDL_BlitSurface(alist->a0->__surface, (SDL_Rect *)alist->a1,
			      alist->a2->__surface, (SDL_Rect *)alist->a3);
}

static void
native_ScaleSurface(oobject_t list, oint32_t ac)
/* int32_t sdl.ScaleSurface(sdl.surface_t src, sdl.rect_t src_rect,
			    sdl.surface_t dst, sdl.rect_t dst_rect); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_srf_rec_srf_rec_t	*alist;

    alist = (nat_srf_rec_srf_rec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    CHECK_NULL_OR_TYPE(alist->a1, t_rect);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_surface);
    CHECK_NULL(alist->a2->__surface);
    CHECK_NULL_OR_TYPE(alist->a3, t_rect);
    r0->t = t_word;
    r0->v.w = SDL_BlitScaled(alist->a0->__surface, (SDL_Rect *)alist->a1,
			     alist->a2->__surface, (SDL_Rect *)alist->a3);
}

static void
native_FreeSurface(oobject_t list, oint32_t ac)
/* void sdl.FreeSurface(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_srf_t			*alist;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_surface);
	odestroy_surface(alist->a0);
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
handle_texture(orenderer_t *ren, otexture_t *tex)
{
    onew_object((oobject_t *)&tex->__handle,
		t_hashentry, sizeof(ohashentry_t));
    tex->__handle->nt = t_word;
    tex->__handle->nv.w = (oword_t)tex->__texture;
    tex->__handle->vt = t_word;
    tex->__handle->vv.w = (oword_t)tex;
    okey_hashentry(tex->__handle);
    assert(oget_hashentry(ren->__textures, tex->__handle) == null);
    oput_hashentry(ren->__textures, tex->__handle);
    tex->__renderer = ren;
}

static void
native_CreateTexture(oobject_t list, oint32_t ac)
/* sdl.texture_t sdl.CreateTexture(sdl.renderer_t ren, uint32_t format,
				   int32_t access, int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    SDL_Texture			*st;
    otexture_t			*ot;
    oregister_t			*r0;
    nat_ren_u32_i32_i32_i32_t	*alist;

    alist = (nat_ren_u32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    if ((st = SDL_CreateTexture(alist->a0->__renderer, alist->a1,
				alist->a2, alist->a3, alist->a4))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(alist->a0, ot);
	query_texture(ot);
	r0->v.o = thread_self->obj;
	r0->t = t_texture;
    }
    else
	r0->t = t_void;
}

static void
native_CreateTextureFromSurface(oobject_t list, oint32_t ac)
/* sdl.texture_t sdl.CreateTextureFromSurface(sdl.renderer_t ren,
					      sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Texture			*st;
    otexture_t			*ot;
    oregister_t			*r0;
    nat_ren_srf_t		*alist;

    alist = (nat_ren_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_surface);
    CHECK_NULL(alist->a1->__surface);
    if ((st = SDL_CreateTextureFromSurface(alist->a0->__renderer,
					   alist->a1->__surface))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(alist->a0, ot);
	query_texture(ot);
	r0->v.o = thread_self->obj;
	r0->t = t_texture;
    }
    else
	r0->t = t_void;
}

static void
native_LoadTexture(oobject_t list, oint32_t ac)
/* sdl.texture_t sdl.LoadTexture(sdl.renderer_t ren, string_t path); */
{
    GET_THREAD_SELF()
    SDL_Texture			*st;
    otexture_t			*ot;
    oregister_t			*r0;
    nat_ren_vec_t		*alist;
    char			 path[BUFSIZ];

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_renderer);
    CHECK_NULL(alist->a0->__renderer);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    if (alist->a1->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->a1->v.ptr, alist->a1->length);
    path[alist->a1->length] = '\0';
    if ((st = IMG_LoadTexture(alist->a0->__renderer, path))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(alist->a0, ot);
	query_texture(ot);
	r0->v.o = thread_self->obj;
	r0->t = t_texture;
    }
    else
	r0->t = t_void;
}

static void
native_ChangeTexture(oobject_t list, oint32_t ac)
/* int32_t sdl.ChangeTexture(sdl.texture_t tex); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    otexture_t			*ot;
    nat_tex_t			*alist;

    alist = (nat_tex_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_texture);
    CHECK_NULL(alist->a0->__texture);
    r0->t = t_word;
    r0->v.w = 0;
    ot = alist->a0;
    if (ot->r != ot->__r || ot->g != ot->__g || ot->b != ot->__b)
	r0->v.w |= SDL_SetTextureColorMod(ot->__texture, ot->r, ot->g, ot->b);
    if (ot->a != ot->__a)
	r0->v.w |= SDL_SetTextureAlphaMod(ot->__texture, ot->a);
    if (ot->blend != ot->__blend)
	r0->v.w |= SDL_SetTextureBlendMode(ot->__texture, ot->blend);
    query_texture(ot);
}

static void
native_DestroyTexture(oobject_t list, oint32_t ac)
/* void sdl.DestroyTexture(sdl.texture_t tex); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_tex_t			*alist;

    alist = (nat_tex_t *)list;
    r0 = &thread_self->r0;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_texture);
	odestroy_texture(alist->a0);
	/* Shortcut it when user splicitly destroyed the texture */
	orem_hashentry(alist->a0->__renderer->__textures, alist->a0->__handle);
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
		    if (ev->window) {
			ev->window->x = ev->window->__x = ev->x;
			ev->window->y = ev->window->__y = ev->y;
		    }
		    break;
		case SDL_WINDOWEVENT_RESIZED:
		    ev->w	= sv->window.data1;
		    ev->h	= sv->window.data2;
		    if (ev->window) {
			ev->window->w = ev->window->__w = ev->w;
			ev->window->h = ev->window->__h = ev->h;
		    }
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
	case SDL_USEREVENT + 1:
	    break;
	case SDL_USEREVENT + 2:
	    ev->channel		= (int32_t)(oword_t)sv->user.data1;
	    break;
	default:
	    abort();
    }
}

static void
native_PollEvent(oobject_t list, oint32_t ac)
/* void sdl.PollEvent(sdl.event_t ev); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_evt_t			*alist;

    alist = (nat_evt_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_event);
    r0->t = t_word;
    if (alist->a0->__event == null)
	onew_object((oobject_t *)&alist->a0->__event,
		    t_void, sizeof(SDL_Event));
    if ((r0->v.w = SDL_PollEvent(alist->a0->__event)))
	translate_event(alist->a0);
}

static void
native_WaitEvent(oobject_t list, oint32_t ac)
/* void sdl.PollEvent(sdl.event_t ev); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_evt_t			*alist;

    alist = (nat_evt_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_event);
    r0->t = t_word;
    if (alist->a0->__event == null)
	onew_object((oobject_t *)&alist->a0->__event,
		    t_void, sizeof(SDL_Event));
    if ((r0->v.w = SDL_WaitEvent(alist->a0->__event)))
	translate_event(alist->a0);
}

static void
native_GetModState(oobject_t list, oint32_t ac)
/* void sdl.GetModeState(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = SDL_GetModState();
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
	odestroy_timer(ot);
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
native_AddTimer(oobject_t list, oint32_t ac)
/* sdl.timer_t sdl.AddTimer(uint32_t ms, auto data); */
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
    ot->msec = alist->a0;
    ot->data = alist->a1;
    if ((st = SDL_AddTimer(alist->a0, timer_callback, ot))) {
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
native_GetTicks(oobject_t list, oint32_t ac)
/* uint32_t sdl.GetTicks(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    ret_u32(r0, SDL_GetTicks());
}

static void
native_Delay(oobject_t list, oint32_t ac)
/* void sdl.Delay(uint32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_u32_t			*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    SDL_Delay(alist->a0);
    r0->t = t_void;
}

static void
native_RemoveTimer(oobject_t list, oint32_t ac)
/* int32_t sdl.RemoveTimer(sdl.timer_t timer); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_tmr_t			*alist;

    alist = (nat_tmr_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_timer);
    if (alist->a0->__timer) {
	r0->v.w = odestroy_timer(alist->a0);
	alist->a0->msec = 0;
	push_timer(alist->a0);
    }
    else
	r0->v.w = false;
}

static void
native_ShowCursor(oobject_t list, oint32_t ac)
/* int32_t sdl.ShowCursor(int32_t toggle); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_int32;
    r0->v.w = SDL_ShowCursor(alist->a0);
}

static void
query_font(ofont_t *of)
{
    TTF_Font			*sf;

    sf = of->__font;
    of->__style = of->style = TTF_GetFontStyle(sf);
    of->__hinting = of->hinting = TTF_GetFontHinting(sf);
    of->__kerning = of->kerning = TTF_GetFontKerning(sf);
    of->__outline = of->outline = TTF_GetFontOutline(sf);
}

static void
native_OpenFont(oobject_t list, oint32_t ac)
/* ttf.font_t ttf.OpenFont(string_t name, int32_t ptsize) */
{
    GET_THREAD_SELF()
    TTF_Font			*sf;
    ofont_t			*of;
    oregister_t			*r0;
    char			*spec;
    nat_vec_i32_t		*alist;
    oword_t			 length;
    char			 path[BUFSIZ];

    alist = (nat_vec_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_string);
    if (alist->a0->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->a0->v.ptr, alist->a0->length);
    path[alist->a0->length] = '\0';
    if ((sf = TTF_OpenFont(path, alist->a1))) {
	onew_object(&thread_self->obj, t_font, sizeof(ofont_t));
	of = (ofont_t *)thread_self->obj;
	of->__font = sf;
	query_font(of);
	/* read only fields */
	of->height = TTF_FontHeight(sf);
	of->ascent = TTF_FontAscent(sf);
	of->descent = TTF_FontDescent(sf);
	of->skip = TTF_FontLineSkip(sf);
	of->faces = TTF_FontFaces(sf);
	if ((spec = TTF_FontFaceStyleName(sf))) {
	    length = strlen(spec);
	    onew_vector((oobject_t *)&of->style_name, t_uint8, length);
	    memcpy(of->style_name->v.ptr, spec, length);
	}
	if ((spec = TTF_FontFaceFamilyName(sf))) {
	    length = strlen(spec);
	    onew_vector((oobject_t *)&of->family_name, t_uint8, length);
	    memcpy(of->family_name->v.ptr, spec, length);
	}
	of->fixed = TTF_FontFaceIsFixedWidth(sf);
	r0->v.o = thread_self->obj;
	r0->t = t_font;
    }
    else
	r0->t = t_void;
}

static void
native_ChangeFont(oobject_t list, oint32_t ac)
/* int32_t ttf.ChangeFont(sdl.font_t font) */
{
    GET_THREAD_SELF()
    TTF_Font			*sf;
    ofont_t			*of;
    oregister_t			*r0;
    nat_fnt_t			*alist;

    alist = (nat_fnt_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    of = alist->a0;
    sf = of->__font;
    r0->t = t_word;
    r0->v.w = 0;
    if (of->style != of->__style)
	TTF_SetFontStyle(sf, of->style);
    if (of->hinting != of->__hinting)
	TTF_SetFontHinting(sf, of->hinting);
    if (of->kerning != of->__kerning)
	TTF_SetFontKerning(sf, !!of->kerning);
    if (of->outline != of->__outline)
	TTF_SetFontOutline(sf, of->outline);
    query_font(of);
}

static void
native_GlyphIsProvided(oobject_t list, oint32_t ac)
/* int32_t ttf.GlyphIsProvided(ttf.font_t font, uint16_t ch) */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_u16_t		*alist;

    alist = (nat_fnt_u16_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    r0->t = t_word;
    r0->v.w = TTF_GlyphIsProvided(alist->a0->__font, alist->a1);
}

static void
native_GlyphMetrics(oobject_t list, oint32_t ac)
/* ttf.glyph_t ttf.GlyphMetrics(ttf.font_t font, uint16_t ch) */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    oglyph_t			 ogm;
    nat_fnt_u16_t		*alist;

    alist = (nat_fnt_u16_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    if (TTF_GlyphMetrics(alist->a0->__font, alist->a1,
			 &ogm.min_x, &ogm.max_x, &ogm.min_y, &ogm.max_y,
			 &ogm.advance) == 0) {
	onew_object(&thread_self->obj, t_glyph, sizeof(oglyph_t));
	memcpy(thread_self->obj, &ogm, sizeof(oglyph_t));
	r0->v.o = thread_self->obj;
	r0->t = t_glyph;
    }
    else
	r0->t = t_void;
}

/* XXX allocation is 16 bytes aligned and padded with zeros, still a
 * XXX pessimization due to not having a flag telling may have been
 *     already overallocated in case has 16 byte aligned length */
#define make_vec_text(V)						\
    do {								\
	if (!(V->length & 15)) {					\
	    orenew_vector(V, V->length + 1);				\
	    --V->length;						\
	}								\
    } while (0)
#define make_vec_unicode(V)						\
    do {								\
	if (!(V->length & 7)) {						\
	    orenew_vector(V, V->length + 1);				\
	    --V->length;						\
	}								\
    } while (0)

static void
native_SizeText(oobject_t list, oint32_t ac)
/* int32_t ttf.SizeText(ttf.font_t font, string_t text, sdl.point_t size); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_vec_pnt_t		*alist;

    alist = (nat_fnt_vec_pnt_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_point);
    make_vec_text(alist->a1);
    r0->t = t_word;
    r0->v.w = TTF_SizeText(alist->a0->__font,
			   (const char *)alist->a1->v.u8,
			   &alist->a2->x, &alist->a2->y);
}

static void
native_SizeUTF8(oobject_t list, oint32_t ac)
/* int32_t ttf.SizeUTF8(ttf.font_t font, string_t text, sdl.point_t size); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_vec_pnt_t		*alist;

    alist = (nat_fnt_vec_pnt_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_point);
    make_vec_text(alist->a1);
    r0->t = t_word;
    r0->v.w = TTF_SizeUTF8(alist->a0->__font,
			   (const char *)alist->a1->v.u8,
			   &alist->a2->x, &alist->a2->y);
}

static void
native_SizeUNICODE(oobject_t list, oint32_t ac)
/* int32_t ttf.SizeUNICODE(ttf.font_t font,
			   uint16_t text[], sdl.point_t size); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_vec_pnt_t		*alist;

    alist = (nat_fnt_vec_pnt_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint16);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_point);
    make_vec_unicode(alist->a1);
    r0->t = t_word;
    r0->v.w = TTF_SizeUNICODE(alist->a0->__font,
			      (const Uint16 *)alist->a1->v.u16,
			      &alist->a2->x, &alist->a2->y);
}

static void
native_RenderText_Solid(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderText_Solid(ttf.font_t font,
				      string_t text, sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderText_Solid(alist->a0->__font,
				   (const char *)alist->a1->v.u8,
				   *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUTF8_Solid(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUTF8_Solid(ttf.font_t font,
				      string_t text, sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderUTF8_Solid(alist->a0->__font,
				   (const char *)alist->a1->v.u8,
				   *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUNICODE_Solid(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUNICODE_Solid(ttf.font_t font,
					 uint16_t text[], sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint16);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_unicode(alist->a1);
    if ((ss = TTF_RenderUNICODE_Solid(alist->a0->__font,
				      (const Uint16 *)alist->a1->v.u16,
				      *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderGlyph_Solid(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderGlyph_Solid(ttf.font_t font,
				       uint16_t glyph, sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_u16_col_t		*alist;

    alist = (nat_fnt_u16_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    if ((ss = TTF_RenderGlyph_Solid(alist->a0->__font, alist->a1,
				    *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderText_Shaded(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderText_Shaded(ttf.font_t font, string_t text,
				       sdl.color_t fg, sdl.color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_col_t	*alist;

    alist = (nat_fnt_vec_col_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderText_Shaded(alist->a0->__font,
				    (const char *)alist->a1->v.u8,
				    *(SDL_Color *)alist->a2,
				    *(SDL_Color *)alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUTF8_Shaded(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUTF8_Shaded(ttf.font_t font, string_t text,
				       sdl.color_t fg, sdl.color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_col_t	*alist;

    alist = (nat_fnt_vec_col_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderUTF8_Shaded(alist->a0->__font,
				    (const char *)alist->a1->v.u8,
				    *(SDL_Color *)alist->a2,
				    *(SDL_Color *)alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUNICODE_Shaded(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUNICODE_Shaded(ttf.font_t font, uint16_t text[],
					  sdl.color_t fg, sdl.color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_col_t	*alist;

    alist = (nat_fnt_vec_col_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint16);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_color);
    make_vec_unicode(alist->a1);
    if ((ss = TTF_RenderUNICODE_Shaded(alist->a0->__font,
				       (const Uint16 *)alist->a1->v.u16,
				       *(SDL_Color *)alist->a2,
				       *(SDL_Color *)alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderGlyph_Shaded(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderGlyph_Shaded(ttf.font_t font, uint16_t glyph,
					sdl.color_t fg, sdl.color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_u16_col_col_t	*alist;

    alist = (nat_fnt_u16_col_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_color);
    if ((ss = TTF_RenderGlyph_Shaded(alist->a0->__font, alist->a1,
				     *(SDL_Color *)alist->a2,
				     *(SDL_Color *)alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderText_Blended(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderText_Blended(ttf.font_t font,
					string_t text, sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderText_Blended(alist->a0->__font,
				     (const char *)alist->a1->v.u8,
				     *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUTF8_Blended(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUTF8_Blended(ttf.font_t font,
					string_t text, sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderUTF8_Blended(alist->a0->__font,
				     (const char *)alist->a1->v.u8,
				     *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUNICODE_Blended(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUNICODE_Blended(ttf.font_t font,
					   uint16_t text[], sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint16);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_unicode(alist->a1);
    if ((ss = TTF_RenderUNICODE_Blended(alist->a0->__font,
					(const Uint16 *)alist->a1->v.u16,
					*(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderGlyph_Blended(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderGlyph_Blended(ttf.font_t font,
					 uint16_t glyph, sdl.color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_u16_col_t		*alist;

    alist = (nat_fnt_u16_col_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    if ((ss = TTF_RenderGlyph_Blended(alist->a0->__font, alist->a1,
				      *(SDL_Color *)alist->a2))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderText_Blended_Wrapped(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderText_Blended_Wrapped(ttf.font_t font, string_t text,
						sdl.color_t fg,
						uint32_t length); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_u32_t	*alist;

    alist = (nat_fnt_vec_col_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderText_Blended_Wrapped(alist->a0->__font,
					     (const char *)alist->a1->v.u8,
					     *(SDL_Color *)alist->a2,
					     alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUTF8_Blended_Wrapped(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUTF8_Blended_Wrapped(ttf.font_t font, string_t text,
					        sdl.color_t fg,
						uint32_t length); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_u32_t	*alist;

    alist = (nat_fnt_vec_col_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_text(alist->a1);
    if ((ss = TTF_RenderUTF8_Blended_Wrapped(alist->a0->__font,
					     (const char *)alist->a1->v.u8,
					     *(SDL_Color *)alist->a2,
					     alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_RenderUNICODE_Blended_Wrapped(oobject_t list, oint32_t ac)
/* sdl.surface_t ttf.RenderUNICODE_Blended_Wrapped(ttf.font_t font,
						   uint16_t text[],
						   sdl.color_t fg,
						   uint32_t length); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_u32_t	*alist;

    alist = (nat_fnt_vec_col_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint16);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_color);
    make_vec_unicode(alist->a1);
    if ((ss = TTF_RenderUNICODE_Blended_Wrapped(alist->a0->__font,
						(const Uint16 *)alist->a1->v.u16,
						*(SDL_Color *)alist->a2,
						alist->a3))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_GetFontKerningSize(oobject_t list, oint32_t ac)
/* int32_t ttf.GetFontKerningSize(font_t font, uint16_t prev, uint16_t ch); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_u16_u16_t		*alist;

    alist = (nat_fnt_u16_u16_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_font);
    CHECK_NULL(alist->a0->__font);
    r0->t = t_word;
    r0->v.w = TTF_GetFontKerningSize(alist->a0->__font, alist->a1, alist->a2);
}

static void
native_CloseFont(oobject_t list, oint32_t ac)
/* void ttf.CloseFont(ttf.font_t font); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_t			*alist;

    alist = (nat_fnt_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_font);
	odestroy_font(alist->a0);
    }
}

static void
native_OpenAudio(oobject_t list, oint32_t ac)
/* mix.audio_t mix.OpenAudio(int32_t frequency, uint16_t format,
			     int32_t channels, int32_t chunksize); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    oaudio_t			 oau;
    nat_i32_u16_i32_i32_t	*alist;

    alist = (nat_i32_u16_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (Mix_OpenAudio(alist->a0, alist->a1, alist->a2, alist->a3) == 0 &&
	Mix_QuerySpec(&oau.frequency, &oau.format, &oau.channels)) {
	onew_object(&thread_self->obj, t_audio, sizeof(oaudio_t));
	memcpy(thread_self->obj, &oau, sizeof(oaudio_t));
	r0->v.o = thread_self->obj;
	r0->t = t_audio;
    }
    else
	r0->t = t_void;
}

static void
native_AllocateChannels(oobject_t list, oint32_t ac)
/* int32_t mix.AllocateChannels(int32_t channels); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_AllocateChannels(alist->a0);
}

static void
native_LoadChunk(oobject_t list, oint32_t ac)
/* mix.chunk_t mix.LoadChunk(string_t path); */
{
    GET_THREAD_SELF()
    Mix_Chunk			*sc;
    ochunk_t			*oc;
    oregister_t			*r0;
    nat_vec_t			*alist;
    char			 path[BUFSIZ];

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_string);
    if (alist->a0->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->a0->v.ptr, alist->a0->length);
    path[alist->a0->length] = '\0';
    if ((sc = Mix_LoadWAV(path))) {
	onew_object(&thread_self->obj, t_chunk, sizeof(ochunk_t));
	oc = (ochunk_t *)thread_self->obj;
	oc->__chunk = sc;
	r0->v.o = thread_self->obj;
	r0->t = t_chunk;
    }
    else
	r0->t = t_void;
}

static void
channel_callback(int channel)
{
    SDL_Event			 ev;

    ev.user.type = SDL_USEREVENT + 2;
    ev.user.code = 0;
    ev.user.data1 = (oobject_t)(oword_t)channel;
    ev.user.data2 = null;
    SDL_PushEvent(&ev);
}

static void
native_PlayChannel(oobject_t list, oint32_t ac)
/* int32_t mix.PlayChannel(int32_t channel,
			   mix.chunk_t chunk, int32_t loops); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_chu_i32_t		*alist;

    alist = (nat_i32_chu_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_chunk);
    CHECK_NULL(alist->a1->__chunk);
    r0->t = t_word;
    r0->v.w = Mix_PlayChannel(alist->a0, alist->a1->__chunk, alist->a2);
}

static void
native_FadeInChannel(oobject_t list, oint32_t ac)
/* int32_t mix.FadeInChannel(int32_t channel, mix.chunk_t chunk,
			     int32_t loops, int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_chu_i32_i32_t	*alist;

    alist = (nat_i32_chu_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_chunk);
    CHECK_NULL(alist->a1->__chunk);
    r0->t = t_word;
    r0->v.w = Mix_FadeInChannel(alist->a0, alist->a1->__chunk,
				alist->a2, alist->a3);
}

static void
native_VolumeChunk(oobject_t list, oint32_t ac)
/* int32_t mix.VolumeChunk(mix.chunk_t chunk, int32_t volume); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_chu_i32_t		*alist;

    alist = (nat_chu_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_chunk);
    CHECK_NULL(alist->a0->__chunk);
    r0->t = t_word;
    r0->v.w = Mix_VolumeChunk(alist->a0->__chunk, alist->a1);
}

static void
native_SetPanning(oobject_t list, oint32_t ac)
/* int32_t mix.SetPanning(int32_t channel, uint8_t left, uint8_t right); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_u8_u8_t		*alist;

    alist = (nat_i32_u8_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetPanning(alist->a0, alist->a1, alist->a2);
}

static void
native_SetPosition(oobject_t list, oint32_t ac)
/* int32_t mix.SetPosition(int32_t channel, int16_t angle, uint8_t dist); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i16_u8_t		*alist;

    alist = (nat_i32_i16_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetPosition(alist->a0, alist->a1, alist->a2);
}

static void
native_SetDistance(oobject_t list, oint32_t ac)
/* int32_t mix.SetDistance(int32_t channel, uint8_t dist); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_u8_t		*alist;

    alist = (nat_i32_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetDistance(alist->a0, alist->a1);
}

static void
native_SetReverseStereo(oobject_t list, oint32_t ac)
/* int32_t mix.SetReverseStereo(int32_t channel, int8_t flip); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i8_t		*alist;

    alist = (nat_i32_i8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetReverseStereo(alist->a0, alist->a1);
}

static void
native_FadeOutChannel(oobject_t list, oint32_t ac)
/* int32_t mix.FadeOutChannel(int32_t channel, int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadeOutChannel(alist->a0, alist->a1);
}

static void
native_Playing(oobject_t list, oint32_t ac)
/* int32_t mix.Playing(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_Playing(alist->a0);
}

static void
native_FadingChannel(oobject_t list, oint32_t ac)
/* int32_t mix.FadingChannel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadingChannel(alist->a0);
}

static void
native_ExpireChannel(oobject_t list, oint32_t ac)
/* int32_t mix.ExpireChannel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_ExpireChannel(alist->a0, alist->a1);
}

static void
native_Pause(oobject_t list, oint32_t ac)
/* void mix.Pause(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_Pause(alist->a0);
}

static void
native_Resume(oobject_t list, oint32_t ac)
/* void mix.Resume(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_Resume(alist->a0);
}

static void
native_Paused(oobject_t list, oint32_t ac)
/* void mix.Paused(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_Paused(alist->a0);
}

static void
native_HaltChannel(oobject_t list, oint32_t ac)
/* int32_t mix.HaltChannel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_HaltChannel(alist->a0);
}

static void
native_FreeChunk(oobject_t list, oint32_t ac)
/* void mix.FreeChunk(mix.chunk_t chunk); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_chu_t			*alist;

    alist = (nat_chu_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_chunk);
	odestroy_chunk(alist->a0);
    }
}

static void
native_GroupChannel(oobject_t list, oint32_t ac)
/* int32_t mix.GroupChannel(int32_t channel, int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupChannel(alist->a0, alist->a1);
}

static void
native_GroupChannels(oobject_t list, oint32_t ac)
/* int32_t mix.GroupChannels(int32_t from_channel, int32_t to_channel,
			     int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_i32_t		*alist;

    alist = (nat_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupChannels(alist->a0, alist->a1, alist->a2);
}

static void
native_GroupAvailable(oobject_t list, oint32_t ac)
/* int32_t mix.GroupAvailable(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupAvailable(alist->a0);
}

static void
native_GroupCount(oobject_t list, oint32_t ac)
/* int32_t mix.GroupCount(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupCount(alist->a0);
}

static void
native_GroupOldest(oobject_t list, oint32_t ac)
/* int32_t mix.GroupOldest(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupOldest(alist->a0);
}

static void
native_GroupNewer(oobject_t list, oint32_t ac)
/* int32_t mix.GroupNewer(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupNewer(alist->a0);
}

static void
native_FadeOutGroup(oobject_t list, oint32_t ac)
/* int32_t mix.FadeOutGroup(int32_t tag, int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadeOutGroup(alist->a0, alist->a1);
}

static void
native_HaltGroup(oobject_t list, oint32_t ac)
/* int32_t mix.HaltGroup(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_HaltGroup(alist->a0);
}

static void
native_LoadMusic(oobject_t list, oint32_t ac)
/* mix.music_t mix.LoadMusic(string_t path); */
{
    GET_THREAD_SELF()
    Mix_Music			*sm;
    omusic_t			*om;
    oregister_t			*r0;
    nat_vec_t			*alist;
    char			 path[BUFSIZ];

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_string);
    if (alist->a0->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->a0->v.ptr, alist->a0->length);
    path[alist->a0->length] = '\0';
    if ((sm = Mix_LoadMUS(path))) {
	onew_object(&thread_self->obj, t_music, sizeof(omusic_t));
	om = (omusic_t *)thread_self->obj;
	om->__music = sm;
	om->type = Mix_GetMusicType(sm);
	r0->v.o = thread_self->obj;
	r0->t = t_music;
    }
    else
	r0->t = t_void;
}

static void
music_callback(void)
{
    SDL_Event			 ev;

    ev.user.type = SDL_USEREVENT + 1;
    ev.user.code = 0;
    ev.user.data1 = null;
    ev.user.data2 = null;
    SDL_PushEvent(&ev);
}

static void
native_PlayMusic(oobject_t list, oint32_t ac)
/* int32_t mix.PlayMusic(mix.music_t music, int32_t loops); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_mus_i32_t		*alist;

    alist = (nat_mus_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_music);
    CHECK_NULL(alist->a0->__music);
    r0->t = t_word;
    r0->v.w = Mix_PlayMusic(alist->a0->__music, alist->a1);
}

static void
native_FadeInMusic(oobject_t list, oint32_t ac)
/* int32_t mix.FadeInMusic(mix.music_t music, int32_t loops, int ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_mus_i32_i32_t		*alist;

    alist = (nat_mus_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_music);
    CHECK_NULL(alist->a0->__music);
    r0->t = t_word;
    r0->v.w = Mix_FadeInMusic(alist->a0->__music, alist->a1, alist->a2);
}

static void
native_VolumeMusic(oobject_t list, oint32_t ac)
/* int32_t mix.VolumeMusic(uint8_t volume); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_VolumeMusic(alist->a0);
}

static void
native_PlayingMusic(oobject_t list, oint32_t ac)
/* int32_t mix.PlayingMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_PlayingMusic();
}

static void
native_FadeOutMusic(oobject_t list, oint32_t ac)
/* int32_t mix.FadeOutMusic(int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadeOutMusic(alist->a0);
}

static void
native_FadingMusic(oobject_t list, oint32_t ac)
/* int32_t mix.FadingMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadingMusic();
}

static void
native_PauseMusic(oobject_t list, oint32_t ac)
/* void mix.PauseMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_PauseMusic();
}

static void
native_ResumeMusic(oobject_t list, oint32_t ac)
/* void mix.ResumeMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_ResumeMusic();
}

static void
native_RewindMusic(oobject_t list, oint32_t ac)
/* void mix.RewindMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_RewindMusic();
}

static void
native_PausedMusic(oobject_t list, oint32_t ac)
/* void mix.PausedMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_PausedMusic();
}

static void
native_SetMusicPosition(oobject_t list, oint32_t ac)
/* int32_t mix.SetMusicPosition(float64_t position); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_f64_t			*alist;

    alist = (nat_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetMusicPosition(alist->a0);
}

static void
native_HaltMusic(oobject_t list, oint32_t ac)
/* int32_t mix.HaltMusic(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_HaltMusic();
}

static void
native_FreeMusic(oobject_t list, oint32_t ac)
/* void mix.FreeMusic(mix.music_t music); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_mus_t			*alist;

    alist = (nat_mus_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_music);
	odestroy_music(alist->a0);
    }
}

static void
native_CloseAudio(oobject_t list, oint32_t ac)
/* void mix.CloseAudio(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_CloseAudio();
}

#if __WORDSIZE == 32
/*   This is only required because the returned value may be
 * stored in an auto variable, and then sign will matter,
 * otherwise, it will just make a trip to a mpz_t (without
 * memory allocation/deallocation) */
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

static void
native_ResolveHost(oobject_t list, oint32_t ac)
/* int32_t net.ResolveHost(net.address_t addr, string_t host, uint16_t port); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_adr_vec_u16_t			*alist;

    alist = (nat_adr_vec_u16_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_net_address);
    if (alist->a1) {
	CHECK_TYPE(alist->a1, t_string);
	make_vec_text(alist->a1);
	r0->v.w = SDLNet_ResolveHost((IPaddress *)alist->a0,
				     alist->a1->v.obj, alist->a2);
    }
    else
	r0->v.w = SDLNet_ResolveHost((IPaddress *)alist->a0, null, alist->a2);
}

static void
native_ResolveIP(oobject_t list, oint32_t ac)
/* string_t  net.ResolveIP( net.address_t addr); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_adr_t				*alist;
    oword_t				 length;
    const char				*address;

    alist = (nat_adr_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_net_address);
    if ((address = SDLNet_ResolveIP((IPaddress *)alist->a0))) {
	length = strlen(address);
	if (address_vector == null)
	    onew_vector((oobject_t *)&address_vector, t_uint8, length);
	else if (length != address_vector->length)
	    orenew_vector(address_vector, length);
	memcpy(address_vector->v.ptr, address, length);
	r0->t = t_string;
	r0->v.o = address_vector;
    }
    else
	r0->t = t_void;
}

static void
native_GetLocalAddresses(oobject_t list, oint32_t ac)
/* sdl.address_t  net.GetLocalAddresses()[]; */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    int					 length;
    int					 offset;
    ovector_t				*vector;
    onet_address_t			*address;
    IPaddress				 addresses[256];

    r0 = &thread_self->r0;
    if ((length = SDLNet_GetLocalAddresses(addresses, 256))) {
	onew_vector(&thread_self->obj, t_net_address, length);
	vector = thread_self->obj;
	for (offset = 0; offset < length; offset++) {
	    onew_object(vector->v.ptr + offset,
			t_net_address, sizeof(onet_address_t));
	    address = vector->v.ptr[offset];
	    address->host = addresses[offset].host;
	    address->port = addresses[offset].port;
	}
	r0->v.o = vector;
	r0->t = t_vector|t_net_address;
    }
    else
	r0->t = t_void;
}

static void
native_AllocSocketSet(oobject_t list, oint32_t ac)
/* net.socket_set_t net.AllocSocketSet(int32_t maxsockets); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    osocket_set_t			*os;
    SDLNet_SocketSet			 ss;
    nat_i32_t				*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    if ((ss = SDLNet_AllocSocketSet(alist->a0))) {
	onew_object(&thread_self->obj, t_socket_set, sizeof(osocket_set_t));
	os = thread_self->obj;
	os->__set = ss;
	r0->v.o = os;
	r0->t = t_socket_set;
    }
    else
	r0->t = t_void;
}

static void
native_AddSocket(oobject_t list, oint32_t ac)
/* int32_t net.AddSocket(net.socket_set_t set,
			 (net.tcp.socket_t|net.udp.socket_t) sock); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_set_tso_t			*alist;

    alist = (nat_set_tso_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_socket_set);
    CHECK_NULL(alist->a0->__set);
    CHECK_NULL(alist->a1);
    switch (otype(alist->a1)) {
	case t_tcp_socket:
	case t_udp_socket:
	    CHECK_NULL(alist->a1->__socket);
	    r0->v.w = SDLNet_AddSocket(alist->a0->__set,
				       (SDLNet_GenericSocket)
				       alist->a1->__socket);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
	    break;
    }
}

static void
native_DelSocket(oobject_t list, oint32_t ac)
/* int32_t net.DelSocket(net.socket_set_t set,
			 (net.tcp.socket_t|net.udp.socket_t) sock); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_set_tso_t			*alist;

    alist = (nat_set_tso_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_socket_set);
    CHECK_NULL(alist->a0->__set);
    CHECK_NULL(alist->a1);
    switch (otype(alist->a1)) {
	case t_tcp_socket:
	case t_udp_socket:
	    CHECK_NULL(alist->a1->__socket);
	    r0->v.w = SDLNet_DelSocket(alist->a0->__set,
				       (SDLNet_GenericSocket)
				       alist->a1->__socket);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
	    break;
    }
}

static void
native_CheckSockets(oobject_t list, oint32_t ac)
/* int32_t net.CheckSockets(net.socket_set_t set, uint32_t timeout); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_set_u32_t			*alist;

    alist = (nat_set_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_socket_set);
    CHECK_NULL(alist->a0->__set);
    r0->v.w = SDLNet_CheckSockets(alist->a0->__set, alist->a1);
}

static void
native_SocketReady(oobject_t list, oint32_t ac)
/* int32_t net.SocketReady((net.tcp.socket_t|net.udp.socket_t) sock); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tso_t				*alist;

    alist = (nat_tso_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    switch (otype(alist->a0)) {
	case t_tcp_socket:
	case t_udp_socket:
	    CHECK_NULL(alist->a0->__socket);
	    r0->v.w = SDLNet_SocketReady((SDLNet_GenericSocket)
					 alist->a0->__socket);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
	    break;
    }
}

static void
native_FreeSocketSet(oobject_t list, oint32_t ac)
/* void net.FreeSocketSet(net.socket_set_t set); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_set_t				*alist;

    alist = (nat_set_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_socket_set);
    odestroy_socket_set(alist->a0);
}

static void
native_tcp_Open(oobject_t list, oint32_t ac)
/* net.tcp.socket_t net.tcp.Open(net.address_t ip); */
{
    GET_THREAD_SELF()
    TCPsocket				 ss;
    otcp_socket_t			*os;
    oregister_t				*r0;
    nat_adr_t				*alist;

    r0 = &thread_self->r0;
    alist = (nat_adr_t *)list;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_net_address);
    if ((ss = SDLNet_TCP_Open((IPaddress *)alist->a0))) {
	onew_object(&thread_self->obj, t_tcp_socket, sizeof(otcp_socket_t));
	os = (otcp_socket_t *)thread_self->obj;
	os->__socket = ss;
	r0->t = t_tcp_socket;
	r0->v.o = os;
    }
    else
	r0->t = t_void;
}

static void
native_tcp_Accept(oobject_t list, oint32_t ac)
/* net.tcp.socket_t net.tcp.Accept(net.tcp.socket_t server); */
{
    GET_THREAD_SELF()
    TCPsocket				 ss;
    otcp_socket_t			*os;
    oregister_t				*r0;
    nat_tso_t				*alist;

    r0 = &thread_self->r0;
    alist = (nat_tso_t *)list;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tcp_socket);
    CHECK_NULL(alist->a0->__socket);
    if ((ss = SDLNet_TCP_Accept(alist->a0->__socket))) {
	onew_object(&thread_self->obj, t_tcp_socket, sizeof(otcp_socket_t));
	os = (otcp_socket_t *)thread_self->obj;
	os->__socket = ss;
	r0->t = t_tcp_socket;
	r0->v.o = os;
    }
    else
	r0->t = t_void;
}

static void
native_tcp_GetPeerAddress(oobject_t list, oint32_t ac)
/* net.address_t net.tcp.GetPeerAddress(net.tcp.socket_t socket); */
{
    GET_THREAD_SELF()
    IPaddress				*si;
    onet_address_t			*oi;
    oregister_t				*r0;
    nat_tso_t				*alist;

    r0 = &thread_self->r0;
    alist = (nat_tso_t *)list;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tcp_socket);
    CHECK_NULL(alist->a0->__socket);
    if ((si = SDLNet_TCP_GetPeerAddress(alist->a0->__socket))) {
	onew_object(&thread_self->obj, t_net_address, sizeof(onet_address_t));
	oi = (onet_address_t *)thread_self->obj;
	oi->host = si->host;
	oi->port = si->port;
	r0->t = t_net_address;
	r0->v.o = oi;
    }
    else
	r0->t = t_void;
}

static void
native_tcp_Send(oobject_t list, oint32_t ac)
/* int32_t net.tcp.Send(net.tcp.socket_t sock, (int8_t|uint8_t|
						int16_t|uint16_t|
						int32_t|uint32_t) data[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_tso_vec_t		*alist;
    oword_t			 offset;
    oword_t			 length;

    alist = (nat_tso_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tcp_socket);
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1) & ~t_vector) {
	case t_int8:		case t_uint8:
	    r0->v.w = SDLNet_TCP_Send(alist->a0->__socket,
				      alist->a1->v.obj, alist->a1->length);
	    break;
	case t_int16:		case t_uint16:
	    length = alist->a1->length << 1;
	    if (thr_str->length < length)
		orenew_vector(thr_str, length);
	    for (offset = 0; offset < alist->a1->length; offset++)
		SDLNet_Write16(alist->a1->v.u16[offset],
			       thr_str->v.u16 + offset);
	    r0->v.w = SDLNet_TCP_Send(alist->a0->__socket,
				      thr_str->v.obj, length) >> 1;
	    break;
	case t_int32:		case t_uint32:
	    length = alist->a1->length << 2;
	    if (thr_str->length < length)
		orenew_vector(thr_str, length);
	    for (offset = 0; offset < alist->a1->length; offset++)
		SDLNet_Write32(alist->a1->v.u32[offset],
			       thr_str->v.u32 + offset);
	    r0->v.w = SDLNet_TCP_Send(alist->a0->__socket,
				      thr_str->v.obj, length) >> 2;
	    break;
	default:
	    othrow(except_type_mismatch);
	    break;
    }
}

static void
native_tcp_Recv(oobject_t list, oint32_t ac)
/* int32_t net.tcp.Recv(net.tcp.socket_t sock, (int8_t|uint8_t|
						int16_t|uint16_t|
						int32_t|uint32_t) data[],
			 int32_t length); */
{

    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_tso_vec_i32_t		*alist;
    oword_t			 offset;
    oword_t			 length;

    alist = (nat_tso_vec_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tcp_socket);
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1) & ~t_vector) {
	case t_int8:		case t_uint8:
	    length = alist->a2;
	    if (thr_str->length < length)
		orenew_vector(thr_str, length);
	    if ((length = SDLNet_TCP_Recv(alist->a0->__socket,
					  thr_str->v.obj,
					  alist->a2)) >= 0) {
		if (alist->a1->length != length)
		    orenew_vector(alist->a1, length);
		memcpy(alist->a1->v.obj, thr_str->v.obj, length);
	    }
	    else
		orenew_vector(alist->a1, 0);
	    break;
	case t_int16:		case t_uint16:
	    length = alist->a2 << 1;
	    if (thr_str->length < length)
		orenew_vector(thr_str, length);
	    if ((length = SDLNet_TCP_Recv(alist->a0->__socket,
					  thr_str->v.obj,
					  length) << 1) >= 0) {
		if (alist->a1->length != length)
		    orenew_vector(alist->a1, length);
		for (offset = 0; offset < length; offset++)
		    alist->a1->v.u16[offset] =
			SDLNet_Read16(thr_str->v.u16 + offset);
	    }
	    break;
	case t_int32:		case t_uint32:
	    length = alist->a2 << 2;
	    if (thr_str->length < length)
		orenew_vector(thr_str, length);
	    if ((length = SDLNet_TCP_Recv(alist->a0->__socket,
					  thr_str->v.obj,
					  alist->a2) << 2) >= 0) {
		if (alist->a1->length != length)
		    orenew_vector(alist->a1, length);
		for (offset = 0; offset < r0->v.w; offset++)
		    alist->a1->v.u32[offset] =
			SDLNet_Read32(thr_str->v.u32 + offset);
	    }
	    break;
	default:
	    othrow(except_type_mismatch);
	    break;
    }
    r0->v.w = length;
}

static void
native_tcp_Close(oobject_t list, oint32_t ac)
/*void net.tcp.Close(net.tcp.socket_t sock); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tso_t				*alist;

    r0 = &thread_self->r0;
    r0->t = t_void;
    alist = (nat_tso_t *)list;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_tcp_socket);
	odestroy_tcp_socket(alist->a0);
    }
}

static void
native_tcp_AddSocket(oobject_t list, oint32_t ac)
/* int32_t net.tcp.AddSocket(net.socket_set_t set, net.tcp.socket_t sock); */
{
    native_AddSocket(list, ac);
}

static void
native_tcp_DelSocket(oobject_t list, oint32_t ac)
/* int32_t net.tcp.DelSocket(net.socket_set_t set, net.tcp.socket_t sock); */
{
    native_DelSocket(list, ac);
}

static void
native_udp_Open(oobject_t list, oint32_t ac)
/* net.udp.socket_t net.udp.Open(uint16_t port); */
{
    GET_THREAD_SELF()
    UDPsocket				 ss;
    oudp_socket_t			*os;
    oregister_t				*r0;
    nat_u16_t				*alist;

    r0 = &thread_self->r0;
    alist = (nat_u16_t *)list;
    if ((ss = SDLNet_UDP_Open(alist->a0))) {
	onew_object(&thread_self->obj, t_udp_socket, sizeof(oudp_socket_t));
	os = (oudp_socket_t *)thread_self->obj;
	os->__socket = ss;
	r0->t = t_udp_socket;
	r0->v.o = os;
    }
    else
	r0->t = t_void;
}

static void
native_udp_SetPacketLoss(oobject_t list, oint32_t ac)
/* void net.udp.SetPacketLoss(udp.socket_t sock, int32_t percent); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_uso_i32_t			*alist;

    r0 = &thread_self->r0;
    alist = (nat_uso_i32_t *)list;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_udp_socket);
    CHECK_NULL(alist->a0->__socket);
    SDLNet_UDP_SetPacketLoss(alist->a0->__socket, alist->a1);
}

static void
native_udp_Bind(oobject_t list, oint32_t ac)
/* int32_t net.udp.Bind(net.udp.socket_t sock,
			int32_t channel, net.address_t address); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_uso_i32_adr_t			*alist;

    r0 = &thread_self->r0;
    alist = (nat_uso_i32_adr_t *)list;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_udp_socket);
    CHECK_NULL(alist->a0->__socket);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_net_address);
    SDLNet_UDP_Bind(alist->a0->__socket, alist->a1,
		    (const IPaddress *)alist->a2);
}

static void
native_udp_Unbind(oobject_t list, oint32_t ac)
/* int32_t net.udp.Unbind(net.udp.socket_t sock, int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_uso_i32_t			*alist;

    r0 = &thread_self->r0;
    alist = (nat_uso_i32_t *)list;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_udp_socket);
    CHECK_NULL(alist->a0->__socket);
    SDLNet_UDP_Unbind(alist->a0->__socket, alist->a1);
}

static void
native_udp_GetPeerAddress(oobject_t list, oint32_t ac)
/* net.address_t net.udp.GetPeerAddress(net.udp.socket_t socket,
					int32_t channel); */
{
    GET_THREAD_SELF()
    IPaddress				*sa;
    onet_address_t			*oa;
    oregister_t				*r0;
    nat_uso_i32_t			*alist;

    r0 = &thread_self->r0;
    alist = (nat_uso_i32_t *)list;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_udp_socket);
    CHECK_NULL(alist->a0->__socket);
    if ((sa = SDLNet_UDP_GetPeerAddress(alist->a0->__socket, alist->a1))) {
	onew_object(&thread_self->obj, t_net_address, sizeof(onet_address_t));
	oa = (onet_address_t *)thread_self->obj;
	oa->host = sa->host;
	oa->port = sa->port;
	r0->t = t_net_address;
	r0->v.o = oa;
    }
    else
	r0->t = t_void;
}

static void
native_udp_Send(oobject_t list, oint32_t ac)
/* int32_t net.udp.Send(net.udp.socket_t socket,
			int32_t channel, net.udp.packet_t packet); */
{
    GET_THREAD_SELF()
    UDPpacket				*sp;
    oudp_packet_t			*op;
    oregister_t				*r0;
    nat_uso_i32_pkt_t			*alist;

    r0 = &thread_self->r0;
    alist = (nat_uso_i32_pkt_t *)list;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_udp_socket);
    CHECK_NULL(alist->a0->__socket);
    op = alist->a2;
    CHECK_NULL(op);
    CHECK_TYPE(op, t_udp_packet);
    /* only accept string buffers; so, does not byte swap, etc */
    CHECK_NULL(op->data);
    CHECK_TYPE(op->data, t_string);
    if ((sp = op->__packet) == null) {
	sp = op->__packet = SDLNet_AllocPacket((op->data->length + 15) & -16);
	if (sp == null)
	    ovm_raise(except_null_dereference);
    }
    else if (sp->maxlen < op->data->length) {
	SDLNet_ResizePacket(sp, (op->data->length + 15) & -16);
	if (sp->maxlen < op->data->length)
	    ovm_raise(except_out_of_bounds);
    }
    sp->channel = op->channel;
    memcpy(sp->data, op->data->v.obj, op->data->length);
    sp->address.host = op->host;
    sp->address.port = op->port;
    sp->len = op->data->length;
    r0->v.w = SDLNet_UDP_Send(alist->a0->__socket, alist->a1, sp);
    op->status = sp->status;
}

static void
native_udp_Recv(oobject_t list, oint32_t ac)
/* int32_t net.udp.Recv(net.udp.socket_t socket,
			net.udp.packet_t packet, int32_t length); */
{
    GET_THREAD_SELF()
    UDPpacket				*sp;
    oudp_packet_t			*op;
    oregister_t				*r0;
    nat_uso_pkt_i32_t			*alist;

    r0 = &thread_self->r0;
    alist = (nat_uso_pkt_i32_t *)list;
    r0->t = t_word;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_udp_socket);
    CHECK_NULL(alist->a0->__socket);
    op = alist->a1;
    CHECK_NULL(op);
    CHECK_TYPE(op, t_udp_packet);
    /* only accept string buffers; so, does not byte swap, etc */
    CHECK_NULL(op->data);
    CHECK_TYPE(op->data, t_string);
    if (alist->a2 > op->data->length)
	orenew_vector(op->data, alist->a2);
    if ((sp = op->__packet) == null) {
	sp = op->__packet = SDLNet_AllocPacket((op->data->length + 15) & -16);
	if (sp == null)
	    ovm_raise(except_null_dereference);
    }
    else if (sp->maxlen < op->data->length) {
	SDLNet_ResizePacket(sp, (op->data->length + 15) & -16);
	if (sp->maxlen < op->data->length)
	    ovm_raise(except_out_of_bounds);
    }
    sp->channel = op->channel;
    sp->address.host = op->host;
    sp->address.port = op->port;
    r0->v.w = SDLNet_UDP_Recv(alist->a0->__socket, sp);
    op->channel = sp->channel;
    if (sp->len > 0)
	memcpy(op->data->v.obj, sp->data, sp->len);
    if (op->data->length != sp->len)
	orenew_vector(op->data, sp->len);
    op->status = sp->status;
}

static void
native_udp_Close(oobject_t list, oint32_t ac)
/* void net.udp.Close(net.udp.socket_t socket); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_uso_t				*alist;

    r0 = &thread_self->r0;
    r0->t = t_void;
    alist = (nat_uso_t *)list;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_udp_socket);
	odestroy_udp_socket(alist->a0);
    }
}

static void
native_udp_AddSocket(oobject_t list, oint32_t ac)
/* int32_t net.udp.AddSocket(net.socket_set_t set, net.udp.socket_t sock); */
{
    native_AddSocket(list, ac);
}

static void
native_udp_DelSocket(oobject_t list, oint32_t ac)
/* int32_t net.udp.DelSocket(net.socket_set_t set, net.udp.socket_t sock); */
{
    native_DelSocket(list, ac);
}

static void
native_ResetAttributes(oobject_t list, oint32_t ac)
/* void sdl.gl.ResetAttributes(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    SDL_GL_ResetAttributes();
    r0->t = t_void;
}

static void
native_SetAttribute(oobject_t list, oint32_t ac)
/* int32_t sdl.gl.SetAttribute(int32_t attr, int32_t value); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_t			*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = SDL_GL_SetAttribute(alist->a0, alist->a1);
}

static void
native_GetAttribute(oobject_t list, oint32_t ac)
/* int32_t sdl.gl.GetAttribute(int32_t attr, int32_t value[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_vec_t			*alist;

    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_int32);
    if (alist->a1->length != 1)
	orenew_vector(alist->a1, 1);
    r0->t = t_word;
    r0->v.w = SDL_GL_GetAttribute(alist->a0, alist->a1->v.i32);
}

static void
native_CreateContext(oobject_t list, oint32_t ac)
/* context_t sdl.gl.CreateContext(sdl.window_t window); */
{
    GET_THREAD_SELF()
    SDL_GLContext			 sc;
    ocontext_t				*oc;
    oregister_t				*r0;
    nat_win_t				*alist;

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_window);
    CHECK_NULL(alist->a0->__window);
    if ((sc = SDL_GL_CreateContext(alist->a0->__window))) {
	onew_object(&thread_self->obj, t_context, sizeof(ocontext_t));
	oc = (ocontext_t *)thread_self->obj;
	oc->__context = sc;
	r0->v.o = thread_self->obj;
	r0->t = t_context;
    }
    else
	r0->t = t_void;
}

static void
native_MakeCurrent(oobject_t list, oint32_t ac)
/* int32_t sdl.gl.MakeCurrent(sdl.window_t window, sdl.gl.context_t context); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_win_ctx_t			*alist;

    alist = (nat_win_ctx_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_window);
    CHECK_NULL(alist->a0->__window);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_context);
    CHECK_NULL(alist->a1->__context);
    r0->t = t_word;
    r0->v.w = SDL_GL_MakeCurrent(alist->a0->__window, alist->a1->__context);
    current_window = alist->a0;
    current_context = alist->a1;
}

static void
native_GetCurrentWindow(oobject_t list, oint32_t ac)
/* sdl.window_t sdl.gl.GetCurrentWindow(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    if (current_window) {
	assert(current_window->__window == SDL_GL_GetCurrentWindow());
	r0->v.o = current_window;
	r0->t = t_window;
    }
    else
	r0->t = t_void;
}

static void
native_GetCurrentContext(oobject_t list, oint32_t ac)
/* sdl.gl.context_t sdl.gl.GetCurrentContext(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    if (current_context) {
	assert(current_context->__context == SDL_GL_GetCurrentContext());
	r0->v.o = current_context;
	r0->t = t_context;
    }
    else
	r0->t = t_void;
}

static void
native_SetSwapInterval(oobject_t list, oint32_t ac)
/* int32_t sdl.gl.SetSwapInterval(int8_t interval); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i8_t				*alist;

    alist = (nat_i8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = SDL_GL_SetSwapInterval(alist->a0);
}

static void
native_GetSwapInterval(oobject_t list, oint32_t ac)
/* int8_t sdl.gl.GetSwapInterval(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = SDL_GL_GetSwapInterval();
}

static void
native_SwapWindow(oobject_t list, oint32_t ac)
/* void sdl.gl.SwapWindow(sdl.window_t window); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_win_t				*alist;

    alist = (nat_win_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_window);
    CHECK_NULL(alist->a0->__window);
    r0->t = t_void;
    SDL_GL_SwapWindow(alist->a0->__window);
}

static void
native_DeleteContext(oobject_t list, oint32_t ac)
/* void sdl.gl.DeleteContext(sdl.gl.context_t context); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_ctx_t				*alist;

    alist = (nat_ctx_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_context);
	odestroy_context(alist->a0);
    }
}

static void
native_ReadPixels(oobject_t list, oint32_t ac)
/* sdl.surface_t sdl.gl.ReadPixels(int32_t x, int32_t y,
				   int32_t width, int32_t height); */
{
    GET_THREAD_SELF()
    SDL_Surface				*ss;
    osurface_t				*os;
    oregister_t				*r0;
    nat_i32_i32_i32_i32_u32_t		*alist;

    alist = (nat_i32_i32_i32_i32_u32_t *)list;
    r0 = &thread_self->r0;
    if ((ss = SDL_CreateRGBSurface(0, alist->a2, alist->a3,
				   32, R_MASK, G_MASK, B_MASK, A_MASK))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	query_surface(os);
	glReadPixels(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4,
		     GL_RGBA, ss->pixels);
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
query_format(SDL_Surface *surface, uint32_t *internal, uint32_t *format)
{
    if (internal)
	*internal = surface->format->BytesPerPixel;
    switch (surface->format->BytesPerPixel) {
	case 3:
	    if (surface->format->Rmask == 0x000000ff)
		*format = GL_RGB;
	    else
		*format = GL_BGR;
	    break;
	case 4:
	    if (surface->format->Rmask == 0x000000ff)
		*format = GL_RGBA;
	    else
		*format = GL_BGRA;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

static void
native_DrawPixels(oobject_t list, oint32_t ac)
/* void sdl.gl.DrawPixels(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Surface				*ss;
    oregister_t				*r0;
    nat_srf_t				*alist;
    ouint32_t				 format;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    ss = alist->a0->__surface;
    query_format(ss, null, &format);
    r0->t = t_void;
    glDrawPixels(ss->w, ss->h, format, GL_UNSIGNED_BYTE, ss->pixels);
}

static void
native_TexImage1D(oobject_t list, oint32_t ac)
/* void sdl.gl.TexImage1D(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Surface				*ss;
    oregister_t				*r0;
    nat_srf_t				*alist;
    ouint32_t				 format;
    ouint32_t				 internal;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    ss = alist->a0->__surface;
    query_format(ss, &internal, &format);
    r0->t = t_void;
    glTexImage1D(GL_TEXTURE_1D, 0, internal, ss->w, 0, format,
		 GL_UNSIGNED_BYTE, ss->pixels);
}

static void
native_TexImage2D(oobject_t list, oint32_t ac)
/* void sdl.gl.TexImage2D(sdl.surface_t surf); */
{
    GET_THREAD_SELF()
    SDL_Surface				*ss;
    oregister_t				*r0;
    nat_srf_t				*alist;
    ouint32_t				 format;
    ouint32_t				 internal;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    ss = alist->a0->__surface;
    query_format(ss, &internal, &format);
    r0->t = t_void;
    glTexImage2D(GL_TEXTURE_2D, 0, internal, ss->w, ss->h, 0, format,
		 GL_UNSIGNED_BYTE, ss->pixels);
}


static void
native_TexSubImage1D(oobject_t list, oint32_t ac)
/* void sdl.gl.TexSubImage1D(sdl.surface_t surf,
			     int32_t xoff, int32_t width); */
{
    GET_THREAD_SELF()
    SDL_Surface				*ss;
    oregister_t				*r0;
    nat_srf_i32_i32_t			*alist;
    ouint32_t				 format;

    alist = (nat_srf_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    ss = alist->a0->__surface;
    r0->t = t_void;
    query_format(ss, null, &format);
    glTexSubImage1D(GL_TEXTURE_1D, 0, alist->a1, alist->a2, format,
		    GL_UNSIGNED_BYTE, ss->pixels);
}

static void
native_TexSubImage2D(oobject_t list, oint32_t ac)
/* void sdl.gl.TexSubImage2D(sdl.surface_t surf, int32_t xoff, int32_t yoff,
			     int32_t width, int32_t height); */
{
    GET_THREAD_SELF()
    SDL_Surface				*ss;
    oregister_t				*r0;
    nat_srf_i32_i32_i32_i32_t		*alist;
    ouint32_t				 format;

    alist = (nat_srf_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_surface);
    CHECK_NULL(alist->a0->__surface);
    ss = alist->a0->__surface;
    if ((ouint32_t)alist->a1 > ss->w || (ouint32_t)alist->a3 > ss->w ||
	alist->a1 + alist->a3 > ss->w ||
	(ouint32_t)alist->a2 > ss->h ||	(ouint32_t)alist->a4 > ss->h ||
	alist->a2 + alist->a4 > ss->w)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    query_format(ss, null, &format);
    glTexSubImage2D(GL_TEXTURE_2D, 0, alist->a1, alist->a2,
		    alist->a3, alist->a4, format, GL_UNSIGNED_BYTE, ss->pixels);
}
