class_name Button3D
extends StaticBody3D

@export var increment_amount: int = 1

@export_category("Meta")
@export var inactive: MeshInstance3D
@export var active: MeshInstance3D
@export var housing: MeshInstance3D
@export var pivot: Node3D

signal pressed
signal released

# Godot is single-threaded for gameplay logic,
# so we don't need an explicit lock like in C#
var _pressed_count: int = 0

func _ready() -> void:
	pass


func interact() -> void:
	_pressed_count += 1
	inactive.visible = false
	emit_signal("pressed")


func release() -> void:
	_pressed_count -= 1

	if _pressed_count <= 0:
		_pressed_count = 0
		inactive.visible = true
		emit_signal("released")
