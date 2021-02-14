#pragma once

#include <Application/BaseWithKeyboard.h>
#include <glm/common.hpp>
#include <gl3/Camera.h>
#include <Config.h>
#include <iostream>
#include <sstream>
#include <istream>

#include <gl3/MeshLoader.h>
#include <gl3/ShaderProgram.h>
#include <gl3/VertexArrayObject.h>

namespace Application
{
	class GL3_TP2 : public BaseWithKeyboard
	{
		//Nombre lumière
		int nb_lum = 0;
		//TEXTURE 
		GLuint m_textureIdDiffus;	// Identifiant de la texture diffus
		GLuint m_textureIdSpec;		// Identifiant de la texture spec
		GLuint m_textureIdNormal;	// Identifiant de la texture Normal
		//Shader
		gl3::ShaderProgram *  shader;

		//mesh
		gl3::MeshLoader meshLoader;
		gl3::Mesh* mesh;

		//Materiel de Phong
		gl3::PhongMaterial phong;

		//Lumiere 
		glm::vec3 pos_Lum = glm::vec3(1.5, 1.5, 2);
		glm::vec3 couleur_Lum = glm::vec3(100., 100., 100.);

		std::vector<glm::vec3>* pos_lums = new std::vector<glm::vec3>();
		std::vector<glm::vec3>* couleur_lums = new std::vector<glm::vec3>();
		std::vector< std::pair<glm::vec3,bool>>* couleur_lums_memoire = new std::vector< std::pair<glm::vec3,bool>>();
		//VAO
		gl3::VertexArrayObject m_vao;
		/**/
		gl3::Camera m_camera;
		float m_cameraSpeed;
		float m_cameraRotationSpeed;
		float m_lastDt;

		/*Attribut Phong*/
		glm::vec3 Couleur_diffuse= glm::vec3(0.6, 0.6, 0.6);
		glm::vec3 Couleur_spéculaire = glm::vec3(0.4, 0, 0);
		float shininess = 10.0;


	public:
		GL3_TP2()
		{}

		virtual ~GL3_TP2()
		{}

	protected:
		virtual void reshape(GLint width, GLint height)
		{
			BaseWithKeyboard::reshape(width, height);

		}

		float demandeCouleur() {
			while (true)
			{
				float couleur;
				::std::cin >> couleur;
				if (nb_lum >= 0 && nb_lum <= 255) { return couleur; }
				else {
					::std::cout << "Couleur incorrecte" << ::std::flush;
				}
			}
		}
		float demandePosition() {
			float pos;
			::std::cin >> pos;
			return pos; 
		}
		/// <summary>
	/// Loads a text file into a string.
	/// </summary>
	/// <param name="file">The file to load.</param>
	/// <returns></returns>
		static std::string loadTextFile(const std::filesystem::path & file)
		{
			if (!std::filesystem::exists(file))
			{
				std::cerr << "File " << file.string() << " does not exists" << std::endl;
				throw std::ios_base::failure(file.string() + " does not exists");
			}
			std::stringstream result;
			std::ifstream input(file);
			while (!input.eof())
			{
				std::string tmp;
				std::getline(input, tmp);
				result << tmp << std::endl;
			}
			return result.str();
		}

