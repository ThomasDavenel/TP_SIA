#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <Application/BaseWithKeyboard.h>
#include <Application/KeyboardStatus.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/Rotate.h>
#include <SceneGraph/Translate.h>
#include <SceneGraph/Scale.h>
#include <SceneGraph/Sphere.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <GL/compatibility.h>
#include <HelperGl/Material.h>
#include <HelperGl/Color.h>
#include <Math/Vectorf.h>

	class Insecte {
	protected:
		HelperGl::Material* mat_corp= new HelperGl::Material();
		HelperGl::Material* mat_oeil = new HelperGl::Material();
		HelperGl::Material* mat_aile = new HelperGl::Material();
		SceneGraph::Group* corp = new SceneGraph::Group();
		SceneGraph::Translate* insecte =new SceneGraph::Translate(Math::makeVector(0.f, 0.0f, 0.0f));
		SceneGraph::Rotate* rot_x = new SceneGraph::Rotate(0, Math::makeVector(1.f, 0.0f, 0.0f));
		SceneGraph::Rotate* rot_y = new SceneGraph::Rotate(0, Math::makeVector(0.f, 1.0f, 0.0f));
		SceneGraph::Rotate* rot_z = new SceneGraph::Rotate(0, Math::makeVector(0.f, 0.0f, 1.0f));
		SceneGraph::Group* aile;
		SceneGraph::Rotate* rotation_aile1;
		SceneGraph::Rotate* rotation_aile2;
		SceneGraph::Sphere* oeil;
		float battement_ailes=0.78;
		float angle = 7.f;
	public:
		Insecte() :rotation_aile1(nullptr), rotation_aile2(nullptr) {

			//Definition materiaux de l'insecte
			mat_corp->setDiffuse(HelperGl::Color(1.f, 1.f, 1.f));
			mat_corp->setSpecular(HelperGl::Color(1.f, 1.f, 1.f));
			mat_oeil->setDiffuse(HelperGl::Color(1.f, 0.f, 0.f));
			mat_oeil->setSpecular(HelperGl::Color(1.f, 0.f, 0.f));
			mat_aile->setDiffuse(HelperGl::Color(0.f, 1.f, 0.f));
			mat_aile->setSpecular(HelperGl::Color(0.f, 1.f, 0.f));
			//
			insecte->addSon(rot_z);
			rot_z->addSon(rot_y);
			rot_y->addSon(rot_x);
			rot_x->addSon(corp);
			//Corp ovoïde
			SceneGraph::Scale* corp_mat = new SceneGraph::Scale(Math::makeVector(1.f, 0.3f, 0.3f));
			corp_mat->addSon(new SceneGraph::Sphere(*mat_corp));
			corp->addSon(corp_mat);
			//oeil: cylindre de rayon 0.05
			oeil = new SceneGraph::Sphere(*mat_oeil, 0.05);
			//oeil1
			SceneGraph::Translate* translation_oeil1 = new SceneGraph::Translate(Math::makeVector(0.7f, 0.2f, 0.2f));
			corp->addSon(translation_oeil1);
			translation_oeil1->addSon(oeil);
			//oeil2
			SceneGraph::Translate* translation_oeil2= new SceneGraph::Translate(Math::makeVector(0.7f, -0.2f, 0.2f));
			corp->addSon(translation_oeil2);
			translation_oeil2->addSon(oeil);
			//aile: ovoïde
			SceneGraph::Scale* aile = new SceneGraph::Scale(Math::makeVector(0.3f, 0.3f, 0.05f));
			aile->addSon(new SceneGraph::Sphere(*mat_aile));
			//aile1
			SceneGraph::Translate* translationAile1 = new SceneGraph::Translate(Math::makeVector(0.f, 0.6f, 0.f));
			rotation_aile1 = new SceneGraph::Rotate(-battement_ailes, Math::makeVector(1.f, 0.f, 0.f));
			rotation_aile1->addSon(translationAile1);
			corp->addSon(rotation_aile1);
			translationAile1->addSon(aile);
			//aile2
			SceneGraph::Translate* translationAile2 = new SceneGraph::Translate(Math::makeVector(0.f, -0.6f, 0.f));
			rotation_aile2 = new SceneGraph::Rotate(battement_ailes, Math::makeVector(1.f, 0.f, 0.f));
			rotation_aile2->addSon(translationAile2);
			corp->addSon(rotation_aile2);
			translationAile2->addSon(aile);
		}

		SceneGraph::Group* getInsecte() { 
			return insecte;
		}
		void animationAile(float dt) {
			battement_ailes += dt*angle;
			if (battement_ailes>0.78 || battement_ailes<-1.2) angle = -angle;
			rotation_aile1->setAngle(-battement_ailes);
			rotation_aile2->setAngle(battement_ailes);
		}
		float getAngleAile() {
			return battement_ailes;
		}
		void mouvement(Math::Vector3f v) {
			insecte->setTranslation(v);
		}
		void Rota_X(float angle) {
			rot_x->setAngle(angle + Math::pi);
		}
		void Rota_Y(float angle) {
			rot_y->setAngle(angle + (Math::pi / 2));
		}
		void Rota_Z(float angle) {
			rot_z->setAngle(angle + Math::pi );
		}
	};