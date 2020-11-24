#include"render/Shader.h"

#define GET_PROPERTY_ID(NAME) glGetUniformLocation(m_ID, (NAME).c_str())

namespace rtx
{
	namespace render
	{

		Shader::Shader() :m_ID(0)
		{
		}

		Shader::Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath)
		{

#pragma region load shader code from file

			char* vshaderCode;
			char* fshaderCode;
			std::ifstream vertIn;
			std::ifstream fragIn;
			std::string vstr, fstr;
			vertIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fragIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				vertIn.open(vertexShaderPath);
				fragIn.open(fragmentShaderPath);
				std::stringstream vstream, fstream;
				vstream << vertIn.rdbuf();
				fstream << fragIn.rdbuf();
				vertIn.close();
				fragIn.close();
				vstr = vstream.str();
				fstr = fstream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cerr << e.what() << std::endl;
			}
			vshaderCode = (char*)vstr.c_str();
			fshaderCode = (char*)fstr.c_str();

#pragma endregion

#pragma region vertex shader

			//  cerate vertex shader
			unsigned int vShader;
			vShader = glCreateShader(GL_VERTEX_SHADER);
			//  load shader code

			glShaderSource(vShader, 1, &vshaderCode, NULL);
			//  compile code
			glCompileShader(vShader);
			int success;
			char infoLog[512];
			glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vShader, 512, NULL, infoLog);
				std::cerr << "SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
			}
			else
			{
				std::cout << "SHADER: compile vertex shader successed! \n";
			}

#pragma endregion

#pragma region fragment shader

			//  cerate fragment shader
			unsigned int fShader;
			fShader = glCreateShader(GL_FRAGMENT_SHADER);
			//  load shader code

			glShaderSource(fShader, 1, &fshaderCode, NULL);
			//  compile 
			glCompileShader(fShader);
			char finfoLog[512];
			glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fShader, 512, NULL, finfoLog);
				std::cerr << "SHADER::FRAGMENT::COMPILATION_FAILED\n" << finfoLog << "\n";
			}
			else
			{
				std::cout << "SHADER: compile fragment shader successed! \n";
			}

#pragma endregion

#pragma region shader program

			m_ID = glCreateProgram();
			//  add shader to program
			glAttachShader(m_ID, vShader);
			glAttachShader(m_ID, fShader);
			//  link shaders from program
			glLinkProgram(m_ID);

			char pinfoLog[512];
			glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(m_ID, 512, NULL, pinfoLog);
				std::cerr << "SHADER::PROGRAM::LINK_FAILED\n" << pinfoLog << "\n";
			}
			else
			{
				std::cout << "SHADER: load shader program successed! \n";
			}

			//  release shaders
			glDeleteShader(vShader);
			glDeleteShader(fShader);

#pragma endregion


		}

		Shader::~Shader()
		{
			glDeleteProgram(m_ID);
		}

		void Shader::SetBool(const std::string& name,
			const bool& value) const
		{
			SetInt(name, value);
		}

		void Shader::SetInt(const std::string& name,
			const int& value) const
		{
			glUniform1f(GET_PROPERTY_ID(name), value);
		}

		void Shader::SetFloat(const std::string& name,
			const float& value) const
		{
			glUniform1f(GET_PROPERTY_ID(name), value);
		}

		void Shader::SetVector2(const std::string& name,
			const glm::vec2& value) const
		{
			glUniform2f(GET_PROPERTY_ID(name), value.x, value.y);
		}

		void Shader::SetVector3(const std::string& name,
			const glm::vec3& value) const
		{
			glUniform3f(GET_PROPERTY_ID(name), value.x, value.y, value.z);
		}

		void Shader::SetVector4(const std::string& name,
			const glm::vec4& value)const
		{
			glUniform4f(GET_PROPERTY_ID(name), value.x, value.y, value.z, value.w);
		}

		void Shader::SetColor(const std::string& name, const util::Color& color) const
		{
			SetVector4(name, color.RGBA());
		}

	}
}