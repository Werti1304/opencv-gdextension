#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class OpenCVExtension : public RefCounted {
	GDCLASS(OpenCVExtension, RefCounted)

protected:
	static void _bind_methods();

public:
	OpenCVExtension() = default;
	~OpenCVExtension() override = default;

	void print_type(const Variant &p_variant) const;
};
