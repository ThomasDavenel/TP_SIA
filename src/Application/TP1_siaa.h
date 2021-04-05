#ifndef _Application_TP1_siaa_H
#define _Application_TP1_siaa_H

#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <Application/BaseWithKeyboard.h>
#include <Application/KeyboardStatus.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <GL/compatibility.h>
#include <Math/SphericalCoordinates.h>
#include <Sia/Insecte.h>
#include <Sia/Interpol_Traj.h>
#include <Sia/Interpol_Traj_v2.h>

namespace Application
{
	class TP1_siaa : public BaseWithKeyboard
	{
	protected:

		//interpolation
		Math::Vector3f p0 = Math::makeVector(0.f, 0.f, 0.f);
		Math::Vector3f d0 = Math::makeVector(0.f, 0.f, 0.f);
		Math::Vector3f p1 = Math::makeVector(3.f, 3.f, 3.f);
		Math::Vector3f d1 = Math::makeVector(0.f, 1.f, 0.f);
		Interpol_Traj m_interpolation = Interpol_Traj(p0, d0, p1, d1);
		//Interpolation version 2
		std::vector<Math::Vector3f> * l_position = new std::vector<Math::Vector3f>();
		std::vector<Math::Vector3f> * l_vitesse = new std::vector<Math::Vector3f>();
		Interpol_Traj_v2  m_interpolationV2 = Interpol_Traj_v2(5, *l_position, *l_vitesse);
		//TEMPS ecoule
		double tmp=0;
		//Coord Spherique
		Math::SphericalCoordinates * sphericalCoord = new Math::SphericalCoordinates(Math::makeVector(0.0f, 0.0f, 0.0f) );

		HelperGl::Camera m_camera ;

		SceneGraph::Group m_root ;

		Insecte* insecte;

		virtual void handleKeys() 
		{
			// The camera translation speed
			float cameraSpeed = 5.0f ;
			// The camera rotation speed (currently not used)
			float rotationSpeed = float(Math::pi/2.0) ;
			// quit
			if(m_keyboard.isPressed('q')) { quit() ; }
			// Go front
			if(m_keyboard.isPressed('+')) { m_camera.translateLocal(Math::makeVector(0.0f,0.0f,-cameraSpeed*(float)getDt())) ; } 
			// Go back
			if(m_keyboard.isPressed('-')) { m_camera.translateLocal(Math::makeVector(0.0f,0.0f,(float)cameraSpeed*(float)getDt())) ; } 
			// Go left
			if(m_keyboard.isPressed('1')) { m_camera.translateLocal(Math::makeVector(-cameraSpeed*(float)getDt(),0.0f,0.0f)) ; }
			// Go right
			if(m_keyboard.isPressed('3')) { m_camera.translateLocal(Math::makeVector((float)cameraSpeed*(float)getDt(),0.0f,0.0f)) ; }
			// Go down
			if(m_keyboard.isPressed('2')) { m_camera.translateLocal(Math::makeVector(0.0f,-cameraSpeed*(float)getDt(),0.0f)) ; }
			// Go up
			if(m_keyboard.isPressed('5')) { m_camera.translateLocal(Math::makeVector(0.0f,(float)cameraSpeed*(float)getDt(),0.0f)) ; }
		}

	public:
		TP1_siaa()
		{
			insecte = new Insecte();
			
		}

		virtual void initializeRendering()
		{	

			//insecte->Rota_X(Math::pi/2);

			m_root.addSon( insecte->getInsecte());
			//interpolation version2
			
			l_position->push_back(Math::makeVector(-3.0f, 3.0f, 3.0f) );
			l_position->push_back(Math::makeVector(3.0f, 3.0f, 6.0f));
			l_position->push_back(Math::makeVector(3.0f, -3.0f, 9.0f));
			l_position->push_back(Math::makeVector(-3.0f, -3.0f, 12.0f));
			l_position->push_back(Math::makeVector(-3.0f, 3.0f, 3.0f));

			l_vitesse->push_back(Math::makeVector(0.0f, 1.0f, 0.0f));
			l_vitesse->push_back(Math::makeVector(0.0f, 0.0f, 1.0f));
			l_vitesse->push_back(Math::makeVector(0.0f, 1.0f, 0.0f));
			l_vitesse->push_back(Math::makeVector(1.0f, 0.0f, 0.0f));
			l_vitesse->push_back(Math::makeVector(0.0f, 1.0f, 0.0f));
			
			//camera
			m_camera.setPosition(Math::makeVector(0.f, 0.f, 10.f));
			// Light
			HelperGl::Color lightColor(1.0,1.0,1.0);
			HelperGl::Color lightAmbiant(1.0,1.0,1.0,1.0);
			Math::Vector4f lightPosition = Math::makeVector(0.0f,0.0f,10000.0f,1.0f) ; // Point light centered in 0,0,0
			HelperGl::LightServer::Light * light = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor) ;
			light->enable();
		}

		virtual void render(double dt)
		{
			//En mode debug les ailes se bloquent contrairement en mode release !
			//m_interpolationV2 ou m_interpolation en fonction de la version de l'interpolation que l'on veut avoir
			tmp += dt;
			//std::cout << tmp << "  ->  " << std::endl;
			Math::Vector3f pos = m_interpolationV2.SplineHermite(tmp);
			//std::cout << "pos : " << pos[0] << " - " << pos[1] << " - " << pos[2] << std::endl;
			Math::Vector3f v_vitesse = m_interpolationV2.vitesse(tmp);
			sphericalCoord->set(v_vitesse);
			insecte->Rota_X(0);
			insecte->Rota_Y(sphericalCoord->getPhy());
			insecte->Rota_Z(sphericalCoord->getTheta());
			insecte->mouvement(pos);
			//animation aile
			insecte->animationAile(dt);
			handleKeys() ;
			GL::loadMatrix(m_camera.getInverseTransform()) ;
			m_root.draw() ;
		}
	};
}

#endif