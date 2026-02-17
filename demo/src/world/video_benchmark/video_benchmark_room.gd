extends Node3D


@export var label_update_sec : float = 1.0

@export var subViewportIn : SubViewport
@export var videoStreamPlayer : VideoStreamPlayer
@export var spriteOut : Sprite3D
@export var stats_label : Label3D
@export var stats_label_2 : Label3D
@export var global_stats_label : Label3D

var viewportTester : OpenCVViewportTester

var processing_fps : float = 60
var last_processing_time : float = 0.0
var frame_time : float

var last_label_update_time : float = 0.0

func _ready() -> void:
	viewportTester = OpenCVViewportTester.new()
	
	viewportTester.set_sub_viewport(subViewportIn)
	
	frame_time = 1.0 / processing_fps

func _process(delta: float) -> void:
	if(!videoStreamPlayer.is_playing()):
		return
		
	last_processing_time += delta

	if last_processing_time > frame_time:
		last_processing_time = frame_time - last_processing_time
		
		spriteOut.texture = ImageTexture.create_from_image(viewportTester.get_processed_viewport_texture())
		
	last_label_update_time += delta
	if last_label_update_time > label_update_sec:
		#stats_label.text = viewportTester.get_averaged_timings_string().trim_prefix(" ")
		#stats_label_2.text = stats_label.text
		Character.current_character.gui.top_right_label.text = "OpenCV Video Benchmark:\n" + viewportTester.get_averaged_timings_string().trim_prefix(" ")
		global_stats_label.text = "Global Averaged Stats:\n" + viewportTester.get_global_timings_string()
		last_label_update_time = label_update_sec - last_label_update_time

func _on_viewport_size_settings_viewportsize_changed(new_size: Vector2i) -> void:
	subViewportIn.size = new_size
	videoStreamPlayer.size = new_size
	
func _on_viewport_fps_setting_fps_changed(new_fps: int) -> void:
	processing_fps = new_fps
	frame_time = 1.0 / processing_fps

func _on_grayscale_lever_change(change: bool):
	viewportTester.set_gray_scale(change)

func _on_resize_lever_change(change: bool):
	viewportTester.set_resize(change)

func _on_gaussian_blur_lever_change(change: bool):
	viewportTester.set_gaussian_blur(change)

func _on_canny_edge_lever_change(change: bool):
	viewportTester.set_canny_edge(change)

func _on_dilate_lever_change(change: bool):
	viewportTester.set_dilate(change)

func _on_find_contours_lever_change(change: bool):
	viewportTester.set_find_contours(change)

func _on_start_button_pressed() -> void:
	videoStreamPlayer.play()

func _on_video_stream_player_finished() -> void:
	viewportTester.save_global_timings_to_file()
