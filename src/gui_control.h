#ifndef GUI_CONTROL_H_
#define GUI_CONTROL_H_
#include "raygui.h"
#include "string.h"

#define MAX_LAYOUT_RECS 16

typedef struct GuiControlLayoutState {
  bool seedEditMode;
  char seedEditText[128];
  bool seedApplyPressed;
  bool seedRandomizePressed;
  bool wireframeModePressed;
  Rectangle layoutRecs[MAX_LAYOUT_RECS];
} GuiControlLayoutState;

GuiControlLayoutState InitGuiControlState();
void GuiControlLayout(GuiControlLayoutState* state);

#endif