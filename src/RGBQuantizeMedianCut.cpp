#include "RGBQuantize/MedianCut.hpp"

#include <algorithm>

rgb::ChannelCompare::ChannelCompare(const uint8_t ch)
	: _ch(ch) {}
bool rgb::ChannelCompare::operator() (
		const pixel& p1, const pixel& p2)
{
	return (p1[_ch] < p2[_ch]);
}

bool rgb::ChannelCompare::operator() (
		const pixel& p, const double d)
{
	return (p[_ch] < d);
}

std::pair<uint8_t,uint8_t> getMaxRangeAndChannel(
		std::vector<rgb::pixel>& pix)
{
	std::pair<uint8_t,uint8_t> range_and_channel;
	for (uint8_t ch = 0; ch < 3; ++ch) {
		auto minmax_el = std::minmax_element(pix.begin(), pix.end(),
				rgb::ChannelCompare(ch));

		uint8_t range = (*minmax_el.second)[ch] - (*minmax_el.first)[ch];
		if (range > range_and_channel.first) {
			range_and_channel.first = range;
			range_and_channel.second = ch;
		}
	}
	return range_and_channel;
}

std::pair<std::vector<rgb::pixel>, std::vector<rgb::pixel>>
splitVectorByChannelMean(std::vector<rgb::pixel>& pix, uint8_t channel)
{
	double sum = 0;
	for (auto p : pix) {
		sum += p[channel];
	}
	double mean = (sum / static_cast<double>(pix.size()));

	std::vector<rgb::pixel> pix_lower, pix_upper;
	for(auto p : pix) {
		if (p[channel] < mean) {
			pix_lower.push_back(p);
		} else {
			pix_upper.push_back(p);
		}
	}
	return {pix_lower, pix_upper};
}

rgb::pixel reduceToMean(const std::vector<rgb::pixel>& pix)
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

std::vector<rgb::pixel> rgb::quantizeMedianCut(
		const std::vector<pixel>& pixels, uint32_t num_colors, uint32_t iters)
{
	std::vector<std::vector<pixel>> buckets;
	buckets.push_back(pixels);

	// Split the vectors in buckets in half at the mean value,
	// do this 'iters' times for 2^'iters' buckets
	for (uint8_t i = 0; i < iters; ++i) {
		std::vector<std::vector<pixel>> tmp;
		tmp.reserve(buckets.size() * 2);

		for (auto b : buckets) {
			auto range_channel = getMaxRangeAndChannel(b);

			auto halves = splitVectorByChannelMean(
					b, range_channel.second);

			tmp.push_back(std::move(halves.first));
			tmp.push_back(std::move(halves.second));
		}
		buckets = std::move(tmp);
	}

	// Select the 'num_colors' most populated buckets and erase rest
	if (num_colors < buckets.size()) {
		std::sort(buckets.begin(), buckets.end(),[]
				(const std::vector<pixel>& a, const std::vector<pixel>& b) {
				return a.size() < b.size();
				});
		buckets.erase(buckets.begin(), buckets.end() - num_colors);
	}
	// Return the average of the remaining buckets
	std::vector<pixel> colors;
	for (auto b : buckets) {
		colors.push_back(reduceToMean(b));
	}

	return colors;
}
