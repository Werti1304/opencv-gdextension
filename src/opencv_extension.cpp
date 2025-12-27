#include "opencv_extension.h"

#include <opencv2/core.hpp>

void OpenCVExtension::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &OpenCVExtension::print_type);
}

void OpenCVExtension::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: 53 %d", p_variant.get_type()));
	print_line(vformat("OpenCV version: %s", CV_VERSION));
}

