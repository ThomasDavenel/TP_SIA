#pragma once

#include <HelperGl/Text.h>
#include <Application/BaseWithKeyboard.h>
#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <HelperGl/Loader3ds.h>
#include <HelperGl/LoaderAssimp.h>
#include <HelperGl/PhongShader.h>
#include <SceneGraph/CoordinateSystem.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/Translate.h>
#include <SceneGraph/Rotate.h>
#include <SceneGraph/Scale.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <MotionPlanning/VPTree.h>
#include <glm/common.hpp>
#include <Crowds/Boid.h>
#include <Crowds/Simulator.h>
#include <Crowds/GraphicsFactory.h>
#include <tbb/parallel_invoke.h>
#include <tbb/task_group.h>
#include <Crowds/Prey.h>
#include <Crowds/Predator.h>
#include <Crowds/Messages.h>
#include <Crowds/Water.h>
#include <memory>

namespace Application
{
	class SIAA_TP5_behavior : public BaseWithKeyboard
	{
		HelperGl::Camera m_camera;
		float m_cameraSpeed;
		float m_lastDt;

		Crowds::Simulator m_simulator;

		SceneGraph::Group m_root;

		std::vector<Crowds::GraphicsFactory::Graphics> m_agentsGraphics;

		size_t m_nbCollisions;
		size_t m_belongsToGroup;
		size_t m_nbPanicMessages;
		size_t m_nbKilled;

		tbb::task_group * m_tasksGroup;

	public:
		SIAA_TP5_behavior()
		{
			m_tasksGroup = new tbb::task_group;
		}

		virtual ~SIAA_TP5_behavior()
		{
			try
			{
				delete m_tasksGroup;
			}
			catch (const std::exception & e)
			{
				std::cout << "Exception  caught: " << e.what() << std::endl;
			}
		}

	protected:

