#ifndef _CON_BACKEND_H_
#define _CON_BACKEND_H_

#include "engine.h"

class Logger;

class ConsoleBackend : public IOBackend
{
public:
    ConsoleBackend(Logger& logger);
    virtual ~ConsoleBackend();

    virtual char handle_inputs() const;
    virtual void draw(const char * screen) const;

private:
    void initialize();
};

#endif
