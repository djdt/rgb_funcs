#ifndef _RGB_QUANTIZE_MATCH_COLORS_H_
#define _RGB_QUANTIZE_MATCH_COLORS_H_

#include <array>
#include <vector>

namespace rgb
{
	typedef std::array<uint8_t, 3> pixel;

	std::vector<pixel> matchColors(
			const std::vector<pixel>& colors, const std::vector<pixel>& matches);
};

#endif /* ifndef _RGB_QUANTIZE_MATCH_COLORS_H_ */