		void handleKeys()
		{
			glm::vec3 xAxis(1.0, 0.0, 0.0);
			glm::vec3 yAxis(0.0, 1.0, 0.0);
			if (m_keyboard.isPressed('!')) { quit(); }
			if (m_keyboard.isPressed('r')) { m_camera.translateFront(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('f')) { m_camera.translateFront(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('d')) { m_camera.translateRight(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('q')) { m_camera.translateRight(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('z')) { m_camera.translateUp(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('s')) { m_camera.translateUp(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('g')) { m_camera.rotateLocal(yAxis, m_cameraRotationSpeed*m_lastDt); }
			if (m_keyboard.isPressed('j')) { m_camera.rotateLocal(yAxis, -m_cameraRotationSpeed * m_lastDt); }
			if (m_keyboard.isPressed('y')) { m_camera.rotateLocal(xAxis, m_cameraRotationSpeed*m_lastDt); }
			if (m_keyboard.isPressed('h')) { m_camera.rotateLocal(xAxis, -m_cameraRotationSpeed * m_lastDt); }
		}

		virtual void initializeRendering()
		{
			//Choix nombre de lumière
			bool choix = false;
			while (!choix)
			{
				::std::cout << "Selection nombre de lumieres (1 - 16) : " << ::std::flush;
				::std::cin >> nb_lum;
				if (nb_lum >= 1 && nb_lum <=16) { choix=true; }
			}
			for (int i = 1; i <= nb_lum; i++) {
				::std::cout << "Position (x,y,z) de la lumière numero "<<i << ::std::endl;
				::std::cout << "x :" << ::std::flush;
				float x = demandePosition();
				::std::cout << "y :" << ::std::flush;
				float y = demandePosition();
				::std::cout << "z :" << ::std::flush;
				float z = demandePosition();
				pos_lums->push_back( glm::vec3(x, y, z) );
				::std::cout << "couleur (rouge,vert,bleu) de la lumière numero " << i << ::std::endl;
				::std::cout << "rouge :" << ::std::flush;
				float r = demandeCouleur();
				::std::cout << "vert :" << ::std::flush;
				float v = demandeCouleur();
				::std::cout << "bleu :" << ::std::flush;
				float b = demandeCouleur();
				couleur_lums->push_back(glm::vec3(r, v, b));
				couleur_lums_memoire->push_back(std::pair < glm::vec3,bool > (glm::vec3(r, v, b), true));
			}
			for (int i = 0; i < 16 - nb_lum; i++) {
				pos_lums->push_back(glm::vec3(0., 0., 0.));
				couleur_lums->push_back(glm::vec3(0., 0., 0.));
				couleur_lums_memoire->push_back(std::pair < glm::vec3, bool >(glm::vec3(0., 0., 0.), true));
			}
			//mesh setup
			std::filesystem::path path= Config::dataPath() / "Shapes" / "cube.fbx";
			std::pair<gl3::MeshLoader::const_iterator, gl3::MeshLoader::const_iterator> tmp = meshLoader.load(path);
			mesh = tmp.first[0].first;
			// 0 - Camera setup
			m_camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
			m_cameraSpeed = 1.0f;
			m_cameraRotationSpeed = Math::pi / 5.0;
			m_lastDt = 0.1f;

			//Q2
			phong.setDiffuse(glm::vec3(0.6, 0.6, 0.6));
			phong.setSpecular(glm::vec3(1, 0, 0));
			phong.setShininess(2.0);

			
			/*SHADER*/
			//Vertex Shader
			const ::std::filesystem::path pathCodeVertex = Config::dataPath() / "Shaders" / "TP2" / "tp2.vert";
			//Fragment shader
			const ::std::filesystem::path pathCodeFragment = Config::dataPath() / "Shaders" / "TP2" / "tp2.frag";

			//Creation shader
			shader = new gl3::ShaderProgram(pathCodeVertex, pathCodeFragment);
			// 5. Chargement de la texture
			/*DIFFUS*/
			const ::std::filesystem::path pathTexture = Config::dataPath() / "textures" / "container" / "container_diffuse.png";
			const char * filename = pathTexture.string().c_str();
			std::cout << filename << std::endl;
			m_textureIdDiffus = SOIL_load_OGL_texture(filename, 0, 0, SOIL_FLAG_MIPMAPS);
			glBindTexture(GL_TEXTURE_2D, m_textureIdDiffus);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			/*SPECULAIRE*/
			const ::std::filesystem::path pathTextureSpec = Config::dataPath() / "textures" / "container" / "container_specular.png";
			const char * filenameSpec = pathTextureSpec.string().c_str();
			std::cout << filenameSpec << std::endl;
			m_textureIdSpec = SOIL_load_OGL_texture(filenameSpec, 0, 0, SOIL_FLAG_MIPMAPS);
			glBindTexture(GL_TEXTURE_2D, m_textureIdSpec);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			/*Normal*/
			const ::std::filesystem::path pathTextureNormal = Config::dataPath() / "textures" / "container" / "container_normals.png";
			const char * filenameNormal = pathTextureNormal.string().c_str();
			std::cout << filenameNormal << std::endl;
			m_textureIdNormal = SOIL_load_OGL_texture(filenameNormal, 0, 0, SOIL_FLAG_POWER_OF_TWO);
			glBindTexture(GL_TEXTURE_2D, m_textureIdNormal);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			// Menu Lumière
			Application::Menu * menu = new Application::Menu("Activer/Desactiver lumiere");
			for (int l = 0; l < nb_lum; l++) {
				std::stringstream nom ;
				nom << "Lumiere " << l + 1;
				menu->addItem(nom.str(), [this, l]() {if (couleur_lums_memoire->at(l).second) {
					couleur_lums->at(l) = glm::vec3(0, 0, 0);
				}
				else {
					couleur_lums->at(l) = couleur_lums_memoire->at(l).first;
				}	
				couleur_lums_memoire->at(l).second = !couleur_lums_memoire->at(l).second; });
				}
			getMenu()->addSubMenu(menu);
		}

		virtual void render(double dt)
		{
			// We set the last dt
			m_lastDt = dt;
			// Handles interactions with the keyboard
			handleKeys();
			// Draw the scene
			//USE
			shader->use();
			//uniforms 
			shader->setUniform("uni_mat_view", m_camera.getInverseTransform());

			glm::mat4 model = glm::translate(glm::vec3(0.0, 0.0, 0.0));
			shader->setUniform("uni_mat_model", model);
			float resolution = getConfiguration().width / getConfiguration().height;
			glm::mat4 projection = glm::perspective(glm::pi<float>() / 2.0f, resolution, 0.001f, 10000.0f);			// Matrice de projection
			shader->setUniform("uni_mat_projection", projection);
			glm::mat3 mat_Normal = glm::transpose(glm::inverse(glm::mat3(model)));
			shader->setUniform("uni_mat_normale", mat_Normal);

			//shader->setUniform("uni_diffuse_Mat", phong.getDiffuse());
			//shader->setUniform("uni_speculaire_Mat", phong.getSpecular());
			shader->setUniform("uni_shininess_Mat", phong.getShininess());

			shader->setUniform("uni_position_Cam", m_camera.getPosition());

			std::vector<glm::vec3>::iterator it_pos = pos_lums->begin();
			std::vector<glm::vec3>::iterator it_color = couleur_lums->begin();

			shader->setUniform("uni_light1.pos", *it_pos);
			shader->setUniform("uni_light1.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light2.pos", *it_pos);
			shader->setUniform("uni_light2.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light3.pos", *it_pos);
			shader->setUniform("uni_light3.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light4.pos", *it_pos);
			shader->setUniform("uni_light4.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light5.pos", *it_pos);
			shader->setUniform("uni_light5.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light6.pos", *it_pos);
			shader->setUniform("uni_light6.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light7.pos", *it_pos);
			shader->setUniform("uni_light7.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light8.pos", *it_pos);
			shader->setUniform("uni_light8.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light9.pos", *it_pos);
			shader->setUniform("uni_light9.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light10.pos", *it_pos);
			shader->setUniform("uni_light10.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light11.pos", *it_pos);
			shader->setUniform("uni_light11.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light12.pos", *it_pos);
			shader->setUniform("uni_light12.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light13.pos", *it_pos);
			shader->setUniform("uni_light13.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light14.pos", *it_pos);
			shader->setUniform("uni_light14.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light15.pos", *it_pos);
			shader->setUniform("uni_light15.color", *it_color);
			it_pos++;
			it_color++;
			shader->setUniform("uni_light16.pos", *it_pos);
			shader->setUniform("uni_light16.color", *it_color);
			shader->setUniform("uni_nb_lum",nb_lum);
			//set Attributs
			const gl3::VertexBufferObject * m_vboVertices = mesh->getVerticesVbo();
			//shader->setAttribute("in_position", m_vboVertices);
			const gl3::VertexBufferObject * m_vboNormal = mesh->getNormalsVbo();
			//shader->setAttribute("in_normal", m_vboNormal);
			const gl3::VertexBufferObject * m_vboTexture = mesh->getTextureCoordinatesVbo();
			//shader->setAttribute("in_texCoords", m_vboTexture); 
			const gl3::VertexBufferObject * m_vboTangent = mesh->getTagentsVbo();
			//shader->setAttribute("in_tangent", m_vboTangent);
			const gl3::VertexBufferObject * m_vboBitangent = mesh->getBitangentsVbo();
			//shader->setAttribute("in_bitangent", m_vboBitangent);
			//EBO
			const gl3::ElementBufferObject * m_ebo = mesh->getIndicesEbo();
			//VAO
			std::vector< std::pair< std::string, const gl3::VertexBufferObject * >>  vbos;
			vbos.push_back(std::pair< std::string, const gl3::VertexBufferObject * >("in_position", m_vboVertices));
			vbos.push_back(std::pair< std::string, const gl3::VertexBufferObject * >("in_normal", m_vboNormal));
			vbos.push_back(std::pair< std::string, const gl3::VertexBufferObject * >("in_texCoords", m_vboTexture));
			vbos.push_back(std::pair< std::string, const gl3::VertexBufferObject * >("in_tangent", m_vboTangent));
			vbos.push_back(std::pair< std::string, const gl3::VertexBufferObject * >("in_bitangent", m_vboBitangent));
			m_vao = gl3::VertexArrayObject(*shader, vbos, m_ebo);
			//VAO
			m_vao.bind();
			// Connexion de la texture au shader
			
			GLint unitIndex= 0;
			glActiveTexture(GL_TEXTURE0 + unitIndex);
			glBindTexture(GL_TEXTURE_2D, m_textureIdDiffus);
			shader->setUniform("uni_textureDiff", unitIndex);

			GLint unitIndex1 = 1;
			glActiveTexture(GL_TEXTURE0 + unitIndex1);
			glBindTexture(GL_TEXTURE_2D, m_textureIdSpec);
			shader->setUniform("uni_textureSpec", unitIndex1);

			GLint unitIndex2 = 2;
			glActiveTexture(GL_TEXTURE0 + unitIndex2);
			glBindTexture(GL_TEXTURE_2D, m_textureIdNormal);
			shader->setUniform("uni_normalTexture", unitIndex2);
			
			// Affichage de la géométrie sous forme de triangles
			glDrawElements(GL_TRIANGLES, m_vao.eboSize(), GL_UNSIGNED_INT, 0);
			m_vao.unbind();
			shader->unuse();
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}