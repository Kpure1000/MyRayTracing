#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H
#include<string>
#include<iostream>
#include<memory>
#include"Image.h"
#include"Shader.h"

namespace rtx
{
	namespace render
	{

		class RenderWindow
		{
		public:
			
			RenderWindow(int w, int h, const std::string& title);
			
			~RenderWindow();

			int InitRenderWindow();

			void Run();

		public:
			
			GLFWwindow* window;

			int width, height;
			
			std::string m_title;

			unsigned int VAO;
			unsigned int VBO;
			unsigned int EBO;
			unsigned int texture;
			render::Shader *shader;

			render::Image renderTarget;
		};

	}
}
#endif // !RENDERWINDOW_H
