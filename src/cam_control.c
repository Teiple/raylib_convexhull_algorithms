#include "cam_control.h"
#include "raymath.h"
#include "rcamera.h"

void UpdateCam(Camera* cam, CamSettings* camSettings)
{
  Vector3 movement = {0};
  Vector3 rotation = {0};
  float zoom = 0.0f;
  // Move and Look
  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
  {
    // Mouse look
    Vector2 mouseDelta = Vector2Scale(GetMouseDelta(), camSettings->mouseSenstivity);
    rotation.x = mouseDelta.x;
    rotation.y = mouseDelta.y;
    // WASD to move
    if (IsKeyDown(KEY_W)) movement.x += camSettings->moveSpeed * GetFrameTime();
    if (IsKeyDown(KEY_A)) movement.y -= camSettings->moveSpeed * GetFrameTime();
    if (IsKeyDown(KEY_S)) movement.x -= camSettings->moveSpeed * GetFrameTime();
    if (IsKeyDown(KEY_D)) movement.y += camSettings->moveSpeed * GetFrameTime();
    // Change the speed of the camera with scroll wheel
    camSettings->moveSpeed += GetMouseWheelMove() * 5.0f;
    camSettings->moveSpeed = Clamp(camSettings->moveSpeed, 1.0f, 100.0f);
  } else {
    // Zoom
    zoom -= GetMouseWheelMove() * camSettings->zoomSpeed;
  }

  if (IsKeyDown(KEY_LEFT_SHIFT)) movement.z += camSettings->moveSpeed * GetFrameTime();
  if (IsKeyDown(KEY_SPACE)) movement.z -= camSettings->moveSpeed * GetFrameTime();
  UpdateCameraPro(cam, movement, rotation, zoom);
}

