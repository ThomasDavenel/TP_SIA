#pragma once

#include <Application/BaseWithKeyboard.h>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gl3/Camera.h>
#include <fstream>

namespace Application
{
	// -----------------
	// Premier TP OpenGL
	// -----------------
	class GL3_TP1 : public BaseWithKeyboard
	{
		gl3::Camera m_camera;
		float m_cameraSpeed, m_cameraRotationSpeed;
		float m_lastDt;


	GLuint programId; // Identifiant du shader program
		std::vector<glm::vec3> vertices; // Les sommets de la géométrie
		std::vector<GLuint> indexes; //Indices des sommets des triangles
		std::vector<glm::vec3> colors; // Couleur des sommets de la géométrie
		//VBO
		GLuint m_vboVertices, m_vboColors;
		//EBO
		GLuint m_eboIndexes;
		//VAO
		GLuint m_vao;
		//uniform


	public:
		GL3_TP1()
		{}

		virtual ~GL3_TP1()
		{}

	protected:
		/// <summary>
		/// Method called when the size of the window changes.
		/// </summary>
		/// <param name="width">The new width.</param>
		/// <param name="height">The new height.</param>
		virtual void reshape(GLint width, GLint height)
		{
			BaseWithKeyboard::reshape(width, height);
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

		virtual void initializeRendering()
		{
			// 0 - Camera setup
			m_camera.setPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			m_cameraSpeed = 1.0f;
			m_cameraRotationSpeed = Math::pi / 5.0;
			m_lastDt = 0.1f;

			// Vertices positions
			vertices = {
				glm::vec3(-0.5f, -0.5f, 0.0f),
				glm::vec3(0.5f, -0.5f, 0.0f),
				glm::vec3(0.0f,  0.5f, 0.0f)
			};
			// Vertices colors
			colors = {
				glm::vec3(1.0, 0.0, 0.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.0, 0.0, 1.0)
			};
			// Indexes for the EBO
			indexes = { 0, 1, 2 };

			// The base path of the shader files
			std::filesystem::path shaderPath = Config::dataPath() / "Shaders";

			//Qestion 1: Creation du Vertex Buffer Object
			glGenBuffers(1, &m_vboVertices);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
			//Question 7:VBO color
			glGenBuffers(1, &m_vboColors);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*colors.size(), colors.data(), GL_STATIC_DRAW);

			//Question 2: Compilation des shaders

			//Vertex Shader
			::std::filesystem::path pathCode = Config::dataPath() / "Shaders" / "TP1" / "tp1.vert"; ;
			std::string StringCode = loadTextFile(pathCode);
			const char * sourceCode = StringCode.c_str();
			// We create and compile the shader
			GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShaderId, 1, &sourceCode, NULL);
			glCompileShader(vertexShaderId);
			// We check the compilation status and report any errors
			GLint shaderStatus;
			glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &shaderStatus);
			// If the shader failed to compile, display the info log
			if (shaderStatus == GL_FALSE)
			{
				GLint infoLogLength;
				glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
				GLchar *strInfoLog = new GLchar[infoLogLength + 1];
				glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, strInfoLog);
				std::cerr << "Shader: Vertex shader compilation failed. " << std::endl << strInfoLog << std::endl;
			}
			//Fragment Shader

			pathCode = Config::dataPath() / "Shaders" / "TP1" / "tp1.frag"; ;
			StringCode = loadTextFile(pathCode);
			sourceCode = StringCode.c_str();
			// We create and compile the shader
			GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShaderId, 1, &sourceCode, NULL);
			glCompileShader(fragmentShaderId);
			// We check the compilation status and report any errors
			shaderStatus;
			glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &shaderStatus);
			// If the shader failed to compile, display the info log
			if (shaderStatus == GL_FALSE)
			{
				GLint infoLogLength;
				glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
				GLchar *strInfoLog = new GLchar[infoLogLength + 1];
				glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, strInfoLog);
				std::cerr << "Shader: Fragment shader compilation failed. " << std::endl << strInfoLog << std::endl;
			}

			// We create the program, attach the shaders and link
			programId = glCreateProgram();
			glAttachShader(programId, vertexShaderId);
			glAttachShader(programId, fragmentShaderId);
			glLinkProgram(programId);
			// Check the status
			GLint linkStatus;
			glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
			// If the link failed, we output the error
			if (linkStatus == GL_FALSE)
			{
				GLint infoLogLength = 4096;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
				GLchar *strInfoLog = new GLchar[infoLogLength + 1];
				glGetProgramInfoLog(programId, infoLogLength, NULL, strInfoLog);
				std::cerr << "ShaderProgram: Failed to link shader program." << std::endl
					<< strInfoLog << std::endl;
			}

			// 3. Initialisation de l'EBO
			glGenBuffers(1, &m_eboIndexes);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboIndexes);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*indexes.size(), indexes.data(), GL_STATIC_DRAW);
			//question 6 :Initialisation du VAO
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);
			// Enregistrement de la connexion du VBO au shader program
			/**/
			glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
			GLint posAttrib = glGetAttribLocation(programId, "in_position");
			glEnableVertexAttribArray(posAttrib);
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, false, 0, nullptr);
			// Enregistrmenet de l'attachement de l'EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboIndexes);
			/**/
			glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
			GLint color = glGetAttribLocation(programId, "in_color");
			glEnableVertexAttribArray(color);
			glVertexAttribPointer(color, 3, GL_FLOAT, false, 0, nullptr);
			// Terminaison de l'enregistrement
			glBindVertexArray(0);
		}

		virtual void render(double dt)
		{
			// We set the last dt
			m_lastDt = dt;
			// Handles interactions with the keyboard
			handleKeys();
			// Utilisation du shader program
			glUseProgram(programId);
			// Initialisation du uniform
			GLint uniView = glGetUniformLocation(programId, "uni_mat_view");
			glm::mat4 m_view = m_camera.getInverseTransform();
			glUniformMatrix4fv(uniView, 1, GL_FALSE,glm::value_ptr(m_view));

			GLint uniMonde = glGetUniformLocation(programId, "uni_mat_model");
			glm::mat4 model= glm::translate(glm::vec3(0.5, 0.0, 0.0));
			glUniformMatrix4fv(uniMonde, 1, GL_FALSE, glm::value_ptr(model));

			GLint uniProjection = glGetUniformLocation(programId, "uni_mat_projection");
			float resolution = getConfiguration().width / getConfiguration().height;
			glm::mat4 projection = glm::perspective(glm::pi<float>() / 2.0f, resolution, 0.001f, 10000.0f);			// Matrice de projection
			glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));
			/*
			// Connexion du VBO au shader program
			glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
			GLint posAttrib = glGetAttribLocation(programId, "in_position");
			glEnableVertexAttribArray(posAttrib);
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, false, 0, nullptr);
			// Attachement de l'EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboIndexes);
			*/
			// Utilisation du VAO
			glBindVertexArray(m_vao);
			// Affichage de la géométrie sous forme de triangles
			glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
			
			model =glm::translate(glm::vec3(-0.5, 0.0, 0.0));
			glUniformMatrix4fv(uniMonde, 1, GL_FALSE, glm::value_ptr(model));
			
			glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
			
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}