#ifndef _RGB_QUANTIZE_H_
#define _RGB_QUANTIZE_H_

#include <vector>
#include "RGBImage.hpp"

namespace rgbq
{
	struct RGBChannelCompare
	{
		const uint8_t _ch;
		RGBChannelCompare(const uint8_t ch);
		bool operator() (const RGBPixel& p1, const RGBPixel& p2);
		bool operator() (const RGBPixel& p, const double d);
	};

	std::vector<RGBPixel> ExtractColors_MedianCut(
			RGBImage& img, uint8_t num_colors, uint8_t iters);
	std::vector<RGBPixel> ExtractColors_Histogram(
			RGBImage& img, uint8_t num_colors, uint8_t partitions);
	std::vector<RGBPixel> ExtractColors_KMeans(
			RGBImage& img, uint8_t k);
	std::vector<RGBPixel> ExtractColors_Octree(
			RGBImage& img, uint32_t num_colors, uint8_t max_depth);
}

#endif /* ifndef _RGB_QUANTIZE_H_ */
