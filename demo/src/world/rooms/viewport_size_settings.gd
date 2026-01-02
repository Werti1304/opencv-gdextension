extends Node3D

@export var viewport_size_label : Label3D

signal viewportsize_changed(new_size : Vector2i)

var viewport_sizes := [
	Vector2i(320, 180),    # Tiny / very low-res
	Vector2i(480, 270),    # Low SD
	Vector2i(640, 360),    # SD
	Vector2i(854, 480),    # SD+
	Vector2i(960, 540),    # qHD
	Vector2i(1280, 720),   # HD
	Vector2i(1366, 768),   # HD+
	Vector2i(1600, 900),   # HD wide
	Vector2i(1920, 1080),  # Full HD
	Vector2i(2560, 1440),  # QHD
	Vector2i(3840, 2160)   # 4K UHD
]

# Current index (start at Full HD)
var current_index : int = 8
var viewportsize : Vector2i = viewport_sizes[current_index]

func _on_button_pressed_smaller() -> void:
	if current_index > 0:
		current_index -= 1
		viewportsize = viewport_sizes[current_index]
		print("Viewport size changed to: ", viewportsize)
		viewport_size_label.text = "%d x %d" % [viewportsize.x, viewportsize.y]
		emit_signal("viewportsize_changed", viewportsize)

func _on_button_pressed_bigger() -> void:
	if current_index < viewport_sizes.size() - 1:
		current_index += 1
		viewportsize = viewport_sizes[current_index]
		print("Viewport size changed to: ", viewportsize)
		viewport_size_label.text = "%d x %d" % [viewportsize.x, viewportsize.y]
		emit_signal("viewportsize_changed", viewportsize)
