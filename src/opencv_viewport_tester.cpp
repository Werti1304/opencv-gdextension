#include "opencv_viewport_tester.hpp"

#include "godot_cpp/core/class_db.hpp"
#include <godot_cpp/classes/viewport_texture.hpp>

#include <opencv2/opencv.hpp>

#include "opencv_extension.h"

OpenCVViewportTester::ProcessingArgs processing_args;

void OpenCVViewportTester::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_processed_viewport_texture"), &OpenCVViewportTester::getProcessedViewportTexture);
	godot::ClassDB::bind_method(D_METHOD("set_sub_viewport", "sub_viewport"), &OpenCVViewportTester::SetSubViewport);
	godot::ClassDB::bind_method(D_METHOD("get_averaged_timings_string"), &OpenCVViewportTester::getAveragedTimingsString);

	// ProcessingArgs Setters

	godot::ClassDB::bind_method(D_METHOD(
		"set_gray_scale", "grayScale"), 
		&OpenCVViewportTester::setGrayScale
	);

	godot::ClassDB::bind_method(D_METHOD(
		"set_resize", "enable", "interpolation", "width", "height"
	), &OpenCVViewportTester::setResize,
		DEFVAL(int(cv::INTER_LINEAR)), // interpolation
		DEFVAL(640),                   // width
		DEFVAL(480)                    // height
	);

	godot::ClassDB::bind_method(D_METHOD(
		"set_gaussian_blur", "enable", "kernel_width", "kernel_height"
	), &OpenCVViewportTester::setGaussianBlur,
		DEFVAL(5), // kernel_width
		DEFVAL(5)  // kernel_height
	);

	godot::ClassDB::bind_method(D_METHOD(
		"set_canny_edge", "enable", "threshold1", "threshold2"
	), &OpenCVViewportTester::setCannyEdge,
		DEFVAL(80), // threshold1
		DEFVAL(255)  // threshold2
	);

	godot::ClassDB::bind_method(D_METHOD(
		"set_dilate", "enable", "size"
	), &OpenCVViewportTester::setDilate,
		DEFVAL(10) // size
	);

	godot::ClassDB::bind_method(D_METHOD(
		"set_find_contours", "enable", "retrieval_mode", "approx_mode"
	), &OpenCVViewportTester::setFindContours,
		DEFVAL(int(cv::RETR_EXTERNAL)),       // retrieval_mode
		DEFVAL(int(cv::CHAIN_APPROX_SIMPLE))  // approx_mode
	);
}

void OpenCVViewportTester::SetSubViewport(SubViewport* sub_viewport) {
	viewport = sub_viewport;
}

Ref<Image> OpenCVViewportTester::getProcessedViewportTexture() {
	using clock = std::chrono::steady_clock;

	const auto t_start = clock::now();

	// --- Grab texture ---
	auto t0 = clock::now();
	Ref<ViewportTexture> vp_texture = viewport->get_texture();
	Ref<Image> image = vp_texture->get_image();
	auto t1 = clock::now();

	// --- Image → Mat ---
	cv::Mat mat = OpenCVExtension::image_to_mat(image);
	auto t2 = clock::now();

	// TODO: Do the processing

    // Grayscale
    if (processing_args.grayScale) {
        cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
    } else {
        mat = mat; // keep original channels if not gray
    }

    // Resize
    if (processing_args.resize) {
        cv::resize(mat, mat,
                   processing_args.resizeSize,
                   0, 0,
                   processing_args.resizeInterpolationFlags);
    }

    // Gaussian Blur
    if (processing_args.gaussianBlur) {
        cv::GaussianBlur(mat, mat,
                         processing_args.gaussianBlurKernelSize,
                         0);
    }

	// Dilate
    if (processing_args.dilate) {
        int ksize = processing_args.dilate_size;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
		cv::dilate(mat, mat, kernel);
    }

    // Canny Edge
    cv::Mat edge_mat;
    if (processing_args.cannyEdge) {
        cv::Canny(mat, edge_mat,
                  processing_args.cannyThreshold1,
                  processing_args.cannyThreshold2);
    }

    // Find Contours
    if (processing_args.findContours) {
        cv::Mat binary_for_contours;
        if (processing_args.cannyEdge) {
            binary_for_contours = edge_mat;
        } else if(processing_args.grayScale) {
            // Use default threshold if Canny wasn't applied
            cv::threshold(mat, binary_for_contours, 128, 255, cv::THRESH_BINARY);
        }
		else {
			cv::Mat gray_mat = mat.clone();
			cv::cvtColor(gray_mat, gray_mat, cv::COLOR_BGR2GRAY);
			cv::threshold(gray_mat, binary_for_contours, 128, 255, cv::THRESH_BINARY);
		}

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(binary_for_contours, contours, hierarchy,
                         processing_args.findContoursRetrievalMode,
                         processing_args.findContoursApproxMode);

        // Optional: draw contours on the image
        cv::drawContours(mat, contours, -1, cv::Scalar(255, 0, 0), 2); // blue lines
    }
	else if (processing_args.cannyEdge) {
		mat = edge_mat;
	}

	auto t3 = clock::now();

	// --- Back to BGRA ---
    if (mat.channels() == 1) {
        cv::cvtColor(mat, mat, cv::COLOR_GRAY2BGRA);
    } else if (mat.channels() == 3) {
        cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
    } else {
        mat = mat.clone(); // already 4 channels
    }
	auto t4 = clock::now();

	// --- Mat → Image ---
	Ref<Image> result = OpenCVExtension::mat_to_image(mat);
	auto t5 = clock::now();

	auto ms = [](auto a, auto b) {
		return std::chrono::duration<double, std::milli>(b - a).count();
	};

	timing_accumulator.grab_ms += ms(t0, t1);
	timing_accumulator.image_to_mat_ms += ms(t1, t2);
	timing_accumulator.processed_ms += ms(t2, t3);
	timing_accumulator.back_to_rgba_ms += ms(t3, t4);
	timing_accumulator.mat_to_image_ms += ms(t4, t5);
	timing_accumulator.total_ms += ms(t_start, t5);

	timing_samples++;

	return result;
}

