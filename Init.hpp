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



#define LIGHT_SPOT 0
#define LIGHT_POINT 1
#define LIGHT_DIRECTIONAL 2


struct InitParams
{
	int width = 800;
	int height = 600;
	Nz::String title = "Nazara Window";
	bool fullscreen = false;
	float sensitivity = 0.2f;
	float cameraSpeed = 20.f;
	Nz::String skybox = "";
	Nz::Color sky_color = Nz::Color::Cyan;
	//float ground_width = 10000.f;
	//float ground_height = 10000.f;
	float zFar = 5000.f;
	float zNear = 0.1f;
	Nz::String ground_texture;
	Nz::Color ground_color = Nz::Color::White;
	int light_type = LIGHT_DIRECTIONAL;
	float gravity = 5.0f;
	float eye_height = 100.f;
	float ground_radius = 1000.f;
};


inline void InitFromLua(Nz::LuaInstance &lua, InitParams &params)
{
	lua.PushGlobal("LightTypeSpot", LIGHT_SPOT);
	lua.PushGlobal("LightTypePoint", LIGHT_POINT);
	lua.PushGlobal("LightypeDirectional", LIGHT_DIRECTIONAL);


	if (lua.ExecuteFromFile("init.lua"))
	{
		if (lua.GetGlobal("width") == Nz::LuaType_Number)
			params.width = (int)lua.ToInteger(-1);
		lua.Pop();

		if (lua.GetGlobal("height") == Nz::LuaType_Number)
			params.height = (int)lua.ToInteger(-1);
		lua.Pop();
		
		if (lua.GetGlobal("title") == Nz::LuaType_String)
			params.title = lua.ToString(-1);
		lua.Pop();
		
		if (lua.GetGlobal("fullscreen") == Nz::LuaType_Boolean)
			params.fullscreen = lua.ToBoolean(-1);
		lua.Pop();
		
		if (lua.GetGlobal("sensitivity") == Nz::LuaType_Number)
			params.sensitivity = (float)lua.ToNumber(-1);
		lua.Pop();
		
		if (lua.GetGlobal("speed") == Nz::LuaType_Number)
			params.cameraSpeed = (float)lua.ToNumber(-1);
		lua.Pop();
		
		if (lua.GetGlobal("skybox") == Nz::LuaType_String)
			params.skybox = lua.ToString(-1);
		else
		{
			params.sky_color = lua.CheckGlobal<Nz::Color>("skybox");
		}
		lua.Pop();
		
		/*if (lua.GetGlobal("ground_width") == Nz::LuaType_Number)
			params.ground_width = (float)lua.ToNumber(-1);
		lua.Pop();
		
		if (lua.GetGlobal("ground_height") == Nz::LuaType_Number)
			params.ground_height = (float)lua.ToNumber(-1);
		lua.Pop();*/
		
		if (lua.GetGlobal("zFar") == Nz::LuaType_Number)
			params.zFar = (float)lua.ToNumber(-1);
		lua.Pop();
		
		if (lua.GetGlobal("zNear") == Nz::LuaType_Number)
			params.zNear = (float)lua.ToNumber(-1);
		lua.Pop();
		
		if (lua.GetGlobal("ground_texture") == Nz::LuaType_String)
			params.ground_texture = lua.ToString(-1);

		params.ground_color = lua.CheckGlobal<Nz::Color>("ground_color");
		lua.Pop();
		
		if (lua.GetGlobal("light_type") == Nz::LuaType_Number)
			params.light_type = (int)lua.ToInteger(-1);
		lua.Pop();
		
		if (lua.GetGlobal("gravity") == Nz::LuaType_Number)
			params.gravity = (float)lua.ToNumber(-1);
		lua.Pop();
		
		if (lua.GetGlobal("eye_height") == Nz::LuaType_Number)
			params.eye_height = (float)lua.ToNumber(-1);
		lua.Pop();

		if (lua.GetGlobal("ground_radius") == Nz::LuaType_Number)
			params.ground_radius = (float)lua.ToNumber(-1);
		lua.Pop();
	}
}



inline void Input(float speed, InitParams &initParams, Nz::Vector3f &targetPos, Ndk::NodeComponentHandle &camera_node)
{
	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LShift))
		speed = initParams.cameraSpeed*1.5f;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Z) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up))
		targetPos += Nz::Vector3f(camera_node->GetForward().x*speed*0.01f, 0.f, camera_node->GetForward().z*speed*0.01f);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Q) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left))
		targetPos += camera_node->GetLeft()*speed*0.01f;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::D) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right))
		targetPos += camera_node->GetRight()*speed*0.01f;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::S) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down))
		targetPos += Nz::Vector3f(camera_node->GetBackward().x*speed*0.01f, 0.f, camera_node->GetBackward().z*speed*0.01f);
}



inline bool gravity(float &dist, Ndk::Application &app, InitParams &initParams, Nz::Vector3f &vecGround, Nz::Vector3f &targetPos)
{
	if (dist > initParams.ground_radius)
	{
		vecGround *= app.GetUpdateTime();
		vecGround *= initParams.gravity;
		targetPos += vecGround;
		return false;
	}
	else
	{
		return true;
	}
}


inline float getDistance(Nz::Vector3f &pos1, Nz::Vector3f &pos2, float height_correction=0.f)
{
	return sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y + height_correction, 2) + pow(pos2.z - pos1.z, 2));
}