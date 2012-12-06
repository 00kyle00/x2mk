#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <memory>
#include <vector>
#include "state.h"

std::shared_ptr<IDirectInput8> CreateDirectInput();

std::vector<DIDEVICEINSTANCE> ListConnectedDevices(std::shared_ptr<IDirectInput8> dinput);

class Controller {
public:
  Controller(std::shared_ptr<IDirectInput8> dinput, const DIDEVICEINSTANCE& dev);
  //static std::shared_ptr<Controller> Create(std::shared_ptr<IDirectInput8> dinput, const DIDEVICEINSTANCE& dev);
  //static std::shared_ptr<Controller> Create(int no);

  void query_state(InputState& input);
private:
  std::shared_ptr<IDirectInput8> dinput_;
  std::shared_ptr<IDirectInputDevice8> device_;
  std::shared_ptr<void> acquire_guard_;
};