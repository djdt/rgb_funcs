#ifndef _RGB_QUANTIZE_H_
#define _RGB_QUANTIZE_H_

#include <array>
#include <vector>

typedef std::array<uint8_t, 3> RGBPixel;

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
			const std::vector<RGBPixel>& pixels, uint32_t num_colors, uint32_t iters);
	std::vector<RGBPixel> ExtractColors_Histogram(
			const std::vector<RGBPixel>& pixels, uint32_t num_colors, uint32_t partitions);
	std::vector<RGBPixel> ExtractColors_KMeans(
			const std::vector<RGBPixel>& pixels, uint32_t k, uint32_t every_n_pixel);
	std::vector<RGBPixel> ExtractColors_Octree(
			const std::vector<RGBPixel>& pixels, uint32_t num_colors, uint32_t max_depth);
	// Returns a vector of length matches.size() with colors that match cloest with matches.
	std::vector<RGBPixel> MatchColors(const std::vector<RGBPixel>& colors, const std::vector<RGBPixel>& matches);
}

#endif /* ifndef _RGB_QUANTIZE_H_ */
