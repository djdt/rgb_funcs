#ifndef _RGB_QUANTIZE_MEDIAN_CUT_H_
#define _RGB_QUANTIZE_MEDIAN_CUT_H_

#include <array>
#include <vector>


namespace rgb
{
	typedef std::array<uint8_t, 3> pixel;
	struct ChannelCompare
	{
		const uint8_t _ch;
		ChannelCompare(const uint8_t ch);
		bool operator() (const pixel& p1, const pixel& p2);
		bool operator() (const pixel& p, const double d);
	};

	std::vector<pixel> quantizeMedianCut(
			const std::vector<pixel>& pixels, uint32_t num_colors, uint32_t iters);
};

#endif /* ifndef _RGB_QUANTIZE_MEDIAN_CUT_H_ */
