#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define release(b) !(input->buttons[b].is_down && input->buttons[b].changed)

float player_pos_x = 0.f;
float player_pos_y = 0.f;

// Helper function to update animation based on attack type
void update_animation(Animation* running_animation, Animation* attack_animation, float dt) {
    if (attack_animation) {
        attack_animation->elapsed_time += dt;
        if (attack_animation->elapsed_time >= attack_animation->frame_duration) {
            attack_animation->current_frame = (attack_animation->current_frame + 1) % attack_animation->frame_count; // Cycle through frames
            attack_animation->elapsed_time = 0.0f; // Reset elapsed time
        }
    }
    else {
        running_animation->elapsed_time += dt;
        if (running_animation->elapsed_time >= running_animation->frame_duration) {
            running_animation->current_frame = (running_animation->current_frame + 1) % running_animation->frame_count; // Cycle through frames
            running_animation->elapsed_time = 0.0f; // Reset elapsed time
        }
    }
}

internal void simulate_game(Input* input, float dt) {
    clear_screen(0xff5500);
    const float speed = 140.f; // Speed in units per second

    // Movement flags
    bool is_moving = false;
    bool is_moving_right = false;
    bool is_moving_left = false;
    bool is_moving_up = false;
    bool is_moving_down = false;

    // Attack state
    static bool basic_attacking = false; // Track if the knight is in the attack animation
    static bool super_attacking = false; // Track if the knight is in super attack animation
    static float attack_elapsed_time = 0.0f; // Time elapsed for the attack animation

    // Check movement inputs
    if (is_down(BUTTON_UP)) {
        player_pos_y += speed * dt;
        is_moving = true;
        is_moving_up = true;
    }
    if (is_down(BUTTON_DOWN)) {
        player_pos_y -= speed * dt;
        is_moving = true;
        is_moving_down = true;
    }
    if (is_down(BUTTON_LEFT)) {
        player_pos_x -= speed * dt;
        is_moving = true;
        is_moving_left = true;
    }
    if (is_down(BUTTON_RIGHT)) {
        player_pos_x += speed * dt;
        is_moving = true;
        is_moving_right = true;
    }

    // Check for attack input
    if (pressed(BUTTON_A) && !basic_attacking) {
        basic_attacking = true; // Start the attack
        attack_elapsed_time = 0.0f; // Reset attack elapsed time
    }
    if (pressed(BUTTON_S) && !super_attacking) {
        super_attacking = true; 
        attack_elapsed_time = 0.0f;
    }

    // Update sprite animation based on movement and attack type
    if (basic_attacking) {
        // Handle attack animation for the respective direction
        if (is_moving_right) {
            update_animation(&knight_basic_right, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_basic_right, 2.0f);
        }
        else if (is_moving_left) {
            update_animation(&knight_basic_left, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_basic_left, 2.0f);
        }
        else if (is_moving_up) {
            update_animation(&knight_basic_up, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_basic_up, 2.0f);
        }
        else if (is_moving_down) {
            update_animation(&knight_basic_down, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_basic_down, 2.0f);
        }
        else {
            update_animation(&knight_basic_right, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_basic_right, 2.0f);
        }

        // Check if the attack animation is complete
        attack_elapsed_time += dt; // Increment the elapsed time for the attack animation
        if (attack_elapsed_time >= knight_basic_right.animation_duration) { // Assuming all basic attacks have the same duration
            basic_attacking = false; // Reset the attacking state
            attack_elapsed_time = 0.0f; // Reset attack elapsed time
        }
    }
    else if (super_attacking){
        if (is_moving_right) {
            update_animation(&knight_super_right, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_super_right, 2.0f);
        }
        else if (is_moving_left) {
            update_animation(&knight_super_left, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_super_left, 2.0f);
        }
        else if (is_moving_up) {
            update_animation(&knight_super_up, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_super_up, 2.0f);
        }
        else if (is_moving_down) {
            update_animation(&knight_super_down, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_super_down, 2.0f);
        }
        else {
            update_animation(&knight_super_right, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_super_right, 2.0f);
        }

        // Check if the attack animation is complete
        attack_elapsed_time += dt; // Increment the elapsed time for the attack animation
        if (attack_elapsed_time >= knight_super_right.animation_duration) { // Assuming all basic attacks have the same duration
            super_attacking = false; // Reset the attacking state
            attack_elapsed_time = 0.0f; // Reset attack elapsed time
        }
    }
    else {
        // Normal movement animations
        if (is_moving) {
            if (is_moving_right) {
                update_animation(&knight_run_right, nullptr, dt);
                display_animation(player_pos_x, player_pos_y, &knight_run_right, 2.0f);
            }
            else if (is_moving_left) {
                update_animation(&knight_run_left, nullptr, dt);
                display_animation(player_pos_x, player_pos_y, &knight_run_left, 2.0f);
            }
            else if (is_moving_up) {
                update_animation(&knight_run_up, nullptr, dt);
                display_animation(player_pos_x, player_pos_y, &knight_run_up, 2.0f);
            }
            else if (is_moving_down) {
                update_animation(&knight_run_down, nullptr, dt);
                display_animation(player_pos_x, player_pos_y, &knight_run_down, 2.0f);
            }
        }
        else {
            // Knight is idling
            update_animation(&knight_idle, nullptr, dt);
            display_animation(player_pos_x, player_pos_y, &knight_idle, 2.0f);
        }
    }
}
