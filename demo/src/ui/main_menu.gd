extends Control

@export var video_benchmark_scene : PackedScene
@export var hide_and_seek_scene : PackedScene
@export var webcam_scene : PackedScene


func _on_video_benchmark_button_pressed() -> void:
	get_tree().change_scene_to_packed(video_benchmark_scene)


func _on_hide_and_seek_button_pressed() -> void:
	get_tree().change_scene_to_packed(hide_and_seek_scene)


func _on_webcam_obj_det_button_pressed() -> void:
	get_tree().change_scene_to_packed(webcam_scene)
