require "constants"
require "utilities"

Kbd = Keyboard
Mse = Mouse
Ctl = Controller

-- Function is invoked whenever state of controller button is changed
function button_changed(btn)
  local btn_state = Controller.button(btn)
  local value = btn_state and 1 or 0

  -- execute on press and release
  if     btn == CB_A then Mouse.press_release(MB_L, btn_state)
  elseif btn == CB_B then Mouse.press_release(MB_R, btn_state)
  elseif btn == CB_BCK then Keyboard.press_release(VK_ESCAPE, btn_state)
  elseif btn == CB_X then Mouse.press_release(MB_M, btn_state)
  elseif btn == CB_Y then Keyboard.press_release(VK_BROWSER_REFRESH, btn_state)
  --elseif btn == CB_GUIDE then Controller.rumble(value, value)
  elseif btn == CB_UP then Keyboard.press_release(VK_UP, btn_state)
  elseif btn == CB_DOWN then Keyboard.press_release(VK_DOWN, btn_state)
  elseif btn == CB_LEFT then Keyboard.press_release(VK_LEFT, btn_state)
  elseif btn == CB_RIGHT then Keyboard.press_release(VK_RIGHT, btn_state)
  elseif btn == CB_STA then Keyboard.press_release(VK_SPACE, btn_state)
  end

  -- execute only on press
  if btn_state then
    if btn == CB_GUIDE then reload('controller') end
    if btn == CB_RB then spawn('osk.exe') end
    if btn == CB_LB then spawn('notepad.exe') end
  end
end

-- tick function is called every ~10ms. It should contain all logic translating
-- controller state to keyboard/mouse stuff. 
function tick()
  Mouse.move(Controller.stick(0), 0.15, 3);
  Mouse.move(Controller.stick(1), 0.15, 12);
  Mouse.wheel(Controller.trigger(0) - Controller.trigger(1), 0.01, 50)
  --Controller.rumble(Controller.trigger(0), Controller.trigger(1))
end

