extends CharacterBody3D
class_name Character

# =========================
# Movement
# =========================
@export_group("Movement")
@export var mouse_sensitivity: float = 0.1
@export var acceleration: float = 20.0
@export var deceleration: float = 25.0
@export var speed: float = 10.0
@export var running_speed: float = 15.0
@export var jump_velocity: float = 8.0
@export var fall_multiplier: float = 1.5
@export var gravity_multiplier: float = 1.5

@export var jump_pressed_buffer: float = 0.1
@export var floor_touched_buffer: float = 0.1
@export var upwards_gravity_multiplier: float = 3.0

var _jump_pressed_timestamp: float = 0.0
var _floor_touched_timestamp: float = 0.0
var _holding_jump := false
var _is_jumping := false

var smoothvel: float = 0.0

# =========================
# Setup
# =========================
@export_group("Setup")
@export var camera: Camera3D
@export var interact_raycast: RayCast3D
@export var push_raycast_1: RayCast3D
@export var push_raycast_2: RayCast3D
@export var push_raycast_3: RayCast3D
@export var body: MeshInstance3D

const RAY_LENGTH := 1000.0

var center: Vector2
var _standard_fov: float = 90.0
var currently_interacting: Object

var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity")


# =========================
# Ready
# =========================
func _ready() -> void:
	center = get_viewport().get_visible_rect().size / 2.0
	_standard_fov = camera.fov
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED


# =========================
# Gravity
# =========================
func handle_gravity(v: Vector3, delta: float) -> Vector3:
	if not is_on_floor():
		var g = gravity * gravity_multiplier
		if v.y < 0.0:
			g *= fall_multiplier
		v.y -= g * delta
	return v


# =========================
# Jump
# =========================
func handle_jump(v: Vector3, delta: float) -> Vector3:
	var current_time = Time.get_unix_time_from_system()
	var jump_held = Input.is_action_pressed("mv_jump")
	var jump_pressed = Input.is_action_just_pressed("mv_jump")
	var up_gravity = gravity * upwards_gravity_multiplier

	if is_on_floor():
		_floor_touched_timestamp = current_time
		_is_jumping = false

	if jump_pressed:
		_jump_pressed_timestamp = current_time

	if _is_jumping and v.y > 0.0 and not _holding_jump:
		v.y -= up_gravity * delta

	if (
		not _is_jumping
		and not _holding_jump
		and current_time - _jump_pressed_timestamp < jump_pressed_buffer
		and current_time - _floor_touched_timestamp < floor_touched_buffer
	):
		v.y += jump_velocity
		_holding_jump = jump_pressed
		_is_jumping = true

	_holding_jump = _holding_jump and jump_held
	return v


# =========================
# Shortest Rotation Helper
# =========================
static func get_shortest_rotation(
	current: Quaternion,
	target: Quaternion
) -> Dictionary:
	var relative := target * current.inverse()
	relative = relative.normalized()

	var axis = relative.get_axis()
	var angle = relative.get_angle()

	if angle > PI:
		angle = 2.0 * PI - angle
		axis = -axis

	return {
		"axis": axis,
		"angle": rad_to_deg(angle)
	}


# =========================
# Physics
# =========================
func _physics_process(delta: float) -> void:
	var v := velocity

	v = handle_gravity(v, delta)
	v = handle_jump(v, delta)

	# Horizontal movement
	var vy = v.y
	v.y = 0.0

	var cur_speed = speed
	var running = Input.is_action_pressed("mv_sprint")
	if running and v.length() > 0.75 * speed:
		cur_speed = running_speed

	var input_dir = Input.get_vector(
		"mv_left", "mv_right",
		"mv_forward", "mv_back"
	)

	var input_vec = transform.basis * Vector3(input_dir.x, 0, input_dir.y)
	var dir = input_vec.rotated(Vector3.UP, camera.rotation.y)

	var accel = acceleration if dir.dot(v) > 0.0 else deceleration
	v = v.lerp(dir * cur_speed, accel * delta)

	v.y = vy

	# FOV based on XZ velocity
	var vel_2d = Vector2(v.x, v.z).length()

	if vel_2d > smoothvel:
		smoothvel = vel_2d
	elif dir.is_zero_approx():
		smoothvel = lerp(smoothvel, vel_2d, 5.0 * delta)
	else:
		smoothvel = lerp(smoothvel, vel_2d, 3.0 * delta)

	if running:
		var weight = max(0.0, (smoothvel - speed) / (running_speed - speed))
		#weight = weight * weight * (3.0 - 2.0 * weight)
		camera.fov = lerp(_standard_fov, 80.0, weight)

	velocity = v
	move_and_slide()


# =========================
# Input
# =========================
func _unhandled_input(event: InputEvent) -> void:
	if Input.mouse_mode != Input.MOUSE_MODE_CAPTURED:
		return

	if event is InputEventMouseMotion:
		handle_mouse_movement(event)
	elif event.is_action_pressed("interact"):
		handle_interact()
	elif event.is_action_released("interact"):
		handle_release()
	elif event.is_action_pressed("interact_alt"):
		handle_alt_interact()
	elif event.is_action_pressed("mouse_capture_release"):
		Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	elif event.is_action_released("mouse_capture_release"):
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED


func handle_alt_interact() -> void:
	var collider = interact_raycast.get_collider()
	if collider is Node:
		print("Alt Interacting with:", collider.name)


func handle_interact() -> void:
	var collider = interact_raycast.get_collider()

	if collider is Node:
		print("Interacting with:", collider.name)

	currently_interacting = collider

	if collider is IInteractable:
		collider.interact()


func handle_release() -> void:
	if currently_interacting is IInteractable:
		currently_interacting.release()


func handle_mouse_movement(ev: InputEventMouseMotion) -> void:
	var cam_rot = camera.rotation_degrees
	cam_rot.y -= ev.relative.x * mouse_sensitivity
	cam_rot.x -= ev.relative.y * mouse_sensitivity

	camera.rotation_degrees = cam_rot
	body.rotation_degrees.y = cam_rot.y
