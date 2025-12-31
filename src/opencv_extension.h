#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/image.hpp>

#include <opencv2/opencv.hpp>

using namespace godot;

class OpenCVExtension : public RefCounted {
	GDCLASS(OpenCVExtension, RefCounted)

protected:
	static void _bind_methods();

public:
	OpenCVExtension() = default;
	~OpenCVExtension() override = default;

	void print_type(const Variant &p_variant) const;
	void testOpenCV();
	cv::Mat createTestImage() const;
	Ref<Image> mat_to_image(const cv::Mat &mat) const;
	Ref<Image> test_mat_to_image() const;
};
