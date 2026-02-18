extends Node2D
class_name BallGame

@export var ball : RigidBody2D

var direction : Vector2

func _on_direction_change(new_direction : Vector2) -> void:
	direction = new_direction.normalized()

func _physics_process(_delta: float) -> void:
	ball.apply_force(direction * 100)
