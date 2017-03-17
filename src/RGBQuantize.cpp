#include "RGBQuantize.hpp"

#include <algorithm>
#include <iostream>
#include <cmath>

struct RGBCompare
{
	const uint8_t _ch;
	RGBCompare(const uint8_t ch) : _ch(ch) {}
	bool operator() (const RGBPixel& p1, const RGBPixel& p2) {
		return (p1[_ch] < p2[_ch]);
	}
	bool operator() (const RGBPixel& p, const double d) {
		return (p[_ch] < d);
	}
};

struct MMCQBlock
{
	std::vector<RGBPixel> _pixels;
	uint8_t _max_channel;
};

std::pair<uint8_t,uint8_t> getMaxRangeAndChannel(std::vector<RGBPixel>& pix)
{
	std::pair<uint8_t,uint8_t> range_and_channel;
	for (uint8_t ch = 0; ch < 3; ++ch) {
		auto minmax_el = std::minmax_element(pix.begin(), pix.end(), RGBCompare(ch));

		uint8_t range = (*minmax_el.second)[ch] - (*minmax_el.first)[ch];
		if (range > range_and_channel.first) {
			range_and_channel.first = range;
			range_and_channel.second = ch;
		}
	}
	return range_and_channel;
}

double getChannelMean(std::vector<RGBPixel>& pix, uint8_t channel)
{
	double sum = 0;
	for (auto p : pix) {
		sum += p[channel];
	}
	return (sum / static_cast<double>(pix.size()));
}

/* std::vector<RGBPixel> splitVectorAtChannelMean(std::vector<RGBPixel>& pix, uint8_t channel) */
/* { */
/* 	double sum = 0; */
/* 	for (auto p : pix) { */
/* 		sum += p[channel]; */
/* 	} */
/* 	double mean = (sum / static_cast<double>(pix.size())); */
/* 	auto split_it = std::lower_bound(pix.begin(), pix.end(), mean, RGBCompare(channel)); */

/* 	std::vector<RGBPixel> second_half(std::make_move_iterator(split_it), */
/* 			std::make_move_iterator(pix.end())); */
/* 	pix.erase(split_it, pix.end()); */

/* 	return second_half; */
/* } */

std::pair<std::vector<RGBPixel>, std::vector<RGBPixel>>
splitVectorByChannelMean(std::vector<RGBPixel>& pix, uint8_t channel)
{
	double sum = 0;
	for (auto p : pix) {
		sum += p[channel];
	}
	double mean = (sum / static_cast<double>(pix.size()));

	std::vector<RGBPixel> pix_lower;
	std::vector<RGBPixel> pix_upper;
	for(auto p : pix) {
		if (p[channel] < mean) {
			pix_lower.push_back(p);
		} else {
			pix_upper.push_back(p);
		}
	}
	return {pix_lower, pix_upper};
}

RGBPixel reduceToMean(const std::vector<RGBPixel>& pix)
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

std::vector<RGBPixel> rgb::extractColors_mmc(RGBImage& img, uint8_t iters)
{
	std::vector<std::vector<RGBPixel>> buckets;
	buckets.push_back(img.pixels());

	for (uint8_t i = 0; i < iters; ++i) {
		std::vector<std::vector<RGBPixel>> tmp;
		tmp.reserve(buckets.size() * 2);

		for (auto b : buckets) {
			auto range_channel = getMaxRangeAndChannel(b);
			/* std::sort(b.begin(), b.end(), RGBCompare(range_channel.second)); */

			auto halves = splitVectorByChannelMean(
					b, range_channel.second);

			tmp.push_back(std::move(halves.first));
			tmp.push_back(std::move(halves.second));
		}
		buckets = std::move(tmp);
	}

	std::vector<RGBPixel> colors;
	for (auto b : buckets) {
		colors.push_back(reduceToMean(b));
	}
	return colors;
}
