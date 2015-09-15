/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdint.h>
#include <SDL/SDL.h>
#include "util.h"
#include "luax.h"


#if _WIN32
  #include <windows.h>
#elif __linux__
  #include <unistd.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#endif


static const char *buttonStr(int id) {
  switch (id) {
    case 1  : return "left";
    case 2  : return "middle";
    case 3  : return "right";
    case 4  : return "wheelup";
    case 5  : return "wheeldown";
    default : return "?";
  }
}

static char keyChar(char c) {
  return (c == '\r') ? '\n' : c;
}

static int l_system_poll(lua_State *L) {
  /* Create events table */
  lua_newtable(L);

  /* Handle events */
  int eventIdx = 1;
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    lua_newtable(L);

    switch (e.type) {
      case SDL_QUIT:
        luax_setfield_string(L, "type", "quit");
        break;

      // /* TODO: */
      //case SDL_VIDEORESIZE:
      //  lua_getfield(L, -3, "setSize");
      //  lua_pushnumber(L, e.resize.w);
      //  lua_pushnumber(L, e.resize.h);
      //  luax_call(L, 2, 0);
      //  luax_setfield_string(L, "type", "resize");
      //  luax_setfield_number(L, "width", e.resize.w);
      //  luax_setfield_number(L, "height", e.resize.h);
      //  break;

      case SDL_KEYDOWN: {
        luax_setfield_string(L, "type", "keydown");
        luax_setfield_fstring(L, "key", "%s",
                              SDL_GetKeyName(e.key.keysym.sym));
        int c = keyChar(e.key.keysym.unicode);
        if (c) {
          luax_setfield_fstring(L, "char", "%c", c);
        }
        break;
      }

      case SDL_KEYUP:
        luax_setfield_string(L, "type", "keyup");
        luax_setfield_fstring(L, "key", "%s",
                              SDL_GetKeyName(e.key.keysym.sym));
        luax_setfield_fstring(L, "char", "%c", keyChar(e.key.keysym.unicode));
        break;

      case SDL_MOUSEMOTION:
        luax_setfield_string(L, "type", "mousemove");
        luax_setfield_number(L, "x", e.motion.x);
        luax_setfield_number(L, "y", e.motion.y);
        break;

      case SDL_MOUSEBUTTONDOWN:
        luax_setfield_string(L, "type", "mousebuttondown");
        luax_setfield_string(L, "button", buttonStr(e.button.button));
        luax_setfield_number(L, "x", e.button.x);
        luax_setfield_number(L, "y", e.button.y);
        break;

      case SDL_MOUSEBUTTONUP:
        luax_setfield_string(L, "type", "mousebuttonup");
        luax_setfield_string(L, "button", buttonStr(e.button.button));
        luax_setfield_number(L, "x", e.button.x);
        luax_setfield_number(L, "y", e.button.y);
        break;

      case SDL_JOYAXISMOTION:
        luax_setfield_string(L, "type", "joyaxismotion");
        luax_setfield_number(L, "joystick", e.jaxis.which);
        luax_setfield_number(L, "axis", e.jaxis.axis);
        int value = e.jaxis.value;
        luax_setfield_number(L, "value", ((double)value+0.5f)/(INT16_MAX+0.5f));
        break;

      case SDL_JOYBALLMOTION:
        luax_setfield_string(L, "type", "joyballmotion");
        luax_setfield_number(L, "joystick", e.jball.which);
        luax_setfield_number(L, "ball", e.jball.ball);
        luax_setfield_number(L, "x", e.jball.xrel);
        luax_setfield_number(L, "y", e.jball.yrel);
        break;

      case SDL_JOYBUTTONDOWN:
        luax_setfield_string(L, "type", "joybuttondown");
        luax_setfield_number(L, "joystick", e.jbutton.which);
        luax_setfield_number(L, "button", e.jbutton.button);
        break;

      case SDL_JOYBUTTONUP:
        luax_setfield_string(L, "type", "joybuttonup");
        luax_setfield_number(L, "joystick", e.jbutton.which);
        luax_setfield_number(L, "button", e.jbutton.button);
        break;

      case SDL_JOYHATMOTION:
        luax_setfield_string(L, "type", "joyhatmotion");
        luax_setfield_number(L, "joystick", e.jhat.which);
        luax_setfield_number(L, "hat", e.jhat.hat);

        lua_newtable(L); /* e.state */
        luax_setfield_bool(L, "up", e.jhat.value & SDL_HAT_UP);
        luax_setfield_bool(L, "down", e.jhat.value & SDL_HAT_DOWN);
        luax_setfield_bool(L, "left", e.jhat.value & SDL_HAT_LEFT);
        luax_setfield_bool(L, "right", e.jhat.value & SDL_HAT_RIGHT);
        lua_setfield(L, -2, "state"); /* push state to event table */
        break;
    }

    /* Push event to events table */
    lua_rawseti(L, -2, eventIdx++);
  }
  return 1;
}


static char *dirname(char *str) {
  char *p = str + strlen(str);
  while (p != str) {
    if (*p == '/' || *p == '\\') {
      *p = '\0';
      break;
    }
    p--;
  }
  return str;
}

static int l_system_info(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  if (!strcmp(str, "os")) {
#if _WIN32
    lua_pushstring(L, "windows");
#elif __linux__
    lua_pushstring(L, "linux");
#elif __FreeBSD__
    lua_pushstring(L, "bsd");
#elif __APPLE__
    lua_pushstring(L, "osx");
#else
    lua_pushstring(L, "?");
#endif

  } else if (!strcmp(str, "exedir")) {
    UNUSED(dirname);
#if _WIN32
    char buf[1024];
    int len = GetModuleFileName(NULL, buf, sizeof(buf) - 1);
    buf[len] = '\0';
    dirname(buf);
    lua_pushfstring(L, "%s", buf);
#elif __linux__
    char path[128];
    char buf[1024];
    sprintf(path, "/proc/%d/exe", getpid());
    int len = readlink(path, buf, sizeof(buf) - 1);
    ASSERT( len != -1 );
    buf[len] = '\0';
    dirname(buf);
    lua_pushfstring(L, "%s", buf);
#elif __FreeBSD__
    /* TODO : Implement this */
    lua_pushfstring(L, ".");
#elif __APPLE__
    char buf[1024];
    uint32_t size = sizeof(buf);
    ASSERT( _NSGetExecutablePath(buf, &size) == 0 );
    dirname(buf);
    lua_pushfstring(L, "%s", buf);
#else
    lua_pushfstring(L, ".");
#endif

  } else if (!strcmp(str, "appdata")) {
#if _WIN32
    lua_pushfstring(L, "%s", getenv("APPDATA"));
#elif __APPLE__
    lua_pushfstring(L, "%s/Library/Application Support", getenv("HOME"));
#else
    lua_pushfstring(L, "%s/.local/share", getenv("HOME"));
#endif
  } else {
    luaL_argerror(L, 1, "invalid string");
  }
  return 1;
}


int luaopen_system(lua_State *L) {
  luaL_Reg reg[] = {
    { "poll",     l_system_poll     },
    { "info",     l_system_info     },
    { NULL, NULL }
  };
  luaL_newlib(L, reg);
  return 1;
}
