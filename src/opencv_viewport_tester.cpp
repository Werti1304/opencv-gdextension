#include "opencv_viewport_tester.hpp"

#include "godot_cpp/core/class_db.hpp"
#include <godot_cpp/classes/viewport_texture.hpp>

#include <opencv2/opencv.hpp>

#include "opencv_extension.h"


void OpenCVViewportTester::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_processed_viewport_texture"), &OpenCVViewportTester::getProcessedViewportTexture);
	godot::ClassDB::bind_method(D_METHOD("set_sub_viewport", "sub_viewport"), &OpenCVViewportTester::SetSubViewport);
	godot::ClassDB::bind_method(D_METHOD("get_averaged_timings_string"), &OpenCVViewportTester::getAveragedTimingsString);
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

	// --- Grayscale ---
	cv::Mat gray_mat;
	cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
	auto t3 = clock::now();

	// --- Back to BGRA ---
	cv::Mat processed_mat;
	cv::cvtColor(gray_mat, processed_mat, cv::COLOR_GRAY2BGRA);
	auto t4 = clock::now();

	// --- Mat → Image ---
	Ref<Image> result = OpenCVExtension::mat_to_image(processed_mat);
	auto t5 = clock::now();

	auto ms = [](auto a, auto b) {
		return std::chrono::duration<double, std::milli>(b - a).count();
	};

	timing_accumulator.grab_ms += ms(t0, t1);
	timing_accumulator.image_to_mat_ms += ms(t1, t2);
	timing_accumulator.grayscale_ms += ms(t2, t3);
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
	avg.grayscale_ms = timing_accumulator.grayscale_ms * inv;
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
			"%7.3f ms  Grayscale\n"
			"%7.3f ms  Gray to RGBA\n"
			"%7.3f ms  Mat to Image\n"
			"%7.3f ms  Total",
			t.grab_ms,
			t.image_to_mat_ms,
			t.grayscale_ms,
			t.back_to_rgba_ms,
			t.mat_to_image_ms,
			t.total_ms);
}
