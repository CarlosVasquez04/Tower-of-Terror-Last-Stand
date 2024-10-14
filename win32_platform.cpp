#include "Utility.cpp"
#include <windows.h>
#include "stb_image.h" // Include stb_image.h

#define STB_IMAGE_IMPLEMENTATION // Ensure this is defined only in one source file
#include "stb_image.h" // Include the implementation

// Global variables
global_variable bool running = true; // Keep game running while true

// Render State structure
struct Render_State {
    int height, width;
    void* memory;

    BITMAPINFO bitmap_info;
};

Render_State render_state;

// Include other source files
#include "Platform_Common.cpp"
#include "renderer.cpp"
#include "Game.cpp"

// Window callback function
LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY: {
        running = false;
    } break;

                   // Handle window resizing
    case WM_SIZE: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        render_state.width = rect.right - rect.left;
        render_state.height = rect.bottom - rect.top;

        int size = render_state.width * render_state.height * sizeof(unsigned int);

        if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
        render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
        render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
        render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
        render_state.bitmap_info.bmiHeader.biPlanes = 1;
        render_state.bitmap_info.bmiHeader.biBitCount = 32;
        render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

    } break;

    default: {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return result;
}

// Main entry point
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Create Window Class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = L"Game Window Class";
    window_class.lpfnWndProc = window_callback;

    // Register Class
    RegisterClass(&window_class);

    // Create Window
    HWND window = CreateWindowW(window_class.lpszClassName, L"My First Game",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1200, 720, 0, 0, hInstance, 0);
    HDC hdc = GetDC(window);

    Input input = {};

    // Frame timing variables
    float delta_time = 0.016666f; // Start with an assumed value (~60 FPS)
    LARGE_INTEGER frame_begin_time;
    QueryPerformanceCounter(&frame_begin_time);

    float performance_frequency;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }

    // Load knight animations
    load_animation(&knight_run_right, "4d_warrior_sprites/Right/Png/WarriorRightWalk.png", knight_run_right.width, knight_run_right.height, knight_run_right.frame_count);
    load_animation(&knight_run_left, "4d_warrior_sprites/Left/Png/WarriorLeftWalk.png", knight_run_left.width, knight_run_left.height, knight_run_left.frame_count);
    load_animation(&knight_run_up, "4d_warrior_sprites/Up/Png/WarriorUpWalk.png", knight_run_up.width, knight_run_up.height, knight_run_up.frame_count);
    load_animation(&knight_run_down, "4d_warrior_sprites/Down/Png/WarriorDownWalk.png", knight_run_down.width, knight_run_down.height, knight_run_down.frame_count);
    load_animation(&knight_idle, "4d_warrior_sprites/Right/Png/WarriorRightIdle.png", knight_idle.width, knight_idle.height, knight_idle.frame_count);

    // Load basic attack knight animations
    load_animation(&knight_basic_right, "4d_warrior_sprites/Right/Png/WarriorRightAttack01.png", knight_basic_right.width, knight_basic_right.height, knight_basic_right.frame_count);
    load_animation(&knight_basic_left, "4d_warrior_sprites/Left/Png/WarriorLeftAttack01.png", knight_basic_left.width, knight_basic_left.height, knight_basic_left.frame_count);
    load_animation(&knight_basic_up, "4d_warrior_sprites/Up/Png/WarriorUpAttack01.png", knight_basic_up.width, knight_basic_up.height, knight_basic_up.frame_count);
    load_animation(&knight_basic_down, "4d_warrior_sprites/Down/Png/WarriorDownAttack01.png", knight_basic_down.width, knight_basic_down.height, knight_basic_down.frame_count);

    // Load super attack for knight
    load_animation(&knight_super_right, "4d_warrior_sprites/Right/Png/WarriorRightAttack02.png", knight_super_right.width, knight_super_right.height, knight_super_right.frame_count);
    load_animation(&knight_super_left, "4d_warrior_sprites/Left/Png/WarriorLeftAttack02.png", knight_super_left.width, knight_super_left.height, knight_super_left.frame_count);
    load_animation(&knight_super_up, "4d_warrior_sprites/Up/Png/WarriorUpAttack02.png", knight_super_up.width, knight_super_up.height, knight_super_up.frame_count);
    load_animation(&knight_super_down, "4d_warrior_sprites/Down/Png/WarriorDownAttack02.png", knight_super_down.width, knight_super_down.height, knight_super_down.frame_count);

    // Game loop
    while (running) {
        // Input handling
        MSG message;

        // Reset input state
        for (int i = 0; i < BUTTON_COUNT; i++) {
            input.buttons[i].changed = false;
        }

        // Process messages
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            switch (message.message) {
            case WM_KEYUP:
            case WM_KEYDOWN: {
                u32 vk_code = (u32)message.wParam;
                bool is_down = ((message.lParam & (1 << 31)) == 0);

#define process_button(b,vk) \
case vk: { \
    input.buttons[b].changed = is_down != input.buttons[b].is_down; \
    input.buttons[b].is_down = is_down; \
} break;

                switch (vk_code) {
                    process_button(BUTTON_UP, VK_UP);
                    process_button(BUTTON_DOWN, VK_DOWN);
                    process_button(BUTTON_LEFT, VK_LEFT);
                    process_button(BUTTON_RIGHT, VK_RIGHT);
                    process_button(BUTTON_A, 'A'); // Process the 'A' key
                    process_button(BUTTON_S, 'S'); // Process the 'S' key
                }
            } break;

            default: {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
            }
        }

        // Simulate game logic
        simulate_game(&input, delta_time);

        // Render the frame
        StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0,
            render_state.width, render_state.height, render_state.memory,
            &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

        // End of the frame - calculate delta time
        LARGE_INTEGER frame_end_time;
        QueryPerformanceCounter(&frame_end_time);

        // Calculate delta time in seconds
        delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;

        // Clamp delta time to avoid huge jumps due to system lag
        if (delta_time > 0.1f) {
            delta_time = 0.1f; // Cap delta_time at 100ms to prevent extreme frame jumps
        }

        // Set frame_begin_time for the next frame
        frame_begin_time = frame_end_time;
    }

    return 0;
}
