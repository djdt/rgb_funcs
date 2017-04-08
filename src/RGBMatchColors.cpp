#include "RGBMatchColors.hpp"

#include <limits>

double relativeDistance(const rgb::pixel& p1, const rgb::pixel& p2)
{
	return (p1[0] - p2[0]) * (p1[0] - p2[0]) +
				 (p1[1] - p2[1]) * (p1[1] - p2[1]) +
				 (p1[2] - p2[2]) * (p1[2] - p2[2]);
}

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
