#include "convex_hull.h"
#include "raymath.h"
#include "rlgl.h"
#include "stack.h"
#include "doubly_linked_list.h"
#include <stdlib.h>
#include <string.h>

static ConvexShapeTriangle * fNewConvexShapeTriangle(int a, int b, int c){
  ConvexShapeTriangle *triangle = MemAlloc(sizeof(ConvexShapeTriangle));
  triangle->indices[0] = a;
  triangle->indices[1] = b;
  triangle->indices[2] = c;
  return triangle;
} 

static ConvexShapeEdge * fNewConvexShapeEdge(int a, int b){
  ConvexShapeEdge *edge = MemAlloc(sizeof(ConvexShapeEdge));
  edge->indices[0] = a;
  edge->indices[1] = b;
  return edge;
}

static Triangle fConvexShapeTriangleToTriangle(ConvexShapeTriangle *triangle, Vector3 vertices[]){
  return (Triangle){
    vertices[triangle->indices[0]],
    vertices[triangle->indices[1]],
    vertices[triangle->indices[2]]
  };
}

static Edge fConvexShapeEdgeToEdge(ConvexShapeEdge *edge, Vector3 vertices[]){
  return (Edge){
    vertices[edge->indices[0]],
    vertices[edge->indices[1]]
  };
}

static void fTryAddHorizonEdgeIfUnique(Vector3 vertices[], int vertexCount, DoublyLinkedList *horizon, DoublyLinkedList *triangles, ConvexShapeEdge* edgeToAdd)
{
  // Check if an edge is shared by two triangles
  DNode *current = horizon->head;
  while (current)
  {
    ConvexShapeEdge *currentEdge = (ConvexShapeEdge *)current->data;
    Edge edge1 = fConvexShapeEdgeToEdge(currentEdge, vertices);
    Edge edge2 = fConvexShapeEdgeToEdge(edgeToAdd, vertices);
    if (CompareEdges(edge1, edge2))
    {
      DListRemoveNode(horizon, current); // Edge is shared, remove from horizon
      return;
    }
    current = current->next;
  }

  // Otherwise, add it to the horizon
  DListPushBack(horizon, (void *)edgeToAdd); // edgeToAdd is already allocated
}

static void fIncrementalConvexHull(Vector3 vertices[], int verticeCount, DoublyLinkedList *triangles, int newVertexIndex)
{
  // The horizon stores the edges surrounding the visible triangles
  DoublyLinkedList *horizon = DListNew();
  // Loop through all triangles and check if the new vertex can "see" them
  // If it can, the triangle should be removed
  DNode *current = triangles->head;
  while (current != NULL)
  {
    ConvexShapeTriangle *indexedTrig = (ConvexShapeTriangle*) current->data;
    Triangle trig = (Triangle){
      vertices[indexedTrig->indices[0]],
      vertices[indexedTrig->indices[1]],
      vertices[indexedTrig->indices[2]]
    };
    if (CanSee(trig, vertices[newVertexIndex]))
    {
      // If the edge is not share, it is part of the horizon
      fTryAddHorizonEdgeIfUnique(vertices, verticeCount, horizon, triangles, fNewConvexShapeEdge(indexedTrig->indices[0], indexedTrig->indices[1]));
      fTryAddHorizonEdgeIfUnique(vertices, verticeCount, horizon, triangles, fNewConvexShapeEdge(indexedTrig->indices[1], indexedTrig->indices[2]));
      fTryAddHorizonEdgeIfUnique(vertices, verticeCount, horizon, triangles, fNewConvexShapeEdge(indexedTrig->indices[2], indexedTrig->indices[0]));
      
      DNode *temp = current;
      current = current->next;
      DListRemoveNode(triangles, temp);
    }
    else
    {
      current = current->next;
    }
  }

  // Form new triangles with the horizon edges
  // The correct order is guaranteed by adding the new vertex as the last vertex
  current = horizon->head;
  while (current)
  {
    ConvexShapeEdge *indexedEdge = (ConvexShapeEdge *)current->data;
    ConvexShapeTriangle *newTrig = fNewConvexShapeTriangle(indexedEdge->indices[0], indexedEdge->indices[1], newVertexIndex);
    DListPushBack(triangles, newTrig);
    current = current->next;
  }

  // Free the horizon
  DListClear(horizon);
  MemFree(horizon);
}

