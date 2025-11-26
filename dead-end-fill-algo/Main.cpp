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
    bool wallLeft = false;
    bool wallRight = false;
    bool wallFront = false;
    bool wallBack = false;
    bool visited = false;

    bool filled = false;
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
    int dest[2] = {7, 7};

    int prev_path[2];

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
        // Change orientation first, then update adjacent cells
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

        update_adjacent_cells();
    }

    void turn_right()
    {
        API::turnRight();
        // Change orientation first, then update adjacent cells
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
        }
        else
        {
            log("Error turning right!");
        }

        update_adjacent_cells();
    }

    void move_forward()
    {

        API::moveForward();

        // Change virtual position first, then update adjacent cells
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

        update_adjacent_cells();
        API::setColor(curr_cell[0], curr_cell[1], 'G');
    }

    void dead_end_fill()
    {
        API::setColor(0, 0, 'G');
        while (!(curr_cell[0] == dest[0] && curr_cell[1] == dest[1]))
        {
            log("here");

            //  Move ahead if no other route:
            if (!API::wallFront() && API::wallLeft() && API::wallRight())
            {
                move_forward();
            }

            else if (API::wallLeft() && API::wallFront() && !API::wallRight())
            {
                // log("Turning right, moving forward.");
                // log_coords();
                turn_right();
                move_forward();
            }
            else if (!API::wallLeft() && API::wallFront() && API::wallRight())
            {
                turn_left();
                move_forward();
            }

            // Dead End
            else if (API::wallFront() && API::wallLeft() && API::wallRight())
            {
                // Go back to previous intersection
                // to_prev_intersection();
                cells[prev_path[0]][prev_path[1]].filled=true;
                turn_left();
                turn_left();
            }
            // Intersection found
            else
            {
                log("Entered else");
                if (!API::wallLeft() && !cells[left_cell[0]][left_cell[1]].filled)
                {
                    turn_left();
                    move_forward();
                    prev_path[0] = curr_cell[0];
                    prev_path[1] = curr_cell[1];
                    continue;
                }
                if (!API::wallFront() && !cells[front_cell[0]][front_cell[1]].filled)
                {
                    move_forward();
                    prev_path[0] = curr_cell[0];
                    prev_path[1] = curr_cell[1];
                    continue;
                }
                if (!API::wallRight() && !cells[right_cell[0]][right_cell[1]].filled)
                {
                    turn_right();
                    move_forward();
                    prev_path[0] = curr_cell[0];
                    prev_path[1] = curr_cell[1];
                    continue;
                }
                turn_left();
                turn_left();
            }
        }
        log("Found Path!");
    }
};

int main(int argc, char *argv[])
{

    Maze maze;
    maze.dead_end_fill();
}
