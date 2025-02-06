#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H
#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "rlgl.h"
#include "doubly_linked_list.h"

#define MAX_INDICES 500
#define MAX_VERTICES 500
#define MAX_TRIANGLES 500


typedef struct Triangle {
  Vector3 p1;
  Vector3 p2;
  Vector3 p3;
} Triangle;

typedef struct Edge {
  Vector3 p1;
  Vector3 p2;
} Edge;

typedef struct ConvexShape
{
  size_t triangleCount;
  Triangle* triangles;
} ConvexShape;

void CreateRandomVertices(Vector3 v[], int n, int seed);

ConvexShape *CreateConvexShape(Vector3 v[], int n);
ConvexShape *CreateConvexShapeStep(Vector3 v[], int n, int step);
void ClearConvexShape(ConvexShape* convexShape);
void DrawConvex(ConvexShape* convexShape, Vector3 position, Color color, Vector3 scale);
void DrawConvexWires(ConvexShape* convexShape, Vector3 position, Color color, Vector3 scale);
Vector3 GetTriangleNormal(Triangle triangle);
Triangle* NewTriangle(Vector3 p1, Vector3 p2, Vector3 p3);
int CanSee(Triangle trig, Vector3 p);
void IncrementalConvexHull(DoublyLinkedList* list, Vector3 p);
void AddHorizonEdgeIfUnique(DoublyLinkedList* horizon, DoublyLinkedList* triangles, Edge edge);
int CompareEdges(Edge a, Edge b);
void DrawVertices(Vector3 v[], int n);
void DrawVertexCoords(Vector3 v[], int n, Camera camera);
#endif