void CreateRandomVertices(Vector3 v[], int n, int seed){
  int maxExtent = 5; 

  SetRandomSeed(seed);
  for (int i = 0; i < n; i++)
  {
    v[i] = (Vector3){
      GetRandomValue(-100, 100) / 100.0f * maxExtent,
      GetRandomValue(-100, 100) / 100.0f * maxExtent,
      GetRandomValue(-100, 100) / 100.0f * maxExtent
    };
  }
}

ConvexShape *CreateConvexShape(Vector3 v[], int n, int step)
{
  if (n < 4 || step == 0)
  {
    return NULL;
  }

  // Object ownership, since ConvexShape also maintains an array of vertices
  Vector3 *vertices = MemAlloc(sizeof(Vector3) * n);
  vertices = memcpy(vertices, v, sizeof(Vector3) * n);
  int vertexCount = n;

  DoublyLinkedList *triangles = DListNew();
  
  for (int i = 0; i < vertexCount; i++)
  {
    Vector3 *vertex = (Vector3 *)MemAlloc(sizeof(Vector3));
    *vertex = vertices[i];
  }

  // Form the initial tetrahedron
  Vector3 a = vertices[0];
  Vector3 b = vertices[1];
  Vector3 c = vertices[2];

  Vector3 ab = Vector3Subtract(b, a);
  Vector3 ac = Vector3Subtract(c, a);

  Vector3 crossProduct = Vector3CrossProduct(ab, ac);

  int dIndex = -1; 
  for (int i = 3; i < vertexCount; i++)
  {
    Vector3 candidate = vertices[i];
    float dot = Vector3DotProduct(Vector3Subtract(candidate, a), crossProduct);
    if (dot < -EPSILON)
    {
      // When D is behind the ABC plane
      dIndex = i;
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(0, 1, 2)); // ABC
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(0, 2, i)); // ACD
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(0, i, 1)); // ADB
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(1, i, 2)); // BDC
      break;
    }
    else if (dot > EPSILON)
    {
      // When D is in front of the ABC plane
      dIndex = i;
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(0, 2, 1)); // ACB
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(0, i, 2)); // ADC
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(0, 1, i)); // ABD
      DListPushBack(triangles, (void *)fNewConvexShapeTriangle(1, 2, i)); // BCD
      break;
    }
  }
  if (dIndex < 0)
  {
    return NULL;
  }

  int buildStep = 1;
  // Add new vertices and form new convex hull everytime
  for (int i = 3; i < vertexCount; i++)
  {
    if (i == dIndex)
    {
      continue;
    }
    if (buildStep == step)
    {
       ConvexShape *shape = (ConvexShape *)MemAlloc(sizeof(ConvexShape));
      shape->vertexCount = n;
      shape->vertices = vertices;
      shape->vertexCount = vertexCount;
      shape->triangles = DListToArray(triangles, sizeof(ConvexShapeTriangle), &shape->triangleCount);
      
      // Free memory
      DListClear(triangles);
      MemFree(triangles);

      return shape;
    }
    fIncrementalConvexHull(vertices, vertexCount, triangles, i);
    buildStep++;
  }


  ConvexShape *shape = (ConvexShape *)MemAlloc(sizeof(ConvexShape));
  shape->vertexCount = n;
  shape->vertices = vertices;
  shape->vertexCount = vertexCount;
  shape->triangles = DListToArray(triangles, sizeof(ConvexShapeTriangle), &shape->triangleCount);
  
  // Free memory
  DListClear(triangles);
  MemFree(triangles);

  return shape;
}

void ClearConvexShape(ConvexShape *convexShape)
{
  if (convexShape == NULL)
  {
    return;
  }
  convexShape->triangleCount = 0;
  MemFree(convexShape->triangles);
  convexShape->triangles = NULL;
  convexShape->vertexCount = 0;
  MemFree(convexShape->vertices);
}

