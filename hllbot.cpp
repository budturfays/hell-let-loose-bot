#include <windows.h>
#include <mmsystem.h>  // Include the header for PlaySound
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>  // For setting decimal precision

// Atomic flag to control the loop
std::atomic<bool> running(false);
int shotCount = 0;  // Global variable to keep track of the number of shots
bool clickThrough = true;  // Boolean flag to indicate if the window is in click-through mode

// Function to press a key using virtual key code
void PressKey(WORD virtualKeyCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKeyCode;
    input.ki.dwFlags = 0;  // Key press
    SendInput(1, &input, sizeof(INPUT));
}

// Function to release a key using virtual key code
void ReleaseKey(WORD virtualKeyCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKeyCode;
    input.ki.dwFlags = KEYEVENTF_KEYUP;  // Key release
    SendInput(1, &input, sizeof(INPUT));
}

// Function to perform a left mouse click
void LeftClick() {
    INPUT inputs[2] = {0};

    // Mouse left button down
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    // Mouse left button up
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(2, inputs, sizeof(INPUT));

    // Increment shot count
    shotCount++;
    // Update the top row with the shot count and current action
    std::cout << "\rShot count: " << shotCount << " | Left mouse click                " << std::flush;
}

// Function to make the console window stay on top, fix its size, set transparency, position to top-right, and set click-through
void MakeWindowStayOnTopAndFixSize(int width, int height, BYTE transparency, bool enableClickThrough) {
    HWND consoleWindow = GetConsoleWindow();  // Get handle to the console window
    if (consoleWindow != NULL) {
        // Set the window to be always on top
        SetWindowPos(consoleWindow, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        // Get screen width and set position to top-right
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        int screenWidth = desktop.right;
        int xPosition = screenWidth - width;  // Calculate x-position for top-right
        int yPosition = 0;  // Top-right, so y-position is 0

        // Set the window size and move to top-right
        MoveWindow(consoleWindow, xPosition, yPosition, width, height, TRUE);

        // Disable resizing by modifying window styles
        LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
        style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);  // Disable maximize button and resizing
        SetWindowLong(consoleWindow, GWL_STYLE, style);

        // Make the window layered, set its transparency, and optionally make it click-through
        LONG exStyle = GetWindowLong(consoleWindow, GWL_EXSTYLE);
        if (enableClickThrough) {
            SetWindowLong(consoleWindow, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
        } else {
            SetWindowLong(consoleWindow, GWL_EXSTYLE, (exStyle | WS_EX_LAYERED) & ~WS_EX_TRANSPARENT);
        }
        SetLayeredWindowAttributes(consoleWindow, 0, transparency, LWA_ALPHA);
    }
}

// Function to set new transparency level
void SetWindowTransparency(BYTE transparency) {
    HWND consoleWindow = GetConsoleWindow();  // Get handle to the console window
    if (consoleWindow != NULL) {
        SetLayeredWindowAttributes(consoleWindow, 0, transparency, LWA_ALPHA);
    }
}

// Function to display a countdown in seconds with one decimal place in a single row
void DisplayCountdown(double seconds) {
    for (double t = seconds; t >= 0; t -= 0.1) {
        // Ensure the output stays on the same line
        std::cout << "\rCountdown: " << std::fixed << std::setprecision(1) << t << " seconds...                " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Function to start the loop
void StartLoop() {
    const int totalSteps = 5;  // Number of steps in the sequence
    while (running) {
        for (int i = 1; i <= totalSteps && running; ++i) {
            // Update the first line with shot count and current action
            switch (i) {
                case 1:
                    std::cout << "\rShot count: " << shotCount << " | Simulating R key press...                " << std::flush;
                    PressKey(0x52);  // Press R key
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    ReleaseKey(0x52);  // Release R key
                    break;
                case 2:
                    std::cout << "\rShot count: " << shotCount << " | Waiting for 3.5 seconds...                " << std::flush;
                    DisplayCountdown(3.5);  // Show countdown from 3.5 seconds
                    break;
                case 3:
                    std::cout << "\rShot count: " << shotCount << " | Simulating F1 key for 1.8 seconds...                " << std::flush;
                    PressKey(VK_F1);  // Pressr F1 key
                    DisplayCountdown(1.8);  // Show countdown from 1.8 seconds
                    ReleaseKey(VK_F1);  // Release F1 key
                    break;
                case 4:
                    std::cout << "\rShot count: " << shotCount << " | Simulating left mouse click...                " << std::flush;
                    LeftClick();  // Perform a left mouse click
                    break;
                case 5:
                    std::cout << "\rShot count: " << shotCount << " | Simulating F2 key hold for 1.8 seconds...                " << std::flush;
                    PressKey(VK_F2);  // Press F2 key
                    DisplayCountdown(1.8);  // Show countdown from 1.8 seconds
                    ReleaseKey(VK_F2);  // Release F2 key
                    break;
            }
        }
        // After the sequence is completed, update the first line
        std::cout << "\rShot count: " << shotCount << " | Sequence completed.                " << std::flush;
    }
}

// Function to handle keyboard input
void KeyboardListener() {
    int f10PressCount = 0;  // Track the number of F10 presses

    while (true) {
        if (GetAsyncKeyState(VK_F10) & 0x8000) {
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                // Ctrl + F10 pressed: Toggle click-through mode
                clickThrough = !clickThrough;
                MakeWindowStayOnTopAndFixSize(400, 80, clickThrough ? 100 : 200, clickThrough);
                std::cout << "\rShot count: " << shotCount << " | Toggling click-through mode...                " << std::flush;
            } else {
                f10PressCount++;

                if (f10PressCount == 1) {
                    // Toggle the running flag
                    running = !running;

                    if (running) {
                        std::cout << "\rShot count: " << shotCount << " | Starting the loop...                " << std::flush;

                        // Set transparency to 200 when the loop starts
                        SetWindowTransparency(200);

                        // Start the loop in a separate thread
                        std::thread loopThread(StartLoop);
                        loopThread.detach();
                    } else {
                        std::cout << "\rShot count: " << shotCount << " | Stopping the loop...                " << std::flush;

                        // Set transparency back to the default level (e.g., 100) when stopping the loop
                        SetWindowTransparency(100);
                    }
                } else if (f10PressCount == 2) {
                    // Reset the shot count when F10 is pressed twice
                    shotCount = 0;
                    std::cout << "\rShot count: " << shotCount << " | Resetting shot count...                " << std::flush;
                } else if (f10PressCount == 3) {
                    // Exit the program when F10 is pressed three times
                    std::cout << "\rShot count: " << shotCount << " | Closing the program...                " << std::flush;
                    exit(0);
                }
            }

            // Avoid multiple triggers by waiting until the key is released
            while (GetAsyncKeyState(VK_F10) & 0x8000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } else {
            // Reset the F10 press count if F10 is not being pressed
            f10PressCount = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    // Make the console window always stay on top, fix its size to 400x80 pixels, set transparency, and position to top-right
    BYTE initialTransparency = 100;  // Initial transparency value between 0 (completely transparent) to 255 (completely opaque)
    MakeWindowStayOnTopAndFixSize(400, 80, initialTransparency, true);

    // Display initial instructions in exactly two rows
    std::cout << "Shot count: 0 | Waiting for F10 to start...                \nCountdown: Waiting...                " << std::flush;

    // Start listening for keyboard input
    KeyboardListener();

    return 0;
}
