#include "convex_hull.h"
#include "doubly_linked_list.h"
#include "stack.h"

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

ConvexShape *CreateConvexShape(Vector3 v[], int n)
{
  if (n < 4)
  {
    return NULL;
  }

  DoublyLinkedList *triangles = DListNew();
  Stack *openVertices = StackNew();

  for (int i = 0; i < n; i++)
  {
    Vector3 *vertex = (Vector3 *)MemAlloc(sizeof(Vector3));
    *vertex = v[i];
    StackPushBack(openVertices, vertex);
  }

  // Form the initial terahedron
  Vector3 a = v[0];
  Vector3 b = v[1];
  Vector3 c = v[2];
  Vector3 d = (Vector3){0};

  Vector3 ab = Vector3Subtract(b, a);
  Vector3 ac = Vector3Subtract(c, a);

  Vector3 cross = Vector3CrossProduct(ab, ac);

  bool found = false;
  for (int i = 3; i < n; i++)
  {
    Vector3 candidate = v[i];
    float dot = Vector3DotProduct(Vector3Subtract(candidate, a), cross);
    if (fabsf(dot) > EPSILON)
    {
      d = candidate;
      found = true;
      break;
    }
  }

  if (!found)
  {
    return NULL;
  }

  DListPushBack(triangles, (void *)NewTriangle(a, b, c));
  DListPushBack(triangles, (void *)NewTriangle(a, c, d));
  DListPushBack(triangles, (void *)NewTriangle(a, d, b));
  DListPushBack(triangles, (void *)NewTriangle(b, d, c));

  // Constanly add new vertices until there are no more vertices left
  while (openVertices->size)
  {
    Vector3 *vertex = (Vector3 *)StackPeek(openVertices);
    IncrementalConvexHull(triangles, *vertex);
    StackPopBack(openVertices);
  }

  ConvexShape *shape = (ConvexShape *)MemAlloc(sizeof(ConvexShape));
  shape->triangles = DListToArray(triangles, sizeof(Triangle), &(shape->triangleCount));

  // Free memory
  DListClear(triangles);
  MemFree(triangles);
  StackClear(openVertices);
  MemFree(openVertices);

  return shape;
}

ConvexShape *CreateConvexShapeStep(Vector3 v[], int n, int step){
  if (n < 4 || step < 1)
  {
    return NULL;
  }

  DoublyLinkedList *triangles = DListNew();
  Stack *openVertices = StackNew();

  for (int i = 0; i < n; i++)
  {
    Vector3 *vertex = (Vector3 *)MemAlloc(sizeof(Vector3));
    *vertex = v[i];
    StackPushBack(openVertices, vertex);
  }

  // The first step is to build the initial tetrahedron
  Vector3 a = v[0];
  Vector3 b = v[1];
  Vector3 c = v[2];
  Vector3 d = (Vector3){0};

  Vector3 ab = Vector3Subtract(b, a);
  Vector3 ac = Vector3Subtract(c, a);

  Vector3 cross = Vector3CrossProduct(ab, ac);

  bool found = false;
  bool positive = false;
  for (int i = 3; i < n; i++)
  {
    Vector3 candidate = v[i];
    float dot = Vector3DotProduct(Vector3Subtract(candidate, a), cross);
    if (fabsf(dot) > EPSILON)
    {
      if (dot > 0){
        positive = true;
      }
      d = candidate;
      found = true;
      break;
    }
  }

  if (!found)
  {
    return NULL;
  }

  if (!positive){
    DListPushBack(triangles, (void *)NewTriangle(a, b, c));
    DListPushBack(triangles, (void *)NewTriangle(a, c, d));
    DListPushBack(triangles, (void *)NewTriangle(a, d, b));
    DListPushBack(triangles, (void *)NewTriangle(b, d, c));
  } else {
    DListPushBack(triangles, (void *)NewTriangle(a, c, b));
    DListPushBack(triangles, (void *)NewTriangle(a, d, c));
    DListPushBack(triangles, (void *)NewTriangle(a, b, d));
    DListPushBack(triangles, (void *)NewTriangle(b, c, d));
  }
  
  ConvexShape *shape = (ConvexShape *)MemAlloc(sizeof(ConvexShape));

  int i = 0;
  // Constanly add new vertices until there are no more vertices left
  while (openVertices->size)
  {
    if (i + 1 == step)
    {
      shape->triangles = DListToArray(triangles, sizeof(Triangle), &(shape->triangleCount));
      // Free memory
      DListClear(triangles);
      MemFree(triangles);
      StackClear(openVertices);
      MemFree(openVertices);
      return shape;
    }
    
    Vector3 *vertex = (Vector3 *)StackPeek(openVertices);
    IncrementalConvexHull(triangles, *vertex);
    StackPopBack(openVertices);
    
    i++;
  }
  
  shape->triangles = DListToArray(triangles, sizeof(Triangle), &(shape->triangleCount));
  
  // Free memory
  DListClear(triangles);
  MemFree(triangles);
  StackClear(openVertices);
  MemFree(openVertices);

  return shape;
} 

