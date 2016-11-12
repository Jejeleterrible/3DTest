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




struct InitParams
{
	int width;
	int height;
	Nz::String title;
	bool fullscreen = false;
	float sensitivity = 0.2f;
	float cameraSpeed = 0.4f;
	Nz::String skybox;
	float ground_width = 10000.f;
	float ground_height = 10000.f;
	float zFar = 5000.f;
	float zNear = 0.1f;
	Nz::String ground_texture;
	int light_type;
	float gravity = 5.0f;
	float eye_height = 100.f;
};


inline void InitFromLua(Nz::LuaInstance &lua, InitParams &params)
{
	lua.PushGlobal("LightTypeSpot", 0);
	lua.PushGlobal("LightTypePoint", 1);
	lua.PushGlobal("LightypeDirectionnal", 2);


	if (lua.ExecuteFromFile("init.lua"))
	{
		lua.GetGlobal("width");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.width = (int)lua.ToInteger(-1);

		lua.GetGlobal("height");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.height = (int)lua.ToInteger(-1);

		lua.GetGlobal("title");
		if (lua.GetType(-1) == Nz::LuaType_String)
			params.title = lua.ToString(-1);

		lua.GetGlobal("fullscreen");
		if (lua.GetType(-1) == Nz::LuaType_Boolean)
			params.fullscreen = lua.ToBoolean(-1);

		lua.GetGlobal("sensitivity");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.sensitivity = (float)lua.ToNumber(-1);

		lua.GetGlobal("speed");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.cameraSpeed = (float)lua.ToNumber(-1);
		
		lua.GetGlobal("skybox");
		if (lua.GetType(-1) == Nz::LuaType_String)
			params.skybox = lua.ToString(-1);

		lua.GetGlobal("ground_width");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.ground_width = (float)lua.ToNumber(-1);

		lua.GetGlobal("ground_height");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.ground_height = (float)lua.ToNumber(-1);

		lua.GetGlobal("zFar");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.zFar = (float)lua.ToNumber(-1);

		lua.GetGlobal("zNear");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.zNear = (float)lua.ToNumber(-1);

		lua.GetGlobal("ground_texture");
		if (lua.GetType(-1) == Nz::LuaType_String)
			params.ground_texture = lua.ToString(-1);

		lua.GetGlobal("light_type");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.light_type = (int)lua.ToInteger(-1);

		lua.GetGlobal("gravity");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.gravity = (float)lua.ToNumber(-1);

		lua.GetGlobal("eye_height");
		if (lua.GetType(-1) == Nz::LuaType_Number)
			params.eye_height = (float)lua.ToNumber(-1);

	}
}