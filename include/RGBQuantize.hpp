#ifndef _RGB_QUANTIZE_H_
#define _RGB_QUANTIZE_H_

#include <vector>
#include "RGBImage.hpp"

namespace rgb
{
	std::vector<RGBPixel> extractColors_mmc(RGBImage& img, uint8_t iters);

}

#endif /* ifndef _RGB_QUANTIZE_H_ */
