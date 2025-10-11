#include <iostream>
#include <string>

#include "API.h"

void log(const std::string &text)
{
    std::cerr << text << std::endl;
}

class Cell
{
public:
    bool wallLeft;
    bool wallRight;
    bool wallFront;
    bool wallBack;
    bool visited;

    int cost;
};

class Maze
{
public:
    Cell cells[16][16];
    int x = 0;
    int y = 0;
    char orientation = 'N';

    //  Constructor function
    Maze()
    {
        cells[x][y].visited = true;
    }

    void update_walls_at_cell()
    {
        if (orientation == 'N')
        {
            if (API::wallLeft())
            {
                cells[x][y].wallLeft = true;
            }
            else if (API::wallRight())
            {
                cells[x][y].wallRight = true;
            }
            else if (API::wallFront())
            {
                cells[x][y].wallFront = true;
            }
            else if (API::wallBack())
            {
                cells[x][y].wallBack = true;
            }
        }
        else if (orientation == 'E')
        {
            if (API::wallLeft())
            {
                cells[x][y].wallFront = true;
            }
            else if (API::wallRight())
            {
                cells[x][y].wallBack = true;
            }
            else if (API::wallFront())
            {
                cells[x][y].wallRight = true;
            }
            else if (API::wallBack())
            {
                cells[x][y].wallLeft = true;
            }
        }
        else if (orientation == 'S')
        {
            if (API::wallLeft())
            {
                cells[x][y].wallRight = true;
            }
            else if (API::wallRight())
            {
                cells[x][y].wallLeft = true;
            }
            else if (API::wallFront())
            {
                cells[x][y].wallBack = true;
            }
            else if (API::wallBack())
            {
                cells[x][y].wallFront = true;
            }
        }
        else if (orientation == 'W')
        {
            if (API::wallLeft())
            {
                cells[x][y].wallBack = true;
            }
            else if (API::wallRight())
            {
                cells[x][y].wallFront = true;
            }
            else if (API::wallFront())
            {
                cells[x][y].wallLeft = true;
            }
            else if (API::wallBack())
            {
                cells[x][y].wallRight = true;
            }
        }
        else
        {
            log("Error updating walls!");
        }
    }

    void turn_left()
    {
        API::turnLeft();
        // Change orientation
        if (orientation == 'N')
        {
            orientation = 'W';
            // log("N to W");
        }
        else if (orientation == 'E')
        {
            orientation = 'N';
            // log("E to N");
        }
        else if (orientation == 'S')
        {
            orientation = 'E';
        }
        else if (orientation == 'W')
        {
            orientation = 'S';
        }
        else
        {
            log("Error turning left!");
        }
    }

    void turn_right()
    {
        API::turnRight();
        // Change orientation
        if (orientation == 'N')
        {
            orientation = 'E';
        }
        else if (orientation == 'E')
        {
            orientation = 'S';
        }
        else if (orientation == 'S')
        {
            orientation = 'W';
        }
        else if (orientation == 'W')
        {
            orientation = 'N';
            log("W to N");
        }
        else
        {
            log("Error turning right!");
        }
    }

    void move_forward()
    {
        API::moveForward();
        // Change virtual position
        if (orientation == 'N')
        {
            y++;
        }
        else if (orientation == 'E')
        {
            x++;
        }
        else if (orientation == 'S')
        {
            y--;
        }
        else if (orientation == 'W')
        {
            x--;
        }
        else
        {
            log("Error changing position");
        }
    }

    void log_coords()
    {
        log("(" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    void log_orientation()
    {
        log("Orientation: " + std::string(1, orientation));
    }

    void left_hand_explore()
    {
        log("Started left hand exploration algorithm...");
        API::setColor(0, 0, 'G');
        API::setText(0, 0, "abc");
        while (true)
        {
            cells[x][y].visited = true;
            API::setColor(x, y, 'G');
            // Updating walls
            update_walls_at_cell();

            if (!API::wallLeft())
            {
                //  Turn left
                turn_left();
                log_orientation();
            }
            while (API::wallFront())
            {
                // Turn right
                turn_right();
                log_orientation();
            }

            // Move forward
            move_forward();

            log_coords();
        }
    }

    void DFS_explore()
    {
    }
};

int main(int argc, char *argv[])
{
    Maze maze;
    maze.left_hand_explore();
}
