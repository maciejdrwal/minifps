#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <string>

#include "logger.h"

const float PI = 3.14159f;
const float TWOPI = 2*3.14159f;

class IOBackend
{
public:
    IOBackend() {}
    virtual ~IOBackend() = default;
    
    void set_logger(Logger * l) { logger = l; }
    virtual char handle_inputs() const = 0;
    virtual void draw(const char * screen) const = 0;

protected:
    Logger * logger;
};

class RenderingEngine
{
public:
    RenderingEngine(Logger * l = nullptr) : 
        backend(nullptr), 
        logger(l), 
        screen(nullptr), 
        screen_width(120), 
        screen_height(40),
        player_x(14.5),
        player_y(12.5),
        player_a(3.14),
        FOV(3.14159f / 4.0f),
        depth(16.0f),
        speed(1.0f),
        map_height(0),
        map_width(0)
        {}
        
    ~RenderingEngine() = default;
    
    void run();
    void load_map_from_file(const char * filename);
    void register_backend(IOBackend * b);
    void update_status_bar();
    void update_minimap();
    void update_view();
    void update_movement(char key_code);
    
private:
    std::string map_data;
    IOBackend * backend;
    Logger * logger;
    
    char * screen;
    
    double player_x;
    double player_y;
    double player_a;
    
    double FOV;
    double depth;
    double speed;
    
    unsigned int screen_width;
    unsigned int screen_height;
    
    unsigned int map_width;
    unsigned int map_height;
    
};

#endif
