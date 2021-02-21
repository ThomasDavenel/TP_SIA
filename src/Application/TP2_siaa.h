#ifndef _Application_TP2_siaa_H
#define _Application_TP2_siaa_H

#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <Application/BaseWithKeyboard.h>
#include <Application/KeyboardStatus.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <GL/compatibility.h>
#include <Animation/KinematicChain.h>
#include <SceneGraph/Cylinder.h>

namespace Application
{
	class TP2_siaa : public BaseWithKeyboard
	{
	protected:
		HelperGl::Camera m_camera;

		SceneGraph::Group m_root;
		
		//Correspondance Graphe de scène et chaine poly
		std::vector < std::pair<SceneGraph::Group, Animation::KinematicChain::Node>>* v_correspondance= new std::vector < std::pair<SceneGraph::Group, Animation::KinematicChain::Node>>();
		//Chaine poly
		Animation::KinematicChain * ma_chaine = new Animation::KinematicChain();

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
		}

		//Fonction pour ajouter une chaine polyarticulée au graphe de scène
		void addChaineArt(unsigned int nb_Seg) {
			//cylindre
			float hauteur_cylindre = 1;
			HelperGl::Material mat_cylindre = HelperGl::Material();
			mat_cylindre.setDiffuse(HelperGl::Color(1.f, 1.f, 1.f));
			mat_cylindre.setSpecular(HelperGl::Color(1.f, 1.f, 1.f));
			SceneGraph::Cylinder* cylindre = new SceneGraph::Cylinder(mat_cylindre,0.1, 0.1, hauteur_cylindre);
			//sphère
			float rayon_sphere = 0.2;
			HelperGl::Material mat_sphere = HelperGl::Material();
			mat_sphere.setDiffuse(HelperGl::Color(0.f, 1.f, 0.f));
			mat_sphere.setSpecular(HelperGl::Color(0.f, 1.f, 0.f));
			SceneGraph::Sphere* sphere =new  SceneGraph::Sphere(mat_sphere, rayon_sphere);
			if (nb_Seg > 0) {
				m_root.addSon(cylindre);
			}
			//Scene de graphe
			SceneGraph::Translate* translation = new SceneGraph::Translate(Math::makeVector(0.f, 0.f, hauteur_cylindre));
			m_root.addSon(translation);
			//chaine poly
			Animation::KinematicChain::Node * chaintmp = ma_chaine->addStaticTranslation(ma_chaine->getRoot(), Math::makeVector(0.f, 0.f, hauteur_cylindre));
			/*~~~~~~~~*/
			SceneGraph::Group * nodetmp= translation;
			Math::Interval<float> interval = Math::makeInterval<float>(-Math::pi/2,Math::pi/2);
			for (int i = 1; i < nb_Seg; i++) {
				nodetmp->addSon(sphere);
				//Scene de graphe
				SceneGraph::Rotate * rotate_x = new SceneGraph::Rotate(0, Math::makeVector(1, 0, 0));
				nodetmp->addSon(rotate_x);
				//chaine poly
				Animation::KinematicChain::Node * rotate_xPoly = ma_chaine->addDynamicRotation(chaintmp, Math::makeVector(1, 0, 0),interval,0);
				//vecteur de correspondance
				v_correspondance->push_back(std::pair<SceneGraph::Group, Animation::KinematicChain::Node>(*rotate_x, rotate_xPoly));
				/*~~~~~~~~*/
				//Scene de graphe
				SceneGraph::Rotate * rotate_z = new SceneGraph::Rotate(0, Math::makeVector(0, 0, 1));
				rotate_x->addSon(rotate_z);
				//chaine poly
				Animation::KinematicChain::Node * rotate_zPoly = ma_chaine->addDynamicRotation(rotate_xPoly, Math::makeVector(0, 0, 1), interval, 0);
				//vecteur de correspondance
				v_correspondance->push_back(std::pair<SceneGraph::Group, Animation::KinematicChain::Node>(*rotate_z, rotate_zPoly));
				/*~~~~~~~~*/
				rotate_z->addSon(cylindre);
				//Scene de graphe
				SceneGraph::Translate* new_Translation = new SceneGraph::Translate(Math::makeVector(0.f, 0.f, hauteur_cylindre));
				rotate_z->addSon(new_Translation);
				//chaine poly
				chaintmp = ma_chaine->addStaticTranslation(rotate_zPoly, Math::makeVector(0.f, 0.f, hauteur_cylindre));
				/*~~~~~~~~*/
				nodetmp = new_Translation;
			}
		}

	public:
		TP2_siaa()
		{

		}

		virtual void initializeRendering()
		{
			//init TP2
			addChaineArt(5);
			//camera
			m_camera.setPosition(Math::makeVector(0.f, 0.f, 10.f));
			// Light
			HelperGl::Color lightColor(1.0, 1.0, 1.0);
			HelperGl::Color lightAmbiant(1.0, 1.0, 1.0, 1.0);
			Math::Vector4f lightPosition = Math::makeVector(10000.0f, 0.0f, 10000.0f, 1.0f); // Point light centered in 0,0,0
			HelperGl::LightServer::Light * light = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor);
			lightPosition = Math::makeVector(-10000.0f, 0.0f, 10000.0f, 1.0f); // Point light centered in 0,0,0
			light->enable();
			HelperGl::LightServer::Light * light2 = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor);
			light2->enable();
		}

		virtual void render(double dt)
		{
			
			handleKeys();
			GL::loadMatrix(m_camera.getInverseTransform());
			m_root.draw();
		}
	};
}

#endif