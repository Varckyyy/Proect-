#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
using namespace std;

struct Vector2D { float x, y; };

class BouncingBall {
    Vector2D pos, vel;
    struct { int r, g, b; } color;
    int w, h;

    void Log(const string& s) {
        ofstream f("collisions.txt", ios::app);
        if (f.is_open()) f << s << "\n";
    }

public:
    BouncingBall(float x, float y, float vx, float vy, int w, int h)
        : pos{x, y}, vel{vx, vy}, color{255, 255, 255}, w{w}, h{h} {}

    int X() const { return (int)pos.x; }
    int Y() const { return (int)pos.y; }
    int R() const { return color.r; }
    int G() const { return color.g; }
    int B() const { return color.b; }

    void UpdateColor(int hour) {
        float t = hour / 23.0f;
        auto c = [](float t, int v) { return 255 + (int)(t * (v - 255)); };
        color.r = c(t, 220); color.g = c(t, 20); color.b = c(t, 60);
    }

    void UpdatePos() {
        pos.x += vel.x; pos.y += vel.y;
        if (pos.x <= 1 || pos.x >= w - 2) { vel.x = -vel.x; Log("X"); }
        if (pos.y <= 1 || pos.y >= h - 2) { vel.y = -vel.y; Log("Y"); }
    }
};

int main() {
    const int W = 70, H = 25;
    BouncingBall b(W/2.0f, H/2.0f, 1.4f, 0.9f, W, H);
    ofstream("collisions.txt", ios::trunc) << "--- Start ---\n";
    int mode = 1;

    while (true) {
        time_t now = time(nullptr);
        tm* local = localtime(&now);
        int hour = local ? local->tm_hour : 0;
        b.UpdateColor(hour);
        b.UpdatePos();

        cout << "\033[2J\033[H";
        cout << "+" << string(W, '-') << "+\n";

        for (int r = 0; r < H; ++r) {
            cout << "|";
            for (int c = 0; c < W; ++c) {
                if (r == b.Y() && c == b.X())
                    cout << "\033[38;2;" << b.R() << ";" << b.G() << ";" << b.B() << "m●\033[0m";
                else cout << " ";
            }
            cout << "|\n";
        }

        cout << "+" << string(W, '-') << "+\n";
        cout << " Hour: " << hour << " | RGB: " << b.R() << "," << b.G() << "," << b.B() << "\n";

        switch (mode) {
            case 1: cout << " Mode: Normal\n"; break;
            case 2: cout << " Mode: Eco\n"; break;
            default: cout << " Mode: Unknown\n";
        }
        cout << " Ctrl+C to exit\n";

        this_thread::sleep_for(chrono::milliseconds(45));
    }
}