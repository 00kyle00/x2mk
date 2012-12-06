#include "luabind.h"
#include "state.h"
#include "controller.h"
#include "virtual_devices.h"

#include <stdio.h>
#include <string.h>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

namespace {

ProgramState* glob;

int lua_get_stick_x(lua_State* L) {
  int top = lua_gettop(L);
  if(top != 1) luaL_error(L, "invalid number of parameters");

  int idx = lua_tointeger(L, 1);
  if(idx < 0 && idx > 1) luaL_error(L, "index out of range");
  
  lua_pushnumber(L, glob->state->sticks[idx].x);
  return 1;
}
int lua_get_stick_y(lua_State* L) {
  int top = lua_gettop(L);
  if(top != 1) luaL_error(L, "invalid number of parameters");

  int idx = lua_tointeger(L, 1);
  if(idx < 0 && idx > 1) luaL_error(L, "index out of range");
  
  lua_pushnumber(L, glob->state->sticks[idx].y);
  return 1;
}
int lua_get_trigger(lua_State* L) {
  int top = lua_gettop(L);
  if(top != 1) luaL_error(L, "invalid number of parameters");

  int idx = lua_tointeger(L, 1);
  if(idx < 0 && idx > 1) luaL_error(L, "index out of range");

  lua_pushnumber(L, glob->state->triggers[idx]);
  return 1;
}
int lua_get_button(lua_State* L) {
  int top = lua_gettop(L);
  if(top != 1) luaL_error(L, "invalid number of parameters");
  
  int idx = lua_tointeger(L, 1);
  if(idx < 0 && idx > 15) luaL_error(L, "index out of range");

  lua_pushboolean(L, glob->state->buttons[idx]);
  return 1;
}
int lua_get_stick(lua_State* L) {
  int top = lua_gettop(L);
  if(top != 1) luaL_error(L, "invalid number of parameters");

  int idx = lua_tointeger(L, 1);
  if(idx < 0 && idx > 1) luaL_error(L, "index out of range");
 
  lua_createtable(L, 2, 0);
  int new_table = lua_gettop(L);

  lua_pushnumber(L, glob->state->sticks[idx].x);
  lua_rawseti(L, new_table, 1);
  lua_pushnumber(L, glob->state->sticks[idx].y);
  lua_rawseti(L, new_table, 2);

  return 1;
}
int lua_set_rumble(lua_State* L) {
  int top = lua_gettop(L);
  if(top != 2) luaL_error(L, "invalid number of parameters");
  
  float left = lua_tonumber(L, 1);
  float right = lua_tonumber(L, 2);
  glob->ctl->rumble(left, right);

  return 0;
}
void register_controler_state(lua_State *L) {
  const luaL_Reg funcs[] = {
    {"trigger", lua_get_trigger},
    {"button", lua_get_button},
    {"stick_x", lua_get_stick_x},
    {"stick_y", lua_get_stick_y},
    {"stick", lua_get_stick},
    {"rumble", lua_set_rumble},
    {0, 0}
  };
  luaL_newlib(L, funcs);
  lua_setglobal(L, "Controller");
}

int lua_mouse_move_s(lua_State *L) {
  int top = lua_gettop(L);
  if(top != 4) luaL_error(L, "invalid number of parameters");

  float dx = static_cast<float>(lua_tonumber(L, 1));
  float dy = static_cast<float>(lua_tonumber(L, 2));
  float dead = static_cast<float>(lua_tonumber(L, 3));
  float speed = static_cast<float>(lua_tonumber(L, 4));
  glob->mouse->move(dx, dy, dead, speed);
  return 0;
}
int lua_mouse_move(lua_State *L) {
  int top = lua_gettop(L);
  if(top != 3) luaL_error(L, "invalid number of parameters");

  lua_rawgeti(L, 1, 1);
  lua_rawgeti(L, 1, 2);

  float dx = static_cast<float>(lua_tonumber(L, -2));
  float dy = static_cast<float>(lua_tonumber(L, -1));
  float dead = static_cast<float>(lua_tonumber(L, 2));
  float speed = static_cast<float>(lua_tonumber(L, 3));
  glob->mouse->move(dx, dy, dead, speed);
  return 0;
}
int lua_mouse_wheel(lua_State *L) {
  int top = lua_gettop(L);
  if(top != 3) luaL_error(L, "invalid number of parameters");

  float dx = static_cast<float>(lua_tonumber(L, 1));
  float dead = static_cast<float>(lua_tonumber(L, 2));
  float speed = static_cast<float>(lua_tonumber(L, 3));
  glob->mouse->wheel(dx, dead, speed);
  return 0;
}
int lua_mouse_press_release(lua_State *L) {
  int top = lua_gettop(L);
  if(top != 2) luaL_error(L, "invalid number of parameters");

  int key = lua_tointeger(L, 1);
  bool pressed = (lua_toboolean(L, 2) != 0);
  glob->mouse->press_release(key, pressed);
  return 0;
}
void register_mouse_control(lua_State *L) {
  const luaL_Reg funcs[] = {
    {"press_release", lua_mouse_press_release},
    {"wheel", lua_mouse_wheel},
    {"move_s", lua_mouse_move_s},
    {"move", lua_mouse_move},
    {0, 0}
  };
  luaL_newlib(L, funcs);
  lua_setglobal(L, "Mouse");
}

int lua_keybord_press_release(lua_State *L) {
  int top = lua_gettop(L);
  if(top != 2) luaL_error(L, "invalid number of parameters");

  int key = lua_tointeger(L, 1);
  bool pressed = (lua_toboolean(L, 2) != 0);
  glob->kbd->press_release(key, pressed);
  return 0;
}
void register_keyboard_control(lua_State *L) {
  const luaL_Reg funcs[] = {
    {"press_release", lua_keybord_press_release},
    {0, 0}
  };
  luaL_newlib(L, funcs);
  lua_setglobal(L, "Keyboard");
}

}

std::shared_ptr<lua_State> CreateLua(ProgramState* controls) {
  glob = controls;
  std::shared_ptr<lua_State> L(luaL_newstate(), lua_close);
  luaL_openlibs(L.get());
  
  register_controler_state(L.get());
  register_mouse_control(L.get());
  register_keyboard_control(L.get());

  return L;
}

void ExecuteScript(std::shared_ptr<lua_State> L, const char* file) {
  luaL_dofile(L.get(), file);
}

void CallFunction(std::shared_ptr<lua_State> L, const char* name) {
  lua_getglobal(L.get(), name);
  lua_call(L.get(), 0, 0);
}

void CallFunction(std::shared_ptr<lua_State> L, const char* name, int arg) {
  lua_getglobal(L.get(), name);
  lua_pushinteger(L.get(), arg);
  lua_call(L.get(), 1, 0);
}
