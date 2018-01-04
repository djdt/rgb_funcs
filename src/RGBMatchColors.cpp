#include "RGBMatchColors.hpp"

#include <limits>

#include "RGBFuncs.hpp"

std::vector<rgb::pixel> rgb::matchColors(
		const std::vector<rgb::pixel>& colors, const std::vector<rgb::pixel>& matches)
{
	std::vector<rgb::pixel> matched;
	matched.reserve(8);

	for (auto match : matches) {
		double near_dist = std::numeric_limits<double>::max();
		rgb::pixel match_color;

		for (auto color : colors) {
			double dist = relativeDistance(color, match);
			if (dist < near_dist) {
				near_dist = dist;
				match_color = color;
			}
		}
		matched.push_back(match_color);
	}
	return matched;
}
