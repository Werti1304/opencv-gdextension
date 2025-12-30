#include "opencv_extension.h"

#include <opencv2/opencv.hpp>

void OpenCVExtension::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &OpenCVExtension::print_type);
	godot::ClassDB::bind_method(D_METHOD("test_opencv"), &OpenCVExtension::testOpenCV);
}

void OpenCVExtension::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: 53 %d", p_variant.get_type()));
	print_line(vformat("OpenCV version: %s", CV_VERSION));
}

void OpenCVExtension::testOpenCV() {
	// Create a 400x400 black image
	cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

	// Draw a blue circle in the center
	cv::circle(image, cv::Point(200, 200), 100, cv::Scalar(255, 0, 0), -1);

	// Show the image in a window
	cv::imshow("Test OpenCV", image);

	std::cout << "Press any key to close the window..." << std::endl;
	cv::waitKey(0); // Wait for a key press
	cv::destroyAllWindows();
}
