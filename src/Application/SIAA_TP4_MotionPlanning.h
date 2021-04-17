#pragma once

#include <Application/BaseWithKeyboard.h>
#pragma warning(push, 0)        
#include <fcl/fcl.h>
#pragma warning(pop)
//#include <MotionPlanning/converter.h>
//#include <MotionPlanning/CollisionManager.h>
#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <HelperGl/Loader3ds.h>
#include <HelperGl/LoaderAssimp.h>
#include <HelperGl/PhongShader.h>
#include <SceneGraph/CoordinateSystem.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/Translate.h>
#include <SceneGraph/Rotate.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/RRT.h>
#include <MotionPlanning/PRM.h>

//#include <MotionPlanning/SixDofPlannerRRTConnect.h>
//#include <MotionPlanning/RandomForest.h>
//#include <MotionPlanning/RRTObstacle.h>
////#include <MotionPlanning/VPTree.h>
//#include <MotionPlanning/PRM.h>
//#include <glm/common.hpp>

namespace Application
{
	class SIAA_TP4_MotionPlanning : public BaseWithKeyboard
	{
		HelperGl::Camera m_camera;
		float m_cameraSpeed;
		float m_lastDt;

		SceneGraph::Group m_root;
		SceneGraph::Group * m_groupWorld;
		SceneGraph::Translate * m_translateMobile;
		SceneGraph::Rotate * m_rotateMobile;
		SceneGraph::Group * m_groupMobile;

		MotionPlanning::CollisionManager m_collisionManager;

		MotionPlanning::CollisionManager::StaticCollisionObject m_worldCollisionObject;
		MotionPlanning::CollisionManager::DynamicCollisionObject m_mobileCollisionObject;
		float m_mobileMaxDistance;

	public:
		SIAA_TP4_MotionPlanning()
		{}

		virtual ~SIAA_TP4_MotionPlanning()
		{}

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

		/// <summary>
		/// Creates a group containing all the meshes.
		/// </summary>
		/// <param name="meshes">The meshes.</param>
		/// <returns></returns>
		static SceneGraph::Group * createGroup(const std::vector<HelperGl::Mesh *> & meshes)
		{
			SceneGraph::Group * group = new SceneGraph::Group;
			for (auto it = meshes.begin(), end = meshes.end(); it != end; ++it)
			{
				(*it)->clean();
				(*it)->computeFacesNormals();
				(*it)->computeVertexNormal();
				group->addSon(new SceneGraph::MeshVBO_v2(*it));
			}
			return group;
		}

		/// <summary>
		/// Creates a collision mesh by merging all provided meshes into one.
		/// </summary>
		/// <param name="meshes">The meshes.</param>
		/// <returns></returns>
		static HelperGl::Mesh * createCollisionMesh(const std::vector<HelperGl::Mesh *> & meshes)
		{
			HelperGl::Mesh * mesh = new HelperGl::Mesh;
			for (auto it = meshes.begin(), end = meshes.end(); it != end; ++it)
			{
				mesh->merge(**it);
			}
			return mesh;
		}

		/// <summary>
		/// Initializes the mobile.
		/// </summary>
		void initializeMobile()
		{
			std::filesystem::path modelPath = Config::dataPath() / "MotionPlanning";
			HelperGl::Loader3ds loader((modelPath / "mobile.3ds").string(), modelPath.string());
			const std::vector<HelperGl::Mesh *> & meshes = loader.getMeshes();
			// Creates the scene graph
			m_groupMobile = createGroup(meshes);
			m_rotateMobile = new SceneGraph::Rotate(0.f, Math::makeVector(1.f, 0.0f, 0.0f));
			m_rotateMobile->addSon(m_groupMobile);
			m_translateMobile = new SceneGraph::Translate();
			m_translateMobile->addSon(m_rotateMobile);
			m_root.addSon(m_translateMobile);
			// Creates the collision object
			HelperGl::Mesh * mobileCollisionMesh = createCollisionMesh(meshes);
			m_mobileCollisionObject = m_collisionManager.registerDynamicObject(mobileCollisionMesh);
			//
			m_mobileMaxDistance = maximumDistanceFromRotationCenter(*mobileCollisionMesh);
		}

		/// <summary>
		/// Initializes the workspace. 
		/// </summary>
		void initializeWorld()
		{
			std::filesystem::path modelPath = Config::dataPath() / "MotionPlanning";
			// You can select the workspace (going from the simplest to the most complex one)
			//HelperGl::Loader3ds loader(modelPath / "world.3ds", modelPath);
			//HelperGl::Loader3ds loader((modelPath / "world2.3ds").string(), modelPath.string());
			//HelperGl::Loader3ds loader((modelPath / "world3.3ds").string(), modelPath.string());
			HelperGl::Loader3ds loader((modelPath / "world_simple.3ds").string(), modelPath.string());
			//HelperGl::Loader3ds loader("", "");

			const std::vector<HelperGl::Mesh *> meshes = loader.getMeshes();
			// Creates the scene graph
			m_groupWorld = createGroup(meshes);
			m_root.addSon(m_groupWorld);
			// Creates the collision object
			HelperGl::Mesh * worldCollisionMesh = createCollisionMesh(meshes);
			m_worldCollisionObject = m_collisionManager.registerStaticObject(worldCollisionMesh);
		}

