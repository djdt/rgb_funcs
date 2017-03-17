#ifndef _RGB_IMAGE_GP_EXT_H_
#define _RGB_IMAGE_GP_EXT_H_

#include "RGBImage.hpp"
#include <gnuplot-iostream.h>

namespace gnuplotio {

	template <>
	struct BinfmtSender<RGBPixel> {
		static void send(std::ostream& stream) {
			BinfmtSender<uint8_t>::send(stream);
			BinfmtSender<uint8_t>::send(stream);
			BinfmtSender<uint8_t>::send(stream);
			BinfmtSender<uint32_t>::send(stream);
		}
	};

	template <>
	struct BinarySender<RGBPixel> {
		static void send(std::ostream& stream, const RGBPixel& p) {
			BinarySender<uint8_t>::send(stream, p.r);
			BinarySender<uint8_t>::send(stream, p.g);
			BinarySender<uint8_t>::send(stream, p.b);
			BinarySender<uint32_t>::send(stream,
					p.r * 65536 + p.g * 256 + p.b);
		}
	};
}


#endif /* ifndef _RGB_IMAGE_GP_EXT_H_ */
