#include "opencv_hide_and_seek.hpp"

#include "godot_cpp/core/class_db.hpp"
#include <godot_cpp/classes/viewport_texture.hpp>

#include <opencv2/opencv.hpp>

#include "opencv_extension.h"

SubViewport* viewport;
int area_threshold = 300;

void OpenCVHideAndSeek::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("set_area_threshold"), &OpenCVHideAndSeek::setAreaThreshold);
	godot::ClassDB::bind_method(D_METHOD("set_sub_viewport", "sub_viewport"), &OpenCVHideAndSeek::SetSubViewport);
	godot::ClassDB::bind_method(D_METHOD("get_processed_texture"), &OpenCVHideAndSeek::getProcessedViewportTexture);
}

Ref<Image> OpenCVHideAndSeek::getProcessedViewportTexture() {

	Ref<ViewportTexture> vp_texture = viewport->get_texture();
	Ref<Image> image = vp_texture->get_image();
	cv::Mat mat = OpenCVExtension::image_to_mat(image);

	cv::cvtColor(mat, mat, cv::COLOR_BGR2HSV);

	// We need 2 range since red goes from around hue 0-10 and 170-180
	cv::Scalar lower_red_1(0, 120, 70);
	cv::Scalar upper_red_1(10, 255, 255);
	cv::Scalar lower_red_2(170, 120, 70);
	cv::Scalar upper_red_2(180, 255, 255);

	// Threshold HSV image
	cv::Mat mask1;
	cv::Mat mask2;
	cv::Mat mask;
	cv::inRange(mat, lower_red_1, upper_red_1, mask1);
	cv::inRange(mat, lower_red_2, upper_red_2, mask2);
	mask = mask1 | mask2;

	 // Optional: reduce noise
	cv::erode(mask, mask, cv::Mat(), cv::Point(-1,-1), 1);
	cv::dilate(mask, mask, cv::Mat(), cv::Point(-1,-1), 2);

	// Find contours
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if(contours.empty()) {
		return nullptr;
	}

	for (const auto& contour : contours) {
		double area = cv::contourArea(contour);
		if (area > 500) { // Filter small noise
				cv::drawContours(mat, std::vector<std::vector<cv::Point>>{contour},
													-1, cv::Scalar(0, 255, 0), 5);
		}
	}

	cv::cvtColor(mat, mat, cv::COLOR_HSV2BGR);
	Ref<Image> result = OpenCVExtension::mat_to_image(mat);
	
	return result;
}

void OpenCVHideAndSeek::setAreaThreshold(int threshold) {
	area_threshold = threshold;
}

void OpenCVHideAndSeek::SetSubViewport(SubViewport* sub_viewport) {
	viewport = sub_viewport;
}

