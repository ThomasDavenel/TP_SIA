#pragma once

#include <Application/BaseWithKeyboard.h>
#include <glm/common.hpp>
#include <gl3/Camera.h>
#include <Config.h>
#include <gl3/MeshLoader.h>
#include <gl3/SkyboxGeometry.h>
#include <gl3/ShaderProgram.h>
#include <gl3/CubeMap.h>
#include <gl3/VertexArrayObject.h>
#include <gl3/FrameBuffer.h>
#include <gl3/ScreenQuad2D.h>
#include <gl3/GlobalState.h>

namespace Application
{
	class GL3_TP3 : public BaseWithKeyboard
	{
		// Data for the camera
		gl3::Camera m_camera;
		float m_cameraSpeed;
		float m_cameraRotationSpeed;
		float m_lastDt;

		// The mesh loader
		gl3::MeshLoader m_meshLoader;

		// The loaded meshes
		std::vector<gl3::Mesh *> m_meshes;
		std::vector<gl3::PhongMaterial*> m_materials;
		std::vector<gl3::VertexArrayObject> m_phongDirectionalVaos;
		gl3::VertexArrayObject m_SkyboxVaos;

		// Shaders
		gl3::ShaderProgram m_phongDirectionalShader;
		gl3::ShaderProgram m_SkyboxShader;
		gl3::ShaderProgram m_Filtre;
		gl3::ShaderProgram m_FlouGaussien;

		// Textures
		gl3::CubeMap m_skyboxTexture;

		// Clipping distance
		float m_clippingDistance;

		//FBO
		GLuint fbo;
		GLuint texture;
		GLuint depth;
		GLenum var = GL_COLOR_ATTACHMENT0;
		GLenum var2 = GL_DEPTH_ATTACHMENT;
		GLuint fboGauss;
		GLuint textureGauss;
		GLuint depthGauss;
		GLenum varGauss = GL_COLOR_ATTACHMENT0;

	public:
		GL3_TP3()
		{}

		virtual ~GL3_TP3()
		{}

	protected:
		virtual void reshape(GLint width, GLint height)
		{
			BaseWithKeyboard::reshape(width, height);
			// Creation FBO filtre
			//1- Buffer de type texture
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//2- Buffer de type render buffer
			/*
			glGenRenderbuffers(1, &depth);
			glBindRenderbuffer(GL_RENDERBUFFER, depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getConfiguration().width, getConfiguration().height);
			*/

			glGenTextures(1, &depth);
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			/*
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			*/

			//3- FBO
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
			glDrawBuffers(1, &var);
			//glDrawBuffers(1,&var2);

			// Creation FBO Flou Gaussien
			//1- Buffer de type texture
			glGenTextures(1, &textureGauss);
			glBindTexture(GL_TEXTURE_2D, textureGauss);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//2- Buffer de type render buffer
			glGenRenderbuffers(1, &depthGauss);
			glBindRenderbuffer(GL_RENDERBUFFER, depthGauss);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			//3- FBO
			glGenFramebuffers(1, &fboGauss);
			glBindFramebuffer(GL_FRAMEBUFFER, fboGauss);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureGauss, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthGauss);
			glDrawBuffers(1, &varGauss);
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

		void forceTextureCoordinates()
		{
			for (size_t cpt = 0; cpt < m_meshes.size(); ++cpt)
			{
				gl3::Mesh * mesh = m_meshes[cpt];
				if (!mesh->getTextureCoordinates())
				{
					std::cout << "Forcing texture coordinates on mesh " << cpt << "/" << m_meshes.size() << std::endl;
					std::vector<glm::vec2> textureCoordinates(mesh->getVertices()->size());
					std::fill(textureCoordinates.begin(), textureCoordinates.end(), glm::vec2(0, 0));
					mesh->setTextureCoodinates(textureCoordinates);
				}
			}
		}

		void computePhongDirectionalVAOs()
		{
			for (size_t cpt = 0; cpt < m_meshes.size(); ++cpt)
			{
				gl3::Mesh * mesh = m_meshes[cpt];
				std::vector<std::pair<std::string, const gl3::VertexBufferObject *>> configuration =
				{
					{"in_position", mesh->getVerticesVbo()},
					{"in_normal", mesh->getNormalsVbo()},
					{"in_textureCoordinates", mesh->getTextureCoordinatesVbo()}
				};
				gl3::VertexArrayObject vao(m_phongDirectionalShader, configuration, mesh->getIndicesEbo());
				m_phongDirectionalVaos.push_back(std::move(vao));
			}
		}
		void computeSkyboxVAOs()
		{
			//std::vector< std::pair< std::string, const gl3::VertexBufferObject * >>  vbos;
			//vbos.push_back(std::pair< std::string, const gl3::VertexBufferObject * >("in_position", m_vboVertices));
			//m_SkyboxVaos= gl3::VertexArrayObject(m_SkyboxShader, vbos, mesh->getIndicesEbo());
		}

