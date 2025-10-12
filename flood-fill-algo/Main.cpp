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

    bool bfs_visited = false;
    int bfs_parent[2];

    int cost = 0;
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
    char route[200];
    int p = 0;

    char orientation = 'N';

    int intersection_stack[15][2];
    int ISLen = 0;

    //  Constructor function
    Maze()
    {
        // initialize adjacent cells for the starting orientation/position
        cells[curr_cell[0]][curr_cell[1]].visited = true;
        update_adjacent_cells();

        //  Name cells:
        for (int i = 0; i < 16; i++)
        {
            API::setText(0, i, std::to_string(i));
            API::setText(i, 0, std::to_string(i));
        }
    }

    void update_walls_at_cell()
    {
        if (orientation == 'N')
        {
            // multiple sensors can report walls; check each independently
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
            }
            if (API::wallBack())
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
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
            }
            if (API::wallBack())
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
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
            }
            if (API::wallBack())
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
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
            }
            if (API::wallBack())
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
        route[p] = 'L';
        p++;
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
        route[p] = 'R';
        p++;
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
        route[p] = 'F';
        p++;
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
            // ensure adjacent-cell indices are current
            update_adjacent_cells();

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
            // ensure adjacent-cell indices are current
            update_adjacent_cells();

            cells[curr_cell[0]][curr_cell[1]].visited = true;
            API::setColor(curr_cell[0], curr_cell[1], 'G');

            //  Keep moving forward till multiple paths are possible:
            if (!API::wallFront() && API::wallLeft() && API::wallRight())
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
                if (!(API::wallFront() && API::wallLeft() && API::wallRight()))
                {
                    log("Intersection found at");
                    log_coords();
                    intersection_stack[ISLen][0] = curr_cell[0];
                    intersection_stack[ISLen][1] = curr_cell[1];
                    ISLen++;
                }
                //  If multiple paths found, take first one.
                if (!API::wallFront() && !cells[front_cell[0]][front_cell[1]].visited)
                {
                    // if (!cells[right_cell[0]][right_cell[1]].visited)
                    // {
                    //     turn_right();
                    //     // DFS_explore();
                    // }
                    // log("Front is:");
                    // log("(" + std::to_string(front_cell[0]) + ", " + std::to_string(front_cell[1]) + ")");

                    move_forward();
                }
                else if (!API::wallLeft() && !cells[left_cell[0]][left_cell[1]].visited)
                {
                    if (true)
                    {
                        turn_left();
                        // log("here");
                        // move_forward();
                        // DFS_explore();
                    }
                }
                else if (!API::wallRight() && !cells[right_cell[0]][right_cell[1]].visited)
                {
                    if (true)
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
                // If all paths visited, go back to previous intersection.
                else
                {
                    // Compute shortest route back to previous intersection using BFS:
                    log("Starting BFS");

                    ISLen--;

                    int coords_visited[256];
                    int len1 = 0;
                    int queue[5][2];
                    int qLen = 0;

                    int u[2];

                    queue[qLen][0] = curr_cell[0];
                    queue[qLen][1] = curr_cell[1];
                    qLen++;

                    cells[queue[qLen - 1][0]][queue[qLen - 1][1]].bfs_visited = true;
                    while (qLen > 0)
                    {
                        u[0] = queue[qLen - 1][0];
                        u[1] = queue[qLen - 1][1];
                        qLen--;

                        //  Add break when the target cell is found.
                        if (u[0] == intersection_stack[ISLen][0] && u[1] == intersection_stack[ISLen][1])
                        {
                            log("Intersection location found!");
                            log("(" + std::to_string(u[0]) + ", " + std::to_string(u[1]) + ")");
                            log("Retraced path is:");
                            log("(" + std::to_string(cells[u[0]][u[1]].bfs_parent[0]) + ", " + std::to_string(cells[u[0]][u[1]].bfs_parent[1]) + ")");

                            // int now[2] = {u[0], u[1]};
                            // while (now[0] == curr_cell[0] && now[1] == curr_cell[1])
                            // {
                            //     log("(" + std::to_string(cells[now[0]][now[1]].bfs_parent[0]) + ", " + std::to_string(cells[now[0]][now[1]].bfs_parent[1]) + ")");
                            //     now[0] = cells[now[0]][now[1]].bfs_parent[0];
                            //     now[1] = cells[now[0]][now[1]].bfs_parent[1];
                            // }
                            // break;
                        }

                        int neighbours[4][2];

                        

                        // int front[2] = {u[0], u[1] + 1};
                        // int back[2] = {u[0], u[1] - 1};
                        // int left[2] = {u[0] - 1, u[1]};
                        // int right[2] = {u[0] + 1, u[1]};

                        // if (!cells[front[0]][front[1]].bfs_visited)
                        // {
                        //     queue[qLen][0] = front[0];
                        //     queue[qLen][1] = front[1];
                        //     qLen++;

                        //     cells[front[0]][front[1]].bfs_visited = true;
                        //     cells[front[0]][front[1]].bfs_parent[0] = u[0];
                        //     cells[front[0]][front[1]].bfs_parent[1] = u[1];
                        // }
                        // if (!cells[back[0]][back[1]].bfs_visited)
                        // {
                        //     queue[qLen][0] = back[0];
                        //     queue[qLen][1] = back[1];
                        //     qLen++;

                        //     cells[back[0]][back[1]].bfs_visited = true;
                        //     cells[back[0]][back[1]].bfs_parent[0] = u[0];
                        //     cells[back[0]][back[1]].bfs_parent[1] = u[1];
                        // }
                        // if (!cells[right[0]][right[1]].bfs_visited)
                        // {
                        //     queue[qLen][0] = right[0];
                        //     queue[qLen][1] = right[1];
                        //     qLen++;

                        //     cells[right[0]][right[1]].bfs_visited = true;
                        //     cells[right[0]][right[1]].bfs_parent[0] = u[0];
                        //     cells[right[0]][right[1]].bfs_parent[1] = u[1];
                        // }
                        // if (!cells[left[0]][left[1]].bfs_visited)
                        // {
                        //     queue[qLen][0] = left[0];
                        //     queue[qLen][1] = left[1];
                        //     qLen++;

                        //     cells[left[0]][left[1]].bfs_visited = true;
                        //     cells[left[0]][left[1]].bfs_parent[0] = u[0];
                        //     cells[left[0]][left[1]].bfs_parent[1] = u[1];
                        // }


                    }
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
