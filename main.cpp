#include "engine.h"
#include "con_backend.h"
#include "logger.h"

int main(int argc, char ** argv)
{
    Logger logger("log.txt");
    RenderingEngine engine(&logger);
    ConsoleBackend console_backend;
    
    try 
    {
        engine.register_backend(&console_backend);
        engine.load_map_from_file("map1.txt");
        engine.run();
    }
    catch (char const * msg)
    {
        logger.log(msg);
    }
    
    return 0;
}