		void handleKeys()
		{
			float cameraRotation = float(Math::pi / 5.);
			Math::Vector3f xAxis = Math::makeVector(1.0, 0.0, 0.0);
			Math::Vector3f yAxis = Math::makeVector(0.0, 1.0, 0.0);
			if (m_keyboard.isPressed('!')) { quit(); }
			if (m_keyboard.isPressed('r')) { m_camera.translateFront(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('f')) { m_camera.translateFront(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('d')) { m_camera.translateRight(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('q')) { m_camera.translateRight(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('z')) { m_camera.translateUp(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('s')) { m_camera.translateUp(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('g')) { m_camera.rotateLocal(yAxis, cameraRotation*m_lastDt); }
			if (m_keyboard.isPressed('j')) { m_camera.rotateLocal(yAxis, -cameraRotation * m_lastDt); }
			if (m_keyboard.isPressed('y')) { m_camera.rotateLocal(xAxis, cameraRotation*m_lastDt); }
			if (m_keyboard.isPressed('h')) { m_camera.rotateLocal(xAxis, -cameraRotation * m_lastDt); }
		}

		virtual void initializeRendering() override
		{
			//testCompilation();

			// 0 - Camera setup
			initializeCamera();
			// 1 - Lighting setup
			initializeLighting();

			// 2 - Agents setup

			size_t size = 100;
			HelperGl::Material white, red, blue, green;
			white.setDiffuse(HelperGl::Color(1.0, 1.0, 1.0));
			red.setDiffuse(HelperGl::Color(1.0, 0.0, 0.0));
			blue.setDiffuse(HelperGl::Color(0.0, 0.0, 1.0));
			green.setDiffuse(HelperGl::Color(0.0, 1.0, 0.0));

			// Creation of the preys
			{
				size_t nbPreys = 1500; // 1500;
				std::cout << "Number of preys: ";
				std::cin >> nbPreys;
				for (size_t cpt = 0; cpt < nbPreys; ++cpt)
				{
					Math::Vector2f position = Math::makeVector(Math::Interval<float>(-50.0, 50.0).random(), Math::Interval<float>(-50.0, 50.0).random());
					float radius = Math::Interval<float>(0.3, 0.6).random();
					float mass = Math::Interval<float>(6.0, 8.0).random();
					float maxSpeed = Math::Interval<float>(2.0, 4.0).random();
					float maxForce = Math::Interval<float>(15.0, 25.0).random();
					std::shared_ptr<Crowds::Prey> prey = m_simulator.createAgent<Crowds::Prey>(position, radius, mass, maxSpeed, maxForce);
					auto graphics = Crowds::GraphicsFactory::create(prey, white);
					graphics.m_scale->setScale(Math::makeVector(prey->getRadius(), prey->getRadius()*0.2f, prey->getRadius()));
					m_agentsGraphics.push_back(graphics);
					m_root.addSon(graphics.m_node);
				}
			}

			// Creation of the predators
			{
				size_t nbPredators = 30;
				std::cout << "Number of predators: ";
				std::cin >> nbPredators;
				for (size_t cpt = 0; cpt < nbPredators; ++cpt)
				{
					Math::Vector2f position = Math::makeVector(Math::Interval<float>(-50.0, 50.0).random(), Math::Interval<float>(-50.0, 50.0).random());
					float radius = Math::Interval<float>(0.3, 0.6).random();
					float mass = Math::Interval<float>(8.0, 10.0).random();
					float maxSpeed = Math::Interval<float>(2.0, 4.0).random();
					float maxForce = Math::Interval<float>(15.0, 25.0).random();
					std::shared_ptr<Crowds::Predator> predator = m_simulator.createAgent<Crowds::Predator>(position, radius, mass, maxSpeed, maxForce);
					auto graphics = Crowds::GraphicsFactory::create(predator, red);
					graphics.m_scale->setScale(Math::makeVector(predator->getRadius(), predator->getRadius()*0.2f, predator->getRadius()));
					m_agentsGraphics.push_back(graphics);
					m_root.addSon(graphics.m_node);
				}
			}

			// Creation of the water
			{
				size_t nbWater = 30;
				std::cout << "Number of water sources: ";
				std::cin >> nbWater;
				for (size_t cpt = 0; cpt < nbWater; ++cpt)
				{
					Math::Vector2f position = Math::makeVector(Math::Interval<float>(-50.0, 50.0).random(), Math::Interval<float>(-50.0, 50.0).random());
					float radius = Math::Interval<float>(8.0, 12.0).random();
					std::shared_ptr<Crowds::Water> prey = m_simulator.createAgent<Crowds::Water>(position, radius);
					auto graphics = Crowds::GraphicsFactory::create(prey, blue);
					graphics.m_scale->setScale(Math::makeVector(prey->getRadius(), prey->getRadius(), 0.01f));
					m_agentsGraphics.push_back(graphics);
					m_root.addSon(graphics.m_node);
				}
			}

			std::cout << "Number of agents: " << m_agentsGraphics.size() << std::endl;

			// Creation of messages observers
			m_nbKilled = 0;
			m_simulator.createReceiver<Crowds::PreyKilledMessage>([this](const Crowds::PreyKilledMessage &) { m_nbKilled++; }, m_simulator.spyTarget());
			m_simulator.createReceiver<Crowds::CollisionMessage>([this](const Crowds::CollisionMessage &) { m_nbCollisions++; }, m_simulator.spyTarget());
			m_simulator.createReceiver<Crowds::BelongsToGroupMessage>([this](const Crowds::BelongsToGroupMessage &) { m_belongsToGroup++; }, m_simulator.spyTarget());
			m_simulator.createReceiver<Crowds::PanicMessage>([this](const Crowds::PanicMessage &) { m_nbPanicMessages++; }, m_simulator.spyTarget());
		}

		void initializeCamera()
		{
			m_camera.setPosition(Math::makeVector(0.0f, 0.0f, 100.0f));
			m_cameraSpeed = 8.0f;
			m_lastDt = 0.1f;
		}

		void initializeLighting()
		{
			HelperGl::Color lightColor(1.0, 1.0, 1.0);
			HelperGl::Color lightAmbiant(0.0, 0.0, 0.0, 0.0);
			Math::Vector4f lightPosition = Math::makeVector(0.0f, 0.0f, 10000.0f, 1.0f); // Point light centered in 0,0,0
			HelperGl::LightServer::Light * light = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor);
			light->enable();
		}

		virtual void render(double dt) override
		{
			// Time related stuff
			dt = std::min(dt, 0.1); // A minimum of 10 Hz for the refresh rate
			static unsigned int count = 0;
			static float time = 0.0f;
			time += (float)dt;

			// Collision count
			m_nbCollisions = 0;
			m_belongsToGroup = 0;
			m_nbPanicMessages = 0;

			// We handle the keys
			handleKeys();

			// Simulation
			auto simulationUpdate = [this, dt]()
			{
				m_simulator.update(dt);
				//if (m_nbCollisions > 0)
				//{
				//	std::cout << "Collisions: " << m_nbCollisions << std::endl;
				//}
			};

			auto sceneGraphUpdate = [this]()
			{
				// Update of the scene graph
				for (auto it = m_agentsGraphics.begin(), end = m_agentsGraphics.end(); it != end; ++it)
				{
					it->update();
				}
			};

			// We draw the scene
			auto drawScene = [this]()
			{
				GL::loadMatrix(m_camera.getInverseTransform());
				m_root.draw();
			};

			// We run the rendering and the agent simulation in parallel
			// Then we run the scene graph update in parallel with freeglut.

			// Waits for the scene graph update to finish
			m_tasksGroup->wait();
			// Runs the rendering and the simulation in parallel
			m_tasksGroup->run(simulationUpdate);
			drawScene();
			m_tasksGroup->wait();
			// Updates the scene graph in parallel with freeglut processing
			m_tasksGroup->run(sceneGraphUpdate);

			HelperGl::Text::getStream() << "Collisions: " << m_nbCollisions
				<< ", belongs to group: " << m_belongsToGroup
				<< ", panic messages: " << m_nbPanicMessages
				<< ", preys killed: " << m_nbKilled
				<< std::endl;
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}