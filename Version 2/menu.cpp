#include "menu.hpp"

using namespace std;
using namespace sf;

void Menu::displayMenu(RenderWindow &app) {
    int w = 32;

    Texture bg;
    if (!bg.loadFromFile("res/menuTheme.png")) {
        cout << "Error loading texture\n";
        app.close();
        exit(1);
    }
    Sprite s(bg);

    Vector2u windowSize = app.getSize();
    Vector2u textureSize = s.getTexture()->getSize();
    s.setScale((float) windowSize.x / textureSize.x, (float) windowSize.y / textureSize.y);

    Font font;
	if (!font.loadFromFile("res/arial.ttf")) {
        cout << "Error loading font\n";
        app.close();
        exit(1);
    }

    RectangleShape play_r(Vector2f(3 * w, 2 * w));
    RectangleShape load_r(Vector2f(3 * w, 2 * w));
    RectangleShape exit_r(Vector2f(3 * w, 2 * w));
    play_r.setPosition(Vector2f(3 * w, 14 * w));
    play_r.setFillColor(Color(225, 225, 225));
    play_r.setOutlineThickness(5);
	play_r.setOutlineColor(Color(187, 187, 187));
	load_r.setPosition(Vector2f(9 * w, 14 * w));
    load_r.setFillColor(Color(225, 225, 225));
    load_r.setOutlineThickness(5);
	load_r.setOutlineColor(Color(187, 187, 187));
	exit_r.setPosition(Vector2f(15 * w, 14 * w));
    exit_r.setFillColor(Color(225, 225, 225));
    exit_r.setOutlineThickness(5);
	exit_r.setOutlineColor(Color(187, 187, 187));

    Text play("Play", font);
    play.setFillColor(Color::Black);
    play.setCharacterSize(20);
    Text load("Load", font);
    load.setFillColor(Color::Black);
    load.setCharacterSize(20);
    Text exit("Exit", font);
    exit.setFillColor(Color::Black);
    exit.setCharacterSize(20);

    FloatRect rect1Bounds = play_r.getGlobalBounds();
    FloatRect text1Bounds = play.getGlobalBounds();
    FloatRect rect2Bounds = load_r.getGlobalBounds();
    FloatRect text2Bounds = load.getGlobalBounds();
    FloatRect rect3Bounds = exit_r.getGlobalBounds();
    FloatRect text3Bounds = exit.getGlobalBounds();

    play.setPosition(rect1Bounds.left + (rect1Bounds.width / 2) - (text1Bounds.width / 2),
                     rect1Bounds.top + (rect1Bounds.height / 2) - (text1Bounds.height - 4));  //
    load.setPosition(rect2Bounds.left + (rect2Bounds.width / 2) - (text2Bounds.width / 2),
                     rect2Bounds.top + (rect2Bounds.height / 2) - text2Bounds.height);
    exit.setPosition(rect3Bounds.left + (rect3Bounds.width / 2) - (text3Bounds.width / 2),
                     rect3Bounds.top + (rect3Bounds.height / 2) - text3Bounds.height);

    Game game;
    while (app.isOpen()) {
        app.clear(Color(211, 211, 211));
        app.draw(s);
        app.draw(play_r);
        app.draw(load_r);
        app.draw(exit_r);
        app.draw(play);
        app.draw(load);
        app.draw(exit);

        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
            else if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                if (rect1Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Play clicked\n";
                    game.play(app);
                } else if (rect2Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Load clicked\n";
                    game.load(app);
                } else if (rect3Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Exit clicked\n";
                    app.close();
                }
            }
        }
        app.display();
    }
}
