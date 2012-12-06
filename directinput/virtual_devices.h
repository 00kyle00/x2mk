#pragma once

#include <windows.h>
#include <vector>

class VirtualKeyboard {
public:
  void press_release(int code, bool pressed);

  void perform();

private:
  std::vector<INPUT> events_;
};

class VirtualMouse {
public:
  VirtualMouse();

  void move(float dx, float dy, float dead, float speed);
  void wheel(float dx, float dead, float speed);
  void press_release(int button, bool pressed);

  void perform();

private:
  void clear();
  int x_;
  int y_;
  DWORD flags_;
  int wheel_;
};
