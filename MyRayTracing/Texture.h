#ifndef TEXTURE_H
#define TEXTURE_H
#include"Color.h"
#include<functional>
#include<stb/stb_image.h>
namespace ry
{
	class Texture
	{
	public:

		virtual Vector3 Value(const float& u, const float& v, const Vector3& p)const = 0;

	};

	/*纯色纹理*/
	class Constant_Texture : public Texture
	{
	public:

		Constant_Texture() {}
		
		Constant_Texture(Color color)
			:color(color)
		{}

		virtual Vector3 Value(const float& u, const float& v, const Vector3& p)const
		{
			return color.rgb;
		}

		Color color;

	};

	/*自定义检测纹理（棋盘格之类的）*/
	class Check_Texture : public Texture
	{
	public:

		Check_Texture() :color(nullptr)
		{}

		Check_Texture(std::function<Vector3(const float&, const float&, const Vector3&)>&& ColorCallBack)
			:color(ColorCallBack)
		{}

		virtual Vector3 Value(const float& u, const float& v, const Vector3& p)const
		{
			if (color != nullptr)
			{
				return color(u, v, p);
			}
			return { 0,0,0 };
		}

		std::function<Vector3(const float&, const float&, const Vector3&)> color;
	};

	class Image_Texture : public Texture
	{
	public:
		Image_Texture() {}

		Image_Texture(const char* filePath)
		{
			stbi_set_flip_vertically_on_load(true);
			data = stbi_load(filePath, &width, &height, &nChannel, 0);
			printf("读取图片\'%s\': w:%d, h:%d, nc:%d .\n", filePath, width, height, nChannel);
		}

		virtual Vector3 Value(const float& u, const float& v, const Vector3& p)const
		{
			int w = (int)((1 - u) * (float)width);
			int h = (int)((1 - v) * (float)height - 0.001f);
			Color re;
			for (int i = 0; i < nChannel; i++)
			{
				re[i] = data[w * nChannel + h * width * nChannel + i] / TO_RGB;
			}
			return re.rgb;
		}

		int width, height, nChannel;

		unsigned char* data;

	};


}
#endif // !TEXTURE_H
