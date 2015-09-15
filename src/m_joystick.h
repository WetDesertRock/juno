/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <SDL/SDL.h>

#define JOYSTICK_CLASS_NAME "Joystick"

typedef struct {
  SDL_Joystick *joystick;
} Joystick;

#endif
