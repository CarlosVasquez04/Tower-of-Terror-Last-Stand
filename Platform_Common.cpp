struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_A,
	BUTTON_S,

	BUTTON_COUNT, 
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};


struct Sprite {

};


struct Animation {
    u32* frames;       // Array of frames
    int frame_count;   // Number of frames
    int width;         // Frame width
    int height;        // Frame height

    int current_frame;    // Current frame being displayed
    float frame_duration; // Duration to display each frame
    float elapsed_time;   // Accumulator for frame timing
    float animation_duration; // Total duration for the animation

    // Constructor 
    Animation(int frame_count, int width, int height, float frame_duration, float animation_duration)
        : frame_count(frame_count), width(width), height(height),
        current_frame(0), frame_duration(frame_duration),
        elapsed_time(0.0f), animation_duration(animation_duration) {
        frames = new u32[frame_count * width * height];  // Allocate space for all frames
    }

    // Destructor
    ~Animation() {
        delete[] frames;
    }
};



// Define knight animations with total duration
Animation knight_run_right(8, 48, 48, 0.07f, 0.56f); // Total duration = 8 frames * 0.07s/frame
Animation knight_run_left(8, 48, 48, 0.07f, 0.56f);
Animation knight_run_up(8, 48, 48, 0.07f, 0.56f);
Animation knight_run_down(8, 48, 48, 0.07f, 0.56f);
Animation knight_idle(5, 48, 48, 0.1f, 0.5f); // Total duration = 5 frames * 0.1s/frame

// Define knight basic attack animations with total duration
Animation knight_basic_right(6, 48, 48, 0.07f, 0.42f); // Total duration = 6 frames * 0.07s/frame
Animation knight_basic_left(6, 48, 48, 0.07f, 0.42f);
Animation knight_basic_up(6, 48, 48, 0.07f, 0.42f);
Animation knight_basic_down(6, 48, 48, 0.07f, 0.42f);

// Define knight super attack animations with total duration
Animation knight_super_right(6, 48, 48, 0.07f, 0.42f); // Total duration = 6 frames * 0.07s/frame
Animation knight_super_left(6, 48, 48, 0.07f, 0.42f);
Animation knight_super_up(6, 48, 48, 0.07f, 0.42f);
Animation knight_super_down(6, 48, 48, 0.07f, 0.42f);