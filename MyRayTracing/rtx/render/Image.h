#ifndef IMAGE_H
#define IMAGE_H

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<stb/stb_image.h>

namespace rtx
{
	namespace render
	{

		class Image
		{
		public:

			Image() :width(0), height(0), channel(0), data(nullptr), loadMode(Load_Mode::NONE) {}

			void LoadFromMemory(const int& w, const int& h, const int& ch, unsigned char* data)
			{
				loadMode = Load_Mode::LOAD_MEMORY;
				this->width = w;
				this->height = h;
				this->channel = ch;
				this->data = data;
			}

			void LoadFromFile(const char* path)
			{
				loadMode = Load_Mode::LOAD_FILE;
				stbi_set_flip_vertically_on_load(true);
				data = stbi_load(path, &width, &height, &channel, 0);
			}

			void LoadFromFile(const char* path, bool isFlip)
			{
				loadMode = Load_Mode::LOAD_FILE;
				stbi_set_flip_vertically_on_load(isFlip);
				data = stbi_load(path, &width, &height, &channel, 0);
			}

			void Use()const
			{
				if (data != nullptr)
				{
					if (channel == 3)
					{
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
					else if (channel == 4)
					{
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
					else
					{
						std::cerr << "Texture image channel numbers error.\n";
					}
				}
				else
				{
					if (loadMode == Load_Mode::LOAD_FILE)
					{
						std::cerr << "Load Image Error: from file.\n";
					}
					else
					{
						std::cerr << "Load Image Error: from other source.\n";
					}
				}
			}

			~Image()
			{
				if (loadMode == Load_Mode::LOAD_FILE && data != nullptr)
				{
					stbi_image_free(data);
					data = nullptr;
				}
			}

			int GetWidth() { return width; }
			int GetHeight() { return height; }
			int GetChannel() { return channel; }

		private:

			int width, height, channel;
			unsigned char* data;

			enum class Load_Mode
			{
				NONE,
				LOAD_MEMORY,
				LOAD_FILE
			};

			Load_Mode loadMode;

		};

	}
}
#endif // !IMAGE_H
