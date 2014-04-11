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
    ofloat64_t		 f64;
} nat_f64_t;
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
    ovector_t		*vec;
} nat_vec_t;
typedef struct {
    oevent_t		*ev;
} nat_ev_t;
typedef struct {
    osurface_t		*srf;
} nat_srf_t;
typedef struct {
    ofont_t		*fnt;
} nat_fnt_t;
typedef struct {
    otimer_t		*tm;
} nat_tm_t;
typedef struct {
    ochunk_t		*chu;
} nat_chu_t;
typedef struct {
    omusic_t		*mus;
} nat_mus_t;
typedef struct {
    oint32_t		 i32;
    oint8_t		 i8;
} nat_i32_i8_t;
typedef struct {
    oint32_t		 i32;
    ouint8_t		 u8;
} nat_i32_u8_t;
typedef struct {
    oint32_t		 si0;
    ouint8_t		 si1;
} nat_i32_i32_t;
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
    ovector_t		*vec;
    oint32_t		 i32;
} nat_vec_i32_t;
typedef struct {
    ochunk_t		*chu;
    oint32_t		 i32;
} nat_chu_i32_t;
typedef struct {
    omusic_t		*mus;
    oint32_t		 i32;
} nat_mus_i32_t;
typedef struct {
    ofont_t		*fnt;
    ouint16_t		 u16;
} nat_fnt_u16_t;
typedef struct {
    oint32_t		 i32;
    ouint8_t		 ui0;
    ouint8_t		 ui1;
} nat_i32_u8_u8_t;
typedef struct {
    oint32_t		 i32;
    oint16_t		 i16;
    uint8_t		 u8;
} nat_i32_i16_u8_t;
typedef struct {
    oint32_t		 si0;
    oint32_t		 si1;
    oint32_t		 si2;
} nat_i32_i32_i32_t;
typedef struct {
    oint32_t		 si0;
    ochunk_t		*chu;
    oint32_t		 si1;
} nat_i32_chu_i32_t;
typedef struct {
    owindow_t		*win;
    oint32_t		 i32;
    ouint32_t		 u32;
} nat_win_i32_u32_t;
typedef struct {
    orenderer_t		*ren;
    oint32_t		 si0;
    oint32_t		 si1;
} nat_ren_i32_i32_t;
typedef struct {
    omusic_t		*mus;
    oint32_t		 si0;
    oint32_t		 si1;
} nat_mus_i32_i32_t;
typedef struct {
    ofont_t		*fnt;
    ouint16_t		 ui0;
    ouint16_t		 ui1;
} nat_fnt_u16_u16_t;
typedef struct {
    ofont_t		*fnt;
    ovector_t		*vec;
    opoint_t		*pnt;
} nat_fnt_vec_pnt_t;
typedef struct {
    ofont_t		*fnt;
    ouint16_t		 u16;
    ocolor_t		*col;
} nat_fnt_u16_col_t;
typedef struct {
    ofont_t		*fnt;
    ovector_t		*vec;
    ocolor_t		*col;
} nat_fnt_vec_col_t;
typedef struct {
    oint32_t		si0;
    ouint16_t		u16;
    oint32_t		si1;
    oint32_t		si2;
} nat_i32_u16_i32_i32_t;
typedef struct {
    oint32_t		 si0;
    ochunk_t		*chu;
    oint32_t		 si1;
    oint32_t		 si2;
} nat_i32_chu_i32_i32_t;
typedef struct {
    ofont_t		*fnt;
    ovector_t		*vec;
    ocolor_t		*col;
    ouint32_t		 u32;
} nat_fnt_vec_col_u32_t;
typedef struct {
    ofont_t		*fnt;
    ouint16_t		 u16;
    ocolor_t		*cfg;
    ocolor_t		*cbg;
} nat_fnt_u16_col_col_t;
typedef struct {
    ofont_t		*fnt;
    ovector_t		*vec;
    ocolor_t		*cfg;
    ocolor_t		*cbg;
} nat_fnt_vec_col_col_t;
typedef struct {
    orenderer_t		*ren;
    otexture_t		*tex;
    orect_t		*src;
    orect_t		*dst;
} nat_ren_tex_rec_rec_t;
typedef struct {
    orenderer_t		*ren;
    ouint32_t		 si0;
    oint32_t		 si1;
    oint32_t		 si2;
    oint32_t		 si3;
} nat_ren_i32_i32_i32_i32_t;
typedef struct {
    orenderer_t		*ren;
    ouint32_t		 u32;
    oint32_t		 si0;
    oint32_t		 si1;
    oint32_t		 si2;
} nat_ren_u32_i32_i32_i32_t;
typedef struct {
    orenderer_t		*ren;
    otexture_t		*tex;
    orect_t		*src;
    orect_t		*dst;
    ofloat64_t		 f64;
    opoint_t		*cnt;
    int32_t		 i32;
} nat_ren_tex_rec_rec_f64_pnt_i32_t;
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
static void native_render_draw_point(oobject_t list, oint32_t ac);
static void native_render_draw_points(oobject_t list, oint32_t ac);
static void native_render_draw_line(oobject_t list, oint32_t ac);
static void native_render_draw_lines(oobject_t list, oint32_t ac);
static void native_render_draw_rect(oobject_t list, oint32_t ac);
static void native_render_draw_rects(oobject_t list, oint32_t ac);
static void native_render_fill_rect(oobject_t list, oint32_t ac);
static void native_render_fill_rects(oobject_t list, oint32_t ac);
static void native_render_copy(oobject_t list, oint32_t ac);
static void native_render_copy_ex(oobject_t list, oint32_t ac);
static void native_render_present(oobject_t list, oint32_t ac);
static void native_destroy_renderer(oobject_t list, oint32_t ac);
static void native_destroy_renderer(oobject_t list, oint32_t ac);
static void native_free_surface(oobject_t list, oint32_t ac);
static void query_texture(otexture_t *ot);
static void handle_texture(orenderer_t *ren, otexture_t *tex);
static void native_create_texture(oobject_t list, oint32_t ac);
static void native_create_texture_from_surface(oobject_t list, oint32_t ac);
static void native_load_texture(oobject_t list, oint32_t ac);
static void native_change_texture(oobject_t list, oint32_t ac);
static void native_destroy_texture(oobject_t list, oint32_t ac);
static inline void translate_window(oevent_t *ev, ouint32_t id);
static void translate_event(oevent_t *ev);
static void native_poll_event(oobject_t list, oint32_t ac);
static void native_wait_event(oobject_t list, oint32_t ac);
static void query_font(ofont_t *of);
static void native_open_font(oobject_t list, oint32_t ac);
static void native_change_font(oobject_t list, oint32_t ac);
static void native_glyph_is_provided(oobject_t list, oint32_t ac);
static void native_glyph_metrics(oobject_t list, oint32_t ac);
static void native_size_text(oobject_t list, oint32_t ac);
static void native_size_utf8(oobject_t list, oint32_t ac);
static void native_size_unicode(oobject_t list, oint32_t ac);
static void native_render_text_solid(oobject_t list, oint32_t ac);
static void native_render_utf8_solid(oobject_t list, oint32_t ac);
static void native_render_unicode_solid(oobject_t list, oint32_t ac);
static void native_render_glyph_solid(oobject_t list, oint32_t ac);
static void native_render_text_shaded(oobject_t list, oint32_t ac);
static void native_render_utf8_shaded(oobject_t list, oint32_t ac);
static void native_render_unicode_shaded(oobject_t list, oint32_t ac);
static void native_render_glyph_shaded(oobject_t list, oint32_t ac);
static void native_render_text_blended(oobject_t list, oint32_t ac);
static void native_render_utf8_blended(oobject_t list, oint32_t ac);
static void native_render_unicode_blended(oobject_t list, oint32_t ac);
static void native_render_glyph_blended(oobject_t list, oint32_t ac);
static void native_render_text_blended_wrapped(oobject_t list, oint32_t ac);
static void native_render_utf8_blended_wrapped(oobject_t list, oint32_t ac);
static void native_render_unicode_blended_wrapped(oobject_t list, oint32_t ac);
static void native_get_kerning(oobject_t list, oint32_t ac);
static void native_close_font(oobject_t list, oint32_t ac);
static Uint32 timer_callback(Uint32 ms, void *data);
static void pop_timer(oobject_t *pointer);
static void push_timer(otimer_t *timer);
static void native_add_timer(oobject_t list, oint32_t ac);
static void native_get_ticks(oobject_t list, oint32_t ac);
static void native_delay(oobject_t list, oint32_t ac);
static void native_remove_timer(oobject_t list, oint32_t ac);
static void native_open_audio(oobject_t list, oint32_t ac);
static void native_allocate_channels(oobject_t list, oint32_t ac);
static void native_load_chunk(oobject_t list, oint32_t ac);
static void channel_callback(int channel);
static void native_play_channel(oobject_t list, oint32_t ac);
static void native_fade_in_channel(oobject_t list, oint32_t ac);
static void native_volume_chunk(oobject_t list, oint32_t ac);
static void native_panning_channel(oobject_t list, oint32_t ac);
static void native_position_channel(oobject_t list, oint32_t ac);
static void native_distance_channel(oobject_t list, oint32_t ac);
static void native_reverse_stereo_channel(oobject_t list, oint32_t ac);
static void native_fade_out_channel(oobject_t list, oint32_t ac);
static void native_playing_channel(oobject_t list, oint32_t ac);
static void native_fading_channel(oobject_t list, oint32_t ac);
static void native_expire_channel(oobject_t list, oint32_t ac);
static void native_pause_channel(oobject_t list, oint32_t ac);
static void native_resume_channel(oobject_t list, oint32_t ac);
static void native_paused_channel(oobject_t list, oint32_t ac);
static void native_halt_channel(oobject_t list, oint32_t ac);
static void native_free_chunk(oobject_t list, oint32_t ac);
static void native_group_channel(oobject_t list, oint32_t ac);
static void native_group_channels(oobject_t list, oint32_t ac);
static void native_group_available(oobject_t list, oint32_t ac);
static void native_group_count(oobject_t list, oint32_t ac);
static void native_group_oldest(oobject_t list, oint32_t ac);
static void native_group_newer(oobject_t list, oint32_t ac);
static void native_fade_out_group(oobject_t list, oint32_t ac);
static void native_halt_group(oobject_t list, oint32_t ac);
static void native_load_music(oobject_t list, oint32_t ac);
static void music_callback(void);
static void native_play_music(oobject_t list, oint32_t ac);
static void native_fade_in_music(oobject_t list, oint32_t ac);
static void native_volume_music(oobject_t list, oint32_t ac);
static void native_playing_music(oobject_t list, oint32_t ac);
static void native_fade_out_music(oobject_t list, oint32_t ac);
static void native_fading_music(oobject_t list, oint32_t ac);
static void native_pause_music(oobject_t list, oint32_t ac);
static void native_resume_music(oobject_t list, oint32_t ac);
static void native_rewind_music(oobject_t list, oint32_t ac);
static void native_paused_music(oobject_t list, oint32_t ac);
static void native_set_music_position(oobject_t list, oint32_t ac);
static void native_halt_music(oobject_t list, oint32_t ac);
static void native_free_music(oobject_t list, oint32_t ac);
static void native_close_audio(oobject_t list, oint32_t ac);

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
    { "EventMusicFinished",		SDL_USEREVENT + 1 },
    { "EventChannelFinished",		SDL_USEREVENT + 2 },
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
    /* render_copy_ex */
    { "FlipNone",			SDL_FLIP_NONE },
    { "FlipHorizontal",			SDL_FLIP_HORIZONTAL },
    { "FlipVertical",			SDL_FLIP_VERTICAL },
    /* font_t.style */
    { "StyleNormal",			TTF_STYLE_NORMAL },
    { "StyleBold",			TTF_STYLE_BOLD },
    { "StyleItalic",			TTF_STYLE_ITALIC },
    { "StyleUnderline",			TTF_STYLE_UNDERLINE },
    { "StyleStrikethrough",		TTF_STYLE_STRIKETHROUGH },
    /* font_t.hinting */
    { "HintingNormal",			TTF_HINTING_NORMAL },
    { "HintingLight",			TTF_HINTING_LIGHT },
    { "HintingMono",			TTF_HINTING_MONO },
    { "HintingNone",			TTF_HINTING_NONE },
    /* open_audio flags */
    { "MixerDefaultFrequency",		MIX_DEFAULT_FREQUENCY },
    { "MixerDefaultFormat",		MIX_DEFAULT_FORMAT },
    { "MixerDefaultChannels",		MIX_DEFAULT_CHANNELS },
    /* music_t.type */
    { "MusicNone",			MUS_NONE },
    { "MusicCmd",			MUS_CMD },
    { "MusicWav",			MUS_WAV },
    { "MusicMod",			MUS_MOD },
    { "MusicMid",			MUS_MID },
    { "MusicOgg",			MUS_OGG },
    { "MusicMp3",			MUS_MP3 },
    { "MusicMp3Mad",			MUS_MP3_MAD },
    { "MusicFlac",			MUS_FLAC },
    { "MusicModplug",			MUS_MODPLUG },
    /* Special channel effects identifier */
    { "MixerChannelPost",		MIX_CHANNEL_POST },
    /* Mix_Fadding */
    { "FadingNone",			MIX_NO_FADING },
    { "FadingOut",			MIX_FADING_OUT },
    { "FadingIn",			MIX_FADING_IN },
};
static ovector_t		*error_vector;
static ovector_t		*timer_vector;
static ohashtable_t		*window_table;

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
    onew_hashtable(&window_table, 4);
    for (offset = 0; offset < osize(consts); ++offset) {
	string = consts[offset].name;
	onew_constant(sdl_record, oget_string((ouint8_t *)string,
					      strlen(string)),
		      consts[offset].value);
    }

