extends CharacterBody3D

# Movement parameters
@export var speed : float = 5.0
@export var jump_velocity : float = 4.5
@export var gravity : float = ProjectSettings.get_setting("physics/3d/default_gravity")

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _physics_process(delta: float) -> void:
	var input_direction = Vector3.ZERO

	# Gather input
	if Input.is_action_pressed("move_forward"):
		input_direction.z -= 1
	if Input.is_action_pressed("move_back"):
		input_direction.z += 1
	if Input.is_action_pressed("move_left"):
		input_direction.x += 1
	if Input.is_action_pressed("move_right"):
		input_direction.x -= 1

	# Normalize input to prevent faster diagonal movement
	input_direction = input_direction.normalized()

	# Transform local input direction to world space
	var velocityLocal = self.velocity
	var forward = -transform.basis.z
	var right = transform.basis.x
	var direction = (forward * input_direction.z + right * input_direction.x) * speed

	velocityLocal.x = direction.x
	velocityLocal.z = direction.z

	# Apply gravity
	if not is_on_floor():
		velocityLocal.y -= gravity * delta
	else:
		# Jump
		if Input.is_action_just_pressed("move_jump"):
			velocityLocal.y = jump_velocity

	# Move the character
	self.velocity = velocityLocal
	move_and_slide()
