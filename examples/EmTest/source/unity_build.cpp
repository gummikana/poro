#ifndef PORO_PLAT_EMSCRIPTEN
	#define PORO_PLAT_EMSCRIPTEN
#endif

#include "../../../source/poro/default_application.cpp"
#include "../../../source/poro/event_recorder.cpp"
#include "../../../source/poro/iplatform.cpp"
#include "../../../source/poro/joystick.cpp"
#include "../../../source/poro/keyboard.cpp"
#include "../../../source/poro/mouse.cpp"
#include "../../../source/poro/touch.cpp"

#include "../../../source/poro/emscripten/platform_emscripten.cpp"

#if 0
#include "../../../source/poro/desktop/joystick_impl.cpp"
#include "../../../source/poro/desktop/event_playback_impl.cpp"
#include "../../../source/poro/desktop/event_recorder_impl.cpp"
#include "../../../source/poro/desktop/graphics_buffer_opengl.cpp"
#include "../../../source/poro/desktop/graphics_opengl.cpp"
#include "../../../source/poro/desktop/mouse_impl.cpp"
#include "../../../source/poro/desktop/platform_desktop.cpp"
#include "../../../source/poro/desktop/windows/platform_win.cpp"
#include "../../../source/poro/desktop/render_texture_opengl.cpp"
#include "../../../source/poro/desktop/shader_opengl.cpp"
#include "../../../source/poro/desktop/sound_sdl.cpp"
#include "../../../source/poro/desktop/soundplayer_sdl.cpp"
#include "../../../source/poro/desktop/texture3d_opengl.cpp"
#include "../../../source/poro/desktop/texture_opengl.cpp"
#endif
