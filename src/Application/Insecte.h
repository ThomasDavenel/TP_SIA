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

namespace Application
{
	class Insecte {
	protected:
		HelperGl::Material* mat_corp= new HelperGl::Material();
		HelperGl::Material* mat_oeil = new HelperGl::Material();
		HelperGl::Material* mat_aile = new HelperGl::Material();
		SceneGraph::Group corp;
		SceneGraph::Group* aile;
		SceneGraph::Rotate* rotation_aile;
		SceneGraph::Sphere* oeil;
		float battement_ailes = 0;
	public:
		Insecte() :corp(), rotation_aile(nullptr) {
			//Definition materiaux de l'insecte
			mat_corp->setDiffuse(HelperGl::Color(1.f, 1.f, 1.f));
			mat_corp->setSpecular(HelperGl::Color(1.f, 1.f, 1.f));
			mat_oeil->setDiffuse(HelperGl::Color(1.f, 0.f, 0.f));
			mat_oeil->setSpecular(HelperGl::Color(1.f, 0.f, 0.f));
			mat_aile->setDiffuse(HelperGl::Color(0.f, 1.f, 0.f));
			mat_aile->setSpecular(HelperGl::Color(0.f, 1.f, 0.f));
			//Corp ovoïde
			SceneGraph::Scale* corp_mat = new SceneGraph::Scale(Math::makeVector(1.f, 0.3f, 0.3f));
			corp_mat->addSon(new SceneGraph::Sphere(*mat_corp));
			corp.addSon(corp_mat);
			//oeil: cylindre de rayon 0.05
			oeil = new SceneGraph::Sphere(*mat_oeil, 0.05);
			//oeil1
			SceneGraph::Translate* translation_oeil1 = new SceneGraph::Translate(Math::makeVector(0.7f, 0.2f, 0.2f));
			corp.addSon(translation_oeil1);
			translation_oeil1->addSon(oeil);
			//oeil2
			SceneGraph::Translate* translation_oeil2= new SceneGraph::Translate(Math::makeVector(0.7f, -0.2f, 0.2f));
			corp.addSon(translation_oeil2);
			translation_oeil2->addSon(oeil);
			//aile: ovoïde
			SceneGraph::Scale* aile = new SceneGraph::Scale(Math::makeVector(0.3f, 0.3f, 0.05f));
			aile->addSon(new SceneGraph::Sphere(*mat_aile));
			//aile1
			SceneGraph::Translate* translationAile1 = new SceneGraph::Translate(Math::makeVector(0.f, 0.3f, 0.f));
			corp.addSon(translationAile1);
			SceneGraph::Rotate* rotation_aile1 = new SceneGraph::Rotate(battement_ailes, Math::makeVector(0.f, 0.f, 1.f));
			translationAile1->addSon(rotation_aile1);
			rotation_aile1->addSon(aile);
			//aile2
			SceneGraph::Translate* translationAile2 = new SceneGraph::Translate(Math::makeVector(0.f, -0.3f, 0.f));
			corp.addSon(translationAile2);
			SceneGraph::Rotate* rotation_aile2 = new SceneGraph::Rotate(battement_ailes, Math::makeVector(0.f, 0.f, 1.f));
			translationAile2->addSon(rotation_aile2);
			rotation_aile2->addSon(aile);
		}

		SceneGraph::Group* getInsecte() { 
			return &corp;
		}
	};
}