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
    int curr_cell[2] = {0, 0};
    int front_cell[2];
    int left_cell[2];
    int right_cell[2];
    int back_cell[2];

    char orientation = 'N';

    //  Constructor function
    Maze()
    {
        cells[curr_cell[0]][curr_cell[1]].visited = true;
    }

    void update_walls_at_cell()
    {
        if (orientation == 'N')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
            else if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
            }
            else if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
            }
            else if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
            }
        }
        else if (orientation == 'E')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
            }
            else if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
            }
            else if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
            }
            else if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
        }
        else if (orientation == 'S')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
            }
            else if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
            else if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
            }
            else if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
            }
        }
        else if (orientation == 'W')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
            }
            else if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
            }
            else if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
            else if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
            }
        }
        else
        {
            log("Error updating walls!");
        }
    }

    void update_adjacent_cells()
    {
        if (orientation == 'N')
        {
            left_cell[0] = curr_cell[0] - 1;
            left_cell[1] = curr_cell[1];

            right_cell[0] = curr_cell[0] + 1;
            right_cell[1] = curr_cell[1];

            front_cell[0] = curr_cell[0];
            front_cell[1] = curr_cell[1] + 1;

            back_cell[0] = curr_cell[0];
            back_cell[1] = curr_cell[1] - 1;
        }
        else if (orientation == 'E')
        {
            left_cell[0] = curr_cell[0];
            left_cell[1] = curr_cell[1] + 1;

            right_cell[0] = curr_cell[0];
            right_cell[1] = curr_cell[1] - 1;

            front_cell[0] = curr_cell[0] + 1;
            front_cell[1] = curr_cell[1];

            back_cell[0] = curr_cell[0] - 1;
            back_cell[1] = curr_cell[1];
        }
        else if (orientation == 'S')
        {
            left_cell[0] = curr_cell[0] + 1;
            left_cell[1] = curr_cell[1];

            right_cell[0] = curr_cell[0] - 1;
            right_cell[1] = curr_cell[1];

            front_cell[0] = curr_cell[0];
            front_cell[1] = curr_cell[1] - 1;

            back_cell[0] = curr_cell[0];
            back_cell[1] = curr_cell[1] + 1;
        }
        else if (orientation == 'W')
        {
            left_cell[0] = curr_cell[0];
            left_cell[1] = curr_cell[1] - 1;

            right_cell[0] = curr_cell[0];
            right_cell[1] = curr_cell[1] + 1;

            front_cell[0] = curr_cell[0] - 1;
            front_cell[1] = curr_cell[1];

            back_cell[0] = curr_cell[0] + 1;
            back_cell[1] = curr_cell[1];
        }
        else
        {
            log("Error updating adjacent cells!");
        }
    }

    void turn_left()
    {
        API::turnLeft();
        update_adjacent_cells();
        // Change orientation
        if (orientation == 'N')
        {
            orientation = 'W';
        }
        else if (orientation == 'E')
        {
            orientation = 'N';
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
        update_adjacent_cells();

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
            // log("W to N");
        }
        else
        {
            log("Error turning right!");
        }
    }

    void move_forward()
    {
        API::moveForward();
        update_adjacent_cells();

        // Change virtual position
        if (orientation == 'N')
        {
            curr_cell[1] = curr_cell[1] + 1;
        }
        else if (orientation == 'E')
        {
            curr_cell[0] = curr_cell[0] + 1;
        }
        else if (orientation == 'S')
        {
            curr_cell[1] = curr_cell[1] - 1;
        }
        else if (orientation == 'W')
        {
            curr_cell[0] = curr_cell[0] - 1;
        }
        else
        {
            log("Error changing position");
        }
    }

    void log_coords()
    {
        log("(" + std::to_string(curr_cell[0]) + ", " + std::to_string(curr_cell[1]) + ")");
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
            cells[curr_cell[0]][curr_cell[1]].visited = true;
            API::setColor(curr_cell[0], curr_cell[1], 'G');
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
        while (true)
        {

            cells[curr_cell[0]][curr_cell[1]].visited = true;
            API::setColor(curr_cell[0], curr_cell[1], 'G');

            //  Keep moving forward till multiple paths are possible:
            if (!API::wallFront())
            {
                move_forward();
            }
            else if (API::wallLeft() && API::wallFront() && !API::wallRight())
            {
                turn_right();
                move_forward();
            }
            else if (!API::wallLeft() && API::wallFront() && API::wallRight())
            {
                turn_left();
                move_forward();
            }
            //  Executes only when an intersection found
            else
            {
                //  If multiple paths found, take first one.
                if (!API::wallLeft())
                {
                    if (!cells[left_cell[0]][left_cell[1]].visited)
                    {
                        turn_left();
                        // DFS_explore();
                    }
                }
                else if (!API::wallRight())
                {
                    if (!cells[right_cell[0]][right_cell[1]].visited)
                    {
                        turn_right();
                        // DFS_explore();
                    }
                }
                // Dead end case:
                else if (API::wallFront() && API::wallLeft() && API::wallRight())
                {
                    turn_left();
                    turn_left();
                }
            }
        }
    }
};

int main(int argc, char *argv[])
{
    Maze maze;
    // maze.left_hand_explore();
    maze.DFS_explore();
}
