#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <sstream>
#include "HexagonSettingWindow.h"
#include "GenFlextangle.h"

std::string AskQuestionGetString(const std::string& question) {
	std::cout << question;
	std::string ans;
	std::cin >> ans;
	return ans;
}

std::string Int2String(int i) {
	std::stringstream str;
	str << i;
	return str.str();
}

enum KeyBindings {
	Key_Enter = 13, Key_ESC = 27, Key_Space = 32, Key_BS = 8
};

std::vector<std::string> ImageExtentions() {
	std::string image_exts[] = { ".jpg", ".JPG", ".jpeg", ".JPEG", ".bmp", ".BMP",
		".png", ".PNG", ".dib", ".DIB", ".pbm", ".PBM", ".pgm", ".PGM",
		".ppm", ".PPM", ".sr", ".SR", ".ras", ".RAS" };

	return std::vector<std::string>(image_exts, image_exts + sizeof(image_exts) / sizeof(std::string));
}


bool hasExtention(const std::string& file_name, const std::vector<std::string>& extentions)
{
	std::size_t pos = file_name.find_last_of(".");
	if (pos == std::string::npos)
		return false;

	std::string ext = file_name.substr(pos);

	std::vector<std::string>::const_iterator it = extentions.begin(),
		it_e = extentions.end();
	for (; it != it_e; it++) {
		if (ext == *it)
			return true;
	}
	return false;
}


bool hasImageExtention(const std::string& filename)
{
	return hasExtention(filename, ImageExtentions());
}

void printHowToUse()
{
	std::cout << "*******************************************************" << std::endl;
	std::cout << "This program generate developed figure for flextangle." << std::endl;
	std::cout << "1. Enter an image file path, then the image is displayed " << std::endl;
	std::cout << "   in new window." << std::endl;
	std::cout << "2. Surround image area by a hexagon.  Drag center of " << std::endl;
	std::cout << "   hexagon to move, and drag corner to rescale and rotate." << std::endl;
	std::cout << "3. Press enter or space key to select a next image." << std::endl;
	std::cout << "   Or press ESC or 'q' key to quit program." << std::endl;
	std::cout << "4. Repeat for 4 images." << std::endl;
	std::cout << "5. Enter image file name path to save." << std::endl;
	std::cout << "*******************************************************" << std::endl;
}

int main(int argc, char * argv[])
{
	///// Parameters ///////
	cv::Size max_window_size = cv::Size(1024, 1024);
	int output_width = 2048;
	cv::Point output_offset = cv::Point(40, 40);
	///////////////////////

	GenFlextangle gen_flex(output_width, output_offset);
	std::vector<cv::Mat> images;
	std::vector<std::vector<cv::Point2f> > src_hexagons;

	printHowToUse();
	std::cout << "Input 4 image path:" << std::endl;

	int count = 0;
	while(count < 4){
		std::string img_file = AskQuestionGetString(Int2String(count + 1) + ": ");
		cv::Mat im = cv::imread(img_file);
		if (!im.empty()) {
			HexagonSettingWindow hex_window(im, Int2String(count + 1) + ": " + img_file);
			hex_window.setMaxWindowSize(max_window_size);
			bool loop = true;
			while (loop) {
				// Get user input
				int iKey = hex_window.GetWindowKey();
				// Press ESC to close this program, any unsaved changes will be discarded
				if (iKey == Key_ESC || iKey == 'q' || iKey == 'Q') {
					std::cout << "Stop Process" << std::endl;
					return -1;
				}
				// Press Space or Enter to save marked objects on current image and proceed to the next image
				else if (iKey == Key_Space || iKey == Key_Enter) {
					std::vector<cv::Point2f> hexagon;
					hex_window.GetHexagon(hexagon);
					images.push_back(im);
					src_hexagons.push_back(hexagon);
					loop = false;
				}
				// Reload Image
				else if (iKey == Key_BS) {
					std::cout << "Try again" << std::endl;
					count--;
					loop = false;
				}
			}
			count++;
		}
		else {
			std::cout << "Fail to load image. Please try again." << std::endl;
		}
	}

	cv::Mat output_img = gen_flex.GenerateDevelopedFigure(images, src_hexagons);
	std::string save_file = AskQuestionGetString("output file name: ");
	if (!hasImageExtention(save_file))
		save_file = save_file + ".png";
	cv::imwrite(save_file, output_img);

	return 0;
}