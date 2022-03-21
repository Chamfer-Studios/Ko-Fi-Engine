#ifndef __COLOR_H__
#define __COLOR_H__

struct Color
{
	float r, g, b, a;

	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{}

	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
	{}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}
};

extern Color Red;
extern Color Green;
extern Color Blue;
extern Color Black;
extern Color White;

struct FadeColor
{
	FadeColor() : color(Color()), pos(0.5f)
	{}

	FadeColor(Color c, float p) : color(c), pos(p)
	{}

	FadeColor(float r, float g, float b, float a, float p) : color(Color(r, g, b, a)), pos(p)
	{}

	bool operator == (const FadeColor& fc)
	{
		return (this->pos == fc.pos && this->color.r == fc.color.r && this->color.g == fc.color.g && this->color.b == fc.color.b && this->color.a == fc.color.a);
	}

public:
	float pos;
	Color color;
};

#endif // !__COLOR_H__
