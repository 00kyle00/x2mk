#pragma once

struct InputState {
  struct Stick {
    float x;
    float y;
  };
  Stick sticks[2];
  float triggers[2];
  bool buttons[16];
};

class Controller;
class VirtualKeyboard;
class VirtualMouse;

struct ProgramState {
  InputState* state;
  Controller* ctl;
  VirtualKeyboard* kbd;
  VirtualMouse* mouse;
};