// Free the memory
void ClearConvexShape(ConvexShape *convexShape)
{
  if (convexShape == NULL)
  {
    return;
  }
  convexShape->triangleCount = 0;
  MemFree(convexShape->triangles);
  convexShape->triangles = NULL;
}

void DrawConvex(ConvexShape *convexShape, Vector3 position, Color color, Vector3 scale)
{
  // Draw Debug Normal
  for (int i = 0; i < convexShape->triangleCount; i++)
  {
    Vector3 centerPos = Vector3Add(Vector3Add(convexShape->triangles[i].p1, convexShape->triangles[i].p2), convexShape->triangles[i].p3);
    centerPos = Vector3Scale(centerPos, 1.0f / 3.0f);
    Vector3 normal = GetTriangleNormal(convexShape->triangles[i]);
    DrawLine3D(centerPos, Vector3Add(centerPos, normal), GREEN);
    DrawSphere(centerPos, 0.05f, GREEN);
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
    Vector3 a = convexShape->triangles[i].p1;
    Vector3 b = convexShape->triangles[i].p2;
    Vector3 c = convexShape->triangles[i].p3;

    // Normal defined by clockwise order
    Vector3 normal = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(b, a), Vector3Subtract(c, a)));
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
    Vector3 a = convexShape->triangles[i].p1;
    Vector3 b = convexShape->triangles[i].p2;
    Vector3 c = convexShape->triangles[i].p3;

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


Vector3 GetTriangleNormal(Triangle triangle)
{
  Vector3 a = triangle.p1;
  Vector3 b = triangle.p2;
  Vector3 c = triangle.p3;

  Vector3 normal = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(b, a), Vector3Subtract(c, a)));
  return normal;
}

Triangle *NewTriangle(Vector3 p1, Vector3 p2, Vector3 p3)
{
  Triangle *trig = MemAlloc(sizeof(Triangle));
  *trig = (Triangle){p1, p2, p3};
  return trig;
}

int CanSee(Triangle trig, Vector3 p)
{
  Vector3 a = trig.p1;
  Vector3 b = trig.p2;
  Vector3 c = trig.p3;

  // Not normalized face-normal
  Vector3 normal = Vector3CrossProduct(Vector3Subtract(b, a), Vector3Subtract(c, a));
  Vector3 ap = Vector3Subtract(p, a);

  return Vector3DotProduct(normal, ap) >= 0;
}

void IncrementalConvexHull(DoublyLinkedList *triangles, Vector3 p)
{
  DoublyLinkedList *horizon = DListNew(); // Store the horizon edges

  DNode *current = triangles->head;
  while (current != NULL)
  {
    Triangle *tri = (Triangle *)current->data;

    if (CanSee(*tri, p))
    {
      // Check edges before removal
      // Triangle are in counter clockwise order to form the normal
      // Keeping the same order will make the normal point in the same direction
      AddHorizonEdgeIfUnique(horizon, triangles, (Edge){tri->p1, tri->p2});
      AddHorizonEdgeIfUnique(horizon, triangles, (Edge){tri->p2, tri->p3});
      AddHorizonEdgeIfUnique(horizon, triangles, (Edge){tri->p3, tri->p1});

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
  // The correct order is guaranteed by adding p as the last vertex
  current = horizon->head;
  while (current)
  {
    Edge *edge = (Edge *)current->data;
    Triangle *newTrig = NewTriangle(edge->p1, edge->p2, p);
    DListPushBack(triangles, newTrig);
    current = current->next;
  }

  // Free the horizon
  DListClear(horizon);
  MemFree(horizon);
}

void AddHorizonEdgeIfUnique(DoublyLinkedList *horizon, DoublyLinkedList *triangles, Edge edge)
{
  DNode *current = horizon->head;
  while (current)
  {
    Edge *currentEdge = (Edge *)current->data;
    if (CompareEdges(*currentEdge, edge))
    {
      DListRemoveNode(horizon, current); // Edge is shared, remove from horizon
      return;
    }
    current = current->next;
  }

  // Otherwise, add it to the horizon
  Edge *newEdge = MemAlloc(sizeof(Edge));
  *newEdge = (Edge){edge.p1, edge.p2};
  DListPushBack(horizon, newEdge);
}

int CompareEdges(Edge a, Edge b)
{
  return (Vector3Equals(a.p1, b.p1) && Vector3Equals(a.p2, b.p2)) ||
         (Vector3Equals(a.p1, b.p2) && Vector3Equals(a.p2, b.p1));
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

