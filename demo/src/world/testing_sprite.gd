extends Sprite3D

func _ready() -> void:
	var example := OpenCVExtension.new()
	example.print_type(example)
	var image: Image = example.test_mat_to_image()
	print(image.is_empty())
	texture = ImageTexture.create_from_image(image)
	
