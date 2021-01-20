#include "fs_camera_fps.h"

void FS_camera_fps_rotation(int xrel, int yrel) {
    rads -= (double)xrel * 0.005f;

    float old_cylindricalYLookAtPos = cylindricalYLookAtPos;
    cylindricalYLookAtPos -= yrel * 0.02f;
    if (-15.0f > cylindricalYLookAtPos || cylindricalYLookAtPos > 25.0f)
        cylindricalYLookAtPos = old_cylindricalYLookAtPos;

    camera.lookPoint[0] = camera.pos[0] + (10.0f * cos(rads));
    camera.lookPoint[1] = camera.pos[1] + cylindricalYLookAtPos;
    camera.lookPoint[2] = camera.pos[2] - (10.0f * sin(rads));
    commitCamera();
}


void FS_camera_fps_position() {
    float prev_pos[3] = { 0 };
    memcpy(prev_pos, camera.pos, sizeof(float) * 3);

    GLdouble xDelta, zDelta;
    double frads = M_PI * 0.5 - rads;
    
    // We need to normalize speed of x delta and y delta,
    // because there's a scenario that diagonal walking is faster than straight walking.
    // Of course, if x delta or y delta is zero, we don't need to normalize it.
    float resultXmovement = xMovementSpeed;
    float resultYmovement = yMovementSpeed;
    float resultMovementVec = sqrtf((resultXmovement * resultXmovement) + (resultYmovement * resultYmovement));
    dprint("Vec: %f\r", resultMovementVec);
    if (xMovementSpeed && yMovementSpeed) {
        resultXmovement /= resultMovementVec;
        resultYmovement /= resultMovementVec;
    }
    
    xDelta = resultXmovement * cos(rads) + resultYmovement * cos(frads);
    zDelta = -resultXmovement * sin(rads) + resultYmovement * sin(frads);

    camera.pos[0] += (float)xDelta;
    camera.pos[2] += (float)zDelta;
    camera.lookPoint[0] += (float)xDelta;
    camera.lookPoint[2] += (float)zDelta;

    // Check collide, but we didn't implented yet, so just if-false to this.
    if (0) {
        memcpy(camera.pos, prev_pos, sizeof(float) * 3);
        isColliding = true;
    }
    else {
        isColliding = false;
    }

    commitCamera();
}