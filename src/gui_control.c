#include "gui_control.h"


GuiControlLayoutState InitGuiControlState() {
  GuiControlLayoutState state = {0};

  state.seedEditMode = false;
  strcpy(state.seedEditText, "seed");
  state.seedApplyPressed = false;
  state.seedRandomizePressed = false;

  // Bounding GroupBox
  state.layoutRecs[0] = (Rectangle){600, 20, 180, 200};
  // "Seed" Label
  state.layoutRecs[1] = (Rectangle){610, 30, 100, 20};
  // Seed Edit TextBox
  state.layoutRecs[2] = (Rectangle){610, 50, 150, 20};
  // Randomize Seed Button
  state.layoutRecs[3] = (Rectangle){610, 80, 70, 20};
  // Apply Seed Button
  state.layoutRecs[4] = (Rectangle){690, 80, 70, 20};
  // Wireframe Mode Button
  state.layoutRecs[5] = (Rectangle){610, 110, 20, 20};

  return state;
}

void GuiControlLayout(GuiControlLayoutState* state){
  GuiGroupBox(state->layoutRecs[0], "Settings");
  GuiLabel(state->layoutRecs[1], "Seed");
  if (GuiTextBox(state->layoutRecs[2], state->seedEditText, 128, state->seedEditMode)){
    state->seedEditMode = !state->seedEditMode;
  }
  state->seedRandomizePressed = GuiButton(state->layoutRecs[3], "Randomize");
  state->seedApplyPressed = GuiButton(state->layoutRecs[4], "Apply");
  GuiCheckBox(state->layoutRecs[5], "Wireframe", &state->wireframeModePressed);
}