OpenCVViewportTester::ProcessingTimings OpenCVViewportTester::getAveragedTimings() {
	ProcessingTimings avg;

	if (timing_samples == 0) {
		return avg;
	}

	const double inv = 1.0 / timing_samples;

	avg.grab_ms = timing_accumulator.grab_ms * inv;
	avg.image_to_mat_ms = timing_accumulator.image_to_mat_ms * inv;
	avg.processed_ms = timing_accumulator.processed_ms * inv;
	avg.back_to_rgba_ms = timing_accumulator.back_to_rgba_ms * inv;
	avg.mat_to_image_ms = timing_accumulator.mat_to_image_ms * inv;
	avg.total_ms = timing_accumulator.total_ms * inv;

	timing_accumulator = {};
	timing_samples = 0;

	return avg;
}

String OpenCVViewportTester::getAveragedTimingsString() {
	ProcessingTimings t = getAveragedTimings();

	if (t.total_ms <= 0.0) {
		return "No timing samples collected.";
	}

	return timingsToString(t);
}

String OpenCVViewportTester::timingsToString(const ProcessingTimings &t) const {
	return vformat(
			"%7.3f ms  Grab from viewport/gpu\n"
			"%7.3f ms  Image to Mat\n"
			"%7.3f ms  Processing\n"
			"%7.3f ms  Processed -> Godot Color Space\n"
			"%7.3f ms  Mat to Image\n\n"
			"%7.3f ms  Total",
			t.grab_ms,
			t.image_to_mat_ms,
			t.processed_ms,
			t.back_to_rgba_ms,
			t.mat_to_image_ms,
			t.total_ms);
}


void OpenCVViewportTester::setGrayScale(bool grayScale) {
    processing_args.grayScale = grayScale;
}

void OpenCVViewportTester::setResize(bool enable, int interpolation, int width, int height) {
    processing_args.resize = enable;
    processing_args.resizeInterpolationFlags = static_cast<cv::InterpolationFlags>(interpolation);
    processing_args.resizeSize = cv::Size(width, height);
}

void OpenCVViewportTester::setGaussianBlur(bool enable, int kernel_width, int kernel_height) {
    processing_args.gaussianBlur = enable;
    processing_args.gaussianBlurKernelSize = cv::Size(kernel_width, kernel_height);
}

void OpenCVViewportTester::setCannyEdge(bool enable, int threshold1, int threshold2) {
    processing_args.cannyEdge = enable;
    processing_args.cannyThreshold1 = threshold1;
    processing_args.cannyThreshold2 = threshold2;
}

void OpenCVViewportTester::setDilate(bool enable, int size) {
    processing_args.dilate = enable;
    processing_args.dilate_size = size;
}

void OpenCVViewportTester::setFindContours(bool enable, int retrievalMode, int approxMode) {
    processing_args.findContours = enable;
    processing_args.findContoursRetrievalMode = static_cast<cv::RetrievalModes>(retrievalMode);
    processing_args.findContoursApproxMode = static_cast<cv::ContourApproximationModes>(approxMode);
}