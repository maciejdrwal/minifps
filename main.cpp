#include "engine.h"
#include "con_backend.h"
#include "logger.h"

int main(int argc, char ** argv)
{
    Logger logger("log.txt");
    ConsoleBackend console_backend(logger);
    RenderingEngine engine(console_backend, logger);
    
    try 
    {
        engine.load_map_from_file("map1.txt");
        engine.run();
    }
    catch (const std::string& msg)
    {
        logger.log(msg);
    }
    
    return 0;
}
