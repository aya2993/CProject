struct ButtonState
{
	bool isDown;
	bool changed;
};

enum {
	Button_Up,
	Button_Down,
	Button_Left,
	Button_Right,

	Button_W,
	Button_S,
	Button_A,
	Button_D,
	
	Button_Enter,
	
	Button_Count // this should be last
};

struct Input{
	ButtonState buttons[Button_Count]; 
};
