extends Node3D

@export var fps_label : Label3D

signal fps_changed(new_fps : int)

var fps_intervals := [
	5,
	10,
	20,
	30,
	40,
	50,
	60
]

# Current index (start at Full HD)
var current_index : int = 6
var fps_count : int = fps_intervals[current_index]

func _on_button_pressed_smaller() -> void:
	if current_index > 0:
		current_index -= 1
		fps_count = fps_intervals[current_index]
		print("Viewport size changed to: ", fps_count)
		fps_label.text = "FPS:\n%d" % [fps_count]
		emit_signal("fps_changed", fps_count)

func _on_button_pressed_bigger() -> void:
	if current_index < fps_intervals.size() - 1:
		current_index += 1
		fps_count = fps_intervals[current_index]
		print("FPS changed to: ", fps_count)
		fps_label.text = "FPS:\n%d" % [fps_count]
		emit_signal("fps_changed", fps_count)
