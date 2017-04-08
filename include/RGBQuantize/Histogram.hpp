#ifndef _RGB_QUANTIZE_HISTOGRAM_H_
#define _RGB_QUANTIZE_HISTOGRAM_H_

#include <array>
#include <vector>

namespace rgb
{
	typedef std::array<uint8_t, 3> pixel;

	std::vector<pixel> quantizeHistogram(
			const std::vector<pixel>& pixels, uint32_t num_colors, uint32_t partitions);
};

#endif /* ifndef _RGB_QUANTIZE_HISTOGRAM_H_ */
