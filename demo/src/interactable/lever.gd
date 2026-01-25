class_name Lever3D
extends StaticBody3D

@export var state : bool = false

@export_category("Meta")
@export var inactive: MeshInstance3D
@export var active: MeshInstance3D
@export var housing: MeshInstance3D
@export var pivot: Node3D

signal pressed
signal released
signal changed(new_state : bool)

func _ready() -> void:
	update_state()

func is_pressed() -> bool:
	return state

func interact() -> void:
	state = !state
	update_state()
	
	# Only emit signals when interacted with
	if state:
		emit_signal("pressed")
		emit_signal("changed", true)
	else:
		emit_signal("released")
		emit_signal("changed", false)

		
func update_state() -> void:
	if state:
		set_active()
		return
	else:
		set_inactive()

func set_active() -> void:
	active.show()
	inactive.visible = false

func set_inactive() -> void:
	inactive.visible = true
	active.hide()
