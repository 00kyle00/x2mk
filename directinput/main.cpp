#include "virtual_devices.h"
#include "controller.h"
#include "luabind.h"
#include "state.h"
#include "window.h"

#include <thread>
#include <memory>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <atomic>

std::atomic<bool> gui_finished(false);

void print_exception_info() {
  try {
    throw;
  }
  catch(std::exception& e) {
    std::cout << e.what() << "\n";
  }
  catch(...) {
    std::cout << "unknown exception\n";
  }
}

int main() {
  std::thread gui_thread(RunGUI);

  try {
    //initialize stuff
    auto dinput = CreateDirectInput();
    auto devices = ListConnectedDevices(dinput);

    //select device
    size_t devnum = 0;
    if(devnum >= devices.size())
      throw std::runtime_error("bad device number");

    bool use_xinput = true;
    auto ctl = use_xinput ? Controller::Create(devnum) :
      Controller::Create(dinput, devices[devnum]);

    VirtualMouse mouse;
    VirtualKeyboard keyboard;
    InputState state = {};

    ProgramState prog_state = {
      &state, ctl.get(), &keyboard, &mouse };

    auto L = CreateLua(&prog_state);
    ExecuteScript(L, "controller.lua");

    while(!gui_finished.load()) {
      // Gather state.
      InputState& input = *prog_state.state;
      ctl->query_state(input);
      static InputState old_input = input;

      // Notify about changed buttons.
      for(auto liter = input.buttons, riter = old_input.buttons;
        liter != std::end(input.buttons);
        ++liter, ++riter)
      {
        if(*liter != *riter)
          CallFunction(L, "button_changed", liter - input.buttons);
      }

      // Notify about elapsed timeslice.
      CallFunction(L, "tick");

      // Commit scripted input.
      mouse.perform();
      keyboard.perform();

      old_input = input;
      Sleep(10);
    }
  }
  catch(...) {
    print_exception_info();
    ShowWindow();
    // Notify gui thread that we are done.
    // Not technically sound (WM_QUIT is not a normal message), but should do.
    PostThreadMessage(
      GetThreadId(gui_thread.native_handle()), WM_QUIT, 0, 0);

    system("pause");
  }
  
  gui_thread.join();
}
