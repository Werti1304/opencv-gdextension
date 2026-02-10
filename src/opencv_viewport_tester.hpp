#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp//classes/sub_viewport.hpp>
#include <godot_cpp/classes/image_texture.hpp>

#include <opencv2/opencv.hpp>

using namespace godot;

class OpenCVViewportTester : public RefCounted {
	GDCLASS(OpenCVViewportTester, RefCounted)

protected:
	static void _bind_methods();

private:
	struct ProcessingTimings {
		double grab_ms = 0.0;
		double image_to_mat_ms = 0.0;
		double processed_ms = 0.0;
		double back_to_rgba_ms = 0.0;
		double mat_to_image_ms = 0.0;
		double total_ms = 0.0;
	};

	ProcessingTimings timing_accumulator;
	ProcessingTimings timing_accumulator_global;
	uint64_t timing_samples = 0;
	uint64_t timing_samples_global = 0;

	ProcessingTimings getAveragedTimings();
	String timingsToString(const ProcessingTimings &t) const;

public:
	String getAveragedTimingsString();
	String getAveragedGlobalTimingsString();

	// Timing stuff end
	OpenCVViewportTester() = default;
	~OpenCVViewportTester() override = default;

	void SetSubViewport(SubViewport* sub_viewport);
	Ref<Image> getProcessedViewportTexture();
		
	struct ProcessingArgs
	{
		bool grayScale = false;

		bool resize = false;
		cv::InterpolationFlags resizeInterpolationFlags = cv::INTER_LINEAR;
		cv::Size resizeSize = cv::Size(640, 480);

		bool gaussianBlur = false;
		cv::Size gaussianBlurKernelSize = cv::Size(5, 5);

		bool cannyEdge = false;
		int cannyThreshold1 = 200;
		int cannyThreshold2 = 400;
		 
		bool dilate = false;
		int dilate_size = 5;

		bool findContours = false;
		cv::RetrievalModes findContoursRetrievalMode = cv::RETR_EXTERNAL;
		cv::ContourApproximationModes findContoursApproxMode = cv::CHAIN_APPROX_SIMPLE;
	};
	    
    void setGrayScale(bool grayScale);

    void setResize(bool enable,
                   int interpolation,
                   int width,
                   int height);

    void setGaussianBlur(bool enable,
                         int kernel_width,
                         int kernel_height);

    void setCannyEdge(bool enable,
                      int threshold1,
                      int threshold2);

    void setDilate(bool enable,
                   int size);

    void setFindContours(bool enable,
                         int retrievalMode,
                         int approxMode);

private:
	SubViewport* viewport;
};
