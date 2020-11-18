#ifndef TEXTURE_H
#define TEXTURE_H
#include"Color.h"
#include<functional>

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

	/*自定义检测纹理（棋盘格啥的）*/
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

}
#endif // !TEXTURE_H
