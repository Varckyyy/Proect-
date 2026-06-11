#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

auto calcColor = [](float t, int v) { 
    return 255 + static_cast<int>(t * (v - 255)); 
};

const int TARGET_R = 220;
const int TARGET_G = 20;
const int TARGET_B = 60;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 700;

class BouncingBall {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float windowWidth;
    float windowHeight;
    float ballRadius;
    
    void LogCollision(const std::string& side) {
        std::ofstream logFile("collisions.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << side << "\n";
        }
    }
    
public:
    BouncingBall(float startX, float startY, float velX, float velY, 
                 float winW, float winH, float radius = 20.0f)
        : windowWidth(winW), windowHeight(winH), ballRadius(radius) {
        
        shape.setRadius(ballRadius);
        shape.setOrigin(ballRadius, ballRadius);
        shape.setPosition(startX, startY);
        shape.setFillColor(sf::Color(255, 255, 255));
        
        velocity.x = velX;
        velocity.y = velY;
    }
    
    void UpdateColor(int hour) {
        float t = hour / 23.0f;
        
        auto c = [](float t, int v) { 
            return 255 + static_cast<int>(t * (v - 255)); 
        };
        
        int r = c(t, TARGET_R);
        int g = c(t, TARGET_G);
        int b = c(t, TARGET_B);
        
        shape.setFillColor(sf::Color(r, g, b));
    }
    
    void UpdatePosition() {
        sf::Vector2f pos = shape.getPosition();
        
        pos.x += velocity.x;
        pos.y += velocity.y;
        
        if (pos.x - ballRadius <= 0 || pos.x + ballRadius >= windowWidth) {
            velocity.x = -velocity.x;
            pos.x = std::max(ballRadius, std::min(pos.x, windowWidth - ballRadius));
            LogCollision("X");
        }
        
        if (pos.y - ballRadius <= 0 || pos.y + ballRadius >= windowHeight) {
            velocity.y = -velocity.y;
            pos.y = std::max(ballRadius, std::min(pos.y, windowHeight - ballRadius));
            LogCollision("Y");
        }
        
        shape.setPosition(pos);
    }
    
    sf::CircleShape& GetShape() { return shape; }
    sf::Vector2f GetPosition() const { return shape.getPosition(); }
    sf::Color GetColor() const { return shape.getFillColor(); }
    float GetRadius() const { return ballRadius; }
    sf::Vector2f GetVelocity() const { return velocity; }
};

class ColorGraph {
private:
    sf::RectangleShape background;
    std::vector<sf::CircleShape> pointsR;
    std::vector<sf::CircleShape> pointsG;
    std::vector<sf::CircleShape> pointsB;
    sf::Text title;
    sf::Font font;
    
    float graphX, graphY;
    float graphWidth, graphHeight;
    
public:
    ColorGraph(float x, float y, float w, float h, const sf::Font& f) 
        : graphX(x), graphY(y), graphWidth(w), graphHeight(h), font(f) {
        
        background.setPosition(x, y);
        background.setSize(sf::Vector2f(w, h));
        background.setFillColor(sf::Color(30, 30, 30));
        background.setOutlineColor(sf::Color(100, 100, 100));
        background.setOutlineThickness(1);
        
        title.setFont(font);
        title.setString("Color change graph (0-23 hours)");
        title.setCharacterSize(14);
        title.setFillColor(sf::Color::White);
        title.setPosition(x + 10, y + 5);
    }
    
    void Update() {
        pointsR.clear();
        pointsG.clear();
        pointsB.clear();
        
        for (int hour = 0; hour <= 23; hour++) {
            float t = hour / 23.0f;
            
            {
                int value = calcColor(t, TARGET_R);
                float px = graphX + (hour / 23.0f) * graphWidth;
                float py = graphY + graphHeight - (value / 255.0f) * graphHeight;
                
                sf::CircleShape point(2.5f);
                point.setPosition(px - 2.5f, py - 2.5f);
                point.setFillColor(sf::Color(255, 80, 80));
                pointsR.push_back(point);
            }
            
            {
                int value = calcColor(t, TARGET_G);
                float px = graphX + (hour / 23.0f) * graphWidth;
                float py = graphY + graphHeight - (value / 255.0f) * graphHeight;
                
                sf::CircleShape point(2.5f);
                point.setPosition(px - 2.5f, py - 2.5f);
                point.setFillColor(sf::Color(80, 255, 80));
                pointsG.push_back(point);
            }
            
            {
                int value = calcColor(t, TARGET_B);
                float px = graphX + (hour / 23.0f) * graphWidth;
                float py = graphY + graphHeight - (value / 255.0f) * graphHeight;
                
                sf::CircleShape point(2.5f);
                point.setPosition(px - 2.5f, py - 2.5f);
                point.setFillColor(sf::Color(80, 80, 255));
                pointsB.push_back(point);
            }
        }
    }
    
