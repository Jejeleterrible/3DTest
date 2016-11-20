#pragma once
#include "Init.hpp"
#include <Ndk/State.hpp>
#include <NDK/StateMachine.hpp>


class StateEarth : public Ndk::State
{
public:
	StateEarth(Ndk::WorldHandle &world, InitParams &params, Nz::RenderWindow &window, Nz::LuaInstance &lua);
	void Enter(Ndk::StateMachine &fsm) override;
	void Leave(Ndk::StateMachine &fsm) override;
	bool Update(Ndk::StateMachine &fsm, float elapsedTime) override;


private:
	Ndk::WorldHandle m_world;
	InitParams m_initParams;
	Nz::SkyboxBackgroundRef m_skybox;

	/*Camera : */
	Ndk::EntityHandle m_camera;
	Ndk::NodeComponentHandle m_camera_node;
	Ndk::CameraComponentHandle m_camera_component;


	/*Ground : */
	Ndk::EntityHandle m_ground;
	Ndk::NodeComponentHandle m_ground_node;
	Ndk::GraphicsComponentHandle m_ground_graph;


	/*Light : */
	Ndk::EntityHandle m_light;
	Ndk::NodeComponentHandle m_light_node;
	Ndk::LightComponent *m_light_comp;


	/*Useful variables : */
	Nz::Vector3f m_targetPos;
	float m_dist;
	bool m_update;
	Nz::Vector3f m_vecGround;
	int m_w;
	int m_h;

	Nz::EventHandler *m_eventHandler;
};