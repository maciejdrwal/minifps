#include <ncurses.h>
#include <iostream>

#include "con_backend.h"

ConsoleBackend::ConsoleBackend()
{
    std::cout << "Initializing console backend" << std::endl;
    initscr();
    clear();
    cbreak();
    noecho();
    timeout(-1);
}

ConsoleBackend::~ConsoleBackend()
{
    //nocbreak();
    //clrtoeol();
    //refresh();
    endwin();
    std::cout << "Destroying console backend" << std::endl;
}
    
char ConsoleBackend::handle_inputs() const
{
    char in_key = (char) getch();
    if (in_key == 'q') return -1;

    return in_key;
}

void ConsoleBackend::draw(const char * screen) const
{
    mvprintw(0, 0, screen);
}
