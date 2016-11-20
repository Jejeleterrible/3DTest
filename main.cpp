#include "StateEarth.hpp"

int main(int argc, char* argv[])
{
	Ndk::Application application(argc, argv);
	InitParams initParams;


	/*===================================================== LUA : =====================================================*/
	Nz::LuaInstance lua;

	Ndk::LuaAPI::RegisterClasses(lua);

	InitFromLua(lua, initParams);

	Nz::RenderTargetParameters params;
	params.antialiasingLevel = 4;

	/*===================================================== WINDOW : =====================================================*/

	Nz::VideoMode videoMode;

	if (initParams.fullscreen)
		videoMode = Nz::VideoMode::GetDesktopMode();
	else
	{
		if (initParams.width != 0)
			videoMode.width = initParams.width;
		else
			videoMode.width = 800;

		if (initParams.height != 0)
			videoMode.height = initParams.height;
		else
			videoMode.height = 600;
	}

	Nz::RenderWindow &window = application.AddWindow<Nz::RenderWindow>(videoMode, initParams.title, (initParams.fullscreen ? Nz::WindowStyle_Fullscreen : Nz::WindowStyle_Default), params);

	Ndk::World& world = application.AddWorld();

	auto stateEarth{ std::make_shared<StateEarth>(world.CreateHandle(), initParams, window, lua) };
	Ndk::StateMachine fsm{ stateEarth };

	while (application.Run()) 
	{
		if (!fsm.Update(application.GetUpdateTime())) {
			return EXIT_FAILURE;
		}

		window.Display();

	}

	return 0;
}
