#include "RGBImage.hpp"

#include <iostream>
#include <png.h>
#include <jpeglib.h>

RGBImage::RGBImage()
{
	_file_path = "";

	_width = 0;
	_height = 0;
	_bit_depth = 0;
	_channels = 0;
}

RGBImage::~RGBImage()
{

}

std::ostream& operator<<(
		std::ostream& os, const RGBImage& img)
{
	os << "--RGBImage--\n";
	os << "\tPath: " << img._file_path << "\n";
	os << "\tSize: " << img._width << "x" << img._height << "\n";
	os << "\tChannels: " << img._channels << "\n";
	os << "\tBit depth: " << img._bit_depth << "\n";
	os << "\tNo. pixles: " << img._pixels.size() << std::endl;
	return os;
}

void png_read_iostream(png_struct* png_ptr, png_byte* data, png_size_t len)
{
	png_voidp a = png_get_io_ptr(png_ptr);
	reinterpret_cast<std::istream*>(a)->read(reinterpret_cast<char*>(data), len);
}

bool RGBImage::loadPngFile(const std::string& file)
{
	png_byte png_sig[8];
	png_byte** row_ptrs = nullptr;
	png_byte* img_data = nullptr;

	std::ifstream ifs(file, std::ios::in | std::ios::binary);
	if (!ifs.good()) {
		std::cerr << "RGBImage: source is not good!" << std::endl;
		return false;
	}

	// Check source is actually png
	ifs.read(reinterpret_cast<char*>(png_sig), 8);
	if (png_sig_cmp(png_sig, 0, 8)) {
		std::cerr << "RGBImage: png signature invalid!" << std::endl;
		return false;
	}

	// Create the required structs
	png_struct* png_ptr = png_create_read_struct(
			PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr) {
		std::cerr << "RGBImage: failed to create png struct!" << std::endl;
		return false;
	}

	png_info* info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, 0, 0);
		std::cerr << "RGBImage: failed to create png info!" << std::endl;
		return false;
	}

	// Set jump for error on read
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		if (row_ptrs) delete[] row_ptrs;
		if (img_data) delete[] img_data;
		_pixels.clear();
		std::cerr << "RGBImage: read error!" << std::endl;
		return false;
	}

	// Set the read function
	png_set_read_fn(png_ptr, &ifs, png_read_iostream);

	// Read signature
	png_set_sig_bytes(png_ptr, 8);

	// Read info
	png_read_info(png_ptr, info_ptr);

	uint32_t bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	uint32_t channels = png_get_channels(png_ptr, info_ptr);
	uint32_t color_type = png_get_color_type(png_ptr, info_ptr);

	/* Conversions to 8bit - RGB(A) */

	// Convert to rgb
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	} else if (color_type == PNG_COLOR_TYPE_GRAY ||
			       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}

	// Ensure bitdepth of 8
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}

	// Set transparency as alpha
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	}

	// Strip alpha
	if (channels > 3) {
		png_set_strip_alpha(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);

	// Assign member variables
	_file_path = file;
	_width = png_get_image_width(png_ptr, info_ptr);
	_height = png_get_image_height(png_ptr, info_ptr);
	_bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	_channels = png_get_channels(png_ptr, info_ptr);

	uint32_t row_bytes = _width * _channels * (_bit_depth / 8);

	row_ptrs = new png_byte*[_height];
	img_data = new png_byte[_height * row_bytes];

	for (uint32_t i = 0; i < _height; ++i) {
		row_ptrs[i] = img_data + (i * row_bytes);
	}
	png_read_image(png_ptr, row_ptrs);

	// Add to new data structure
	_pixels.clear();
	_pixels.reserve(_width * _height);
	for (uint32_t i = 0; i < _height * _width; ++i) {
		/* _pixels.push_back( */
		/* 		RGBPixel(img_data[(i*3)], img_data[(i*3)+1], img_data[(i*3)+2])); */
		_pixels.push_back({{img_data[i*3], img_data[(i*3)+1], img_data[(i*3)+2]}});
	}

	delete[] row_ptrs;
	delete[] img_data;
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	return true;
}

bool RGBImage::loadJpegFile(const std::string& file)
{
	FILE* fp = std::fopen(file.c_str(), "rb");

	if (fp == nullptr) {
		std::cerr << "Cant open file '" << file << "'." << std::endl;
		return false;
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);

	jpeg_read_header(&cinfo, true);

	// Params go here

	jpeg_start_decompress(&cinfo);

	if (cinfo.output_components != 3) {
		std::cerr << "Unsupported format! " << cinfo.output_components << std::endl;
		jpeg_destroy_decompress(&cinfo);
		std::fclose(fp);
		return false;
	}

	_width = cinfo.output_width;
	_height = cinfo.output_height;
	_channels = cinfo.output_components;
	_bit_depth = 8;

	_pixels.clear();
	_pixels.reserve(_width * _height);

	uint32_t row_stride = _width * _channels;
	uint8_t* row_data = new uint8_t[row_stride];

	while (cinfo.output_scanline < _height) {
		jpeg_read_scanlines(&cinfo, &row_data, 1);
		for (uint32_t i = 0; i < _width; ++i) {
			_pixels.push_back(
					{{row_data[i*3], row_data[(i*3)+1], row_data[(i*3)+2]}});
		}
	}
	delete[] row_data;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);
	return true;
}
