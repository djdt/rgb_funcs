#include "RGBQuantize/Histogram.hpp"

#include <algorithm>

#include "RGBFuncs.hpp"

std::vector<rgb::pixel> rgb::quantizeHistogram(
		const std::vector<rgb::pixel>& pixels, uint32_t num_colors, uint32_t partitions)
{
	std::vector<std::vector<rgb::pixel>> buckets;
	buckets.resize(partitions * partitions * partitions);

	double cell_size = 256.0 / static_cast<double>(partitions);

	// Calculate which bucket the pixel belongs using x,y,z pos
	for (auto p : pixels) {
		uint32_t index = 0;
		for (uint8_t i = 0; i < 3; ++i) {
			index += i * static_cast<uint32_t>(p[i] / cell_size);
		}
		buckets[index].push_back(p);
	}

	// Reduce buckets to number of colors required
	if (num_colors < buckets.size()) {
		std::sort(buckets.begin(), buckets.end(),[]
				(const std::vector<rgb::pixel>& a, const std::vector<rgb::pixel>& b) {
				return a.size() < b.size();
				});
		buckets.erase(buckets.begin(), buckets.end() - num_colors);
	}

	// Return average of remaining buckets
	std::vector<rgb::pixel> colors;
	for (auto c : buckets) {
		colors.push_back(reduceToMean(c));
	}

	return colors;
}
