extends Node3D

@export var processing_fps : float = 60
@export var label_update_sec : float = 1.0

@export var subViewportIn : SubViewport
@export var spriteOut : Sprite3D
@export var stats_label : Label3D

var viewportTester : OpenCVViewportTester

var last_processing_time : float = 0.0
var frame_time : float

var last_label_update_time : float = 0.0

func _ready() -> void:
	viewportTester = OpenCVViewportTester.new()
	
	viewportTester.set_sub_viewport(subViewportIn)
	
	frame_time = 1.0 / processing_fps

func _process(delta: float) -> void:
	last_processing_time += delta

	if last_processing_time > frame_time:
		last_processing_time = frame_time - last_processing_time
		
		spriteOut.texture = ImageTexture.create_from_image(viewportTester.get_processed_viewport_texture())
		
	last_label_update_time += delta
	if last_label_update_time > label_update_sec:
		stats_label.text = viewportTester.get_averaged_timings_string().trim_prefix(" ")
		last_label_update_time = label_update_sec - last_label_update_time



func _on_viewport_size_settings_viewportsize_changed(new_size: Vector2i) -> void:
	subViewportIn.size = new_size
