extends Node3D

@export var viewport : SubViewport
@export var sprite_out : Sprite3D

@export var camera : Camera3D
@export var camera_anchor : Node3D

@export var found_label : Label3D

var hide_and_seek : OpenCVHideAndSeek

var processing_fps : float = 5.0
var frame_time : float
var last_processing_time : float = 0.0

func _ready() -> void:
	
	hide_and_seek = OpenCVHideAndSeek.new()
	hide_and_seek.set_sub_viewport(viewport)
	
	frame_time = 1.0 / processing_fps
	
	camera.global_position = camera_anchor.global_position

func _process(delta: float) -> void:
	last_processing_time += delta

	if last_processing_time > frame_time:
		last_processing_time = frame_time - last_processing_time
		
		var contoured_image = hide_and_seek.get_processed_texture()
		if (contoured_image != null):
			# Something was found!
			sprite_out.texture = ImageTexture.create_from_image(contoured_image)
			found_label.text = "Found you!"
			found_label.modulate = Color.GREEN
		else:
			sprite_out.texture = viewport.get_texture()
			found_label.text = "Searching..."
			found_label.modulate = Color.WHITE
	
func _on_area_slider(new_area : int) -> void:
	hide_and_seek.set_area_threshold(new_area)
