require "constants"

function spawn(what)
  return os.execute('%windir%\\Sysnative\\cmd.exe /c start ' .. what)
end

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
  elseif btn == CB_GUIDE then Controller.rumble(value, value)
  end

  -- execute only on press
  if btn_state then
    if btn == CB_GUIDE then spawn('notepad.exe') end
	if btn == CB_RB then spawn('osk.exe') end
  end
end

-- tick function is called every ~10ms. It should contain all logic translating
-- controller state to keyboard/mouse stuff. 
function tick()
  Mouse.move(Controller.stick(0), 0.15, 5);
  Mouse.move(Controller.stick(1), 0.15, 15);
  Mouse.wheel(Controller.trigger(0) - Controller.trigger(1), 0.01, 50)
  --Controller.rumble(Controller.trigger(0), Controller.trigger(1))
end

--[[
-- left, right, up, down
prev_arrows = {false, false, false, false}
vk_arrows = {VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN}

function stick_to_arrows(stickno)
  local curr_arrows = {}
  local stick = Controller.stick(stickno)
  curr_arrows[1] = stick[1] < -0.2
  curr_arrows[2] = stick[1] > 0.2
  curr_arrows[3] = stick[2] < -0.2
  curr_arrows[4] = stick[2] > 0.2
  emmit_arrows(prev_arrows, curr_arrows)
  prev_arrows = curr_arrows
end

function emmit_arrows(prev, curr)
  for i=1,4 do
	if prev[i] ~= curr[i] then Keyboard.press_release(vk_arrows[i], curr[i]) end
  end
end

function tick()
  Mouse.move(Controller.stick(0), 0.15, 10);
  stick_to_arrows(1)
  Mouse.wheel(Controller.trigger(0) - Controller.trigger(1), 0.01, 50);
end
]]--