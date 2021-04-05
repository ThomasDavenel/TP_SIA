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
#include <Sia/CCD.h>
#include <Sia/Interpol_Traj.h>

namespace Application
{
	class TP2_siaa : public BaseWithKeyboard
	{
	/*README*/
		//Sphere jaune: point de départ du bras articulé.
		//Sphere Rouge: point d'arrivé du bras articulé.
		//Sphère orange: point ciblé intermédiaire.
		//Cylidre gris: socle du bras articulé.
	protected:
		HelperGl::Camera m_camera;

		SceneGraph::Group m_root;
		Animation::KinematicChain::Node * extremiteNoeud;
		CCD* m_CDD;
		//Nombre d'articulation
		int nb_os = 0;
		/*Point de départ*/
		SceneGraph::Translate* pt_Depart;
		/*Cible*/
		SceneGraph::Translate* cible;
		SceneGraph::Translate* cible_Intermediaire;
		//Correspondance Graphe de scène et chaine poly
		std::vector < std::pair<SceneGraph::Rotate *, const Animation::KinematicChain::DegreeOfFreedom *> > * v_correspondance = new std::vector < std::pair<SceneGraph::Rotate *,const Animation::KinematicChain::DegreeOfFreedom *>>();
		//Chaine poly
		Animation::KinematicChain * ma_chaine = new Animation::KinematicChain();
		//interpolation
		Interpol_Traj m_interpolation= Interpol_Traj(Math::makeVector(0, 0, 0), Math::makeVector(0, 0, 0), Math::makeVector(0, 0, 0), Math::makeVector(0, 0, 0));;
		//TEMPS ecoule
		double tmp = 0;

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
			// Nouvelle cible
			if (m_keyboard.isPressed('n')) {
				float x_rng = pow(-1, (rand() % 100))*((float)(rand() % 100) / 100)*(nb_os / 2);
				float y_rng = pow(-1, (rand() % 100))*((float)(rand() % 100) / 100)*(nb_os / 2);
				float z_rng = ((float)(rand() % 100) / 100)*(nb_os / 2);
				/*
				std::cout << x_rng << std::endl;
				std::cout << y_rng << std::endl;
				std::cout << z_rng << std::endl;
				*/
				cible->setTranslation(Math::makeVector(x_rng, y_rng, z_rng));
				m_interpolation = Interpol_Traj(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0), Math::makeVector(0, 0, 0), cible->getTranslation(), Math::makeVector(0, 0, 0));
				tmp = 0;
				cible_Intermediaire->setTranslation(m_interpolation.SplineHermite(tmp));
				pt_Depart->setTranslation(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0));
			}
		}

		//Fonction pour ajouter une chaine polyarticulée au graphe de scène
		Animation::KinematicChain::Node * addChaineArt(unsigned int nb_Seg) {
			//socle
			HelperGl::Material mat_socle = HelperGl::Material();
			mat_socle.setDiffuse(HelperGl::Color(0.1f, 0.1f, 0.1f));
			mat_socle.setSpecular(HelperGl::Color(0.1f, 0.1f, 0.1f));
			SceneGraph::Cylinder* socle = new SceneGraph::Cylinder(mat_socle, 0.5, 0.5, 0.01);
			m_root.addSon(socle);
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
				v_correspondance->push_back(std::pair<SceneGraph::Rotate *, const Animation::KinematicChain::DegreeOfFreedom *>(rotate_x, &(rotate_xPoly->getDOF()[0])));
				/*~~~~~~~~*/
				//Scene de graphe
				SceneGraph::Rotate * rotate_z = new SceneGraph::Rotate(0, Math::makeVector(0, 0, 1));
				rotate_x->addSon(rotate_z);
				//chaine poly
				Animation::KinematicChain::Node * rotate_zPoly = ma_chaine->addDynamicRotation(rotate_xPoly, Math::makeVector(0, 0, 1), interval, 0);
				//vecteur de correspondance
				v_correspondance->push_back(std::pair<SceneGraph::Rotate *, const Animation::KinematicChain::DegreeOfFreedom *>(rotate_z,&(rotate_zPoly->getDOF()[0]) ));
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
			return chaintmp;
		}

		//Cette Méthode permet de rafraichier le graphe de scene grace au DOF
		void RefreshGraph() {
			for (auto pair = v_correspondance->begin(); pair < v_correspondance->end();pair++) {
				//std::cout << pair->second->operator float() << std::endl;
				(pair->first)->setAngle(pair->second->operator float());
			}
		}
	public:
		TP2_siaa()
		{

		}

		virtual void initializeRendering()
		{
			//init TP2
			nb_os = 10;
			extremiteNoeud =addChaineArt(nb_os);
			/*cible*/
			float rayon_sphere = 0.2;
			HelperGl::Material mat_sphere = HelperGl::Material();
			mat_sphere.setDiffuse(HelperGl::Color(1.f, 0.f, 0.f));
			mat_sphere.setSpecular(HelperGl::Color(1.f, 0.f, 0.f));
			float x_rng = pow(-1, (rand() % 100))*((float)(rand() % 100) / 100)*(nb_os/2);
			float y_rng = pow(-1, (rand() % 100))*((float)(rand() % 100) / 100)*(nb_os/2);
			float z_rng = ((float)(rand() % 100) / 100)*(nb_os/2);
			/*
			std::cout << x_rng << std::endl;
			std::cout << y_rng << std::endl;
			std::cout << z_rng << std::endl;
			*/
			cible = new SceneGraph::Translate(Math::makeVector(x_rng, y_rng, z_rng));
			cible->addSon(new SceneGraph::Sphere(mat_sphere, rayon_sphere));
			m_interpolation = Interpol_Traj(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0), Math::makeVector(0, 0, 0), cible->getTranslation(), Math::makeVector(0, 0, 0));
			tmp = 0;
			cible_Intermediaire = new SceneGraph::Translate(m_interpolation.SplineHermite(tmp));
			pt_Depart = new SceneGraph::Translate(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0));
			m_interpolation = Interpol_Traj(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0,0,0), Math::makeVector(0, 0, 0), cible->getTranslation(), Math::makeVector(0, 0, 0));
			mat_sphere.setDiffuse(HelperGl::Color(1.f, 0.4f, 0.f));
			mat_sphere.setSpecular(HelperGl::Color(1.f, 0.4f, 0.f));
			cible_Intermediaire->addSon(new SceneGraph::Sphere(mat_sphere, rayon_sphere/2));
			/*Point de départ*/
			pt_Depart = new SceneGraph::Translate(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0));
			mat_sphere.setDiffuse(HelperGl::Color(1.f, 1.f, 0.f));
			mat_sphere.setSpecular(HelperGl::Color(1.f, 1.f, 0.f));
			pt_Depart->addSon(new SceneGraph::Sphere(mat_sphere, rayon_sphere));
			//CDD
			m_CDD = new CCD(ma_chaine,extremiteNoeud);
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
			//Créer une nouvelle cible (Debug sinon il falllait appuyer su 'n')
			x_rng = pow(-1, (rand() % 100))*((float)(rand() % 100) / 100)*(nb_os / 2);
			y_rng = pow(-1, (rand() % 100))*((float)(rand() % 100) / 100)*(nb_os / 2);
			z_rng = ((float)(rand() % 100) / 100)*(nb_os / 2);
			cible->setTranslation(Math::makeVector(x_rng, y_rng, z_rng));
			m_interpolation = Interpol_Traj(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0), Math::makeVector(0, 0, 0), cible->getTranslation(), Math::makeVector(0, 0, 0));
			cible_Intermediaire->setTranslation(m_interpolation.SplineHermite(tmp));
			pt_Depart->setTranslation(extremiteNoeud->getGlobalTransformation()*Math::makeVector(0, 0, 0));
		}


		virtual void render(double dt)
		{
			/*convergence*/
			Math::Vector3f extremity = extremiteNoeud->getGlobalTransformation()*Math::makeVector(0.0f, 0.0f, 0.0f);
			Math::Vector3f deltaTarget = cible_Intermediaire->getTranslation() - extremity;
			if (m_CDD->solve(deltaTarget, 0.001f)) {
				tmp = tmp + dt;
				cible_Intermediaire->setTranslation(m_interpolation.SplineHermite(tmp));
			}
			/*Actualisation graphique*/
			RefreshGraph();
			/*~~~~~~~~*/
			handleKeys();
			GL::loadMatrix(m_camera.getInverseTransform());
			m_root.draw();
			/*cible*/
			cible->draw();
			cible_Intermediaire->draw();
			/*Point dez départ*/
			pt_Depart->draw();
		}
	};
}
#endif