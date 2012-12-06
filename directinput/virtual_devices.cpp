#include "virtual_devices.h"
#include <cstdlib>
#include <cassert>

void VirtualKeyboard::press_release(int code, bool pressed) {
  INPUT input = {};
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = code;
  input.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);
  events_.push_back(input);
}
void VirtualKeyboard::perform() {
  if(!events_.empty()) {
    SendInput(events_.size(), &events_[0], sizeof(events_[0]));
    events_.clear();
  }
}


VirtualMouse::VirtualMouse() {
  clear();
}

void VirtualMouse::move(float dx, float dy, float dead, float speed) {
  if(fabs(dx) > dead || fabs(dy) > dead) {
    flags_ |= MOUSEEVENTF_MOVE;
    x_ += static_cast<int>(dx * speed);
    y_ += static_cast<int>(dy * speed);
  }
}

void VirtualMouse::wheel(float dx, float dead, float speed) {
  if(fabs(dx) > dead) {
    flags_ |= MOUSEEVENTF_WHEEL;
    wheel_ += static_cast<int>(dx * speed);
  }
}

void VirtualMouse::press_release(int button, bool pressed) {
  assert(button < 3);
  const int lookup[][2] = {
    {MOUSEEVENTF_LEFTUP, MOUSEEVENTF_LEFTDOWN},
    {MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_MIDDLEDOWN},
    {MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_RIGHTDOWN},
  };
  flags_ |= lookup[button][pressed];
}

void VirtualMouse::perform() {
  if(flags_ != 0) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = flags_;
    input.mi.dx = x_;
    input.mi.dy = y_;
    input.mi.mouseData = wheel_;
    SendInput(1, &input, sizeof(input));
    clear();
  }
}

void VirtualMouse::clear() {
  x_ = 0;
  y_ = 0;
  flags_ = 0;
  wheel_ = 0;
}
