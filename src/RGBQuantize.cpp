#include "RGBQuantize.hpp"

#include <algorithm>
#include <iostream>
#include <random>

rgbq::RGBChannelCompare::RGBChannelCompare(const uint8_t ch)
	: _ch(ch) {}
bool rgbq::RGBChannelCompare::operator() (
		const RGBPixel& p1, const RGBPixel& p2)
{
	return (p1[_ch] < p2[_ch]);
}

bool rgbq::RGBChannelCompare::operator() (
		const RGBPixel& p, const double d)
{
	return (p[_ch] < d);
}

std::pair<uint8_t,uint8_t> GetMaxRangeAndChannel(
		std::vector<RGBPixel>& pix)
{
	std::pair<uint8_t,uint8_t> range_and_channel;
	for (uint8_t ch = 0; ch < 3; ++ch) {
		auto minmax_el = std::minmax_element(pix.begin(), pix.end(),
				rgbq::RGBChannelCompare(ch));

		uint8_t range = (*minmax_el.second)[ch] - (*minmax_el.first)[ch];
		if (range > range_and_channel.first) {
			range_and_channel.first = range;
			range_and_channel.second = ch;
		}
	}
	return range_and_channel;
}

/* double GetChannelMean(std::vector<RGBPixel>& pix, uint8_t channel) */
/* { */
/* 	double sum = 0; */
/* 	for (auto p : pix) { */
/* 		sum += p[channel]; */
/* 	} */
/* 	return (sum / static_cast<double>(pix.size())); */
/* } */

std::pair<std::vector<RGBPixel>, std::vector<RGBPixel>>
SplitVectorByChannelMean(std::vector<RGBPixel>& pix, uint8_t channel)
{
	double sum = 0;
	for (auto p : pix) {
		sum += p[channel];
	}
	double mean = (sum / static_cast<double>(pix.size()));

	std::vector<RGBPixel> pix_lower, pix_upper;
	for(auto p : pix) {
		if (p[channel] < mean) {
			pix_lower.push_back(p);
		} else {
			pix_upper.push_back(p);
		}
	}
	return {pix_lower, pix_upper};
}

RGBPixel ReduceToMean(const std::vector<RGBPixel>& pix)
{
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

std::vector<RGBPixel> rgbq::ExtractColors_MedianCut(
		RGBImage& img, uint8_t num_colors, uint8_t iters)
{
	std::vector<std::vector<RGBPixel>> buckets;
	buckets.push_back(img.pixels());

	// Split the vectors in buckets in half at the mean value,
	// do this 'iters' times for 2^'iters' buckets
	for (uint8_t i = 0; i < iters; ++i) {
		std::vector<std::vector<RGBPixel>> tmp;
		tmp.reserve(buckets.size() * 2);

		for (auto b : buckets) {
			auto range_channel = GetMaxRangeAndChannel(b);

			auto halves = SplitVectorByChannelMean(
					b, range_channel.second);

			tmp.push_back(std::move(halves.first));
			tmp.push_back(std::move(halves.second));
		}
		buckets = std::move(tmp);
	}

	// Select the 'num_colors' most populated buckets and erase rest
	if (num_colors < buckets.size()) {
		std::sort(buckets.begin(), buckets.end(),[]
				(const std::vector<RGBPixel>& a, const std::vector<RGBPixel>& b) {
				return a.size() < b.size();
				});
		buckets.erase(buckets.begin(), buckets.end() - num_colors);
	}
	// Return the average of the remaining buckets
	std::vector<RGBPixel> colors;
	for (auto b : buckets) {
		colors.push_back(ReduceToMean(b));
	}

	return colors;
}

std::vector<RGBPixel> rgbq::ExtractColors_Histogram(
		RGBImage& img, uint8_t num_colors, uint8_t partitions)
{
	std::vector<std::vector<RGBPixel>> buckets;
	buckets.resize(partitions * partitions * partitions);

	double cell_size = 256.0 / static_cast<double>(partitions);

	// Calculate which bucket the pixel belongs using x,y,z pos
	for (auto p : img.pixels()) {
		uint32_t index = 0;
		for (uint8_t i = 0; i < 3; ++i) {
			index += i * static_cast<uint32_t>(p[i] / cell_size);
		}
		buckets[index].push_back(p);
	}

	// Reduce buckets to number of colors required
	if (num_colors < buckets.size()) {
		std::sort(buckets.begin(), buckets.end(),[]
				(const std::vector<RGBPixel>& a, const std::vector<RGBPixel>& b) {
				return a.size() < b.size();
				});
		buckets.erase(buckets.begin(), buckets.end() - num_colors);
	}

	// Return average of remaining buckets
	std::vector<RGBPixel> colors;
	for (auto c : buckets) {
		colors.push_back(ReduceToMean(c));
	}

	return colors;
}

double RelDistanceBetweenPoints(const RGBPixel& p1, const RGBPixel& p2)
{
	return (p1[0] - p2[0]) * (p1[0] - p2[0]) +
				 (p1[1] - p2[1]) * (p1[1] - p2[1]) +
				 (p1[2] - p2[2]) * (p1[2] - p2[2]);
}

std::vector<RGBPixel> rgbq::ExtractColors_KMeans(
		RGBImage& img, uint8_t k)
{
	// Select k random starting points
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<uint32_t> rand_i(0, img.pixels().size());

	std::vector<RGBPixel> means;
	means.reserve(k);
	for (uint8_t i = 0; i < k; ++i) {
		means.push_back(img.pixels()[rand_i(gen)]);
	}

	uint32_t info_iters = 0;

	bool means_changed = true;
	while (means_changed) {
		means_changed = false;

		std::vector<std::vector<RGBPixel>> buckets;
		buckets.resize(k);

		// Find closest mean for each pixel
		for (auto p : img.pixels()) {
			uint32_t closest_k = 0;
			double   closest_dist = std::numeric_limits<double>::max();

			for (uint32_t i = 0; i < k; ++i) {
				double dist = RelDistanceBetweenPoints(p, means[i]);
				if (dist < closest_dist) {
					closest_dist = dist;
					closest_k = i;
				}
			}
			// Add all to bucket of closest mean
			buckets[closest_k].push_back(p);
		}

		// Calculate new means
		std::vector<RGBPixel> new_means;
		new_means.reserve(k);
		for (auto b : buckets) {
			new_means.push_back(ReduceToMean(b));
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

	std::cout << "iters: " << info_iters << std::endl;
	return means;
}

#include "RGBOctree.hpp"

std::vector<RGBPixel> rgbq::ExtractColors_Octree(
		RGBImage& img, uint32_t num_colors, uint8_t max_depth)
{
	RGBOctree octree;

	for (auto p : img.pixels()) {
		octree.Insert(p, max_depth);
	}

	uint32_t it = 1;
	while (octree.CountLeaves() > num_colors) {
	std::cout << "iter: " << it << ", leaves: " << octree.CountLeaves() << std::endl;
		octree.ReduceDepth();
	}

	// Return average of remaining leaves
	std::vector<RGBPixel> colors;
	for (auto l : octree.GetLeaves()) {
		colors.push_back(
				{{static_cast<uint8_t>(l->r / l->refs),
				  static_cast<uint8_t>(l->g / l->refs),
					static_cast<uint8_t>(l->b / l->refs)}});
	}

	return colors;
}
