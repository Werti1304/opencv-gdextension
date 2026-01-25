extends Node3D

@export var video_benchmark_room : Node3D
@export var hide_and_seek_room : Node3D

func _on_change_video_benchmark_room_state(new_state : bool) -> void:
	if new_state:
		video_benchmark_room.process_mode = Node.PROCESS_MODE_INHERIT
	else:
		video_benchmark_room.process_mode = Node.PROCESS_MODE_DISABLED
	
func _on_change_hide_and_seek_room_state(new_state : bool) -> void:
	if new_state:
		hide_and_seek_room.process_mode = Node.PROCESS_MODE_INHERIT
	else:
		hide_and_seek_room.process_mode = Node.PROCESS_MODE_DISABLED
	
	