#if __WORDSIZE == 32
#  define word_string		"int32_t"
#  define pointer_string	"uint32_t"
#else
#  define word_string		"int64_t"
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
#define define_builtin7(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6, VARARGS)\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, VARARGS);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
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

    record = type_vector->v.ptr[t_timer];
    add_field("uint32_t",	"*timer*");
    add_field("uint32_t",	"msec");
    add_field("auto",		"data");
    oend_record(record);

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
    define_builtin3(t_int32,    render_draw_point,
		    t_renderer, t_int32, t_int32,
		    false);
    define_builtin2(t_int32,    render_draw_points,
		    t_renderer, t_vector|t_int32,
		    false);
    define_builtin5(t_int32,    render_draw_line,
		    t_renderer, t_int32, t_int32, t_int32, t_int32,
		    false);
    define_builtin2(t_int32,    render_draw_lines,
		    t_renderer, t_vector|t_int32,
		    false);
    define_builtin5(t_int32,    render_draw_rect,
		    t_renderer, t_int32, t_int32, t_int32, t_int32,
		    false);
    define_builtin2(t_int32,    render_draw_rects,
		    t_renderer, t_vector|t_int32,
		    false);
    define_builtin5(t_int32,    render_fill_rect,
		    t_renderer, t_int32, t_int32, t_int32, t_int32,
		    false);
    define_builtin2(t_int32,    render_fill_rects,
		    t_renderer, t_vector|t_int32,
		    false);
    define_builtin4(t_int32,    render_copy,
		    t_renderer, t_texture, t_rect, t_rect,
		    false);
    define_builtin7(t_int32,    render_copy_ex,
		    t_renderer, t_texture, t_rect, t_rect,
		    t_float64, t_point, t_int32,
		    false);
    define_builtin1(t_void,    render_present, t_renderer, false);
    define_builtin1(t_void,    destroy_renderer, t_renderer, false);

    define_builtin1(t_void,    free_surface, t_surface, false);

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

    define_builtin2(t_font,  open_font, t_string, t_int32, false);
    define_builtin1(t_int32, change_font, t_font, false);
    define_builtin2(t_int32, glyph_is_provided, t_font, t_uint16, false);
    define_builtin2(t_glyph, glyph_metrics, t_font, t_uint16, false);
    define_builtin3(t_int32, size_text, t_font, t_string, t_point, false);
    define_builtin3(t_int32, size_utf8, t_font, t_string, t_point, false);
    define_builtin3(t_int32, size_unicode,
		    t_font, t_vector|t_uint16, t_point,
		    false);
    define_builtin3(t_surface, render_text_solid,
		    t_font, t_string, t_color, false);
    define_builtin3(t_surface, render_utf8_solid,
		    t_font, t_string, t_color, false);
    define_builtin3(t_surface, render_unicode_solid,
		    t_font, t_vector|t_uint16, t_color, false);
    define_builtin3(t_surface, render_glyph_solid,
		    t_font, t_uint16, t_color, false);
    define_builtin4(t_surface, render_text_shaded,
		    t_font, t_string, t_color, t_color, false);
    define_builtin4(t_surface, render_utf8_shaded,
		    t_font, t_string, t_color, t_color, false);
    define_builtin4(t_surface, render_unicode_shaded,
		    t_font, t_vector|t_uint16, t_color, t_color, false);
    define_builtin4(t_surface, render_glyph_shaded,
		    t_font, t_uint16, t_color, t_color, false);
    define_builtin3(t_surface, render_text_blended,
		    t_font, t_string, t_color, false);
    define_builtin3(t_surface, render_utf8_blended,
		    t_font, t_string, t_color, false);
    define_builtin3(t_surface, render_unicode_blended,
		    t_font, t_vector|t_uint16, t_color, false);
    define_builtin3(t_surface, render_glyph_blended,
		    t_font, t_uint16, t_color, false);
    define_builtin4(t_surface, render_text_blended_wrapped,
		    t_font, t_string, t_color, t_uint32, false);
    define_builtin4(t_surface, render_utf8_blended_wrapped,
		    t_font, t_string, t_color, t_uint32, false);
    define_builtin4(t_surface, render_unicode_blended_wrapped,
		    t_font, t_vector|t_uint16, t_color, t_uint32, false);
    define_builtin3(t_int32, get_kerning, t_font, t_uint16, t_uint16, false);
    define_builtin1(t_void,  close_font, t_font, false);

    define_builtin2(t_timer,   add_timer, t_uint32, t_void, false);
    define_builtin0(t_uint32,  get_ticks, false);
    define_builtin1(t_void,    delay, t_uint32, false);
    define_builtin1(t_int32,   remove_timer, t_timer, false);

    define_builtin4(t_audio, open_audio,
		    t_int32, t_uint16, t_int32, t_int32, false);
    define_builtin1(t_int32, allocate_channels, t_int32, false);
    define_builtin1(t_chunk, load_chunk, t_string, false);
    define_builtin3(t_int32, play_channel, t_int32, t_chunk, t_int32, false);
    define_builtin4(t_int32, fade_in_channel,
		    t_int32, t_chunk, t_int32, t_int32, false);
    define_builtin2(t_int32, volume_chunk, t_chunk, t_int32, false);
    define_builtin3(t_int32, panning_channel, t_int32, t_uint8, t_uint8, false);
    define_builtin3(t_int32, position_channel,
		    t_int32, t_int16, t_uint8, false);
    define_builtin2(t_int32, distance_channel, t_int32, t_uint8, false);
    define_builtin2(t_int32, reverse_stereo_channel, t_int32, t_int8, false);
    define_builtin2(t_int32, fade_out_channel, t_int32, t_int32, false);
    define_builtin1(t_int32, playing_channel, t_int32, false);
    define_builtin1(t_int32, fading_channel, t_int32, false);
    define_builtin2(t_int32, expire_channel, t_int32, t_int32, false);
    define_builtin1(t_void, pause_channel, t_int32, false);
    define_builtin1(t_void, resume_channel, t_int32, false);
    define_builtin1(t_int32, paused_channel, t_int32, false);
    define_builtin1(t_int32, halt_channel, t_int32, false);
    define_builtin1(t_void,  free_chunk, t_chunk, false);
    define_builtin2(t_int32, group_channel, t_int32, t_int32, false);
    define_builtin3(t_int32, group_channels, t_int32, t_int32, t_int32, false);
    define_builtin1(t_int32, group_available, t_int32, false);
    define_builtin1(t_int32, group_count, t_int32, false);
    define_builtin1(t_int32, group_oldest, t_int32, false);
    define_builtin1(t_int32, group_newer, t_int32, false);
    define_builtin1(t_int32, fade_out_group, t_int32, false);
    define_builtin1(t_int32, halt_group, t_int32, false);
    define_builtin1(t_music, load_music, t_string, false);
    define_builtin2(t_int32, play_music, t_music, t_int32, false);
    define_builtin3(t_int32, fade_in_music, t_music, t_int32, t_int32, false);
    define_builtin1(t_int32, volume_music, t_uint8, false);
    define_builtin0(t_int32, playing_music, false);
    define_builtin1(t_int32,  fade_out_music, t_int32, false);
    define_builtin0(t_int32,  fading_music, false);
    define_builtin0(t_void,  pause_music, false);
    define_builtin0(t_void,  resume_music, false);
    define_builtin0(t_void,  rewind_music, false);
    define_builtin0(t_int32,  paused_music, false);
    define_builtin1(t_int32,  set_music_position, t_float64, false);
    define_builtin0(t_int32,  halt_music, false);
    define_builtin1(t_void,  free_music, t_music, false);
    define_builtin0(t_void,  close_audio, false);

    current_record = record;
