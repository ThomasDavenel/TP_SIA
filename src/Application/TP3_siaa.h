#ifndef _Application_TP3_siaa_H
#define _Application_TP3_siaa_H

#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <Application/BaseWithKeyboard.h>
#include <Application/KeyboardStatus.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <GL/compatibility.h>
#include <Animation/SpringMassSystem.h>
#include <SceneGraph/Patch.h>

namespace Application
{
	class TP3_siaa : public BaseWithKeyboard
	{
	protected:
		/*System masses ressorts*/
		Animation::SpringMassSystem m_SytemMassesRessorts;
		Animation::SpringMassSystem::PatchDescriptor m_Patch;
		SceneGraph::Patch* m_PatchScene;
		/*~~~~~~~~*/
		HelperGl::Camera m_camera;
		SceneGraph::Group m_root;
		//compeur delete
		int c_delete=0;
		//boolean touche 
		bool sNotPressed = true;

		virtual void handleKeys()
		{
			// The camera translation speed
			float cameraSpeed = 5.0f;
			// The camera rotation speed (currently not used)
			float rotationSpeed = float(Math::pi / 2.0);
			// quit
			if (m_keyboard.isPressed('q')) { quit(); }
			// Go front
			if (m_keyboard.isPressed('+')) { m_camera.translateLocal(Math::makeVector(0.0f, 0.0f, -cameraSpeed * (float)getDt())); }
			// Go back
			if (m_keyboard.isPressed('-')) { m_camera.translateLocal(Math::makeVector(0.0f, 0.0f, (float)cameraSpeed*(float)getDt())); }
			// Go left
			if (m_keyboard.isPressed('1')) { m_camera.translateLocal(Math::makeVector(-cameraSpeed * (float)getDt(), 0.0f, 0.0f)); }
			// Go right
			if (m_keyboard.isPressed('3')) { m_camera.translateLocal(Math::makeVector((float)cameraSpeed*(float)getDt(), 0.0f, 0.0f)); }
			// Go down
			if (m_keyboard.isPressed('2')) { m_camera.translateLocal(Math::makeVector(0.0f, -cameraSpeed * (float)getDt(), 0.0f)); }
			// Go up
			if (m_keyboard.isPressed('5')) { m_camera.translateLocal(Math::makeVector(0.0f, (float)cameraSpeed*(float)getDt(), 0.0f)); }
			//supp contraintes
			if (m_keyboard.isPressed('s') && sNotPressed) {
				sNotPressed = false;
				if (c_delete % 4 == 0) {
					std::cout << "delete m_Patch.width() - 1, m_Patch.height() - 1" << std::endl;
					m_Patch.unconstrainPosition(m_Patch.width() - 1, m_Patch.height() - 1);
				}if (c_delete % 4 == 1) {
					std::cout << "delete 0, m_Patch.height() - 1" << std::endl;
					m_Patch.unconstrainPosition(0, m_Patch.height() - 1);
				}if (c_delete % 4 == 2) {
					std::cout << "delete m_Patch.width() - 1,0" << std::endl;
					m_Patch.unconstrainPosition(m_Patch.width() - 1,0);
				}if (c_delete % 4 == 3) {
					std::cout << "delete 0 0" << std::endl;
					m_Patch.unconstrainPosition(0,0);
				}
				c_delete++;
			}
			if (!m_keyboard.isPressed('s')) {
				sNotPressed = true;
			}
		}
	public:
		static ::std::pair<Math::Vector3f, Math::Vector3f> methodeEuler(const Animation::SpringMassSystem::Mass & previous, const Animation::SpringMassSystem::Mass & current, float dt) {
			Math::Vector3f vitesse = current.m_forces*(dt / current.m_mass) + current.m_speed;
			Math::Vector3f position = current.m_position + (vitesse)*(dt);
			return ::std::pair<Math::Vector3f, Math::Vector3f>(position,vitesse);
		}

		static Math::Vector3f graviteTerrestre(const Animation::SpringMassSystem::Mass & mass) {
			return Math::makeVector(0.f, 0.f,-mass.m_mass*9.81f);
		}

		static Math::Vector3f forceRessort(const  Animation::SpringMassSystem::Mass & mass1, const  Animation::SpringMassSystem::Mass & mass2, const  Animation::SpringMassSystem::Link & link) {
			float coefRaideur = 100;
			return (mass2.m_position - mass1.m_position)*coefRaideur*(1-link.m_initialLength/(mass1.m_position- mass2.m_position).norm());
		}
		static Math::Vector3f amortissement(const Animation::SpringMassSystem::Mass & mass) {
			float viscosite = 0.01; 
			return -mass.m_speed * viscosite;
		}
		static ::std::pair<Math::Vector3f, Math::Vector3f> contraintePositionSol(const Animation::SpringMassSystem::Mass & previous,const Animation::SpringMassSystem::Mass & current) {
			Math::Vector3f position = current.m_position;
			Math::Vector3f vitesse = current.m_speed;
			if (current.m_position[2] <= -1.49) {
				position[2] = previous.m_position[2];
				vitesse[2] = 0.f;
			}
			::std::pair<Math::Vector3f, Math::Vector3f> res;
			res.first = position;
			res.second= vitesse;
			return res;
		}

