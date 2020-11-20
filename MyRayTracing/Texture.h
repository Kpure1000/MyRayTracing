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

	/*自定义纹理*/
	class Customize_Texture : public Texture
	{
	public:

		Customize_Texture() :color(nullptr)
		{}

		Customize_Texture(std::function<Vector3(const float&, const float&, const Vector3&)>&& ColorCallBack)
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
			if (data)printf("读取图片成功: \'%s\': w:%d, h:%d, nc:%d .\n", filePath, width, height, nChannel);
		}

		virtual Vector3 Value(const float& u, const float& v, const Vector3& p)const
		{
			if (data)
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
			return { 0,0,0 };
		}

		int width, height, nChannel;

		unsigned char* data;

	};


}
#endif // !TEXTURE_H
