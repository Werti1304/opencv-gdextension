extends Node


func _ready() -> void:
	var example := OpenCVExtension.new()
	example.print_type(example)
	example.test_opencv()
