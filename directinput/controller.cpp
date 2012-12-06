#include "controller.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>
#include "virtual_devices.h"
#include "state.h"

namespace {

void ensure_hresult(HRESULT hr, const char* message) {
  if(hr != DI_OK)
    throw std::runtime_error(message);
}

IDirectInputDevice8* CreateDevice(std::shared_ptr<IDirectInput8> dinput, const DIDEVICEINSTANCE& inst) {
  IDirectInputDevice8* dev = 0;

  ensure_hresult(dinput->CreateDevice(inst.guidInstance, &dev, 0),
    "failed to create device");

  return dev;
}

float Normalize360Stick(int value) {
  float v = value / static_cast<float>(0xffff);
  return v * 2  - 1;
}

float Normalize360Trigger(int value) {
  float v = value / static_cast<float>(0xffff);
  return v * 2  - 1;
}

void print_device_state(const DIJOYSTATE2& state) {
  std::cout << "x0: " << state.lX << "\n";
  std::cout << "y0: " << state.lY << "\n";
  std::cout << "z0: " << state.lZ << "\n"; //triggers
  std::cout << "x1: " << state.lRx << "\n";
  std::cout << "y1: " << state.lRy << "\n";
  std::cout << "z1: " << state.lRz << "\n";
  std::cout << "rgls: " << state.rglSlider[0] << ", " << state.rglSlider[1] << "\n";
  std::cout << "rgdw: " << state.rgdwPOV[0] << ", " << state.rgdwPOV[1] << ", "
      << state.rgdwPOV[2] << ", " << state.rgdwPOV[3] << "\n";
  std::cout << "btns: ";
  for(int b : state.rgbButtons) {
    std::cout << b << ", ";
  }
  std::cout << "\n\n\n\n\n\n\n";
}

void input_from_device_state(const DIJOYSTATE2& istate, InputState& ostate) {
  ostate.sticks[0].x = Normalize360Stick(istate.lX);
  ostate.sticks[0].y = Normalize360Stick(istate.lY);
  ostate.sticks[1].x = Normalize360Stick(istate.lRx);
  ostate.sticks[1].y = Normalize360Stick(istate.lRy);
  float triggers = Normalize360Stick(istate.lZ);
  if(triggers < 0) {
    ostate.triggers[0] = -triggers;
    ostate.triggers[1] = 0;
  }
  else {
    ostate.triggers[0] = 0;
    ostate.triggers[1] = triggers;
  }
  for(int i = 0; i < 16; ++i)
    ostate.buttons[i] = (istate.rgbButtons[i] != 0);
}

}

std::shared_ptr<IDirectInput8> CreateDirectInput() {
  IDirectInput8* dinput = 0;

  ensure_hresult(DirectInput8Create(
    (HINSTANCE) GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, 0),
    "failed to create DirectInput interface");

  return std::shared_ptr<IDirectInput8>(dinput, [](IDirectInput8* ptr){ ptr->Release(); });
}

std::vector<DIDEVICEINSTANCE> ListConnectedDevices(std::shared_ptr<IDirectInput8> dinput) {
  std::vector<DIDEVICEINSTANCE> devices;
  auto callback = [](LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) -> BOOL {
    static_cast<std::vector<DIDEVICEINSTANCE>*>(pvRef)->push_back(*lpddi);
    return DIENUM_CONTINUE;
  };
  ensure_hresult(
    dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, callback, &devices, DIEDFL_ATTACHEDONLY),
    "failed to enum direct input devices");

  return devices;
}

Controller::Controller(std::shared_ptr<IDirectInput8> dinput, const DIDEVICEINSTANCE& dev) {
  device_.reset(CreateDevice(dinput, dev),
    [](IDirectInputDevice8* ptr){ ptr->Release(); });

  ensure_hresult(device_->SetDataFormat(&c_dfDIJoystick2),
    "failed to set data format");
  ensure_hresult(device_->Acquire(),
    "failed to acquire device");

  acquire_guard_.reset((void*)nullptr,
    [=](void*){device_->Unacquire();});
}

void Controller::query_state(InputState& input) {
  DIJOYSTATE2 state;
  ensure_hresult(device_->GetDeviceState(sizeof(DIJOYSTATE2), &state),
    "failed to obtain device state");

  input_from_device_state(state, input);
}
