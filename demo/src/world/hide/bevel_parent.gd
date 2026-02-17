@tool
extends Node3D

@export var tile_scene: PackedScene

@export var tiles_x: int = 5
@export var tiles_y: int = 5
@export var regenerate: bool = false : set = _regenerate


func _regenerate(value: bool) -> void:
	if not Engine.is_editor_hint():
		return
	if value:
		generate_floor()
		regenerate = false

func generate_floor() -> void: 
	if tile_scene == null:
		push_warning("No PackedScene assigned.")
		return

	_clear_children()

	# Create a temporary instance to measure size
	var temp := tile_scene.instantiate()
	add_child(temp)
	temp.owner = null

	var aabb := _get_combined_aabb(temp)
	var tile_width := aabb.size.x
	var tile_depth := aabb.size.z

	remove_child(temp)
	temp.queue_free()

	if tile_width == 0 or tile_depth == 0:
		push_warning("Tile size could not be determined.")
		return

	for x in tiles_x:
		for y in tiles_y:
			var tile := tile_scene.instantiate()
			add_child(tile)
			tile.owner = get_tree().edited_scene_root

			tile.position = Vector3(
				x * tile_width,
				0,
				y * tile_depth
			)

func _get_combined_aabb(node: Node3D) -> AABB:
	var bounds := AABB()
	var first := true

	for child in node.get_children():
		if child is VisualInstance3D:
			var mesh_aabb : AABB = child.get_aabb()
			mesh_aabb.position += child.position

			if first:
				bounds = mesh_aabb
				first = false
			else:
				bounds = bounds.merge(mesh_aabb)

	return bounds

func _clear_children() -> void:
	for child in get_children():
		child.queue_free()