		/// <summary>
		/// Computes the environment bounding box.
		/// </summary>
		/// <returns></returns>
		gl3::BoundingBox computeEnvironmentBoundingBox()
		{
			gl3::BoundingBox result;
			for (size_t cpt = 0; cpt < m_meshes.size(); ++cpt)
			{
				result = result + m_meshes[cpt]->boundingBox();
			}
			return result;
		}

		virtual void initializeRendering()
		{
#ifndef NDEBUG
			// Debug mode will be used in Debug configuration, not in release one.
			// The second parameter decides if the debug mode should automatically trigger
			// a breakpoint when an error / warning is reported.
			gl3::GlobalState::getSingleton()->enableDebugMode(true, true);
#endif

			// 0 - Camera setup
			m_camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
			m_cameraSpeed = 1.0f;
			m_cameraRotationSpeed = Math::pi / 5.0;
			m_lastDt = 0.1f;

			// 1 - We load the shaders
			{
				std::filesystem::path shaderPath = Config::dataPath() / "Shaders" / "TP3";
				m_phongDirectionalShader = gl3::ShaderProgram(shaderPath / "PhongDirectional.vert", shaderPath / "PhongDirectional.frag");
				m_SkyboxShader = gl3::ShaderProgram(shaderPath / "Skybox.vert", shaderPath / "Skybox.frag");
				m_Filtre = gl3::ShaderProgram(shaderPath / "Filtre.vert", shaderPath / "Filtre.frag");
				m_FlouGaussien = gl3::ShaderProgram(shaderPath / "FlouGaussien.vert", shaderPath / "FlouGaussien.frag");
			}

			// 2 - We load the meshes
			{
				std::filesystem::path meshPath = Config::dataPath() / "lake" / "lake.dae";
				std::pair<gl3::MeshLoader::const_iterator, gl3::MeshLoader::const_iterator> range = m_meshLoader.load(meshPath);
				auto meshSelector = [this](const std::pair<gl3::Mesh*, gl3::PhongMaterial*> & p) { return p.first; };
				auto materialSelector = [this](const std::pair<gl3::Mesh*, gl3::PhongMaterial*> & p) { return p.second; };
				std::transform(range.first, range.second, std::back_inserter(m_meshes), meshSelector);
				std::transform(range.first, range.second, std::back_inserter(m_materials), materialSelector);
				// We force the meshes to have textures coordinates
				forceTextureCoordinates();
				// We initialize the VAOs used for the Phong shader
				computePhongDirectionalVAOs();
				// We initialize the VAOs used for the Skybox shader
				//computeSkyboxVAOs();
			}

			// 3 - We load the skybox texture
			{
				std::filesystem::path skyboxPath = Config::dataPath() / "textures" / "cubemaps" / "sky";
				std::unordered_map<gl3::CubeMapPosition, std::filesystem::path> files =
				{
					{ gl3::CubeMapPosition::front,  skyboxPath / "cloudtop_ft.jpg"},
					{ gl3::CubeMapPosition::back,  skyboxPath / "cloudtop_bk.jpg"},
					{ gl3::CubeMapPosition::left,  skyboxPath / "cloudtop_lf.jpg"},
					{ gl3::CubeMapPosition::right,  skyboxPath / "cloudtop_rt.jpg"},
					{ gl3::CubeMapPosition::bottom,  skyboxPath / "cloudtop_dn.jpg"},
					{ gl3::CubeMapPosition::top,  skyboxPath / "cloudtop_up.jpg"}
				};
				m_skyboxTexture = gl3::CubeMap(files);
			}
			m_skyboxTexture.bind();


			// 3 - We modify the parameters of the camera based on the bounding box
			gl3::BoundingBox envBB = computeEnvironmentBoundingBox();
			glm::vec3 cameraPosition = glm::vec3(envBB.min().x + envBB.extent().x*0.5, envBB.min().y + envBB.extent().y*0.5, envBB.max().z);
			m_camera.setPosition(cameraPosition);
			m_cameraSpeed = glm::length(glm::vec2(envBB.extent())) / 40.0;

			// 4 - We create a sub menu for the clipping distance
			Application::Menu * menu = new Application::Menu("Clipping distance");
			float extent = glm::length(envBB.extent());
			menu->addItem("10% extent", [this, extent]() {m_clippingDistance = 0.1 * extent; });
			menu->addItem("20% extent", [this, extent]() {m_clippingDistance = 0.2 * extent; });
			menu->addItem("30% extent", [this, extent]() {m_clippingDistance = 0.3 * extent; });
			menu->addItem("40% extent", [this, extent]() {m_clippingDistance = 0.4 * extent; });
			menu->addItem("50% extent", [this, extent]() {m_clippingDistance = 0.5 * extent; });
			menu->addItem("60% extent", [this, extent]() {m_clippingDistance = 0.6 * extent; });
			menu->addItem("70% extent", [this, extent]() {m_clippingDistance = 0.7 * extent; });
			menu->addItem("80% extent", [this, extent]() {m_clippingDistance = 0.8 * extent; });
			menu->addItem("90% extent", [this, extent]() {m_clippingDistance = 0.9 * extent; });
			menu->addItem("100% extent", [this, extent]() {m_clippingDistance = extent; });
			getMenu()->addSubMenu(menu);
			m_clippingDistance = extent;

			// Creation FBO filtre
			//1- Buffer de type texture
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getConfiguration().width, getConfiguration().height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//2- Buffer de type render buffer
			/*
			glGenRenderbuffers(1, &depth);
			glBindRenderbuffer(GL_RENDERBUFFER, depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getConfiguration().width, getConfiguration().height);
			*/

			glGenTextures(1, &depth);
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, getConfiguration().width, getConfiguration().height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			/*
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			*/
			//3- FBO
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
			glDrawBuffers(1, &var);
			//glDrawBuffers(1,&var2);
			//4- test
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer Filtre is not complete!" << std::endl;
			}
			else {
				std::cout << "Framebuffer Filtre is complete =)" << std::endl;
			}

