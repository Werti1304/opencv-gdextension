extends Node3D

@export var viewport : SubViewport
@export var sprite_out : Sprite3D

@export var camera : Camera3D
@export var camera_anchor : Node3D

@export var found_label : Label3D

@export var room_light : OmniLight3D

var hide_and_seek : OpenCVHideAndSeek

var disable_light_switching : bool = false 

var processing_fps : float = 5.0
var frame_time : float
var last_processing_time : float = 0.0

func _ready() -> void:
	
	hide_and_seek = OpenCVHideAndSeek.new()
	hide_and_seek.set_sub_viewport(viewport)
	
	frame_time = 1.0 / processing_fps
	
	camera.global_position = camera_anchor.global_position
	
	var light_timer = get_tree().create_timer(5.0)
	light_timer.connect("timeout", _on_light_timer_timeout) 
	
	Character.current_character.gui.top_right_sprite.visible = true
	
	

func _process(delta: float) -> void:
	last_processing_time += delta

	if last_processing_time > frame_time:
		last_processing_time = frame_time - last_processing_time
		
		var contoured_image = hide_and_seek.get_processed_texture()
		if (contoured_image != null):
			# Something was found!
			var new_texture = ImageTexture.create_from_image(contoured_image)
			sprite_out.texture = new_texture
			Character.current_character.gui.top_right_sprite.texture = new_texture
			found_label.text = "Found you!"
			found_label.modulate = Color.GREEN
			var c_char_gui = Character.current_character.gui
			c_char_gui.back_to_main_menu.show_menu()
			c_char_gui.back_to_main_menu.label.text = "Player was found!"
			disable_light_switching = true
		else:
			var new_texture = viewport.get_texture()
			sprite_out.texture = new_texture
			Character.current_character.gui.top_right_sprite.texture = new_texture
			found_label.text = "Searching..."
			found_label.modulate = Color.WHITE
	
func _on_area_slider(new_area : int) -> void:
	hide_and_seek.set_area_threshold(new_area)
	
func _on_light_timer_timeout() -> void:
	if !disable_light_switching:
		var tween = get_tree().create_tween()
		if room_light.visible:
			tween.tween_property(room_light, "light_energy", 0.0, 0.5).set_trans(Tween.TRANS_CUBIC)
			tween.tween_property(room_light, "visible", false, 0.01)
		else:
			tween.tween_property(room_light, "visible", true, 0.01)
			tween.tween_property(room_light, "light_energy", 1.0, 0.3).set_trans(Tween.TRANS_CUBIC)
	
	var light_timer = get_tree().create_timer(4.0)
	light_timer.connect("timeout", _on_light_timer_timeout) 


func _on_win_area_body_entered(body: Node3D) -> void:
	if body is Character:
		var c_char_gui = Character.current_character.gui
		c_char_gui.back_to_main_menu.show_menu()
		c_char_gui.back_to_main_menu.label.text = "You win!"
		disable_light_switching = true
		
