#include <windows.h>
#include <mmsystem.h>  // Include the header for PlaySound
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// Atomic flag to control the loop
std::atomic<bool> running(false);
int shotCount = 0;  // Global variable to keep track of the number of shots

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
    // Update the top row with the shot count
    std::cout << "\rShot count: " << shotCount << "                " << std::flush;
}

// Function to make the console window stay on top, fix its size, and set transparency
void MakeWindowStayOnTopAndFixSize(int width, int height, BYTE transparency) {
    HWND consoleWindow = GetConsoleWindow();  // Get handle to the console window
    if (consoleWindow != NULL) {
        // Set the window to be always on top
        SetWindowPos(consoleWindow, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        // Set the window size
        RECT rect;
        GetWindowRect(consoleWindow, &rect);
        MoveWindow(consoleWindow, rect.left, rect.top, width, height, TRUE);

        // Disable resizing by modifying window styles
        LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
        style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);  // Disable maximize button and resizing
        SetWindowLong(consoleWindow, GWL_STYLE, style);

        // Make the window layered and set its transparency
        SetWindowLong(consoleWindow, GWL_EXSTYLE, GetWindowLong(consoleWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
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

// Function to start the loop
void StartLoop() {
    const int totalSteps = 5;  // Number of steps in the sequence
    while (running) {
        for (int i = 1; i <= totalSteps && running; ++i) {
            // Update the second line with the current action
            std::cout << "\rShot count: " << shotCount << "\n";
            switch (i) {
                case 1:
                    std::cout << "Simulating R key press...                " << std::flush;
                    PressKey(0x52);  // Press R key
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    ReleaseKey(0x52);  // Release R key
                    break;
                case 2:
                    std::cout << "Waiting for 3.8 seconds...                " << std::flush;
                    std::this_thread::sleep_for(std::chrono::milliseconds(3800));
                    break;
                case 3:
                    std::cout << "Simulating F1 key hold for 2.1 seconds...                " << std::flush;
                    PressKey(VK_F1);  // Press F1 key
                    std::this_thread::sleep_for(std::chrono::milliseconds(2100));
                    ReleaseKey(VK_F1);  // Release F1 key
                    break;
                case 4:
                    std::cout << "Simulating left mouse click...                " << std::flush;
                    LeftClick();  // Perform a left mouse click
                    break;
                case 5:
                    std::cout << "Simulating F2 key hold for 1.8 seconds...                " << std::flush;
                    PressKey(VK_F2);  // Press F2 key
                    std::this_thread::sleep_for(std::chrono::milliseconds(1800));
                    ReleaseKey(VK_F2);  // Release F2 key
                    break;
            }
        }
        // After the sequence is completed, update the second line
        std::cout << "\rShot count: " << shotCount << "\nSequence completed.                " << std::flush;
    }
}

// Function to handle keyboard input
void KeyboardListener() {
    while (true) {
        if (GetAsyncKeyState(VK_F10) & 0x8000) {
            // Toggle the running flag
            running = !running;

            if (running) {
                std::cout << "\rShot count: " << shotCount << "\nStarting the loop...                " << std::flush;

                // Set transparency to 200 when the loop starts
                SetWindowTransparency(200);

                // Start the loop in a separate thread
                std::thread loopThread(StartLoop);
                loopThread.detach();
            } else {
                std::cout << "\rShot count: " << shotCount << "\nStopping the loop...                " << std::flush;

                // Set transparency back to the default level (e.g., 100) when stopping the loop
                SetWindowTransparency(100);
            }

            // Avoid multiple triggers by waiting until the key is released
            while (GetAsyncKeyState(VK_F10) & 0x8000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    // Make the console window always stay on top, fix its size to 400x120 pixels, and set transparency
    BYTE initialTransparency = 100;  // Initial transparency value between 0 (completely transparent) to 255 (completely opaque)
    MakeWindowStayOnTopAndFixSize(400, 80, initialTransparency);

    // Display initial instructions in exactly two rows
    std::cout << "Shot count: 0                " << std::endl;
    std::cout << "Waiting for F10 to start...                " << std::flush;

    // Start listening for keyboard input
    KeyboardListener();

    return 0;
}
