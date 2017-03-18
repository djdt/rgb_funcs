#include <iostream>
#include <string>

#include "RGBImage.hpp"
#include "RGBQuantize.hpp"

#include <gnuplot-iostream.h>
/* #include "RGBImageGPext.hpp" */

void PrintUsage(const std::string& name)
{
	std::cerr << "Usage: " << name << " [OPTION]... FILE\n"
		<< "Where FILE is a png or jpeg image.\n"
		<< "Options:\n"
		<< "\t-a, --algorithm ALGORITHM\tthe quantisation method\n"
		<< "\t\t\t\t\t(mmc, hist, kmeans)\n"
		<< "\t-h, --help\t\t\tprints this usage message.\n"
		<< "\t-o, --output FILENAME\n"
		<< "\t-t, --terminal TERMINAL\t\tthe gnuplot terminal.\n"
		<< std::endl;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		PrintUsage(argv[0]);
		return 1;
	}

	// Defaults
	std::string algorithm = "mmc";
	std::string output = "gnuplot.png";
	std::string terminal = "pngcairo";

	// Arguments
	for (int i=1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			PrintUsage(argv[0]);
			return 0;
		} else if (arg == "-a" || arg == "--algorithm") {
			algorithm = std::string(argv[i+1]);
		} else if (arg == "-o" || arg == "--output") {
			output = std::string(argv[i+1]);
		} else if (arg == "-t" || arg == "--terminal") {
			terminal = std::string(argv[i+1]);
		}
	}

	RGBImage img;
	std::string img_file(argv[argc - 1]);
	std::string file_ext = img_file.substr(
			img_file.find_last_of('.') + 1);

	if (file_ext == "png") {
		if(!img.LoadPngFile(img_file)) return 1;
	} else if (file_ext == "jpg" || file_ext == "jpeg") {
		if(!img.LoadJpegFile(img_file)) return 1;
	} else {
		std::cerr << "Invalied file extenison!" << std::endl;
		PrintUsage(argv[0]);
		return 1;
	}

	std::cout << img << std::endl;

	std::vector<RGBPixel> colors;
	if (algorithm == "mmc") {
		colors = rgbq::ExtractColors_MedianCut(img, 8, 4);
	} else if (algorithm == "hist") {
		colors = rgbq::ExtractColors_Histogram(img, 8, 4);
	} else {
		std::cerr << "Invalid algorithm!" << std::endl;
		PrintUsage(argv[0]);
		return 1;
	}

	// Plot
	Gnuplot gp;
	gp << "set terminal " << terminal << " size 1000, 1000\n";
	gp << "set output '" << output << "'\n";
	gp << "set xlabel 'Red'\nset ylabel 'Green'\nset zlabel 'Blue'\n";
	gp << "set xrange [0:255]\nset yrange [0:255]\nset zrange [0:255]\n";
	gp << "set xyplane 0\n";
	gp << "rgb(r,g,b) = int(r)*65536 + int(g)*256 + int(b)\n";
	gp << "splot '-' binary" << gp.binFmt1d(img.pixels(), "record")
		 << "using 1:2:3:(rgb($1,$2,$3)) with points pt 5 lc rgb var notitle"
	   << ", '-' binary" << gp.binFmt1d(colors, "record")
		 << "using 1:2:3:(rgb($1,$2,$3)) with points pt 7 ps 8 lc rgb var notitle\n";

	gp.sendBinary1d(img.pixels());
	gp.sendBinary1d(colors);

	return 0;
}
