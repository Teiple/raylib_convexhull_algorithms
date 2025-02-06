#include "gui_control.h"


GuiControlLayoutState InitGuiControlState() {
  GuiControlLayoutState state = {0};

  state.seedEditMode = false;
  strcpy(state.seedEditText, "seed");
  strcpy(state.stepEditText, "0");
  state.seedApplyPressed = false;
  state.seedRandomizePressed = false;
  state.wireframeModePressed = false;
  state.showResultPressed = false;
  state.clearPressed = false;
  state.stepEditMode = false;
  state.stepSubmitted = false;
  state.nextStepPressed = false;
  state.prevStepPressed = false;
  
  // Bounding GroupBox
  state.layoutRecs[0] = (Rectangle){600, 20, 180, 400};
  // Seed Label
  state.layoutRecs[1] = (Rectangle){610, 30, 100, 20};
  // Seed Edit TextBox
  state.layoutRecs[2] = (Rectangle){610, 50, 150, 20};
  // Randomize Seed Button
  state.layoutRecs[3] = (Rectangle){610, 80, 70, 20};
  // Apply Seed Button
  state.layoutRecs[4] = (Rectangle){690, 80, 70, 20};
  // Wireframe Mode Button
  state.layoutRecs[5] = (Rectangle){610, 110, 20, 20};
  // Show Result Button
  state.layoutRecs[6] = (Rectangle){610, 140, 70, 20};
  // Clear Button
  state.layoutRecs[11] = (Rectangle){690, 140, 70, 20};
  // Step Label
  state.layoutRecs[7] = (Rectangle){610, 170, 100, 20};
  // Step Edit TextBox
  state.layoutRecs[8] = (Rectangle){610, 190, 150, 20};
  // Prev | Next Buttons
  state.layoutRecs[9] = (Rectangle){610, 220, 70, 20};
  state.layoutRecs[10] = (Rectangle){690, 220, 70, 20};
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
  state->showResultPressed = GuiButton(state->layoutRecs[6], "Show Result");
  GuiLabel(state->layoutRecs[7], "Step");
  state->stepSubmitted = GuiTextBox(state->layoutRecs[8], state->stepEditText, 128, state->stepEditMode);
  if (state->stepSubmitted){
    state->stepEditMode = !state->stepEditMode;
  }
  state->prevStepPressed = GuiButton(state->layoutRecs[9], "Prev");
  state->nextStepPressed = GuiButton(state->layoutRecs[10], "Next");
  state->clearPressed = GuiButton(state->layoutRecs[11], "Clear");
}