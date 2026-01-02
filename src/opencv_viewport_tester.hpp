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
		double grayscale_ms = 0.0;
		double back_to_rgba_ms = 0.0;
		double mat_to_image_ms = 0.0;
		double total_ms = 0.0;
	};

	ProcessingTimings timing_accumulator;
	uint64_t timing_samples = 0;

	ProcessingTimings getAveragedTimings();
	String timingsToString(const ProcessingTimings &t) const;

public:
	String getAveragedTimingsString();

	// Timing stuff end
	OpenCVViewportTester() = default;
	~OpenCVViewportTester() override = default;

	void SetSubViewport(SubViewport* sub_viewport);
	Ref<Image> getProcessedViewportTexture();

private:
	SubViewport* viewport;
};
