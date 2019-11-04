#include <fstream>
#include <chrono>
#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>

#include <ncurses.h>

#include "engine.h"
#include "logger.h"

template<typename T> std::string tostr(T i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

void RenderingEngine::register_backend(IOBackend * b)
{
    this->backend = b;
    b->set_logger(this->logger);
}

void RenderingEngine::run()
{
    this->logger->log("Engine started.");
    
    this->screen = new char[screen_width * screen_height];
    std::fill_n(this->screen, screen_width * screen_height, ' ');
        
    if (!this->backend)
    {
        //std::cout << "Error: IO backend not initialized." << std::endl;
        throw "Error: IO backend not initialized.";
        return;
    }
    
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();
        
    while (1)
    {
        // We'll need time differential per frame to calculate modification
        // to movement speeds, to ensure consistant movement, as ray-tracing
        // is non-deterministic
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float elapsed_time = elapsedTime.count();
        
        int key_code = this->backend->handle_inputs();
        if (key_code == -1)
        {
            this->logger->log("Quitting.");
            break;
        }

        this->update_movement(key_code);
        this->update_view();
        this->update_status_bar();
        this->update_minimap();
        
        this->backend->draw(this->screen);
    }
}

void RenderingEngine::update_view()
{
    // Execute ray-casting algorithm.
    
	for (int x = 0; x < screen_width; x++)
	{
		// For each column, calculate the projected ray angle into world space
		float ray_angle = (player_a - FOV/2.0f) + ((float) x / (float) screen_width) * FOV;

		// Find distance to wall
		float step_size = 0.1f;		  // Increment size for ray casting, decrease to increase
		float distance_to_wall = 0.0f; //                                      resolution

		bool b_hit_wall = false;		// Set when ray hits wall block
		bool b_boundary = false;		// Set when ray hits boundary between two wall blocks

		float eye_x = sin(ray_angle); // Unit vector for ray in player space
		float eye_y = cos(ray_angle);

		// Incrementally cast ray from player, along ray angle, testing for 
		// intersection with a block
		while (!b_hit_wall && distance_to_wall < depth)
		{
			distance_to_wall += step_size;
			int test_x = (int) (player_x + eye_x * distance_to_wall);
			int test_y = (int) (player_y + eye_y * distance_to_wall);
			
			// Test if ray is out of bounds
			if (test_x < 0 || test_x >= map_width || test_y < 0 || test_y >= map_height)
			{
				b_hit_wall = true;			// Just set distance to maximum depth
				distance_to_wall = depth;
			}
			else
			{
				// Ray is inbounds so test to see if the ray cell is a wall block
				if (map_data.c_str()[test_x * map_width + test_y] == '#')
				{
					// Ray has hit wall
					b_hit_wall = true;

					// To highlight tile boundaries, cast a ray from each corner
					// of the tile, to the player. The more coincident this ray
					// is to the rendering ray, the closer we are to a tile 
					// boundary, which we'll shade to add detail to the walls
					std::vector<std::pair<float, float> > p;

					// Test each corner of hit tile, storing the distance from
					// the player, and the calculated dot product of the two rays
					for (int tx = 0; tx < 2; tx++)
						for (int ty = 0; ty < 2; ty++)
						{
							// Angle of corner to eye
							float vy = (float) test_y + ty - player_y;
							float vx = (float) test_x + tx - player_x;
							float d = std::sqrt(vx*vx + vy*vy); 
							float dot = (eye_x * vx / d) + (eye_y * vy / d);
							p.push_back(std::make_pair(d, dot));
						}

					// Sort Pairs from closest to farthest
					sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });
					
					// First two/three are closest (we will never see all four)
					float bound = 0.01;
					if (acos(p.at(0).second) < bound) b_boundary = true;
					if (acos(p.at(1).second) < bound) b_boundary = true;
					if (acos(p.at(2).second) < bound) b_boundary = true;
				}
			}
		}
	
		// Calculate distance to ceiling and floor
		int n_ceiling = (float) (screen_height / 2.0) - screen_height / ((float) distance_to_wall);
		int n_floor = screen_height - n_ceiling;

		// Shader walls based on distance
		char shade = ' ';
		if (distance_to_wall <= depth / 4.0f)			shade = '@'; // 0x2588;	// Very close	
		else if (distance_to_wall < depth / 3.0f)		shade = 'O'; // 0x2593;
		else if (distance_to_wall < depth / 2.0f)		shade = 'o'; // 0x2592;
		else if (distance_to_wall < depth)				shade = '.'; // 0x2591;
		else											shade = ' ';		// Too far away

		if (b_boundary) shade = ' '; // Black it out
		
		for (int y = 0; y < screen_height; y++)
		{
			// Each Row
			if(y <= n_ceiling)
				screen[y * screen_width + x] = ' ';
			else if(y > n_ceiling && y <= n_floor)
				screen[y * screen_width + x] = shade;
			else // Floor
			{				
				// Shade floor based on distance
				float b = 1.0f - (((float) y - screen_height/2.0f) / ((float) screen_height / 2.0f));
				if (b < 0.25)		shade = '#';
				else if (b < 0.5)	shade = 'x';
				else if (b < 0.75)	shade = '.';
				else if (b < 0.9)	shade = '-';
				else				shade = ' ';
				screen[y * screen_width + x] = shade;
			}
		}
	}
}