void DrawConvex(ConvexShape *convexShape, Vector3 position, Color color, Vector3 scale)
{
  // Draw Debug Normals
  bool drawDebugNormals = true;
  if (drawDebugNormals){
    for (int i = 0; i < convexShape->triangleCount; i++)
    {
      Triangle trig = fConvexShapeTriangleToTriangle(&convexShape->triangles[i], convexShape->vertices);
      Vector3 centerPos = Vector3Add(Vector3Add(trig.p1, trig.p2), trig.p3);
      centerPos = Vector3Scale(centerPos, 1.0f / 3.0f);
      Vector3 normal = GetTriangleNormal(trig);
      DrawLine3D(centerPos, Vector3Add(centerPos, normal), GREEN);
      DrawSphere(centerPos, 0.05f, GREEN);
    }
  }
  rlPushMatrix();
    // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
    rlTranslatef(position.x, position.y, position.z);
    // rlRotatef(45, 0, 1, 0);
    rlScalef(scale.x, scale.y, scale.z); // NOTE: Vertices are directly scaled on definition
    rlBegin(RL_TRIANGLES);
      rlColor4ub(color.r, color.g, color.b, color.a);

      for (int i = 0; i < convexShape->triangleCount; i++)
      {
        Triangle trig = fConvexShapeTriangleToTriangle(&convexShape->triangles[i], convexShape->vertices);
        Vector3 a = trig.p1;
        Vector3 b = trig.p2;
        Vector3 c = trig.p3;

        // Normal defined by clockwise order
        Vector3 normal = GetTriangleNormal(trig);
        rlNormal3f(normal.x, normal.y, normal.z);
        rlVertex3f(a.x, a.y, a.z);
        rlVertex3f(b.x, b.y, b.z);
        rlVertex3f(c.x, c.y, c.z);
      }

    rlEnd();
  rlPopMatrix();
}

void DrawConvexWires(ConvexShape *convexShape, Vector3 position, Color color, Vector3 scale){
  rlPushMatrix();
  // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
  rlTranslatef(position.x, position.y, position.z);
  // rlRotatef(45, 0, 1, 0);
  rlScalef(scale.x, scale.y, scale.z); // NOTE: Vertices are directly scaled on definition
  rlBegin(RL_LINES);
  rlColor4ub(color.r, color.g, color.b, color.a);

  for (int i = 0; i < convexShape->triangleCount; i++)
  {
    Triangle trig = fConvexShapeTriangleToTriangle(&convexShape->triangles[i], convexShape->vertices);
    Vector3 a = trig.p1;
    Vector3 b = trig.p2;
    Vector3 c = trig.p3;

    rlVertex3f(a.x, a.y, a.z);
    rlVertex3f(b.x, b.y, b.z);

    rlVertex3f(b.x, b.y, b.z);
    rlVertex3f(c.x, c.y, c.z);

    rlVertex3f(c.x, c.y, c.z);
    rlVertex3f(a.x, a.y, a.z);
  }

  rlEnd();
  rlPopMatrix();
}

bool CanSee(Triangle trig, Vector3 p)
{
  Vector3 a = trig.p1;
  Vector3 b = trig.p2;
  Vector3 c = trig.p3;

  // Not normalized face-normal
  Vector3 normal = Vector3CrossProduct(Vector3Subtract(b, a), Vector3Subtract(c, a));
  Vector3 ap = Vector3Subtract(p, a);

  return Vector3DotProduct(normal, ap) >= 0;
}

void DrawVertices(Vector3 v[], int n)
{
  for (int i = 0; i < n; i++)
  {
    DrawSphere(v[i], 0.05f, BLACK);
  }
}

void DrawVertexCoords(Vector3 v[], int n, Camera camera)
{
  for (int i = 0; i < n; i++)
  {
    Vector2 screenPos = GetWorldToScreen(v[i], camera);
    DrawText(TextFormat("%.2f, %.2f, %.2f", v[i].x, v[i].y, v[i].z), (int) screenPos.x + 10, (int) screenPos.y - 4, 8, BLACK);
  }
}

void DrawVertexIndices(Vector3 v[], int n, Camera camera)
{
  for (int i = 0; i < n; i++)
  {
    Vector2 screenPos = GetWorldToScreen(v[i], camera);
    DrawText(TextFormat("%d", i), (int) screenPos.x + 10, (int) screenPos.y - 4, 8, BLUE);
  }
}