		static ::std::pair<Math::Vector3f, Math::Vector3f> contraintePositionSphere(const Animation::SpringMassSystem::Mass & previous, const Animation::SpringMassSystem::Mass & current) {
			Math::Vector3f position = Math::makeVector(current.m_position[0], current.m_position[1], current.m_position[2]);
			Math::Vector3f vitesse = Math::makeVector(current.m_speed[0], current.m_speed[1], current.m_speed[2]);
			if (sqrt(pow(-current.m_position[0],2)+ pow(-0.3-current.m_position[1], 2)+ pow(-1-current.m_position[2], 2))<=0.51) {
				position = previous.m_position;
				vitesse = 0;
			}
			::std::pair<Math::Vector3f, Math::Vector3f> res;
			res.first = position;
			res.second = vitesse;
			return res;
		}

		TP3_siaa():m_PatchScene(nullptr)
		{
			m_Patch = m_SytemMassesRessorts.createPatch(3, 50, 3, 50, 2, 10);
			HelperGl::Material mat_Patch;
			mat_Patch.setDiffuse(HelperGl::Color(0.f, 1.f, 0.f));
			mat_Patch.setSpecular(HelperGl::Color(0.f, 1.f, 0.f));
			m_PatchScene = new SceneGraph::Patch(m_Patch.height(), m_Patch.width(), mat_Patch);
		}

		virtual void initializeRendering()
		{
			//Sytem masses ressorts
			m_SytemMassesRessorts.setIntegrator(methodeEuler);
			//Ajout gravité terrestre
			m_SytemMassesRessorts.addForceFunction(graviteTerrestre);
			//Ajout force ressort
			m_SytemMassesRessorts.addLinkForceFunction(forceRessort);
			//Ajout force amortissemebt
			m_SytemMassesRessorts.addForceFunction(amortissement);
			//Contraintes position
			if (m_Patch.isValid(0, 0)) {
				//std::cout << "valid" << std::endl;
				m_Patch.constrainPosition(0, 0);
			}
			if (m_Patch.isValid(0, m_Patch.height() - 1)) {
				//std::cout << "valid" << std::endl;
				m_Patch.constrainPosition(0, m_Patch.height()-1);
			}
			if (m_Patch.isValid(m_Patch.width() - 1, 0)) {
				//std::cout << "valid" << std::endl;
				m_Patch.constrainPosition(m_Patch.width()-1, 0);
			}
			if (m_Patch.isValid(m_Patch.width() - 1, m_Patch.height() - 1)) {
				//std::cout << "valid" << std::endl;
				m_Patch.constrainPosition(m_Patch.width()-1, m_Patch.height()-1);
			}
			//Contrainte de position sol
			m_SytemMassesRessorts.addPositionConstraint(contraintePositionSol);
			//Rendering sol
			HelperGl::Material mat_sol = HelperGl::Material();
			mat_sol.setDiffuse(HelperGl::Color(0.1,0.1,0.1));
			mat_sol.setSpecular(HelperGl::Color(0.1, 0.1, 0.1));
			SceneGraph::Translate * translationSol= new SceneGraph::Translate (Math::makeVector(0.f,0.f,-1.51f));
			translationSol->addSon(new SceneGraph::Cylinder(mat_sol, 20, 20, 0.01));
			m_root.addSon(translationSol);
			//Contrainte Sphere
			m_SytemMassesRessorts.addPositionConstraint(contraintePositionSphere);
			//Rendering Sphere
			HelperGl::Material mat_sphere = HelperGl::Material();
			mat_sphere.setDiffuse(HelperGl::Color(0., 0., 0.1));
			mat_sphere.setSpecular(HelperGl::Color(0., 0., 0.1));
			SceneGraph::Translate * translationSphere = new SceneGraph::Translate(Math::makeVector(0.f, -0.3f, -1.f));
			translationSphere->addSon(new SceneGraph::Sphere(mat_sphere, 0.5));
			m_root.addSon(translationSphere);
			//camera
			m_camera.setPosition(Math::makeVector(0.f, 0.f, 1.f));
			// Light
			HelperGl::Color lightColor(1.0, 1.0, 1.0);
			HelperGl::Color lightAmbiant(1.0, 1.0, 1.0, 1.0);
			Math::Vector4f lightPosition = Math::makeVector(0.0f, 0.0f, 10000.0f, 1.0f); // Point light centered in 0,0,0
			HelperGl::LightServer::Light * light = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor);
			light->enable();
			m_root.addSon(m_PatchScene);
		}


		virtual void render(double dt)
		{
			m_SytemMassesRessorts.copyMassesPositionsTo(m_PatchScene->getVertices().begin());
			handleKeys();
			GL::loadMatrix(m_camera.getInverseTransform());
			m_root.draw();
			m_SytemMassesRessorts.update(dt);
		}
	};
}
#endif