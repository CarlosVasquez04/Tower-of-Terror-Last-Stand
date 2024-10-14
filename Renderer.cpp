#include <stdio.h>  // For printf
#include <stdlib.h> // For malloc and free

// Function to clear the screen with a specified color
internal void clear_screen(u32 color) {
    u32* pixel = (u32*)render_state.memory;
    for (int y = 0; y < render_state.height; y++) {
        for (int x = 0; x < render_state.width; x++) {
            *pixel++ = color;
        }
    }
}

// Function to draw a rectangle in pixels
internal void draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {
    // Clamp coordinates to ensure they are within bounds
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);

    for (int y = y0; y < y1; y++) {
        u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;
        for (int x = x0; x < x1; x++) {
            *pixel++ = color;
        }
    }
}

// Global variable for render scale
global_variable float render_scale = 0.01f;

// Function to draw a rectangle using float coordinates and half sizes
internal void draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color) {
    // Adjust to be relative to screen
    x *= render_state.height * render_scale;
    y *= render_state.height * render_scale;
    half_size_x *= render_state.height * render_scale;
    half_size_y *= render_state.height * render_scale;

    x += render_state.width / 2.f;
    y += render_state.height / 2.f;

    // Convert to pixel coordinates
    int x0 = x - half_size_x;
    int x1 = x + half_size_x;
    int y0 = y - half_size_y;
    int y1 = y + half_size_y;

    // Draw the rectangle in pixels
    draw_rect_in_pixels(x0, y0, x1, y1, color);
}

// Function to load an animation from a sprite sheet
void load_animation(Animation* animation, const char* file_path, int frame_width, int frame_height, int frame_count) {
    // Load the sprite sheet image
    int width, height, channels;
    unsigned char* image_data = stbi_load(file_path, &width, &height, &channels, 0);

    if (!image_data) {
        printf("Failed to load image: %s\n", stbi_failure_reason());
        return; // Exit if image loading fails
    }

    // Allocate memory for the frames
    animation->frame_count = frame_count;
    animation->width = frame_width;
    animation->height = frame_height;
    animation->frames = (u32*)malloc(frame_width * frame_height * frame_count * sizeof(u32));

    if (!animation->frames) {
        printf("Failed to allocate memory for animation frames\n");
        stbi_image_free(image_data); // Free image data if allocation fails
        return; // Exit if memory allocation fails
    }

    // Copy the frames from the loaded image data
    for (int i = 0; i < frame_count; i++) {
        for (int y = 0; y < frame_height; y++) {
            for (int x = 0; x < frame_width; x++) {
                // Flip the y-coordinate to account for the upside-down image
                int flipped_y = (frame_height - 1) - y;

                // Calculate the pixel index for the current frame
                int pixel_index = (flipped_y * width + (i * frame_width + x)) * channels;

                u32 color = 0;
                if (channels == 4) { // RGBA
                    color = (image_data[pixel_index + 3] << 24) |
                        (image_data[pixel_index + 0] << 16) |
                        (image_data[pixel_index + 1] << 8) |
                        (image_data[pixel_index + 2]);
                }
                else if (channels == 3) { // RGB
                    color = 0xFF000000 |
                        (image_data[pixel_index + 0] << 16) |
                        (image_data[pixel_index + 1] << 8) |
                        (image_data[pixel_index + 2]);
                }

                // Store the color in the correct frame array
                animation->frames[i * frame_width * frame_height + y * frame_width + x] = color;
            }
        }
    }

    // Free the image data after copying
    stbi_image_free(image_data);
}

// Function to free animation memory
void free_animation(Animation* animation) {
    if (animation->frames) {
        free(animation->frames);
        animation->frames = nullptr; // Prevent dangling pointer
    }
}

// Function to display the animation at a specified position
internal void display_animation(float x, float y, Animation* animation, float scale) {
    int frame_width = animation->width;
    int frame_height = animation->height;

    // Loop through the pixels of the current frame
    for (int y_offset = 0; y_offset < frame_height; y_offset++) {
        for (int x_offset = 0; x_offset < frame_width; x_offset++) {
            // Get the color of the current pixel
            u32 color = animation->frames[animation->current_frame * frame_width * frame_height + y_offset * frame_width + x_offset];

            // Skip fully transparent pixels (alpha = 0)
            if ((color & 0xFF000000) != 0) {
                // Draw the pixel scaled by the given factor
                int x_scaled = x + (int)(x_offset * scale);
                int y_scaled = y + (int)(y_offset * scale);
                int x_scaled_end = x_scaled + (int)scale;
                int y_scaled_end = y_scaled + (int)scale;

                // Draw a rectangle representing the scaled pixel
                draw_rect_in_pixels(x_scaled, y_scaled, x_scaled_end, y_scaled_end, color);
            }
        }
    }
}
