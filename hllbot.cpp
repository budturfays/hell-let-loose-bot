#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>

// Atomic flag to control the loop
std::atomic<bool> running(false);

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
}

// Function to make the console window stay on top and fix its size
void MakeWindowStayOnTopAndFixSize(int width, int height) {
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
    }
}

// Function to start the loop
void StartLoop() {
    while (running) {
        std::cout << "Simulating R key press...\n";
        PressKey(0x52);  // Press R key
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ReleaseKey(0x52);  // Release R key

        std::this_thread::sleep_for(std::chrono::seconds(4));

        std::cout << "Simulating F1 key hold for 2.1 seconds...\n";
        PressKey(VK_F1);  // Press F1 key
        std::this_thread::sleep_for(std::chrono::milliseconds(2100));
        ReleaseKey(VK_F1);  // Release F1 key

        std::cout << "Simulating left mouse click...\n";
        LeftClick();  // Perform a left mouse click

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Simulating F2 key hold for 2.1 seconds...\n";
        PressKey(VK_F2);  // Press F2 key
        std::this_thread::sleep_for(std::chrono::milliseconds(2100));
        ReleaseKey(VK_F2);  // Release F2 key

        std::cout << "Sequence completed.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Function to handle keyboard input
void KeyboardListener() {
    while (true) {
        if (GetAsyncKeyState(VK_F10) & 0x8000) {
            // Toggle the running flag
            running = !running;

            if (running) {
                std::cout << "F10 pressed, starting the loop...\n";
                std::thread loopThread(StartLoop);
                loopThread.detach();
            } else {
                std::cout << "F10 pressed again, stopping the loop...\n";
            }

            // Avoid multiple triggers by waiting until the key is released
            while (GetAsyncKeyState(VK_F10) & 0x8000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // Check for mouse movement to stop the loop
        static POINT lastPos = {0, 0};
        POINT currentPos;
        GetCursorPos(&currentPos);

        if (currentPos.x != lastPos.x || currentPos.y != lastPos.y) {
            if (running) {
                std::cout << "Mouse moved, stopping the loop...\n";
                running = false;
            }
            lastPos = currentPos;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    // Make the console window always stay on top and fix its size to 400x300 pixels
    MakeWindowStayOnTopAndFixSize(350, 100);

    std::cout << "Program running, press F10 to start or stop the sequence, or move the mouse to stop.\n";

    // Start listening for keyboard input
    KeyboardListener();

    return 0;
}
