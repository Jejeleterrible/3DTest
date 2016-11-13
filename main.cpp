#include "init.hpp"




int main(int argc, char* argv[])
{
	Ndk::Application application(argc, argv);
	InitParams initParams;


	/* Lua : */
	Nz::LuaInstance lua;

	Ndk::LuaAPI::RegisterClasses(lua);

	InitFromLua(lua, initParams);

	Nz::RenderTargetParameters params;
	params.antialiasingLevel = 4;

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

	Nz::RenderWindow& window = application.AddWindow<Nz::RenderWindow>(videoMode, initParams.title, (initParams.fullscreen ? Nz::WindowStyle_Fullscreen : Nz::WindowStyle_Default), params);

	Ndk::World& world = application.AddWorld();

	Nz::TextureRef texture = Nz::Texture::New();
	if (texture->LoadCubemapFromFile(initParams.skybox))
	{
		Nz::SkyboxBackgroundRef skybox = Nz::SkyboxBackground::New(std::move(texture));
		Ndk::RenderSystem& renderSystem = world.GetSystem<Ndk::RenderSystem>();
		renderSystem.SetDefaultBackground(std::move(skybox));
	}
	else
	{
		std::cout << "Failed to load skybox" << std::endl;
		world.GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color(90, 150, 240)));
	}
		
	window.SetCursor(Nz::WindowCursor_None);

	/*Camera : */
	Ndk::EntityHandle viewEntity = world.CreateEntity();
	Ndk::NodeComponentHandle camera_node = viewEntity->AddComponent<Ndk::NodeComponent>().CreateHandle();

	camera_node->SetPosition(0.f, initParams.eye_height, 0.f);

	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	Ndk::CollisionComponent3D camera_cols = viewEntity->AddComponent<Ndk::CollisionComponent3D>();
	viewer.SetTarget(&window);

	viewer.SetZFar(initParams.zFar);
	viewer.SetZNear(initParams.zNear);


	/*Ground : */
	Ndk::EntityHandle ground = world.CreateEntity();
	Ndk::CollisionComponent3D ground_cols = ground->AddComponent<Ndk::CollisionComponent3D>();
	Ndk::NodeComponentHandle ground_node = ground->AddComponent<Ndk::NodeComponent>().CreateHandle();
	Ndk::GraphicsComponentHandle ground_graph = ground->AddComponent<Ndk::GraphicsComponent>().CreateHandle();

	ground_node->SetPosition(Nz::Vector3f(0.f, 0.f, 0.f));

	Nz::TextureRef tex = Nz::Texture::New();
	Nz::MaterialRef mat = Nz::Material::New();

	if(tex->LoadFromFile(initParams.ground_texture))
	{
		mat->SetDiffuseMap(tex);
		Nz::TextureSampler sampler = mat->GetDiffuseSampler();
		sampler.SetWrapMode(Nz::SamplerWrap_Repeat);
		sampler.SetDefaultWrapMode(Nz::SamplerWrap_Repeat);
		mat->SetDiffuseSampler(sampler);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		mat->SetSpecularColor(Nz::Color::Black);
		mat->SetDiffuseColor(Nz::Color(100, 255, 100));
	}

	

	Nz::MeshRef mesh = Nz::Mesh::New();
	mesh->CreateStatic();
	Nz::SubMeshRef sub = mesh->BuildSubMesh(Nz::Primitive::Box(Nz::Vector3f(initParams.ground_width, 1.f, initParams.ground_height)));
	Nz::ModelRef model = Nz::Model::New();
	model->SetMesh(mesh);
	model->SetMaterial(0, mat);

	ground_graph->Attach(model);


	/*Light : */
	Ndk::EntityHandle light = world.CreateEntity();
	Ndk::NodeComponentHandle light_node = light->AddComponent<Ndk::NodeComponent>().CreateHandle();


	Ndk::LightComponent light_comp;

	switch(initParams.light_type)
	{
	case LIGHT_SPOT:
		 light_comp = light->AddComponent<Ndk::LightComponent>(Nz::LightType_Spot);
		break;

	case LIGHT_POINT:
		light_comp = light->AddComponent<Ndk::LightComponent>(Nz::LightType_Point);
		break;

	case LIGHT_DIRECTIONAL:
	
	default:
		light_comp = light->AddComponent<Ndk::LightComponent>(Nz::LightType_Directional);
	}


	light_comp.SetColor(Nz::Color(125, 125, 125));
	light_node->SetRotation(Nz::EulerAnglesf(0.f, 102.f, 0.f));

	Nz::Vector3f targetPos(0.f, 0.f, 0.f);
	float ySpeed = 0.4f;
	float speed;
	bool update = true;
	bool isJumping = false;

	/*--------------------------------------------- EVENTS ---------------------------------------------*/

	auto& eventHandler = window.GetEventHandler();

	eventHandler.OnMouseMoved.Connect([initParams, camera_node, &window](const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& e)
	{
		Nz::EulerAnglesf camAngles(camera_node->GetRotation());

		camAngles.yaw = Nz::NormalizeAngle(camAngles.yaw - e.deltaX*initParams.sensitivity);
		camAngles.pitch = Nz::Clamp(camAngles.pitch - e.deltaY*initParams.sensitivity, -89.f, 89.f);
		
		camera_node->SetRotation(camAngles);

		Nz::Mouse::SetPosition(window.GetWidth() / 2, window.GetHeight() / 2, window);
	}
	);


	eventHandler.OnKeyPressed.Connect([&application, &isJumping, camera_node, &ySpeed, &targetPos](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& e)
	{

		switch(e.code)
		{
		case Nz::Keyboard::Escape:
			application.Quit();

		case Nz::Keyboard::Space:
			if(!isJumping)
			{
				camera_node->Move(Nz::Vector3f(0.f, 1.0f, 0.f));
				ySpeed = 0.9f;
				isJumping = true;
			}
		}
	}
	);

	lua.PushGlobal("World", world.CreateHandle());
	lua.PushGlobal("Camera", viewEntity);
	lua.PushGlobal("Ground", ground);


	while (application.Run()) 
	{
		if(update)
		{
			if (!lua.ExecuteFromFile("update.lua"))
			{
				std::cout << "Can't execute file \"update.lua\" : \n" << lua.GetLastError() << std::endl;
				update = false;
			}
		}

		targetPos = camera_node->GetPosition();
		
		speed = initParams.cameraSpeed;

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LShift))
			speed = initParams.cameraSpeed*1.5f;

		if(Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Z) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up))
			targetPos += Nz::Vector3f(camera_node->GetForward().x*speed*0.01f, 0.f, camera_node->GetForward().z*speed*0.01f);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Q) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left))
			targetPos += camera_node->GetLeft()*speed*0.01f;

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::D) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right))
			targetPos += camera_node->GetRight()*speed*0.01f;

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::S) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down))
			targetPos += Nz::Vector3f(camera_node->GetBackward().x*speed*0.01f, 0.f, camera_node->GetBackward().z*speed*0.01f);

		if(!model->GetMesh()->GetAABB().Contains(camera_node->GetPosition()-Nz::Vector3f(0.f, initParams.eye_height, 0.f)))
			ySpeed -= initParams.gravity/1000.f;
		else
		{
			ySpeed = 0.f;
			isJumping = false;
		}
		targetPos.y += ySpeed;
		camera_node->SetPosition(targetPos, Nz::CoordSys_Global);


		window.Display();
	}

	return 0;
}