    void Draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(title);
        
        for (const auto& p : pointsR) window.draw(p);
        for (const auto& p : pointsG) window.draw(p);
        for (const auto& p : pointsB) window.draw(p);
    }
};

class InfoPanel {
private:
    sf::RectangleShape panel;
    sf::Text timeText;
    sf::Text rgbText;
    sf::Text modeText;
    sf::Text tText;
    sf::Text controlsText;
    sf::Font font;
    
public:
    InfoPanel(float x, float y, float w, float h, const sf::Font& f) : font(f) {
        panel.setPosition(x, y);
        panel.setSize(sf::Vector2f(w, h));
        panel.setFillColor(sf::Color(40, 40, 40));
        panel.setOutlineColor(sf::Color(100, 100, 100));
        panel.setOutlineThickness(1);
        
        auto setupText = [&](sf::Text& text, float px, float py, unsigned int size = 16) {
            text.setFont(font);
            text.setCharacterSize(size);
            text.setFillColor(sf::Color::White);
            text.setPosition(px, py);
        };
        
        setupText(timeText, x + 15, y + 15);
        setupText(rgbText, x + 15, y + 40);
        setupText(tText, x + 15, y + 65);
        setupText(modeText, x + 15, y + 90);
        setupText(controlsText, x + 15, y + 130, 13);
        
        controlsText.setFillColor(sf::Color(180, 180, 180));
        controlsText.setString(
            "Controls:\n"
            "  Left/Right arrows - Change hour\n"
            "  [R] - Reset to current time\n"
            "  [Space] - Pause/Resume\n"
            "  [1/2] - Switch mode\n"
            "  [ESC] - Exit"
        );
    }
    
    void Update(int hour, const sf::Color& color, int mode, bool paused) {
        float t = hour / 23.0f;
        
        std::string timeStr = "Time: " + std::to_string(hour) + ":00";
        std::string period;
        if (hour >= 0 && hour < 6) period = " (Night)";
        else if (hour >= 6 && hour < 12) period = " (Morning)";
        else if (hour >= 12 && hour < 18) period = " (Day)";
        else period = " (Evening)";
        timeStr += period;
        if (paused) timeStr += " [PAUSED]";
        timeText.setString(timeStr);
        
        std::string rgbStr = "RGB: (" + 
            std::to_string(static_cast<int>(color.r)) + ", " +
            std::to_string(static_cast<int>(color.g)) + ", " +
            std::to_string(static_cast<int>(color.b)) + ")";
        rgbText.setString(rgbStr);
        
        std::ostringstream tStream;
        tStream.precision(3);
        tStream << "t (coefficient): " << std::fixed << t;
        tText.setString(tStream.str());
        
        std::string modeStr = "Mode: ";
        switch (mode) {
            case 1: modeStr += "Normal"; break;
            case 2: modeStr += "Eco (Energy Saving)"; break;
            default: modeStr += "Unknown"; break;
        }
        modeText.setString(modeStr);
    }
    
    void Draw(sf::RenderWindow& window) {
        window.draw(panel);
        window.draw(timeText);
        window.draw(rgbText);
        window.draw(tText);
        window.draw(modeText);
        window.draw(controlsText);
    }
};

class TrendLine {
private:
    sf::VertexArray line;
    sf::RectangleShape colorPreview;
    sf::Text labelText;
    sf::Font font;
    
public:
    TrendLine(float startX, float endX, float y, const sf::Color& color, 
              const std::string& label, const sf::Font& f) : font(f) {
        
        line = sf::VertexArray(sf::Lines, 2);
        line[0].position = sf::Vector2f(startX, y);
        line[0].color = color;
        line[1].position = sf::Vector2f(endX, y);
        line[1].color = color;
        
        colorPreview.setSize(sf::Vector2f(20, 20));
        colorPreview.setFillColor(color);
        colorPreview.setPosition(startX - 30, y - 10);
        
        labelText.setFont(font);
        labelText.setString(label);
        labelText.setCharacterSize(12);
        labelText.setFillColor(color);
        labelText.setPosition(endX + 10, y - 8);
    }
    
