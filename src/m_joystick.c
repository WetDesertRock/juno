/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <SDL/SDL.h>
#include "luax.h"
#include "m_joystick.h"
#include "util.h"

#define CLASS_NAME JOYSTICK_CLASS_NAME

Joystick *joystick_new(lua_State *L) {
  Joystick *self = lua_newuserdata(L, sizeof(*self));
  luaL_setmetatable(L, CLASS_NAME);
  memset(self, 0, sizeof(*self));
  return self;
}

static int l_joystick_gc(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  SDL_JoystickClose(self->joystick);
  return 0;
}

static int l_joystick_index(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushnumber(L, SDL_JoystickIndex(self->joystick));
  return 1;
}

static int l_joystick_getAxisCount(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushnumber(L, SDL_JoystickNumAxes(self->joystick));
  return 1;
}

static int l_joystick_getBallCount(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushnumber(L, SDL_JoystickNumBalls(self->joystick));
  return 1;
}

static int l_joystick_getHatCount(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushnumber(L, SDL_JoystickNumHats(self->joystick));
  return 1;
}

static int l_joystick_getButtonCount(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushnumber(L, SDL_JoystickNumButtons(self->joystick));
  return 1;
}

static int l_joystick_getAxis(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  int idx = luaL_checknumber(L, 2);
  lua_pushnumber(L, SDL_JoystickGetAxis(self->joystick,idx));
  return 1;
}

static int l_joystick_getBall(lua_State *L) {
  int dx,dy;
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  int idx = luaL_checknumber(L, 2);
  SDL_JoystickGetBall(self->joystick,idx,&dx,&dy);
  lua_pushnumber(L, dx);
  lua_pushnumber(L, dy);
  return 2;
}

static int l_joystick_getHat(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  int idx = luaL_checknumber(L, 2);
  int value = SDL_JoystickGetHat(self->joystick,idx);

  lua_newtable(L);
  luax_setfield_bool(L, "up", value & SDL_HAT_UP);
  luax_setfield_bool(L, "down", value & SDL_HAT_DOWN);
  luax_setfield_bool(L, "left", value & SDL_HAT_LEFT);
  luax_setfield_bool(L, "right", value & SDL_HAT_RIGHT);

  return 1;
}

static int l_joystick_getButton(lua_State *L) {
  Joystick *self = luaL_checkudata(L, 1, CLASS_NAME);
  int idx = luaL_checknumber(L, 2);
  lua_pushnumber(L, SDL_JoystickGetButton(self->joystick,idx));
  return 1;
}

static int l_joystick_init(lua_State *L) {
  if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
    luaL_error(L, "could not init joystick");
  }
  /* Make SDL push events through the event system */
  SDL_JoystickEventState(SDL_ENABLE);
  return 0;
}

static int l_joystick_getCount(lua_State *L) {
  lua_pushnumber(L, SDL_NumJoysticks());
  return 1;
}


static int l_joystick_getName(lua_State *L) {
  int idx = luaL_checknumber(L, 1);
  lua_pushstring(L, SDL_JoystickName(idx));
  return 1;
}

static int l_joystick_open(lua_State *L) {
  int idx = luaL_checknumber(L, 1);
  Joystick *self = joystick_new(L);
  self->joystick = SDL_JoystickOpen(idx);
  if (self->joystick == NULL) {
    luaL_error(L, "Could not open game controller. Error: %s", SDL_GetError());
    return 0;
  }
  return 1;
}

int luaopen_joystick(lua_State *L) {
  /* Used for the Joystick object. */
  luaL_Reg reg[] = {
    { "__gc",            l_joystick_gc              },
    { "index",           l_joystick_index           },
    { "getAxisCount",    l_joystick_getAxisCount    },
    { "getBallCount",   l_joystick_getBallCount   },
    { "getHatCount",    l_joystick_getHatCount    },
    { "getButtonCount", l_joystick_getButtonCount },
    { "getAxis",         l_joystick_getAxis         },
    { "getBall",         l_joystick_getBall         },
    { "getHat",          l_joystick_getHat          },
    { "getButton",       l_joystick_getButton       },
    { NULL, NULL }
  };
  ASSERT( luaL_newmetatable(L, CLASS_NAME) );
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}

int luaopen_joystick_functions(lua_State *L) {
  /* Used for the joystick module. */
  luaL_Reg reg[] = {
    { "init",         l_joystick_init        },
    { "open",         l_joystick_open        },
    { "getCount",     l_joystick_getCount    },
    { "getName",      l_joystick_getName     },
    { NULL, NULL }
  };
  luaL_newlib(L, reg);
  return 1;
}
