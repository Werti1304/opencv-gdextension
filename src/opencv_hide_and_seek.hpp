#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp//classes/sub_viewport.hpp>
#include <godot_cpp/classes/image_texture.hpp>

#include <opencv2/opencv.hpp>

using namespace godot;

class OpenCVHideAndSeek : public RefCounted {
	GDCLASS(OpenCVHideAndSeek, RefCounted)

protected:
	static void _bind_methods();

	Ref<Image> getProcessedViewportTexture();

	void setAreaThreshold(int threshold);

	void SetSubViewport(SubViewport *sub_viewport);

private:
	
public:
	
	// Timing stuff end
	OpenCVHideAndSeek() = default;
	~OpenCVHideAndSeek() override = default;
};
