#ifndef _RGB_FUNCS_H_
#define _RGB_FUNCS_H_

#include <array>
#include <cstdint>
#include <vector>

namespace rgb {
	typedef std::array<uint8_t, 3> pixel;

	std::array<float, 3> toHsv(const pixel& rgb);
	pixel fromHsv(const std::array<float, 3>& rgb);

	pixel reduceToMean(const std::vector<pixel>& pix);
	double relativeDistance(const pixel& p1, const pixel& p2);
}

#endif /* ifndef _RGB_FUNCS_H_ */
