#include "StateEarth.hpp"



StateEarth::StateEarth(Ndk::WorldHandle &world, InitParams &params, Nz::RenderWindow &window, Nz::LuaInstance &lua)
{
	m_world = world;
	m_initParams = params;

	/*===================================================== SKYBOX : =====================================================*/

	Nz::TextureRef texture = Nz::Texture::New();
	if (texture->LoadCubemapFromFile(m_initParams.skybox))
	{
		m_skybox = Nz::SkyboxBackground::New(std::move(texture));
		Ndk::RenderSystem& renderSystem = m_world->GetSystem<Ndk::RenderSystem>();
		renderSystem.SetDefaultBackground(std::move(m_skybox));
	}
	else
	{
		std::cout << "Failed to load skybox" << std::endl;
		m_world->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(m_initParams.sky_color));
	}

	window.SetCursor(Nz::WindowCursor_None);

	/*====================================================================================================================*/


	/*===================================================== CAMERA : =====================================================*/

	m_camera = m_world->CreateEntity();

	m_camera_node = m_camera->AddComponent<Ndk::NodeComponent>().CreateHandle();
	m_camera_component = m_camera->AddComponent<Ndk::CameraComponent>().CreateHandle();

	m_camera_component->SetTarget(&window);
	m_camera_component->SetZFar(m_initParams.zFar);
	m_camera_component->SetZNear(m_initParams.zNear);

	/*====================================================================================================================*/


	/*===================================================== GROUND : =====================================================*/

	m_ground = m_world->CreateEntity();

	m_ground_node = m_ground->AddComponent<Ndk::NodeComponent>().CreateHandle();
	m_ground_graph = m_ground->AddComponent<Ndk::GraphicsComponent>().CreateHandle();

	Nz::TextureRef tex = Nz::Texture::New();
	Nz::MaterialRef mat = Nz::Material::New();

	if (tex->LoadFromFile(m_initParams.ground_texture))
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
		mat->SetDiffuseColor(m_initParams.ground_color);
	}
	mat->SetShader("PhongLighting");


	Nz::MeshRef mesh = Nz::Mesh::New();
	mesh->CreateStatic();
	Nz::SubMeshRef sub = mesh->BuildSubMesh(Nz::Primitive::UVSphere(m_initParams.ground_radius, 64, 64));
	Nz::ModelRef model = Nz::Model::New();
	model->SetMesh(mesh);
	model->SetMaterial(0, mat);

	m_ground_graph->Attach(model);

	/*====================================================================================================================*/


	/*===================================================== LIGHT : ======================================================*/

	m_light = m_world->CreateEntity();

	m_light_node = m_light->AddComponent<Ndk::NodeComponent>().CreateHandle();

	switch (m_initParams.light_type)
	{
	case LIGHT_SPOT:
		m_light_comp = &m_light->AddComponent<Ndk::LightComponent>(Nz::LightType_Spot);
		break;

	case LIGHT_POINT:
		m_light_comp = &m_light->AddComponent<Ndk::LightComponent>(Nz::LightType_Point);
		break;

	case LIGHT_DIRECTIONAL:

	default:
		m_light_comp = &m_light->AddComponent<Ndk::LightComponent>(Nz::LightType_Directional);
	}

	/*====================================================================================================================*/

	m_targetPos = Nz::Vector3f(0.f, 0.f, 0.f);
	m_vecGround = Nz::Vector3f(0.f, 0.f, 0.f);
	m_update = true;
	m_dist = 0.f;

	m_w = window.GetWidth()/2;
	m_h = window.GetHeight()/2;

	/*===================================================== EVENTS : =====================================================*/

	m_eventHandler = &window.GetEventHandler();

	

	/*====================================================================================================================*/


	lua.PushGlobal("World", world);
	lua.PushGlobal("Camera", m_camera);
	lua.PushGlobal("Ground", m_ground);
}





void StateEarth::Enter(Ndk::StateMachine & fsm)
{
	
	/*===================================================== CAMERA : =====================================================*/
	
	m_camera_node->SetPosition(0.f, m_initParams.eye_height + m_initParams.ground_radius, 0.f);

	/*====================================================================================================================*/


	/*===================================================== GROUND : =====================================================*/

	m_ground_node->SetPosition(Nz::Vector3f(0.f, 0.f, 0.f));
	m_camera_node->SetParent(m_ground_node);

	/*====================================================================================================================*/


	/*===================================================== LIGHT : ======================================================*/

	m_light_comp->SetColor(Nz::Color(255, 255, 255));
	m_light_node->SetPosition(0.f, 11000.f, 0.f);
	m_light_node->SetRotation(Nz::EulerAnglesf(180.f, 0.f, 0.f));
	m_light_comp->SetDiffuseFactor(0.5f);

	/*====================================================================================================================*/


	/*===================================================== EVENTS : =====================================================*/

	m_eventHandler->OnMouseMoved.Connect([this](const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& e)
	{
		Nz::EulerAnglesf camAngles(m_camera_node->GetRotation());

		camAngles.yaw = Nz::NormalizeAngle(camAngles.yaw - e.deltaX*m_initParams.sensitivity);
		camAngles.pitch = Nz::Clamp(camAngles.pitch - e.deltaY*m_initParams.sensitivity, -89.f, 89.f);

		m_camera_node->SetRotation(camAngles, Nz::CoordSys_Global);

		Nz::Mouse::SetPosition(m_w, m_h);
	}
	);


	m_eventHandler->OnKeyPressed.Connect([&](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& e)
	{
		if(e.code == Nz::Keyboard::Escape)
		{
			Ndk::Application::Instance()->Quit();
		}
	}
	);

	/*====================================================================================================================*/

}


void StateEarth::Leave(Ndk::StateMachine & fsm)
{
}


bool StateEarth::Update(Ndk::StateMachine & fsm, float elapsedTime)
{
	float rotateSpeed = 0.001f;

	m_targetPos = m_camera_node->GetPosition();

	m_vecGround = m_ground_node->GetPosition() - m_camera_node->GetPosition();
	m_dist = m_camera_node->GetPosition().Distance(m_ground_node->GetPosition() + Nz::Vector3f(0.f, m_initParams.eye_height, 0.f));
	gravity(m_dist, elapsedTime, m_initParams, m_vecGround, m_targetPos);

	Input(m_initParams.cameraSpeed, elapsedTime, m_initParams, m_targetPos, m_camera_node);

	m_camera_node->SetPosition(m_targetPos, Nz::CoordSys_Global);
	m_ground_node->Rotate(Nz::Quaternionf(Nz::EulerAnglesf(0.f, rotateSpeed, 0.f)));
	
	return true;
}
