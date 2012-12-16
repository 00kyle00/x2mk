x2mk

The tool periodically queries controller state and converts this input to keyboard
strokes or mouse movements through user written lua scripts.

directinput.exe - The binary, will jump to system tray on startup. Assumes scripts are in PATH.
controller.lua  - Essential script loaded by the main binary to handle user input.
                  Function 'tick()' is invoked periodically, while 'button_changed(id)' is
                  invoked whenever controllers button changed their state.
constants.lua   - Non essential script with constants that both 'controller.lua' and main binary use.
utilities.lua   - Non essential script with various helper functions.
