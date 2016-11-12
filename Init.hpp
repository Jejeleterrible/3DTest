#pragma once
#include <Nazara/Audio.hpp>
#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Lua.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Noise.hpp>
#include <Nazara/Physics2D.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/LuaAPI.hpp>
#include <iostream>


int width = 0;
int height = 0;
Nz::String title = "";
bool fullscreen = false;
float sensitivity = 0.2f;
float cameraSpeed = 0.4f;
Nz::String file_skybox = "";
float ground_width = 10000.f;
float ground_height = 10000.f;
float zFar = 5000.f;
float zNear = 0.1f;
Nz::String file_ground_tex = "";
int light_type = 0;
float gravity = 5.f;
float eye_height = 100.f;


inline void InitFromLua(Nz::LuaInstance &lua)
{
	lua.PushGlobal("LightTypeSpot", 0);
	lua.PushGlobal("LightTypePoint", 1);
	lua.PushGlobal("LightypeDirectionnal", 2);


	if (lua.ExecuteFromFile("init.lua"))
	{
		lua.GetGlobal("width");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			width = (int)lua.ToInteger(-1);

		lua.GetGlobal("height");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			height = (int)lua.ToInteger(-1);

		lua.GetGlobal("title");

		if (lua.GetType(-1) == Nz::LuaType_String)
			title = lua.ToString(-1);

		lua.GetGlobal("fullscreen");

		if (lua.GetType(-1) == Nz::LuaType_Boolean)
			fullscreen = lua.ToBoolean(-1);

		lua.GetGlobal("sensitivity");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			sensitivity = (float)lua.ToNumber(-1);

		lua.GetGlobal("speed");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			cameraSpeed = (float)lua.ToNumber(-1);

		lua.GetGlobal("skybox");

		if (lua.GetType(-1) == Nz::LuaType_String)
			file_skybox = lua.ToString(-1);

		lua.GetGlobal("ground_width");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			ground_width = (float)lua.ToNumber(-1);

		lua.GetGlobal("ground_height");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			ground_height = (float)lua.ToNumber(-1);

		lua.GetGlobal("zFar");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			zFar = (float)lua.ToNumber(-1);

		lua.GetGlobal("zNear");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			zNear = (float)lua.ToNumber(-1);

		lua.GetGlobal("ground_texture");

		if (lua.GetType(-1) == Nz::LuaType_String)
			file_ground_tex = lua.ToString(-1);

		lua.GetGlobal("light_type");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			light_type = (int)lua.ToInteger(-1);

		lua.GetGlobal("gravity");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			gravity = (float)lua.ToNumber(-1);

		lua.GetGlobal("eye_height");

		if (lua.GetType(-1) == Nz::LuaType_Number)
			eye_height = (float)lua.ToNumber(-1);

	}
}