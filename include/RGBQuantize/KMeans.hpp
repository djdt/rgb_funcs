#ifndef _RGB_QUANTIZE_KMEANS_H_
#define _RGB_QUANTIZE_KMEANS_H_

#include <array>
#include <vector>

namespace rgb
{
	typedef std::array<uint8_t, 3> pixel;

	std::vector<pixel> quantizeKMeans(
			const std::vector<pixel>& pixels, uint32_t k, uint32_t every_n_pixel);
};

#endif /* ifndef _RGB_QUANTIZE_KMEANS_H_ */