		std::vector<MotionPlanning::SixDofPlannerBase::Configuration> m_computedPlan;

		virtual void initializeRendering()
		{
			// 0 - Camera setup
			m_camera.setPosition(Math::makeVector(0.0f, 0.0f, 5.0f));
			m_cameraSpeed = 1.0f;
			m_lastDt = 0.1f;
			// 1 - Lighting setup
			HelperGl::Color lightColor(1.0, 1.0, 1.0);
			HelperGl::Color lightAmbiant(0.0, 0.0, 0.0, 0.0);
			Math::Vector4f lightPosition = Math::makeVector(0.0f, 0.0f, 10000.0f, 1.0f); // Point light centered in 0,0,0
			HelperGl::LightServer::Light * light = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor);
			light->enable();
			// 2 - Loads and initializes the problem
			initializeMobile();
			initializeWorld();
			// 3 - Computes the plan
			computePlan();
		}

		/// <summary>
		/// Computes the plan.
		/// </summary>
		void computePlan()
		{
			// Tries a planning phase
			MotionPlanning::SixDofPlannerBase * planner = nullptr;
			float radius = 0.0;
			size_t index = 0;
			do
			{
				std::cout 
					<< "0: RRT" << std::endl
					<< "1: PRM " << std::endl
					<< "choice: ";
				std::cin >> index;
			} while (index > 1);
			switch (index)
			{
			case 0: // RRT 
				{
					using RRT = MotionPlanning::RRT;
					planner = new RRT(&m_collisionManager, m_mobileCollisionObject);
					std::cout << "Enter radius for RRT: ";
					std::cin >> radius;
					break;
				}
			case 1: // PRM
				{
					MotionPlanning::PRM * prm = new MotionPlanning::PRM(&m_collisionManager, m_mobileCollisionObject);
					size_t neighbourhood, nbNodes;
					std::cout << "Number of neighbours for the creation of the PRM: ";
					std::cin >> neighbourhood;
					std::cout << "Number of nodes for the creation of the PRM: ";
					std::cin >> nbNodes;
					std::cout << "Computing PRM..." << std::flush;
					prm->grow(nbNodes, neighbourhood, 0.02, std::numeric_limits<size_t>::max());
					std::cout << "OK" << std::endl;
					planner = prm;
				}
			break;
			}

			Math::Vector3f translationUp = Math::makeVector(1.0f, -1.0f, 1.0f);
			Math::Vector3f translationDown = Math::makeVector(1.0f, 1.0f, -1.0f);
			Math::Vector3f noRotation = Math::makeVector(0.0f, 0.0f, 0.0f);
			MotionPlanning::SixDofPlannerBase::Configuration start(translationUp, noRotation);
			MotionPlanning::SixDofPlannerBase::Configuration target(translationDown, noRotation);

			if (planner != nullptr)
			{
				std::cout << "Starting planning phase" << std::endl;

				auto begin = std::chrono::high_resolution_clock::now();
				bool success = planner->plan(start, target, radius, 0.02f, m_computedPlan);
				auto end = std::chrono::high_resolution_clock::now();
				std::cout << "Planning phase ended in " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s" << std::endl;
				if (success)
				{
					planner->optimize(m_computedPlan, 0.02f);
					::std::cout << "Planning succeeded, steps : " << m_computedPlan.size() << std::endl;
				}
				else
				{
					::std::cout << "Failed..." << std::endl;
				}
				delete planner;
			}
			else
			{
				std::cout << "No planner provided, doing nothing" << std::endl;
			}
		}

		virtual void render(double dt)
		{
			static float absoluteTime = 0.0f;
			absoluteTime += (float)dt;

			if (m_computedPlan.size() > 0)
			{
				size_t index = size_t(absoluteTime) % m_computedPlan.size();
				size_t index2 = (index + 1) % m_computedPlan.size();
				float t = absoluteTime - floor(absoluteTime);
				MotionPlanning::SixDofPlannerBase::Configuration configuration1 = m_computedPlan[index];
				MotionPlanning::SixDofPlannerBase::Configuration configuration2 = m_computedPlan[index2];
				MotionPlanning::SixDofPlannerBase::Configuration configuration = configuration1.interpolate(configuration2, t);
				m_translateMobile->setTranslation(configuration.m_translation);
				Math::Vector3f axis;
				float angle;
				configuration.m_orientation.getAxisAngle(axis, angle);
				m_rotateMobile->setAxis(axis);
				m_rotateMobile->setAngle(angle);
			}

			m_lastDt = (float)dt;
			// We handle the keys
			handleKeys();
			// We load the camera tranform
			GL::loadMatrix(m_camera.getInverseTransform());
			// We draw the scene graph
			m_root.draw();
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}