#include "Menu.h"

Menu::Menu() {
    quit = false;
}

bool Menu::isQuit() {
    return quit;
}
void Menu::setQuit(bool quit) {
    this->quit = quit;
}
