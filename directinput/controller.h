#pragma once

#include "state.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <memory>
#include <vector>

std::shared_ptr<IDirectInput8> CreateDirectInput();

std::vector<DIDEVICEINSTANCE> ListConnectedDevices(std::shared_ptr<IDirectInput8> dinput);

class Controller {
public:
  static std::shared_ptr<Controller> Create(std::shared_ptr<IDirectInput8> dinput, const DIDEVICEINSTANCE& dev);
  static std::shared_ptr<Controller> Create(int no);

  virtual void query_state(InputState& input) = 0;
  virtual void rumble(float left, float right) = 0;
  virtual ~Controller() {}
};