inline char player_symbol(double x) 
{
	char c = '-';
	if (x >= 0.3927 && x < 1.1781) c = '\\';
	if (x >= 1.1781 && x < 1.9635) c = '|';
	if (x >= 1.9635 && x < 2.7489) c = '/';
	if (x >= 2.7489 && x < 3.5343) c = '-';
	if (x >= 3.5343 && x < 4.3197) c = '\\';
	if (x >= 4.3197 && x < 5.1051) c = '|';
	if (x >= 5.1051 && x < 5.8905) c = '/';
    return c;
}

void RenderingEngine::update_minimap()
{    
    // Display Map
    for (int nx = 0; nx < map_width; nx++)
    {
        for (int ny = 0; ny < map_height; ny++)
        {
            screen[(ny+1) * screen_width + nx] = map_data[ny * map_width + nx];
        }
    }
    screen[((int) player_x + 1) * screen_width + (int) player_y] = player_symbol(player_a);
}

void RenderingEngine::update_status_bar()
{
    char info_bar[40];
    sprintf(info_bar, "X=%03.2f, Y=%03.2f, A=%03.2f", player_x, player_y, player_a);
    
    // copy string without null-terminating character
    int len = std::strlen(info_bar);
    std::copy_n(info_bar, len-1, this->screen);
}

void RenderingEngine::update_movement(char in_key)
{
    // TODO: change it
    double elapsed_time = 1.0;
    
	// Handle CCW Rotation
	if (in_key == 'a') {
		//player_a -= (speed * (TWOPI / 16)) * elapsed_time;
        player_a -= (speed * 0.25) * elapsed_time;
		while (player_a < 0) player_a += TWOPI;
	}

	// Handle CW Rotation
	if (in_key == 'd') {
		//player_a += (speed * (TWOPI / 16)) * elapsed_time;
        player_a += (speed * 0.25) * elapsed_time;
		while (player_a > TWOPI) player_a -= TWOPI;
	}
	
	// Handle Forwards movement & collision
	if (in_key == 'w')
	{
		player_x += sinf(player_a) * speed * elapsed_time;;
		player_y += cosf(player_a) * speed * elapsed_time;;
		if (map_data.c_str()[(int) player_x * map_width + (int) player_y] == '#')
		{
			player_x -= sinf(player_a) * speed * elapsed_time;;
			player_y -= cosf(player_a) * speed * elapsed_time;;
		}			
	}

	// Handle backwards movement & collision
	if (in_key == 's')
	{
		player_x -= sinf(player_a) * speed * elapsed_time;;
		player_y -= cosf(player_a) * speed * elapsed_time;;
		if (map_data.c_str()[(int) player_x * map_width + (int) player_y] == '#')
		{
			player_x += sinf(player_a) * speed * elapsed_time;;
			player_y += cosf(player_a) * speed * elapsed_time;;
		}
	}
}

void RenderingEngine::load_map_from_file(const char * filename)
{
    std::ifstream fin;
    std::string line;
    fin.open(filename);
    while (fin >> line) { 
        this->map_data += line;
        map_width = std::max((size_t) map_width, line.length());
        map_height++;
    }
    fin.close();
}
