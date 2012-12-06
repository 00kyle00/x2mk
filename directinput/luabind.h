#pragma once
#include <memory>

struct ProgramState;
struct lua_State;

std::shared_ptr<lua_State> CreateLua(ProgramState* controls);
void ExecuteScript(std::shared_ptr<lua_State> L, const char* file);
void CallFunction(std::shared_ptr<lua_State> L, const char* name);
void CallFunction(std::shared_ptr<lua_State> L, const char* name, int arg);
