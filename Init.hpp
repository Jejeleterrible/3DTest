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
	float zFar = 5000.f;
	float zNear = 0.1f;
	Nz::String ground_texture;
	Nz::Color ground_color = Nz::Color::White;
	int light_type = LIGHT_DIRECTIONAL;
	float gravity = 5.0f;
	float eye_height = 100.f;
	float ground_radius = 10000.f;
};


inline void InitFromLua(Nz::LuaInstance &lua, InitParams &params)
{
	lua.PushGlobal("LightTypeSpot", LIGHT_SPOT);
	lua.PushGlobal("LightTypePoint", LIGHT_POINT);
	lua.PushGlobal("LightypeDirectional", LIGHT_DIRECTIONAL);


	if (lua.ExecuteFromFile("init.lua"))
	{

		params.width = lua.CheckGlobal<int>("width", params.width);

		params.height = lua.CheckGlobal<int>("height", params.height);
		
		params.title = lua.CheckGlobal<Nz::String>("title", params.title);

		params.fullscreen = lua.CheckGlobal<bool>("fullscreen", params.fullscreen);
		
		params.sensitivity = lua.CheckGlobal<float>("sensitivity", params.sensitivity);
		
		params.cameraSpeed = lua.CheckGlobal<float>("speed", params.cameraSpeed);
		
		params.skybox = lua.CheckGlobal<Nz::String>("skybox", params.skybox);
		if (params.skybox == "")
			params.sky_color = lua.CheckGlobal<Nz::Color>("skybox", params.sky_color);
		
		params.zFar = lua.CheckGlobal<float>("zFar", params.zFar);
		
		params.zNear = lua.CheckGlobal<float>("zNear", params.zNear);
		
		params.ground_texture = lua.CheckGlobal<Nz::String>("ground_texture", params.ground_texture);

		params.ground_texture = lua.CheckGlobal<Nz::String>("ground_texture", params.ground_texture);

		params.ground_color = lua.CheckGlobal<Nz::Color>("ground_color", params.ground_color);
		
		params.light_type = lua.CheckGlobal<int>("light_type", params.light_type);
		
		params.gravity = lua.CheckGlobal<float>("gravity", params.gravity);
		
		params.eye_height = lua.CheckGlobal<float>("eye_height", params.eye_height);

		params.ground_radius = lua.CheckGlobal<float>("ground_radius", params.ground_radius);
	}
}



inline void Input(float speed, float update_time, InitParams &initParams, Nz::Vector3f &velocity, Ndk::NodeComponentHandle &camera_node)
{
	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LShift))
		speed = initParams.cameraSpeed*1.5f;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Z) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up))
		velocity += Nz::Vector3f(camera_node->GetForward().x * update_time * speed, 0.f, camera_node->GetForward().z * update_time * speed);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Q) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left))
		velocity += camera_node->GetLeft() * update_time * speed;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::D) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right))
		velocity += camera_node->GetRight() * update_time * speed;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::S) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down))
		velocity += Nz::Vector3f(camera_node->GetBackward().x * update_time*speed, 0.f, camera_node->GetBackward().z * update_time * speed);
}



inline bool gravity(float &dist, float updateTime, InitParams &initParams, Nz::Vector3f &vecGround, Nz::Vector3f &targetPos)
{
	if (dist > initParams.ground_radius)
	{
		vecGround *= updateTime;
		vecGround *= initParams.gravity;
		targetPos += vecGround;
		return false;
	}
	else if(dist+1 < initParams.ground_radius)
	{
		vecGround *= updateTime;
		vecGround *= initParams.gravity;
		targetPos += -vecGround;
		return true;
	}
	else
	{
		return true;
	}
}



inline Nz::Vector3f rotatePoint(Nz::Vector3f point, Nz::Vector3f center, float speed)
{
	speed *= 3.14f / 180.f;
	
	float rotateX = cos(speed) * (point.x - center.x) - sin(speed) * (point.y - center.y) + center.x;
	float rotateY = 0.f;//sin(speed) * (point.x - center.x) + cos(speed) * (point.y - center.y) + center.y;
	float rotateZ = sin(speed) * (point.x - center.x) + cos(speed) * (point.z - center.z) + center.z;

	return Nz::Vector3f(rotateX, rotateY, rotateZ);
}