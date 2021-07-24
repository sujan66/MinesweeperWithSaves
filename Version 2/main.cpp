#include "game.hpp"
#include "menu.hpp"

int main() {
    //Game game;
    //game.playMinesweeper();
    sf::RenderWindow app(sf::VideoMode(672, 576), "Minesweeper");
    Menu menu;
    menu.displayMenu(app);
    return 0;
}
