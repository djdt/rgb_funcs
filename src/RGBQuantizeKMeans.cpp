#include "RGBQuantize/KMeans.hpp"

#include <random>

#include "RGBFuncs.hpp"

double relativeDistance(const rgb::pixel& p1, const rgb::pixel& p2)
{
	return (p1[0] - p2[0]) * (p1[0] - p2[0]) +
				 (p1[1] - p2[1]) * (p1[1] - p2[1]) +
				 (p1[2] - p2[2]) * (p1[2] - p2[2]);
}

std::vector<rgb::pixel> rgb::quantizeKMeans(
		const std::vector<rgb::pixel>& pixels, uint32_t k, uint32_t every_n_pixel)
{
	// Reduce num of pixels to improve performance
	std::vector<rgb::pixel> reduced;
	if (every_n_pixel > 1) {
		reduced.reserve(pixels.size() / every_n_pixel);
		for (auto it = pixels.begin(); it != pixels.end(); it += 4) {
			reduced.push_back(*it);
		}
	} else {
		reduced = std::move(pixels);
	}
	// Select k random starting points
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<uint32_t> rand_i(0, pixels.size());

	std::vector<rgb::pixel> means;
	means.reserve(k);
	for (uint8_t i = 0; i < k; ++i) {
		means.push_back(pixels[rand_i(gen)]);
	}

	uint32_t info_iters = 0;

	bool means_changed = true;
	while (means_changed) {
		means_changed = false;

		std::vector<std::vector<rgb::pixel>> buckets;
		buckets.resize(k);

		// Find closest mean for each pixel
		for (auto p : reduced) {
			uint32_t closest_k = 0;
			double   closest_dist = std::numeric_limits<double>::max();

			for (uint32_t i = 0; i < k; ++i) {
				double dist = relativeDistance(p, means[i]);
				if (dist < closest_dist) {
					closest_dist = dist;
					closest_k = i;
				}
			}
			// Add all to bucket of closest mean
			buckets[closest_k].push_back(p);
		}

		// Calculate new means
		std::vector<rgb::pixel> new_means;
		new_means.reserve(k);
		for (auto b : buckets) {
			new_means.push_back(reduceToMean(b));
		}

		// Compare means to see if changed
		for (uint8_t i = 0; i < k; ++i) {
			if (means[i] != new_means[i]) {
				means_changed = true;
				break;
			}
		}
		means = std::move(new_means);
		info_iters++;
	}

	return means;
}