    void Draw(sf::RenderWindow& window) {
        window.draw(line);
        window.draw(colorPreview);
        window.draw(labelText);
    }
};

int main() {
    std::ofstream clearLog("collisions.txt", std::ios::trunc);
    clearLog << "--- New animation session ---\n";
    clearLog.close();
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
                           "Bouncing Ball - Color Visualizer");
    window.setFramerateLimit(60);
    
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return 1;
    }
    
    BouncingBall ball(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 
                      4.0f, 3.5f, WINDOW_WIDTH, WINDOW_HEIGHT, 25.0f);
    
    InfoPanel infoPanel(WINDOW_WIDTH - 260, 10, 250, 200, font);
    ColorGraph graph(20, WINDOW_HEIGHT - 250, WINDOW_WIDTH - 40, 200, font);
    
    int currentHour = 12;
    int mode = 1;
    bool paused = false;
    
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    if (local) currentHour = local->tm_hour;
    
    sf::Clock clock;
    float updateTimer = 0;
    const float UPDATE_INTERVAL = 0.05f;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    
                    case sf::Keyboard::Left:
                    case sf::Keyboard::A:
                        if (currentHour > 0) currentHour--;
                        break;
                    
                    case sf::Keyboard::Right:
                    case sf::Keyboard::D:
                        if (currentHour < 23) currentHour++;
                        break;
                    
                    case sf::Keyboard::R:
                        {
                            std::time_t t = std::time(nullptr);
                            std::tm* l = std::localtime(&t);
                            if (l) currentHour = l->tm_hour;
                        }
                        break;
                    
                    case sf::Keyboard::Space:
                        paused = !paused;
                        break;
                    
                    case sf::Keyboard::Num1:
                        mode = 1;
                        break;
                    
                    case sf::Keyboard::Num2:
                        mode = 2;
                        break;
                    
                    case sf::Keyboard::Up:
                        currentHour = (currentHour + 1) % 24;
                        break;
                    
                    case sf::Keyboard::Down:
                        currentHour = (currentHour - 1 + 24) % 24;
                        break;
                }
            }
        }
        
        float deltaTime = clock.restart().asSeconds();
        
        if (!paused) {
            updateTimer += deltaTime;
            
            while (updateTimer >= UPDATE_INTERVAL) {
                updateTimer -= UPDATE_INTERVAL;
                
                ball.UpdateColor(currentHour);
                ball.UpdatePosition();
            }
        }
        
        infoPanel.Update(currentHour, ball.GetColor(), mode, paused);
        graph.Update();
        
        window.clear(sf::Color(20, 20, 20));
        
        window.draw(ball.GetShape());
        
        sf::CircleShape glow(ball.GetRadius() + 10);
        glow.setOrigin(glow.getRadius(), glow.getRadius());
        glow.setPosition(ball.GetPosition());
        sf::Color glowColor = ball.GetColor();
        glowColor.a = 50;
        glow.setFillColor(glowColor);
        window.draw(glow);
        
        infoPanel.Draw(window);
        graph.Draw(window);
        
        TrendLine legendR(WINDOW_WIDTH - 240, WINDOW_WIDTH - 210, 
                         WINDOW_HEIGHT - 30, sf::Color(255, 80, 80), 
                         "R (target: 220)", font);
        TrendLine legendG(WINDOW_WIDTH - 240, WINDOW_WIDTH - 210, 
                         WINDOW_HEIGHT - 55, sf::Color(80, 255, 80), 
                         "G (target: 20)", font);
        TrendLine legendB(WINDOW_WIDTH - 240, WINDOW_WIDTH - 210, 
                         WINDOW_HEIGHT - 80, sf::Color(80, 80, 255), 
                         "B (target: 60)", font);
        
        legendR.Draw(window);
        legendG.Draw(window);
        legendB.Draw(window);
        
        sf::Text fpsText;
        fpsText.setFont(font);
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(1.0f / deltaTime)));
        fpsText.setCharacterSize(12);
        fpsText.setFillColor(sf::Color(150, 150, 150));
        fpsText.setPosition(10, 10);
        window.draw(fpsText);
        
        window.display();
    }
    
    return 0;
}