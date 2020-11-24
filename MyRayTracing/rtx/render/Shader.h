#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

#include"../util/Color.h"

namespace rtx
{
	namespace render
	{
		class Shader
		{
		public:
			Shader();

			Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);

			~Shader();

			inline int GetID()const
			{
				return m_ID;
			}

			inline void Use()const
			{
				glUseProgram(m_ID);
			}

			void SetBool(const std::string& name, const bool& value)const;

			void SetInt(const std::string& name, const int& value)const;

			void SetFloat(const std::string& name, const float& value)const;

			void SetVector2(const std::string& name, const glm::vec2& value)const;

			void SetVector3(const std::string& name, const glm::vec3& value)const;

			void SetVector4(const std::string& name, const glm::vec4& value)const;
		
			void SetColor(const std::string& name, const util::Color& color)const;

		private:

			/// <summary>
			/// id of shader program
			/// </summary>
			unsigned int m_ID;

		};

	}
}

#endif // !SHADER_H