			// Creation FBO Flou Gaussien
			//1- Buffer de type texture
			glGenTextures(1, &textureGauss);
			glBindTexture(GL_TEXTURE_2D, textureGauss);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getConfiguration().width, getConfiguration().height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//2- Buffer de type render buffer
			glGenRenderbuffers(1, &depthGauss);
			glBindRenderbuffer(GL_RENDERBUFFER, depthGauss);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getConfiguration().width, getConfiguration().height);
			//3- FBO
			glGenFramebuffers(1, &fboGauss);
			glBindFramebuffer(GL_FRAMEBUFFER, fboGauss);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureGauss, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthGauss);
			glDrawBuffers(1, &varGauss);
			//4- test
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer Flou Gaussien is not complete!" << std::endl;
			}
			else {
				std::cout << "Framebuffer Flou Gaussien is complete =)" << std::endl;
			}
		}

		virtual void render(double dt)
		{
			// We set the last dt
			m_lastDt = dt;
			// Handles interactions with the keyboard
			handleKeys();
			// 0 - Matrices and initialisations
			glm::mat4 projectionMatrix = glm::perspective(glm::radians<float>(90), (float)getConfiguration().width / (float)getConfiguration().height, 0.01f, m_clippingDistance);
			glm::mat4 viewMatrix = m_camera.getInverseTransform();
			glm::mat4 modelMatrix = glm::mat4(1.0);
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
			glm::vec3 lightDirection = glm::normalize(glm::vec3(0.4f, -0.3f, -1.0f));
			glm::vec3 lightColor = glm::vec3(1, 1, 1);

			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			//1 - we draw the skybox
			{
				glDisable(GL_DEPTH_TEST); // Desactive le test de zBuffer
				glDepthMask(GL_FALSE); // Desactive l'écriture dans le zBuffer
				// 1: Activer le shader...
				m_SkyboxShader.use();
				// 2: Mettre à jour les uniforms
				glm::mat4 noTranslationMatrix = glm::mat4(glm::mat3(viewMatrix));
				m_SkyboxShader.trySetUniform("uni_mat_view", noTranslationMatrix);
				m_SkyboxShader.trySetUniform("uni_mat_projection", projectionMatrix);
				m_skyboxTexture.associateWithTextureUnit(0);
				m_SkyboxShader.trySetUniform("uni_skybox", 0);
				// 3: Rendu du cube
				gl3::SkyboxGeometry::getSingleton()->render(m_SkyboxShader.getAttributeLocation("in_position"));
				// 4 : desactivation du shader...
				m_SkyboxShader.unuse();
				glDepthMask(GL_TRUE); // Réactive l'écriture dans le zBuffer
				glEnable(GL_DEPTH_TEST); // Reactive le test de zBuffer
			}
			// 2 - We draw the scene
			{
				m_phongDirectionalShader.use();
				// We initialize the uniforms shared by every mesh
				m_phongDirectionalShader.trySetUniform("uni_clippingDistance", m_clippingDistance);
				m_skyboxTexture.associateWithTextureUnit(2);
				m_phongDirectionalShader.trySetUniform("uni_skybox", 2);
				m_phongDirectionalShader.trySetUniform("uni_mat_model", modelMatrix);
				m_phongDirectionalShader.trySetUniform("uni_mat_view", viewMatrix);
				m_phongDirectionalShader.trySetUniform("uni_mat_projection", projectionMatrix);
				m_phongDirectionalShader.trySetUniform("uni_mat_normal", normalMatrix);
				m_phongDirectionalShader.trySetUniform("uni_viewerPosition", m_camera.getPosition());
				m_phongDirectionalShader.trySetUniform("uni_light.direction", lightDirection);
				m_phongDirectionalShader.trySetUniform("uni_light.color", lightColor);
				// We draw every mesh 
				for (int cpt = 0; cpt < m_meshes.size(); ++cpt)
				{
					gl3::PhongMaterial * material = m_materials[cpt];
					material->getDiffuseTexture()->associateWithTextureUnit(0);
					m_phongDirectionalShader.trySetUniform("uni_diffuseTexture", 0);
					material->getSpecularTexture()->associateWithTextureUnit(1);
					m_phongDirectionalShader.trySetUniform("uni_specularTexture", 1);
					m_phongDirectionalShader.trySetUniform("uni_material.diffuseColor", material->getDiffuse());
					m_phongDirectionalShader.trySetUniform("uni_material.specularColor", material->getSpecular());
					m_phongDirectionalShader.trySetUniform("uni_material.shininess", material->getShininess());
					gl3::VertexArrayObject * vao = &m_phongDirectionalVaos[cpt];
					vao->bind();
					glDrawElements(GL_TRIANGLES, vao->eboSize(), GL_UNSIGNED_INT, nullptr);
					vao->unbind();
				}
				//glBindFramebuffer(GL_FRAMEBUFFER, 0);
				m_phongDirectionalShader.unuse();
			}
			
			glBindFramebuffer(GL_FRAMEBUFFER, fboGauss);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			// 3 post processing - filtre
			{
				glDisable(GL_DEPTH_TEST);
				m_Filtre.use(); 
				GLint unitIndex = 0;
				glActiveTexture(GL_TEXTURE0 + unitIndex);
				glBindTexture(GL_TEXTURE_2D, texture); 
				m_Filtre.setUniform("uni_texture", unitIndex);
				m_Filtre.setUniform("uni_bandes",(GLfloat) 10);
				m_Filtre.setUniform("uni_facteur", (GLfloat) 1.6);
				gl3::ScreenQuad2D::getSingleton()->render(m_Filtre.getAttributeLocation("in_position"), m_Filtre.getAttributeLocation("in_texCoords"));
				m_Filtre.unuse();
				glEnable(GL_DEPTH_TEST);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			// 3 post processing - Flou Gaussien
			{
				glDisable(GL_DEPTH_TEST);

				m_FlouGaussien.use();
				GLint unitIndex = 0;
				glActiveTexture(GL_TEXTURE0 + unitIndex);
				glBindTexture(GL_TEXTURE_2D, textureGauss);
				m_FlouGaussien.setUniform("uni_texture", unitIndex);
				GLint unitIndex1 = 1;
				glActiveTexture(GL_TEXTURE0 + unitIndex1);
				glBindTexture(GL_TEXTURE_2D, depth);
				m_FlouGaussien.setUniform("uni_profondeur", unitIndex1);
				m_FlouGaussien.setUniform("uni_largeurGaussienne", (int) 5);
				gl3::ScreenQuad2D::getSingleton()->render(m_FlouGaussien.getAttributeLocation("in_position"), m_FlouGaussien.getAttributeLocation("in_texCoords"));
				m_FlouGaussien.unuse();
				glEnable(GL_DEPTH_TEST);
			}
			
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}