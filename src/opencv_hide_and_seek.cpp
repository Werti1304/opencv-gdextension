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

    cv::Mat original = mat.clone();

    cv::cvtColor(mat, mat, cv::COLOR_BGR2HSV);

    // Red ranges
    cv::Scalar lower_red_1(0, 120, 70);
    cv::Scalar upper_red_1(10, 255, 255);
    cv::Scalar lower_red_2(170, 120, 70);
    cv::Scalar upper_red_2(180, 255, 255);

    cv::Mat mask1, mask2, mask;
    cv::inRange(mat, lower_red_1, upper_red_1, mask1);
    cv::inRange(mat, lower_red_2, upper_red_2, mask2);
    mask = mask1 | mask2;

    cv::erode(mask, mask, cv::Mat(), cv::Point(-1,-1), 1);
    cv::dilate(mask, mask, cv::Mat(), cv::Point(-1,-1), 2);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty())
        return nullptr;

    // Find largest contour
    double max_area = 0;
    std::vector<cv::Point> largest_contour;

    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 10 && area > max_area) {
            max_area = area;
            largest_contour = contour;
        }
    }

    if (largest_contour.empty())
        return nullptr;

    cv::Rect box = cv::boundingRect(largest_contour);

    // ✅ Add small padding
    int padding = 20;
    box.x -= padding;
    box.y -= padding;
    box.width += padding * 2;
    box.height += padding * 2;

    // Clamp inside image
    box.x = std::max(0, box.x);
    box.y = std::max(0, box.y);
    box.width = std::min(original.cols - box.x, box.width);
    box.height = std::min(original.rows - box.y, box.height);

    // ✅ Maintain aspect ratio
    float target_aspect = (float)original.cols / (float)original.rows;
    float box_aspect = (float)box.width / (float)box.height;

    if (box_aspect > target_aspect) {
        // Too wide → increase height
        int new_height = static_cast<int>(box.width / target_aspect);
        int diff = new_height - box.height;
        box.y -= diff / 2;
        box.height = new_height;
    } else {
        // Too tall → increase width
        int new_width = static_cast<int>(box.height * target_aspect);
        int diff = new_width - box.width;
        box.x -= diff / 2;
        box.width = new_width;
    }

    // Clamp again after aspect correction
    box.x = std::max(0, box.x);
    box.y = std::max(0, box.y);
    box.width = std::min(original.cols - box.x, box.width);
    box.height = std::min(original.rows - box.y, box.height);

    // Crop
    cv::Mat cropped = original(box);

    // Resize to full viewport (no stretching now)
    cv::Mat zoomed;
    cv::resize(cropped, zoomed, original.size(), 0, 0, cv::INTER_CUBIC);

    Ref<Image> result = OpenCVExtension::mat_to_image(zoomed);
    return result;
}



void OpenCVHideAndSeek::setAreaThreshold(int threshold) {
	area_threshold = threshold;
}

void OpenCVHideAndSeek::SetSubViewport(SubViewport* sub_viewport) {
	viewport = sub_viewport;
}

