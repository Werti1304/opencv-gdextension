extends Control
class_name Webcam

var camera : OpenCVCamera = OpenCVCamera.new();
@export_group("Canvas")
@export var camera_canvas : TextureRect;
@export var overlay_canvas : TextureRect;
@export_group("Input")
@export var threshold_slider : Range;
@export var mode_button : OptionButton;
@export var overlay_button : CheckButton;
@export_group("Label")
@export var rectangle_label : Label;
@export_group("Hand Detection")
@export var color_picker_lower : ColorPickerButton
@export var color_picker_higher : ColorPickerButton
var texture : ImageTexture;
var overlay_texture : ImageTexture;

func _ready():
	camera.open(0, 1920, 1080);
	# camera.open_file("C:/Users/Leo/Downloads/MarkerCube.mp4");
	camera.flip(false, false);
	texture = ImageTexture.new();
	overlay_texture = ImageTexture.new();
	threshold_slider.value_changed.connect(_on_threshold_changed);

func _process(_delta):    
	match mode_button.selected:
		0: # Color
			texture.set_image(camera.get_image());
			camera.get_threshold_image();
		1: # Grey
			texture.set_image(camera.get_gray_image());
			camera.get_threshold_image();
		2: # Threshold
			texture.set_image(camera.get_threshold_image());
		3: # Hand Detection - HSV
			texture.set_image(camera.get_hands(0));
		4: # Hand Detection - Range
			texture.set_image(camera.get_hands(1));
		5: # Hand Detection - Blurred Range
			texture.set_image(camera.get_hands(2));
	rectangle_label.text = str(camera.find_rectangles(overlay_button.button_pressed));
	
	if overlay_button.button_pressed:
		overlay_texture.set_image(camera.get_overlay_image());
	#else:
		#overlay_texture = texture
	
	camera_canvas.texture = texture;
	overlay_canvas.texture = overlay_texture;
	
func _on_threshold_changed(value : float):
	camera.set_threshold(value);


func _on_lower_skin_color_picker_color_changed(_color: Color) -> void:
	camera.set_skin_color_ranges(color_picker_lower.color, color_picker_higher.color)

func _on_upper_skin_color_picker_color_changed(_color: Color) -> void:
	camera.set_skin_color_ranges(color_picker_lower.color, color_picker_higher.color)
