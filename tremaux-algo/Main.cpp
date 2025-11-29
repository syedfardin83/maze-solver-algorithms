#include <iostream>
#include <string>

#include "API.h"
using namespace std;

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

    int mark = 0;
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

    int intersection_stack[100][2];
    int ISLen = 0;

    int prev_removed[2] = {-1, -1};

    int bfs_queue[256][2];
    int q_r = -1, q_f = -1;
    int bfs_loc[2];

    int n_ways = 0;
    int n_visited = 0;

    char orientation = 'N';
    int dest[2] = {7, 7};

    void log_coords()
    {
        log("(" + std::to_string(curr_cell[0]) + ", " + std::to_string(curr_cell[1]) + ")");
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

    void update_walls_at_cell()
    {
        if (orientation == 'N')
        {
            // multiple sensors can report walls; check each independently
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
                // symmetric update: neighbor to the west has right wall
                if (curr_cell[0] > 0)
                    cells[curr_cell[0] - 1][curr_cell[1]].wallRight = true;
            }
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
                if (curr_cell[0] < 15)
                    cells[curr_cell[0] + 1][curr_cell[1]].wallLeft = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
                if (curr_cell[1] < 15)
                    cells[curr_cell[0]][curr_cell[1] + 1].wallBack = true;
            }
            if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
                if (curr_cell[1] > 0)
                    cells[curr_cell[0]][curr_cell[1] - 1].wallFront = true;
            }
        }
        else if (orientation == 'E')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
                if (curr_cell[1] < 15)
                    cells[curr_cell[0]][curr_cell[1] + 1].wallBack = true;
            }
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
                if (curr_cell[1] > 0)
                    cells[curr_cell[0]][curr_cell[1] - 1].wallFront = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
                if (curr_cell[0] < 15)
                    cells[curr_cell[0] + 1][curr_cell[1]].wallLeft = true;
            }
            if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
                if (curr_cell[0] > 0)
                    cells[curr_cell[0] - 1][curr_cell[1]].wallRight = true;
            }
        }
        else if (orientation == 'S')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
                if (curr_cell[0] < 15)
                    cells[curr_cell[0] + 1][curr_cell[1]].wallLeft = true;
            }
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
                if (curr_cell[0] > 0)
                    cells[curr_cell[0] - 1][curr_cell[1]].wallRight = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
                if (curr_cell[1] > 0)
                    cells[curr_cell[0]][curr_cell[1] - 1].wallFront = true;
            }
            if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
                if (curr_cell[1] < 15)
                    cells[curr_cell[0]][curr_cell[1] + 1].wallBack = true;
            }
        }
        else if (orientation == 'W')
        {
            if (API::wallLeft())
            {
                cells[curr_cell[0]][curr_cell[1]].wallBack = true;
                if (curr_cell[1] > 0)
                    cells[curr_cell[0]][curr_cell[1] - 1].wallFront = true;
            }
            if (API::wallRight())
            {
                cells[curr_cell[0]][curr_cell[1]].wallFront = true;
                if (curr_cell[1] < 15)
                    cells[curr_cell[0]][curr_cell[1] + 1].wallBack = true;
            }
            if (API::wallFront())
            {
                cells[curr_cell[0]][curr_cell[1]].wallLeft = true;
                if (curr_cell[0] > 0)
                    cells[curr_cell[0] - 1][curr_cell[1]].wallRight = true;
            }
            if (API::wallBack())
            {
                cells[curr_cell[0]][curr_cell[1]].wallRight = true;
                if (curr_cell[0] < 15)
                    cells[curr_cell[0] + 1][curr_cell[1]].wallLeft = true;
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
        API::setText(curr_cell[0], curr_cell[1], to_string(cells[curr_cell[0]][curr_cell[1]].mark));
    }

    void enq()
    {
        if (q_f == -1)
            q_f = 0;
        q_r++;
        bfs_queue[q_r][0] = bfs_loc[0];
        bfs_queue[q_r][1] = bfs_loc[1];
        // log("Added to queue: "+std::to_string(bfs_loc[0])+","+std::to_string(bfs_loc[1]));
    }

    void deq()
    {
        bfs_loc[0] = bfs_queue[q_f][0];
        bfs_loc[1] = bfs_queue[q_f][1];
        q_f++;
    }

    void to_prev_intersection()
    {

        log("curr loc");
        // log_coords();
        log("Starting backtrack from " + std::to_string(curr_cell[0]) + "," + std::to_string(curr_cell[1]) + "to " + std::to_string(intersection_stack[ISLen - 1][0]) + "," + std::to_string(intersection_stack[ISLen - 1][1]));
        // log("from");
        // log_coords();
        // log("returning to prev intersection: "+std::to_string(intersection_stack[ISLen-1][0])+","+std::to_string(intersection_stack[ISLen-1][1]));
        // prepare BFS: clear previous bfs flags/parents and reset queue
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
            {
                cells[i][j].bfs_visited = false;
                cells[i][j].bfs_parent[0] = -1;
                cells[i][j].bfs_parent[1] = -1;
            }

        q_f = -1;
        q_r = -1;

        // If the top of intersection stack is the current cell, pop it to go to the previous one
        if (ISLen <= 0)
        {
            // log("No intersections in stack");
            log("Exploration complete!");
            // exploration_done = true;
            return;
        }
        if (intersection_stack[ISLen - 1][0] == curr_cell[0] && intersection_stack[ISLen - 1][1] == curr_cell[1])
        {
            // pop current intersection since we're already there
            ISLen = ISLen - 1;
        }
        if (ISLen <= 0)
        {
            log("No previous intersection to go to");
            return;
        }

        // start BFS from current cell
        bfs_loc[0] = curr_cell[0];
        bfs_loc[1] = curr_cell[1];

        // mark start
        cells[bfs_loc[0]][bfs_loc[1]].bfs_visited = true;
        cells[bfs_loc[0]][bfs_loc[1]].bfs_parent[0] = -1;
        cells[bfs_loc[0]][bfs_loc[1]].bfs_parent[1] = -1;

        enq();

        while (q_r - q_f + 1 > 0)
        {
            log("trying!");
            deq();

            if (bfs_loc[0] == intersection_stack[ISLen - 1][0] && bfs_loc[1] == intersection_stack[ISLen - 1][1])
            {
                log("Found path to " + std::to_string(bfs_loc[0]) + "," + std::to_string(bfs_loc[1]));

                // build path from current cell to intersection using bfs_parent
                int path[256][2];
                int plen = 0;

                // start from target (intersection) and walk parents back to start
                int tx = intersection_stack[ISLen - 1][0];
                int ty = intersection_stack[ISLen - 1][1];
                bool aborted = false;
                while (!(tx == curr_cell[0] && ty == curr_cell[1]))
                {
                    path[plen][0] = tx;
                    path[plen][1] = ty;
                    plen++;
                    int px = cells[tx][ty].bfs_parent[0];
                    int py = cells[tx][ty].bfs_parent[1];
                    // safety: if parent not set, abort
                    if (px == -1 && py == -1)
                    {
                        log("bfs parent missing, aborting");
                        aborted = true;
                        break;
                    }
                    tx = px;
                    ty = py;
                    // avoid overflow
                    if (plen >= 256)
                    {
                        aborted = true;
                        break;
                    }
                }

                if (aborted)
                {
                    // cleanup and return without moving
                    for (int i = 0; i < 16; i++)
                        for (int j = 0; j < 16; j++)
                            cells[i][j].bfs_visited = false;
                    q_f = -1;
                    q_r = -1;
                    return;
                }

                // finally add the starting cell
                path[plen][0] = curr_cell[0];
                path[plen][1] = curr_cell[1];
                plen++;

                // path currently: [target, ..., start] -> reverse to [start, ..., target]
                for (int i = 0; i < plen / 2; i++)
                {
                    int tx0 = path[i][0];
                    int ty0 = path[i][1];
                    path[i][0] = path[plen - 1 - i][0];
                    path[i][1] = path[plen - 1 - i][1];
                    path[plen - 1 - i][0] = tx0;
                    path[plen - 1 - i][1] = ty0;
                }

                // Debug: print planned path (start -> target)
                log("Planned path (start->target):");
                for (int ii = 0; ii < plen; ii++)
                {
                    log("(" + std::to_string(path[ii][0]) + "," + std::to_string(path[ii][1]) + ")");
                }

                // follow path from current -> intersection
                for (int i = 1; i < plen; i++)
                {
                    int sx = path[i - 1][0];
                    int sy = path[i - 1][1];
                    int dx = path[i][0];
                    int dy = path[i][1];

                    // determine required absolute direction
                    char need = 'N';
                    if (dx == sx && dy == sy + 1)
                        need = 'N';
                    else if (dx == sx + 1 && dy == sy)
                        need = 'E';
                    else if (dx == sx && dy == sy - 1)
                        need = 'S';
                    else if (dx == sx - 1 && dy == sy)
                        need = 'W';

                    // verify that planned step is not blocked by a wall in the map
                    bool blocked = false;
                    if (need == 'N' && cells[sx][sy].wallFront)
                        blocked = true;
                    if (need == 'E' && cells[sx][sy].wallRight)
                        blocked = true;
                    if (need == 'S' && cells[sx][sy].wallBack)
                        blocked = true;
                    if (need == 'W' && cells[sx][sy].wallLeft)
                        blocked = true;
                    if (blocked)
                    {
                        log("Planned path tries to move through a wall at (" + std::to_string(sx) + "," + std::to_string(sy) + ")");
                        // cleanup and abort
                        for (int ii = 0; ii < 16; ii++)
                            for (int jj = 0; jj < 16; jj++)
                                cells[ii][jj].bfs_visited = false;
                        q_f = -1;
                        q_r = -1;
                        return;
                    }

                    // turn to required direction using minimal turns (in your style)
                    if (orientation == need)
                    {
                        // nothing
                    }
                    else if ((orientation == 'N' && need == 'S') || (orientation == 'S' && need == 'N') ||
                             (orientation == 'E' && need == 'W') || (orientation == 'W' && need == 'E'))
                    {
                        turn_left();
                        turn_left();
                    }
                    else if ((orientation == 'N' && need == 'W') || (orientation == 'W' && need == 'S') ||
                             (orientation == 'S' && need == 'E') || (orientation == 'E' && need == 'N'))
                    {
                        turn_left();
                    }
                    else
                    {
                        turn_right();
                    }

                    // move forward one step
                    move_forward();
                }
                log("Reached to prev intersection: ");
                // log_coords();

                // cleanup bfs_visited flags and queue pointers
                for (int i = 0; i < 16; i++)
                    for (int j = 0; j < 16; j++)
                        cells[i][j].bfs_visited = false;

                q_f = -1;
                q_r = -1;

                // pop this intersection from stack
                n_ways = 0;
                n_visited = 0;
                // log("starting");
                // log(""+std::to_string(API::wallFront()));
                if (!API::wallFront())
                {
                    n_ways++;
                    // log("here");
                    if (cells[front_cell[0]][front_cell[1]].visited)
                        n_visited++;
                }
                if (!API::wallLeft())
                {
                    n_ways++;
                    // log("here");

                    if (cells[left_cell[0]][left_cell[1]].visited)
                        n_visited++;
                }
                if (!API::wallRight())
                {
                    n_ways++;
                    // log("here");

                    if (cells[right_cell[0]][right_cell[1]].visited)
                        n_visited++;
                }
                if (n_visited == n_ways)
                {
                    log("Removing intersection: " + std::to_string(intersection_stack[ISLen - 1][0]) + "," + std::to_string(intersection_stack[ISLen - 1][1]));
                    log(std::to_string(n_ways));
                    prev_removed[0] = intersection_stack[ISLen - 1][0];
                    prev_removed[1] = intersection_stack[ISLen - 1][1];
                    ISLen--;
                }
                return;
            }

            if (bfs_loc[0] > 0 && !cells[bfs_loc[0]][bfs_loc[1]].wallLeft && !cells[bfs_loc[0] - 1][bfs_loc[1]].bfs_visited && cells[bfs_loc[0] - 1][bfs_loc[1]].visited)
            {
                cells[bfs_loc[0] - 1][bfs_loc[1]].bfs_visited = true;
                cells[bfs_loc[0] - 1][bfs_loc[1]].bfs_parent[0] = bfs_loc[0];
                cells[bfs_loc[0] - 1][bfs_loc[1]].bfs_parent[1] = bfs_loc[1];
                bfs_loc[0]--;
                enq();
                bfs_loc[0]++;
            }
            if (bfs_loc[1] < 15 && !cells[bfs_loc[0]][bfs_loc[1]].wallFront && !cells[bfs_loc[0]][bfs_loc[1] + 1].bfs_visited && cells[bfs_loc[0]][bfs_loc[1] + 1].visited)
            {
                cells[bfs_loc[0]][bfs_loc[1] + 1].bfs_visited = true;
                cells[bfs_loc[0]][bfs_loc[1] + 1].bfs_parent[0] = bfs_loc[0];
                cells[bfs_loc[0]][bfs_loc[1] + 1].bfs_parent[1] = bfs_loc[1];
                bfs_loc[1]++;
                enq();
                bfs_loc[1]--;
            }
            if (bfs_loc[0] < 15 && !cells[bfs_loc[0]][bfs_loc[1]].wallRight && !cells[bfs_loc[0] + 1][bfs_loc[1]].bfs_visited && cells[bfs_loc[0] + 1][bfs_loc[1]].visited)
            {
                cells[bfs_loc[0] + 1][bfs_loc[1]].bfs_visited = true;
                cells[bfs_loc[0] + 1][bfs_loc[1]].bfs_parent[0] = bfs_loc[0];
                cells[bfs_loc[0] + 1][bfs_loc[1]].bfs_parent[1] = bfs_loc[1];
                bfs_loc[0]++;
                enq();
                bfs_loc[0]--;
            }
            if (bfs_loc[1] > 0 && !cells[bfs_loc[0]][bfs_loc[1]].wallBack && !cells[bfs_loc[0]][bfs_loc[1] - 1].bfs_visited && cells[bfs_loc[0]][bfs_loc[1] - 1].visited)
            {
                cells[bfs_loc[0]][bfs_loc[1] - 1].bfs_visited = true;
                cells[bfs_loc[0]][bfs_loc[1] - 1].bfs_parent[0] = bfs_loc[0];
                cells[bfs_loc[0]][bfs_loc[1] - 1].bfs_parent[1] = bfs_loc[1];
                bfs_loc[1]--;
                enq();
                bfs_loc[1]++;
            }
            else
            {
                log("no back cell at " + to_string(bfs_loc[0]) + " ," + to_string(bfs_loc[1]));
                log("Current situation of " + to_string(bfs_loc[0]) + " ," + to_string(bfs_loc[1]));
                log(to_string(bfs_loc[1] > 0));
                log(to_string(!cells[bfs_loc[0]][bfs_loc[1] - 1].bfs_visited));
                log(to_string(!cells[bfs_loc[0]][bfs_loc[1] - 1].bfs_visited));
                log(to_string(cells[bfs_loc[0]][bfs_loc[1] - 1].visited));
            }
        }
        log("not found");
    }

    void tremaux()
    {
        log("Started!");
        API::setColor(0, 0, 'G');

        while (!(curr_cell[0] == dest[0] && curr_cell[1] == dest[1]))
        {
            update_walls_at_cell();
            cells[curr_cell[0]][curr_cell[1]].visited = true;

            //  Move ahead if no other route:
            if (!API::wallFront() && API::wallLeft() && API::wallRight())
            {
                move_forward();

                // log("At " + to_string(curr_cell[0]) + ", " + to_string(curr_cell[1]) + " plen set to " + to_string(plen));
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

            //  Dead End
            else if (API::wallFront() && API::wallLeft() && API::wallRight())
            {
                // Go back to previous intersection
                to_prev_intersection();
                cells[back_cell[0]][back_cell[1]].mark = 2;
                // turn_left();
                // turn_left();
            }
            // Intersection found
            else
            {

                intersection_stack[ISLen][0] = curr_cell[0];
                intersection_stack[ISLen][1] = curr_cell[1];
                ISLen++;
                log("Added intersection: " + std::to_string(intersection_stack[ISLen - 1][0]) + "," + std::to_string(intersection_stack[ISLen - 1][1]));
                log("Intersection stack:");

                // First choose paths which were never entered
                if (!API::wallLeft() && cells[left_cell[0]][left_cell[1]].mark == 0)
                {
                    // cells[left_cell[0]][left_cell[1]].mark=1;

                    turn_left();
                    move_forward();
                    cells[curr_cell[0]][curr_cell[1]].mark = 1;

                    continue;
                }
                if (!API::wallFront() && cells[front_cell[0]][front_cell[1]].mark == 0)
                {

                    cells[front_cell[0]][front_cell[1]].mark = 1;
                    move_forward();

                    continue;
                }
                if (!API::wallRight() && cells[right_cell[0]][right_cell[1]].mark == 0)
                {

                    cells[right_cell[0]][right_cell[1]].mark = 1;
                    turn_right();
                    move_forward();

                    continue;
                }
                ISLen--;
                to_prev_intersection();

                //  If no unexplored path found, go back to the previous intersection.
                // if ((!((intersection_stack[ISLen - 1][0] == curr_cell[0]) && (intersection_stack[ISLen - 1][1] == curr_cell[1]))) && (!(curr_cell[0] == prev_removed[0] && curr_cell[1] == prev_removed[1])))
                // {
                //     intersection_stack[ISLen][0] = curr_cell[0];
                //     intersection_stack[ISLen][1] = curr_cell[1];
                //     ISLen++;
                //     log("Added intersection: " + std::to_string(intersection_stack[ISLen - 1][0]) + "," + std::to_string(intersection_stack[ISLen - 1][1]));
                //     log("Intersection stack:");
                // }
            }
        }
        log("Found Path!");
    }
};

int main(int argc, char *argv[])
{
    // log("Running...");
    // API::setColor(0, 0, 'G');
    // API::setText(0, 0, "abc");
    // while (true) {
    //     if (!API::wallLeft()) {
    //         API::turnLeft();
    //     }
    //     while (API::wallFront()) {
    //         API::turnRight();
    //     }
    //     API::moveForward();
    // }

    Maze maze;
    maze.tremaux();
}
