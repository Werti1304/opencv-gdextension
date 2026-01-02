#include "opencv_extension.h"

#include <opencv2/opencv.hpp>

void OpenCVExtension::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &OpenCVExtension::print_type);
	godot::ClassDB::bind_method(D_METHOD("test_opencv"), &OpenCVExtension::testOpenCV);
	godot::ClassDB::bind_method(D_METHOD("test_mat_to_image"), &OpenCVExtension::test_mat_to_image);
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

cv::Mat OpenCVExtension::createTestImage()  const{
	// Create a 400x400 black image
	cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

	// Draw a blue circle in the center (BGR)
	cv::circle(
			image,
			cv::Point(200, 200),
			100,
			cv::Scalar(255, 0, 0),
			-1);

	return image;
}


cv::Mat OpenCVExtension::image_to_mat(const Ref<Image> &image) {
	ERR_FAIL_COND_V(image.is_null(), cv::Mat());

	Ref<Image> img = image;

	// Make sure image data is readable
	if (img->is_compressed()) {
		img = img->duplicate();
		img->decompress();
	}

	const int width = img->get_width();
	const int height = img->get_height();
	const Image::Format format = img->get_format();
	const PackedByteArray data = img->get_data();

	ERR_FAIL_COND_V(data.is_empty(), cv::Mat());

	cv::Mat mat;

	switch (format) {
		case Image::FORMAT_L8: {
			mat = cv::Mat(height, width, CV_8UC1, (void *)data.ptr());
		} break;

		case Image::FORMAT_RGB8: {
			cv::Mat rgb(height, width, CV_8UC3, (void *)data.ptr());
			cv::cvtColor(rgb, mat, cv::COLOR_RGB2BGR);
		} break;

		case Image::FORMAT_RGBA8: {
			cv::Mat rgba(height, width, CV_8UC4, (void *)data.ptr());
			cv::cvtColor(rgba, mat, cv::COLOR_RGBA2BGRA);
		} break;

		default:
			ERR_FAIL_V_MSG(cv::Mat(), "Unsupported Image format");
	}

	// Ensure data ownership & continuity
	return mat.clone();
}

Ref<Image> OpenCVExtension::mat_to_image(const cv::Mat &mat) {
	ERR_FAIL_COND_V(mat.empty(), nullptr);
	ERR_FAIL_COND_V(!mat.isContinuous(), nullptr);

	cv::Mat converted;
	Image::Format format;

	if (mat.type() == CV_8UC3) {
		// BGR → RGB
		cv::cvtColor(mat, converted, cv::COLOR_BGR2RGB);
		format = Image::FORMAT_RGB8;
	} else if (mat.type() == CV_8UC4) {
		// BGRA → RGBA
		cv::cvtColor(mat, converted, cv::COLOR_BGRA2RGBA);
		format = Image::FORMAT_RGBA8;
	} else if (mat.type() == CV_8UC1) {
		// Grayscale
		converted = mat;
		format = Image::FORMAT_L8;
	} else {
		ERR_FAIL_V_MSG(nullptr, "Unsupported cv::Mat format");
	}

	if (!converted.isContinuous()) {
		converted = converted.clone();
		ERR_FAIL_COND_V(!converted.isContinuous(), nullptr);
	}

	int width = converted.cols;
	int height = converted.rows;
	size_t data_size = converted.total() * converted.elemSize();

	PackedByteArray data;
	data.resize(data_size);

	memcpy(data.ptrw(), converted.data, data_size);

	Ref<Image> image = memnew(Image);
	image = Image::create_from_data(
			width,
			height,
			false,
			format,
			data);

	ERR_FAIL_COND_V(image.is_null(), nullptr);
	ERR_FAIL_COND_V(!image.is_valid(), nullptr);
	ERR_FAIL_COND_V(image->is_empty(), nullptr);

	return image;
}

Ref<Image> OpenCVExtension::test_mat_to_image() const {
	cv::Mat mat = createTestImage();
	return mat_to_image(mat);
}
