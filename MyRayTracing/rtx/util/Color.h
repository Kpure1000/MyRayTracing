#ifndef RTX_COLOR_H
#define RTX_COLOR_H
#include<glm/glm.hpp>

#ifndef TO_RGB
#define TO_RGB 255.99f
#endif // !_TO_RGB

namespace rtx
{
	namespace util
	{
		class Color
		{

		public:

			Color() : rgba(0, 0, 0, 1.0f) {}

			Color(float r, float g, float b) :rgba(r, g, b, 1.0f) {}

			Color(glm::vec3 color) :rgba(color, 1.0f) {}

			Color(float r, float g, float b, float a) :rgba(r, g, b, a) {}

			Color(glm::vec3 color, float a) :rgba(color, a) {}

			Color(unsigned long color)
				:rgba((color >> 8 & 0xff) / TO_RGB,
					(color >> 4 & 0xff) / TO_RGB,
					(color >> 2 & 0xff) / TO_RGB,
					(color & 0xff) / TO_RGB)
			{}

			float& operator[](const int& index)
			{
				if (index >= 0 && index < 3)
				{
					return rgba[index];
				}
				return rgba.a;
			}

			float operator[](const int& index)const
			{
				if (index >= 0 && index < 3)
				{
					return rgba[index];
				}
				return 0.0f;
			}

			glm::vec4& RGBA()
			{
				return rgba;
			}

			glm::vec4 RGBA()const
			{
				return rgba;
			}

			float r()const { return rgba.r; }
			float g()const { return rgba.g; }
			float b()const { return rgba.b; }
			float a()const { return rgba.a; }

			float& r() { return rgba.r; }
			float& g() { return rgba.g; }
			float& b() { return rgba.b; }
			float& a() { return rgba.a; }

		private:

			glm::vec4 rgba;
		};
	}

}
#endif // !RTX_COLOR_H