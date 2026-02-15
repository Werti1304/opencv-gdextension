extends CenterContainer
class_name BackToMainMenu

@export var label : Label

func _on_button_pressed() -> void:
	get_tree().change_scene_to_packed(load("res://src/ui/main_menu.tscn"))

func show_menu() -> void:
	Character.current_character.disable_movement = true
	visible = true
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	var gui : Gui = get_parent()
	gui.top_right_sprite.scale = Vector2(1, 1)
	gui.top_right_sprite.expand_mode = TextureRect.ExpandMode.EXPAND_KEEP_SIZE
	gui.top_right_sprite.reparent(self, false)
	gui.color_rect.show()
