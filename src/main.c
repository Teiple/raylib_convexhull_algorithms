#include "raylib.h"
#include "convex_hull.h"
#include "cam_control.h"
#include "time.h"
#include "raygui.h"
#include "string.h"
#include "gui_control.h"

int main() 
{
  // Initialization
  //--------------------------------------------------------------------------------------
  SetRandomSeed(time(NULL) % 10000);
  
  const int screenWidth = 800;
  const int screenHeight = 450;
	
  InitWindow(screenWidth, screenHeight, "raylib");

  Camera camera = { 0 };
  camera.position = (Vector3){ 5.0f, 2.0f, 8.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  CamSettings camSettings = {
    .moveSpeed = 10.0f,
    .mouseSenstivity = 0.5f,
    .spanSpeed = 0.1f,
    .zoomSpeed = 2.0f
  };
  
  // Variables
  const int vertexCount = 20;
  Vector3 vertices[vertexCount];
  int vertexRandomSeed = 8367;//rand() % 10000;
  int constructionStep = 0;
  CreateRandomVertices(vertices, vertexCount, vertexRandomSeed);
  ConvexShape *convexShape = NULL;
  GuiControlLayoutState guiControlLayoutState = InitGuiControlState();
  strcpy(guiControlLayoutState.seedEditText, TextFormat("%d", vertexRandomSeed));

  SetTargetFPS(60);         // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    UpdateCam(&camera, &camSettings);
    //// Randomize the seed
    if (guiControlLayoutState.seedRandomizePressed){
      vertexRandomSeed = rand() % 10000;
      CreateRandomVertices(vertices, vertexCount, vertexRandomSeed);
      
      ClearConvexShape(convexShape);
      MemFree(convexShape);
      convexShape = NULL;
      
      strcpy(guiControlLayoutState.seedEditText, TextFormat("%d", vertexRandomSeed));
      constructionStep = 0;
    }
    //// Apply a different seed
    if (guiControlLayoutState.seedApplyPressed){
      vertexRandomSeed = rand() % 10000;
      CreateRandomVertices(vertices, vertexCount, vertexRandomSeed);
      
      ClearConvexShape(convexShape);
      MemFree(convexShape);
      convexShape = NULL;
      constructionStep = 0;
    }
    //// Show the process
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_N)){
      if (IsKeyPressed(KEY_P)){
        constructionStep--;
      } else{
        constructionStep++;
      }
      ClearConvexShape(convexShape);
      convexShape = CreateConvexShapeStep(vertices, vertexCount, constructionStep);
    }

    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

      ClearBackground(RAYWHITE);

      BeginMode3D(camera);
        DrawVertices(vertices, vertexCount);
        if (convexShape){
          if (guiControlLayoutState.wireframeModePressed){
            DrawConvexWires(convexShape, (Vector3){0, 0, 0}, RED, (Vector3){1, 1, 1});
          } else {
            DrawConvex(convexShape, (Vector3){0, 0, 0}, RED, (Vector3){1, 1, 1});
            DrawConvexWires(convexShape, (Vector3){0, 0, 0}, ORANGE, (Vector3){1, 1, 1});
          }
        }
        DrawGrid(20, 1.0f);
      EndMode3D();

      DrawVertexCoords(vertices, vertexCount, camera);
      DrawText(TextFormat("Seed: %d", vertexRandomSeed), 10, 40, 20, DARKGRAY);
      DrawText(TextFormat("Step: %d", constructionStep), 10, 60, 20, DARKGRAY);
      GuiControlLayout(&guiControlLayoutState);
      
      DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();    // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

  