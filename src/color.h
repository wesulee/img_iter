#pragma once


class Color {
public:
	typedef unsigned char ColorChannel;
	static constexpr ColorChannel defColorChannel = 0;
	static constexpr ColorChannel minColorChannel = 0;
	static constexpr ColorChannel maxColorChannel = 255;
	Color();
	Color(ColorChannel, ColorChannel, ColorChannel);
	Color(const Color&);
	~Color() = default;
	Color& operator=(const Color&);
	void blend(const Color&, const float);
	static ColorChannel blend(const ColorChannel, const ColorChannel, const float);
	static Color blend(const Color&, const Color&, const float);

	ColorChannel R;
	ColorChannel G;
	ColorChannel B;
};
