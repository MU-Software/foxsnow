#ifndef FS_LIB_CAMERA_FPS
#define FS_LIB_CAMERA_FPS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "fs_node_render.h"

extern bool isColliding;
extern double rads;
extern float cylindricalYLookAtPos;
extern float xMovementSpeed, yMovementSpeed;

void FS_camera_fps_rotation(int xrel, int yrel);
void FS_camera_fps_position();

#endif