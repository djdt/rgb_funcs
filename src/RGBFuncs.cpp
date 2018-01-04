#include "RGBFuncs.hpp"

#include <cmath>

std::array<float, 3> rgb::toHsv(const rgb::pixel& rgb)
{
	float cmax = std::max(std::max(rgb[0], rgb[1]), rgb[2]);
	float cmin = std::min(std::min(rgb[0], rgb[1]), rgb[2]);

	float delta = static_cast<float>(cmax - cmin);

	float hue = 0.f;
	float sat = 0.f;

	if (delta > 0) {
		if (cmax == rgb[0]) {
			hue = 60.f * std::fmod((rgb[1] - rgb[2]) / delta, 6.f);
		} else if (cmax == rgb[1]) {
			hue = 60.f * ((rgb[2] - rgb[0]) / delta + 2.f);
		} else {
			hue = 60.f * ((rgb[0] - rgb[1]) / delta + 4.f);
		}
		sat = delta / cmax;
	}
	if (hue < 0) hue += 360.f; 
	return {{hue, sat, cmax / 255.f}};
}

rgb::pixel rgb::fromHsv(const std::array<float, 3>& hsv)
{
	float c = hsv[2] * hsv[1];
	float x = c * (1.f - std::fabs(std::fmod(hsv[0] / 60.f, 2.f) - 1.f));
	float m = hsv[2] - c;

	std::array<float, 3> frgb = {{0.f, 0.f, 0.f}};
	
	if (hsv[0] < 60.f) {
		frgb = {{c, x, 0}};
	} else if (hsv[0] < 120.f) {
		frgb = {{x, c, 0}};
	} else if (hsv[1] < 180.f) {
		frgb = {{0, c, x}};
	} else if (hsv[1] < 240.f) {
		frgb = {{0, x, c}};
	} else if (hsv[1] < 300.f) {
		frgb = {{x, 0, c}};
	} else {
		frgb = {{c, 0, x}};
	}

	return {{static_cast<uint8_t>((frgb[0] + m) * 255),
		       static_cast<uint8_t>((frgb[1] + m) * 255),
					 static_cast<uint8_t>((frgb[2] + m) * 255)}};
}

rgb::pixel rgb::reduceToMean(const std::vector<rgb::pixel>& pix)
{
	if (pix.size() == 0) return {{0, 0, 0}};

	std::array<double, 3> means {{0.0, 0.0, 0.0}};

	for (auto p : pix) {
		means[0] += p[0];
		means[1] += p[1];
		means[2] += p[2];
	}

	return {{static_cast<uint8_t>(means[0] / pix.size()),
		       static_cast<uint8_t>(means[1] / pix.size()),
			     static_cast<uint8_t>(means[2] / pix.size())}};
}

double rgb::relativeDistance(const rgb::pixel& p1, const rgb::pixel& p2)
{
	return (p1[0] - p2[0]) * (p1[0] - p2[0]) +
				 (p1[1] - p2[1]) * (p1[1] - p2[1]) +
				 (p1[2] - p2[2]) * (p1[2] - p2[2]);
}

