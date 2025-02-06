#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H
#include "raylib.h"
#include "doubly_linked_list.h"
#include "geometry.h"

#define MAX_INDICES 500
#define MAX_VERTICES 500
#define MAX_TRIANGLES 500

typedef struct ConvexShapeTriangle {
  int indices[3];
} ConvexShapeTriangle;

typedef struct ConvexShapeEdge {
  int indices[2];
} ConvexShapeEdge;

typedef struct ConvexShape
{
  int vertexCount;
  Vector3* vertices;
  int triangleCount;
  ConvexShapeTriangle* triangles;
} ConvexShape;

void CreateRandomVertices(Vector3 v[], int n, int seed);
ConvexShape *CreateConvexShape(Vector3 v[], int n, int step);
void ClearConvexShape(ConvexShape* convexSshape);
void DrawConvex(ConvexShape* convexShape, Vector3 position, Color color, Vector3 scale);
void DrawConvexWires(ConvexShape* convexShape, Vector3 position, Color color, Vector3 scale);
bool CanSee(Triangle trig, Vector3 p);
void DrawVertices(Vector3 v[], int n);
void DrawVertexCoords(Vector3 v[], int n, Camera camera);
void DrawVertexIndices(Vector3 v[], int n, Camera camera);
#endif