#undef add_field
#undef define_builtin1
#undef define_builtin0
}

void
finish_sdl(void)
{
    orem_root((oobject_t *)&window_table);
    window_table = null;
    orem_root((oobject_t *)&timer_vector);
    orem_root((oobject_t *)&error_vector);
}

void
odestroy_window(owindow_t *window)
{
    if (window->__window) {
	if (window_table)
	    orem_hashentry(window_table, window->__handle);
	SDL_DestroyWindow(window->__window);
	window->__window = null;
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
	SDL_DestroyRenderer(renderer->__renderer);
	renderer->__renderer = null;
    }
}

void
odestroy_texture(otexture_t *texture)
{
    if (texture->__texture) {
	SDL_DestroyTexture(texture->__texture);
	texture->__texture = null;
    }
}

void
odestroy_font(ofont_t *font)
{
    if (font->__font) {
	TTF_CloseFont(font->__font);
	font->__font = null;
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
	       (TTF_Init() != 0) |
	       (Mix_Init(MIX_INIT_FLAC|MIX_INIT_MOD|
			 MIX_INIT_MODPLUG|MIX_INIT_MP3|
			 MIX_INIT_OGG|MIX_INIT_FLUIDSYNTH) == 0));
    Mix_HookMusicFinished(music_callback);
    Mix_ChannelFinished(channel_callback);
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
    Mix_Quit();
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
native_change_window(oobject_t list, oint32_t ac)
/* int32_t change_window(window_t window); */
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
	orenew_vector(ow->__title, length);
	if (length)
	    memcpy(ow->__title->v.u8, buffer, length);
	ow->title = ow->__title;
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
	onew_hashtable(&or->__textures, 4);
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
	if ((entry = oget_hashentry(alist->ren->__textures, &check)) == null)
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
native_render_draw_point(oobject_t list, oint32_t ac)
/* int32_t render_draw_point(renderer_t ren, int32_t, int32_t y); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_t		*alist;

    alist = (nat_ren_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawPoint(alist->ren->__renderer,
				  alist->si0, alist->si1);
}

static void
native_render_draw_points(oobject_t list, oint32_t ac)
/* int32_t render_draw_points(renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_int32) ||
	!alist->vec->length || (alist->vec->length & 1))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawPoints(alist->ren->__renderer,
				   (SDL_Point *)alist->vec->v.i32,
				   alist->vec->length >> 1);
}

static void
native_render_draw_line(oobject_t list, oint32_t ac)
/* int32_t render_draw_point(renderer_t ren, int32_t x1, int32_t y1,
			     int32_t x2, int32_t y2); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_i32_i32_t	*alist;

    alist = (nat_ren_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawLine(alist->ren->__renderer,
				  alist->si0, alist->si1,
				 alist->si2, alist->si3);
}

static void
native_render_draw_lines(oobject_t list, oint32_t ac)
/* int32_t render_draw_lines(renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_int32) ||
	alist->vec->length < 4 || (alist->vec->length & 1))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawLines(alist->ren->__renderer,
				  (SDL_Point *)alist->vec->v.i32,
				  alist->vec->length >> 1);
}

static void
native_render_draw_rect(oobject_t list, oint32_t ac)
/* int32_t render_draw_rect(renderer_t ren, int32_t x, int32_t y,
			     int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_i32_i32_t	*alist;

    alist = (nat_ren_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawRect(alist->ren->__renderer,
				 (SDL_Rect *)&alist->si0);
}

static void
native_render_draw_rects(oobject_t list, oint32_t ac)
/* int32_t render_draw_rects(renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_int32) ||
	(alist->vec->length & 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderDrawRects(alist->ren->__renderer,
				  (SDL_Rect *)alist->vec->v.i32,
				  alist->vec->length >> 2);
}

static void
native_render_fill_rect(oobject_t list, oint32_t ac)
/* int32_t render_fill_rect(renderer_t ren, int32_t x, int32_t y,
			     int32_t w, int32_t h); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_i32_i32_i32_i32_t	*alist;

    alist = (nat_ren_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderFillRect(alist->ren->__renderer,
				 (SDL_Rect *)&alist->si0);
}

static void
native_render_fill_rects(oobject_t list, oint32_t ac)
/* int32_t render_fill_rects(renderer_t ren, int32_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_ren_vec_t		*alist;

    alist = (nat_ren_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_int32) ||
	(alist->vec->length & 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderFillRects(alist->ren->__renderer,
				  (SDL_Rect *)alist->vec->v.i32,
				  alist->vec->length >> 2);
}

static void
native_render_copy(oobject_t list, oint32_t ac)
/* int32_t render_copy(renderer_t ren, texture_t tex,
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
native_render_copy_ex(oobject_t list, oint32_t ac)
/* int32_t render_copy_ex(renderer_t ren, texture_t tex,
		          rect_t src, rect_t dst,
                          float64_t angle, point_t center,
			  int32_t flip); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_ren_tex_rec_rec_f64_pnt_i32_t	*alist;

    alist = (nat_ren_tex_rec_rec_f64_pnt_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->ren == null || otype(alist->ren) != t_renderer ||
	alist->tex == null || otype(alist->tex) != t_texture ||
	(alist->src && otype(alist->src) != t_rect) ||
	(alist->dst && otype(alist->dst) != t_rect) ||
	(alist->cnt && otype(alist->cnt) != t_point))
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = SDL_RenderCopyEx(alist->ren->__renderer, alist->tex->__texture,
			       (SDL_Rect *)alist->src, (SDL_Rect *)alist->dst,
			       alist->f64, (SDL_Point *)alist->cnt, alist->i32);
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
	odestroy_renderer(alist->ren);
    }
    r0->t = t_void;
}

static void
native_free_surface(oobject_t list, oint32_t ac)
/* void free_surface(renderer_t ren); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_srf_t			*alist;

    alist = (nat_srf_t *)list;
    r0 = &thread_self->r0;
    if (alist->srf && alist->srf->__surface) {
	if (otype(alist->srf) != t_surface)
	    ovm_raise(except_invalid_argument);
	SDL_FreeSurface(alist->srf->__surface);
	alist->srf->__surface = null;
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
	handle_texture(alist->ren, ot);
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
	handle_texture(alist->ren, ot);
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
    if (alist->vec->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->vec->v.ptr, alist->vec->length);
    path[alist->vec->length] = '\0';
    if ((st = IMG_LoadTexture(alist->ren->__renderer, path))) {
	onew_object(&thread_self->obj, t_texture, sizeof(otexture_t));
	ot = (otexture_t *)thread_self->obj;
	ot->__texture = st;
	handle_texture(alist->ren, ot);
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
native_open_font(oobject_t list, oint32_t ac)
/* font_t open_font(string_t name, int32_t ptsize) */
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
    if (alist->vec == null || otype(alist->vec) != t_string)
	ovm_raise(except_invalid_argument);
    if (alist->vec->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->vec->v.ptr, alist->vec->length);
    path[alist->vec->length] = '\0';
    if ((sf = TTF_OpenFont(path, alist->i32))) {
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
native_change_font(oobject_t list, oint32_t ac)
/* int32_t change_font(font_t) */
{
    GET_THREAD_SELF()
    TTF_Font			*sf;
    ofont_t			*of;
    oregister_t			*r0;
    nat_fnt_t			*alist;

    alist = (nat_fnt_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font)
	ovm_raise(except_invalid_argument);
    of = alist->fnt;
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
native_glyph_is_provided(oobject_t list, oint32_t ac)
/* int32_t glyph_is_provided(font_t font, uint16_t ch) */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_u16_t		*alist;

    alist = (nat_fnt_u16_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = TTF_GlyphIsProvided(alist->fnt->__font, alist->u16);
}

static void
native_glyph_metrics(oobject_t list, oint32_t ac)
/* glyph_t glyph_metrics(font_t font, uint16_t ch) */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    oglyph_t			 ogm;
    nat_fnt_u16_t		*alist;

    alist = (nat_fnt_u16_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font)
	ovm_raise(except_invalid_argument);
    if (TTF_GlyphMetrics(alist->fnt->__font, alist->u16,
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
native_size_text(oobject_t list, oint32_t ac)
/* int32_t size_text(font_t font, string_t text, point_t size); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_vec_pnt_t		*alist;

    alist = (nat_fnt_vec_pnt_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->pnt == null || otype(alist->pnt) != t_point)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    r0->t = t_word;
    r0->v.w = TTF_SizeText(alist->fnt->__font,
			   (const char *)alist->vec->v.u8,
			   &alist->pnt->x, &alist->pnt->y);
}

static void
native_size_utf8(oobject_t list, oint32_t ac)
/* int32_t size_utf8(font_t font, string_t text, point_t size); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_vec_pnt_t		*alist;

    alist = (nat_fnt_vec_pnt_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->pnt == null || otype(alist->pnt) != t_point)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    r0->t = t_word;
    r0->v.w = TTF_SizeUTF8(alist->fnt->__font,
			   (const char *)alist->vec->v.u8,
			   &alist->pnt->x, &alist->pnt->y);
}

static void
native_size_unicode(oobject_t list, oint32_t ac)
/* int32_t size_unicode(font_t font, uint16_t text[], point_t size); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_vec_pnt_t		*alist;

    alist = (nat_fnt_vec_pnt_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_uint16) ||
	alist->pnt == null || otype(alist->pnt) != t_point)
	ovm_raise(except_invalid_argument);
    make_vec_unicode(alist->vec);
    r0->t = t_word;
    r0->v.w = TTF_SizeUNICODE(alist->fnt->__font,
			      (const Uint16 *)alist->vec->v.u16,
			      &alist->pnt->x, &alist->pnt->y);
}

static void
native_render_text_solid(oobject_t list, oint32_t ac)
/* surface_t render_text_solid(font_t font, string_t text, color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderText_Solid(alist->fnt->__font,
				   (const char *)alist->vec->v.u8,
				   *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_utf8_solid(oobject_t list, oint32_t ac)
/* surface_t render_utf8_solid(font_t font, string_t text, color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderUTF8_Solid(alist->fnt->__font,
				   (const char *)alist->vec->v.u8,
				   *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_unicode_solid(oobject_t list, oint32_t ac)
/* surface_t render_unicode_solid(font_t font, uint16_t text[], color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_uint16) ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_unicode(alist->vec);
    if ((ss = TTF_RenderUNICODE_Solid(alist->fnt->__font,
				      (const Uint16 *)alist->vec->v.u16,
				      *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_glyph_solid(oobject_t list, oint32_t ac)
/* surface_t render_glyph_solid(font_t font, uint16_t glyph, color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_u16_col_t		*alist;

    alist = (nat_fnt_u16_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    if ((ss = TTF_RenderGlyph_Solid(alist->fnt->__font, alist->u16,
				    *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_text_shaded(oobject_t list, oint32_t ac)
/* surface_t render_text_shaded(font_t font, string_t text,
			        color_t fg, color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_col_t	*alist;

    alist = (nat_fnt_vec_col_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->cfg == null || otype(alist->cfg) != t_color ||
	alist->cbg == null || otype(alist->cbg) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderText_Shaded(alist->fnt->__font,
				    (const char *)alist->vec->v.u8,
				    *(SDL_Color *)alist->cfg,
				    *(SDL_Color *)alist->cbg))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_utf8_shaded(oobject_t list, oint32_t ac)
/* surface_t render_utf8_shaded(font_t font, string_t text,
			        color_t fg, color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_col_t	*alist;

    alist = (nat_fnt_vec_col_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->cfg == null || otype(alist->cfg) != t_color ||
	alist->cbg == null || otype(alist->cbg) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderUTF8_Shaded(alist->fnt->__font,
				    (const char *)alist->vec->v.u8,
				    *(SDL_Color *)alist->cfg,
				    *(SDL_Color *)alist->cbg))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_unicode_shaded(oobject_t list, oint32_t ac)
/* surface_t render_unicode_shaded(font_t font, uint16_t text[],
				   color_t fg, color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_col_t	*alist;

    alist = (nat_fnt_vec_col_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_uint16) ||
	alist->cfg == null || otype(alist->cfg) != t_color ||
	alist->cbg == null || otype(alist->cbg) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_unicode(alist->vec);
    if ((ss = TTF_RenderUNICODE_Shaded(alist->fnt->__font,
				       (const Uint16 *)alist->vec->v.u16,
				       *(SDL_Color *)alist->cfg,
				       *(SDL_Color *)alist->cbg))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_glyph_shaded(oobject_t list, oint32_t ac)
/* surface_t render_glyph_shaded(font_t font, uint16_t glyph,
				 color_t fg, color_t bf); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_u16_col_col_t	*alist;

    alist = (nat_fnt_u16_col_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->cfg == null || otype(alist->cfg) != t_color ||
	alist->cbg == null || otype(alist->cbg) != t_color)
	ovm_raise(except_invalid_argument);
    if ((ss = TTF_RenderGlyph_Shaded(alist->fnt->__font, alist->u16,
				     *(SDL_Color *)alist->cfg,
				     *(SDL_Color *)alist->cbg))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_text_blended(oobject_t list, oint32_t ac)
/* surface_t render_text_blended(font_t font, string_t text, color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderText_Blended(alist->fnt->__font,
				     (const char *)alist->vec->v.u8,
				     *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_utf8_blended(oobject_t list, oint32_t ac)
/* surface_t render_utf8_blended(font_t font, string_t text, color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderUTF8_Blended(alist->fnt->__font,
				     (const char *)alist->vec->v.u8,
				     *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_unicode_blended(oobject_t list, oint32_t ac)
/* surface_t render_unicode_blended(font_t font, uint16_t text[], color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_t		*alist;

    alist = (nat_fnt_vec_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != (t_vector|t_uint16) ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_unicode(alist->vec);
    if ((ss = TTF_RenderUNICODE_Blended(alist->fnt->__font,
					(const Uint16 *)alist->vec->v.u16,
					*(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_glyph_blended(oobject_t list, oint32_t ac)
/* surface_t render_glyph_blended(font_t font, uint16_t glyph, color_t fg); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_u16_col_t		*alist;

    alist = (nat_fnt_u16_col_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    if ((ss = TTF_RenderGlyph_Blended(alist->fnt->__font, alist->u16,
				      *(SDL_Color *)alist->col))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_text_blended_wrapped(oobject_t list, oint32_t ac)
/* surface_t render_text_blended_wrapped(font_t font, string_t text,
					 color_t fg, uint32_t length); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_u32_t	*alist;

    alist = (nat_fnt_vec_col_u32_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderText_Blended_Wrapped(alist->fnt->__font,
					     (const char *)alist->vec->v.u8,
					     *(SDL_Color *)alist->col,
					     alist->u32))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_utf8_blended_wrapped(oobject_t list, oint32_t ac)
/* surface_t render_utf8_blended_wrapped(font_t font, string_t text,
					 color_t fg, uint32_t length); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_u32_t	*alist;

    alist = (nat_fnt_vec_col_u32_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_text(alist->vec);
    if ((ss = TTF_RenderUTF8_Blended_Wrapped(alist->fnt->__font,
					     (const char *)alist->vec->v.u8,
					     *(SDL_Color *)alist->col,
					     alist->u32))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_render_unicode_blended_wrapped(oobject_t list, oint32_t ac)
/* surface_t render_unicode_blended_wrapped(font_t font, uint16_t text[],
					    color_t fg, uint32_t length); */
{
    GET_THREAD_SELF()
    osurface_t			*os;
    SDL_Surface			*ss;
    oregister_t			*r0;
    nat_fnt_vec_col_u32_t	*alist;

    alist = (nat_fnt_vec_col_u32_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font ||
	alist->vec == null || otype(alist->vec) != t_string ||
	alist->col == null || otype(alist->col) != t_color)
	ovm_raise(except_invalid_argument);
    make_vec_unicode(alist->vec);
    if ((ss = TTF_RenderUNICODE_Blended_Wrapped(alist->fnt->__font,
						(const Uint16 *)alist->vec->v.u16,
						*(SDL_Color *)alist->col,
						alist->u32))) {
	onew_object(&thread_self->obj, t_surface, sizeof(osurface_t));
	os = (osurface_t *)thread_self->obj;
	os->__surface = ss;
	r0->v.o = thread_self->obj;
	r0->t = t_surface;
    }
    else
	r0->t = t_void;
}

static void
native_get_kerning(oobject_t list, oint32_t ac)
/* int32_t get_kerning(font_t font, uint16_t prev, uint16_t ch); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_u16_u16_t		*alist;

    alist = (nat_fnt_u16_u16_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = TTF_GetFontKerningSize(alist->fnt->__font,
				     alist->ui0, alist->ui1);
}

static void
native_close_font(oobject_t list, oint32_t ac)
/* void close_font(font_t font); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_fnt_t			*alist;

    alist = (nat_fnt_t *)list;
    r0 = &thread_self->r0;
    if (alist->fnt == null || otype(alist->fnt) != t_font)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    odestroy_font(alist->fnt);
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

static void
native_open_audio(oobject_t list, oint32_t ac)
/* audio_t open_audio(int32_t frequency, uint16_t format, int32_t channels,
		      int32_t chunksize); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    oaudio_t			 oau;
    nat_i32_u16_i32_i32_t	*alist;

    alist = (nat_i32_u16_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (Mix_OpenAudio(alist->si0, alist->u16, alist->si1, alist->si2) == 0 &&
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
native_allocate_channels(oobject_t list, oint32_t ac)
/* int32_t allocate_channels(int32_t channels); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_AllocateChannels(alist->i32);
}

static void
native_load_chunk(oobject_t list, oint32_t ac)
/* chunk_t load_chunk(string_t path); */
{
    GET_THREAD_SELF()
    Mix_Chunk			*sc;
    ochunk_t			*oc;
    oregister_t			*r0;
    nat_vec_t			*alist;
    char			 path[BUFSIZ];

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->vec == null || otype(alist->vec) != t_string)
	ovm_raise(except_invalid_argument);
    if (alist->vec->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->vec->v.ptr, alist->vec->length);
    path[alist->vec->length] = '\0';
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
native_play_channel(oobject_t list, oint32_t ac)
/* int32_t play_channel(int32_t channel, chunk_t chunk, int32_t loops); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_chu_i32_t		*alist;

    alist = (nat_i32_chu_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->chu == null || otype(alist->chu) != t_chunk)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = Mix_PlayChannel(alist->si0, alist->chu->__chunk, alist->si1);
}

static void
native_fade_in_channel(oobject_t list, oint32_t ac)
/* int32_t channel_fade_in(int32_t channel, chunk_t chunk,
			  int32_t loops, int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_chu_i32_i32_t	*alist;

    alist = (nat_i32_chu_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->chu == null || otype(alist->chu) != t_chunk)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = Mix_FadeInChannel(alist->si0, alist->chu->__chunk,
				alist->si1, alist->si2);
}

static void
native_volume_chunk(oobject_t list, oint32_t ac)
/* int32_t volume_chunk(chunk_t chunk, int32_t volume); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_chu_i32_t		*alist;

    alist = (nat_chu_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->chu == null || otype(alist->chu) != t_chunk)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = Mix_VolumeChunk(alist->chu->__chunk, alist->i32);
}

static void
native_panning_channel(oobject_t list, oint32_t ac)
/* int32_t panning_channel(int32_t channel, uint8_t left, uint8_t right); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_u8_u8_t		*alist;

    alist = (nat_i32_u8_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetPanning(alist->i32, alist->ui0, alist->ui1);
}

static void
native_position_channel(oobject_t list, oint32_t ac)
/* int32_t position_channel(int32_t channel, int16_t angle, uint8_t dist); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i16_u8_t		*alist;

    alist = (nat_i32_i16_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetPosition(alist->i32, alist->i16, alist->u8);
}

static void
native_distance_channel(oobject_t list, oint32_t ac)
/* int32_t distance_channel(int32_t channel, uint8_t dist); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_u8_t		*alist;

    alist = (nat_i32_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetDistance(alist->i32, alist->u8);
}

static void
native_reverse_stereo_channel(oobject_t list, oint32_t ac)
/* int32_t reverse_stereo_channel(int32_t channel, int8_t flip); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i8_t		*alist;

    alist = (nat_i32_i8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetReverseStereo(alist->i32, alist->i8);
}

static void
native_fade_out_channel(oobject_t list, oint32_t ac)
/* int32_t fade_out_channel(int32_t channel, int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadeOutChannel(alist->si0, alist->si1);
}

static void
native_playing_channel(oobject_t list, oint32_t ac)
/* int32_t playing_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_Playing(alist->i32);
}

static void
native_fading_channel(oobject_t list, oint32_t ac)
/* int32_t fading_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadingChannel(alist->i32);
}

static void
native_expire_channel(oobject_t list, oint32_t ac)
/* int32_t expire_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_ExpireChannel(alist->si0, alist->si1);
}

static void
native_pause_channel(oobject_t list, oint32_t ac)
/* void pause_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_Pause(alist->i32);
}

static void
native_resume_channel(oobject_t list, oint32_t ac)
/* void resume_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_Resume(alist->i32);
}

static void
native_paused_channel(oobject_t list, oint32_t ac)
/* void paused_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_Paused(alist->i32);
}

static void
native_halt_channel(oobject_t list, oint32_t ac)
/* int32_t halt_channel(int32_t channel); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_HaltChannel(alist->i32);
}

static void
native_free_chunk(oobject_t list, oint32_t ac)
/* void free_chunk(chunk_t chunk); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_chu_t			*alist;

    alist = (nat_chu_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->chu == null || otype(alist->chu) != t_chunk)
	ovm_raise(except_invalid_argument);
    if (alist->chu->__chunk) {
	Mix_FreeChunk(alist->chu->__chunk);
	alist->chu->__chunk = null;
    }
}

static void
native_group_channel(oobject_t list, oint32_t ac)
/* int32_t group_channel(int32_t channel, int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupChannel(alist->si0, alist->si1);
}

static void
native_group_channels(oobject_t list, oint32_t ac)
/* int32_t group_channels(int32_t from_channel, int32_t to_channel,
			  int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_i32_t		*alist;

    alist = (nat_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupChannels(alist->si0, alist->si1, alist->si2);
}

static void
native_group_available(oobject_t list, oint32_t ac)
/* int32_t group_available(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupAvailable(alist->i32);
}

static void
native_group_count(oobject_t list, oint32_t ac)
/* int32_t group_count(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupCount(alist->i32);
}

static void
native_group_oldest(oobject_t list, oint32_t ac)
/* int32_t group_oldest(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupOldest(alist->i32);
}

static void
native_group_newer(oobject_t list, oint32_t ac)
/* int32_t group_newer(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_GroupNewer(alist->i32);
}

static void
native_fade_out_group(oobject_t list, oint32_t ac)
/* int32_t fade_out_group(int32_t tag, int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_i32_t		*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadeOutGroup(alist->si0, alist->si1);
}

static void
native_halt_group(oobject_t list, oint32_t ac)
/* int32_t halt_group(int32_t tag); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_HaltGroup(alist->i32);
}

static void
native_load_music(oobject_t list, oint32_t ac)
/* music_t load_music(string_t path); */
{
    GET_THREAD_SELF()
    Mix_Music			*sm;
    omusic_t			*om;
    oregister_t			*r0;
    nat_vec_t			*alist;
    char			 path[BUFSIZ];

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (alist->vec == null || otype(alist->vec) != t_string)
	ovm_raise(except_invalid_argument);
    if (alist->vec->length >= BUFSIZ - 1)
	ovm_raise(except_out_of_bounds);
    memcpy(path, alist->vec->v.ptr, alist->vec->length);
    path[alist->vec->length] = '\0';
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
native_play_music(oobject_t list, oint32_t ac)
/* int32_t play_music(music_t music, int32_t loops); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_mus_i32_t		*alist;

    alist = (nat_mus_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->mus == null || otype(alist->mus) != t_music)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = Mix_PlayMusic(alist->mus->__music, alist->i32);
}

static void
native_fade_in_music(oobject_t list, oint32_t ac)
/* int32_t fade_in_music(music_t music, int32_t loops, int ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_mus_i32_i32_t		*alist;

    alist = (nat_mus_i32_i32_t *)list;
    r0 = &thread_self->r0;
    if (alist->mus == null || otype(alist->mus) != t_music)
	ovm_raise(except_invalid_argument);
    r0->t = t_word;
    r0->v.w = Mix_FadeInMusic(alist->mus->__music, alist->si0, alist->si1);
}

static void
native_volume_music(oobject_t list, oint32_t ac)
/* int32_t volume_music(uint8_t volume); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_VolumeMusic(alist->i32);
}

static void
native_playing_music(oobject_t list, oint32_t ac)
/* int32_t playing_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_PlayingMusic();
}

static void
native_fade_out_music(oobject_t list, oint32_t ac)
/* int32_t fade_out_music(int32_t ms); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_i32_t			*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadeOutMusic(alist->i32);
}

static void
native_fading_music(oobject_t list, oint32_t ac)
/* int32_t fading_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_FadingMusic();
}

static void
native_pause_music(oobject_t list, oint32_t ac)
/* void pause_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_PauseMusic();
}

static void
native_resume_music(oobject_t list, oint32_t ac)
/* void resume_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_ResumeMusic();
}

static void
native_rewind_music(oobject_t list, oint32_t ac)
/* void rewind_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    Mix_RewindMusic();
}

static void
native_paused_music(oobject_t list, oint32_t ac)
/* void paused_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_PausedMusic();
}

static void
native_set_music_position(oobject_t list, oint32_t ac)
/* int32_t set_music_position(float64_t position); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_f64_t			*alist;

    alist = (nat_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_SetMusicPosition(alist->f64);
}

static void
native_halt_music(oobject_t list, oint32_t ac)
/* int32_t halt_music(); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = Mix_HaltMusic();
}

static void
native_free_music(oobject_t list, oint32_t ac)
/* void free_music(music_t music); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_mus_t			*alist;

    alist = (nat_mus_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->mus == null || otype(alist->mus) != t_music)
	ovm_raise(except_invalid_argument);
    if (alist->mus->__music) {
	Mix_FreeMusic(alist->mus->__music);
	alist->mus->__music = null;
    }
}

static void
native_close_audio(oobject_t list, oint32_t ac)
/* void close_audio(); */
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
