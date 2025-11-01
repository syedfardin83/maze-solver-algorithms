#include <stdio.h>
#include <stdbool.h>

#include "API.h"

struct Cell
{
    bool wallLeft;
    bool wallRight;
    bool wallFront;
    bool wallBack;
    bool visited;

    bool bfs_visited;
    int bfs_parent[2];

    int cost;
};

void Cell(struct Cell *cell)
{
    cell->wallLeft = false;
    cell->wallRight = false;
    cell->wallFront = false;
    cell->wallBack = false;
    cell->visited = false;

    cell->bfs_visited = false;

    cell->cost = 0;
}

struct Maze
{
    struct Cell cells[16][16];

    int curr_cell[2];
    int front_cell[2];
    int left_cell[2];
    int right_cell[2];
    int back_cell[2];

    char orientation;

    int intersection_stack[100][2];
    int ISLen;

    int bfs_queue[256][2];
    int q_r, q_f;
    int bfs_loc[2];

    int n_ways;
    int n_visited;

    int prev_removed[2];

    bool exploration_done;

    int dest[2];
};

void Maze(struct Maze *maze)
{
    maze->curr_cell[0] = 0;
    maze->curr_cell[1] = 0;

    maze->ISLen = 0;

    maze->q_r = 0;
    maze->q_f = 0;

    maze->n_ways = 0;
    maze->n_visited = 0;

    maze->prev_removed[0] = -1;
    maze->prev_removed[1] = -1;

    maze->exploration_done = false;

    maze->dest[0] = 7;
    maze->dest[0] = 7;

    maze->intersection_stack[maze->ISLen][0] = maze->curr_cell[0];
    maze->intersection_stack[maze->ISLen][1] = maze->curr_cell[1];
    maze->ISLen++;

    //cells constructor:
    for(int i=0;i<16;i++){
        for(int j=0;j<16;j++){
            Cell(&maze->cells[i][j]);
        }
    }
}

void update_walls_at_cell(struct Maze *maze)
{
    if (maze->orientation == 'N')
    {
        // multiple sensors can report walls; check each independently
        if (API_wallLeft())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallLeft = true;
            // symmetric update: neighbor to the west has right wall
            if (maze->curr_cell[0] > 0)
                maze->cells[maze->curr_cell[0] - 1][maze->curr_cell[1]].wallRight = true;
        }
        if (API_wallRight())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallRight = true;
            // symmetric update: neighbor to the east has left wall
            if (maze->curr_cell[0] < 15)
                maze->cells[maze->curr_cell[0] + 1][maze->curr_cell[1]].wallLeft = true;
        }
        if (API_wallFront())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallFront = true;
            // symmetric update: neighbor to the north has back wall
            if (maze->curr_cell[1] < 15)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] + 1].wallBack = true;
        }
        if (API_wallBack())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallBack = true;
            // symmetric update: neighbor to the south has front wall
            if (maze->curr_cell[1] > 0)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] - 1].wallFront = true;
        }
    }
    else if (maze->orientation == 'E')
    {
        if (API_wallLeft())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallFront = true;
            if (maze->curr_cell[1] < 15)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] + 1].wallBack = true;
        }
        if (API_wallRight())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallBack = true;
            if (maze->curr_cell[1] > 0)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] - 1].wallFront = true;
        }
        if (API_wallFront())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallRight = true;
            if (maze->curr_cell[0] < 15)
                maze->cells[maze->curr_cell[0] + 1][maze->curr_cell[1]].wallLeft = true;
        }
        if (API_wallBack())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallLeft = true;
            if (maze->curr_cell[0] > 0)
                maze->cells[maze->curr_cell[0] - 1][maze->curr_cell[1]].wallRight = true;
        }
    }
    else if (maze->orientation == 'S')
    {
        if (API_wallLeft())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallRight = true;
            if (maze->curr_cell[0] < 15)
                maze->cells[maze->curr_cell[0] + 1][maze->curr_cell[1]].wallLeft = true;
        }
        if (API_wallRight())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallLeft = true;
            if (maze->curr_cell[0] > 0)
                maze->cells[maze->curr_cell[0] - 1][maze->curr_cell[1]].wallRight = true;
        }
        if (API_wallFront())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallBack = true;
            if (maze->curr_cell[1] > 0)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] - 1].wallFront = true;
        }
        if (API_wallBack())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallFront = true;
            if (maze->curr_cell[1] < 15)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] + 1].wallBack = true;
        }
    }
    else if (maze->orientation == 'W')
    {
        if (API_wallLeft())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallBack = true;
            if (maze->curr_cell[1] > 0)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] - 1].wallFront = true;
        }
        if (API_wallRight())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallFront = true;
            if (maze->curr_cell[1] < 15)
                maze->cells[maze->curr_cell[0]][maze->curr_cell[1] + 1].wallBack = true;
        }
        if (API_wallFront())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallLeft = true;
            if (maze->curr_cell[0] > 0)
                maze->cells[maze->curr_cell[0] - 1][maze->curr_cell[1]].wallRight = true;
        }
        if (API_wallBack())
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].wallRight = true;
            if (maze->curr_cell[0] < 15)
                maze->cells[maze->curr_cell[0] + 1][maze->curr_cell[1]].wallLeft = true;
        }
    }
    else
    {
        // //log("Error updating walls!");
    }
}

void update_adjacent_cells(struct Maze *maze)
{
    if (maze->orientation == 'N')
    {
        maze->left_cell[0] = maze->curr_cell[0] - 1;
        maze->left_cell[1] = maze->curr_cell[1];

        maze->right_cell[0] = maze->curr_cell[0] + 1;
        maze->right_cell[1] = maze->curr_cell[1];

        maze->front_cell[0] = maze->curr_cell[0];
        maze->front_cell[1] = maze->curr_cell[1] + 1;

        maze->back_cell[0] = maze->curr_cell[0];
        maze->back_cell[1] = maze->curr_cell[1] - 1;
    }
    else if (maze->orientation == 'E')
    {
        maze->left_cell[0] = maze->curr_cell[0];
        maze->left_cell[1] = maze->curr_cell[1] + 1;

        maze->right_cell[0] = maze->curr_cell[0];
        maze->right_cell[1] = maze->curr_cell[1] - 1;

        maze->front_cell[0] = maze->curr_cell[0] + 1;
        maze->front_cell[1] = maze->curr_cell[1];

        maze->back_cell[0] = maze->curr_cell[0] - 1;
        maze->back_cell[1] = maze->curr_cell[1];
    }
    else if (maze->orientation == 'S')
    {
        maze->left_cell[0] = maze->curr_cell[0] + 1;
        maze->left_cell[1] = maze->curr_cell[1];

        maze->right_cell[0] = maze->curr_cell[0] - 1;
        maze->right_cell[1] = maze->curr_cell[1];

        maze->front_cell[0] = maze->curr_cell[0];
        maze->front_cell[1] = maze->curr_cell[1] - 1;

        maze->back_cell[0] = maze->curr_cell[0];
        maze->back_cell[1] = maze->curr_cell[1] + 1;
    }
    else if (maze->orientation == 'W')
    {
        maze->left_cell[0] = maze->curr_cell[0];
        maze->left_cell[1] = maze->curr_cell[1] - 1;

        maze->right_cell[0] = maze->curr_cell[0];
        maze->right_cell[1] = maze->curr_cell[1] + 1;

        maze->front_cell[0] = maze->curr_cell[0] - 1;
        maze->front_cell[1] = maze->curr_cell[1];

        maze->back_cell[0] = maze->curr_cell[0] + 1;
        maze->back_cell[1] = maze->curr_cell[1];
    }
    else
    {
        // //log("Error updating adjacent cells!");
    }
}

void turn_left(struct Maze *maze)
{
    API_turnLeft();
    // Change orientation first, then update adjacent cells
    if (maze->orientation == 'N')
    {
        maze->orientation = 'W';
    }
    else if (maze->orientation == 'E')
    {
        maze->orientation = 'N';
    }
    else if (maze->orientation == 'S')
    {
        maze->orientation = 'E';
    }
    else if (maze->orientation == 'W')
    {
        maze->orientation = 'S';
    }
    else
    {
        // //log("Error turning left!");
    }

    update_adjacent_cells(maze);
}

void turn_right(struct Maze *maze)
{
    API_turnRight();
    // Change orientation first, then update adjacent cells
    if (maze->orientation == 'N')
    {
        maze->orientation = 'E';
    }
    else if (maze->orientation == 'E')
    {
        maze->orientation = 'S';
    }
    else if (maze->orientation == 'S')
    {
        maze->orientation = 'W';
    }
    else if (maze->orientation == 'W')
    {
        maze->orientation = 'N';
    }
    else
    {
        // //log("Error turning right!");
    }

    update_adjacent_cells(maze);
}

void move_forward(struct Maze *maze)
{
    API_moveForward();
    // Change virtual position first, then update adjacent cells
    if (maze->orientation == 'N')
    {
        maze->curr_cell[1] = maze->curr_cell[1] + 1;
    }
    else if (maze->orientation == 'E')
    {
        maze->curr_cell[0] = maze->curr_cell[0] + 1;
    }
    else if (maze->orientation == 'S')
    {
        maze->curr_cell[1] = maze->curr_cell[1] - 1;
    }
    else if (maze->orientation == 'W')
    {
        maze->curr_cell[0] = maze->curr_cell[0] - 1;
    }
    else
    {
        //log("Error changing position");
    }

    update_adjacent_cells(maze);
    // API_setColor(maze->curr_cell[0], maze->curr_cell[1], 'G');
}

void log_coords(struct Maze *maze)
{
    char buffer[100];
    // sprintf(buffer, "(%d, %d)", maze->curr_cell[0], maze->curr_cell[1]);
    // log(buffer);
}

void log_orientation(struct Maze *maze)
{
    char buffer[100];
    // sprintf(buffer, "Orientation: %c", maze->orientation);
    // log(buffer);
}

void enq(struct Maze *maze)
{
    if (maze->q_f == -1)
        maze->q_f = 0;
    maze->q_r++;
    maze->bfs_queue[maze->q_r][0] = maze->bfs_loc[0];
    maze->bfs_queue[maze->q_r][1] = maze->bfs_loc[1];
    // //log("Added to queue: "+std::to_string(bfs_loc[0])+","+std::to_string(bfs_loc[1]));
}

void deq(struct Maze *maze)
{
    maze->bfs_loc[0] = maze->bfs_queue[maze->q_f][0];
    maze->bfs_loc[1] = maze->bfs_queue[maze->q_f][1];
    maze->q_f++;
}

//  Run a BFS algorithm to find path to previous intersection point.
void to_prev_intersection(struct Maze *maze)
{
    //log("curr loc");
    // log_coords(maze);
    char buffer[200];
    // sprintf(buffer, "Starting backtrack from %d,%d to %d,%d", 
    //         maze->curr_cell[0], maze->curr_cell[1], 
    //         maze->intersection_stack[maze->ISLen - 1][0], 
    //         maze->intersection_stack[maze->ISLen - 1][1]);
    // log(buffer);
    
    // prepare BFS: clear previous bfs flags/parents and reset queue
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
        {
            maze->cells[i][j].bfs_visited = false;
            maze->cells[i][j].bfs_parent[0] = -1;
            maze->cells[i][j].bfs_parent[1] = -1;
        }

    maze->q_f = -1;
    maze->q_r = -1;

    // If the top of intersection stack is the current cell, pop it to go to the previous one
    if (maze->ISLen <= 0)
    {
        maze->exploration_done = true;
        return;
    }
    if (maze->intersection_stack[maze->ISLen - 1][0] == maze->curr_cell[0] && 
        maze->intersection_stack[maze->ISLen - 1][1] == maze->curr_cell[1])
    {
        // pop current intersection since we're already there
        maze->ISLen = maze->ISLen - 1;
    }
    if (maze->ISLen <= 0)
    {
        //log("No previous intersection to go to");
        return;
    }

    // start BFS from current cell
    maze->bfs_loc[0] = maze->curr_cell[0];
    maze->bfs_loc[1] = maze->curr_cell[1];

    // mark start
    maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].bfs_visited = true;
    maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].bfs_parent[0] = -1;
    maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].bfs_parent[1] = -1;

    enq(maze);

    while (maze->q_r - maze->q_f + 1 > 0)
    {
        deq(maze);

        if (maze->bfs_loc[0] == maze->intersection_stack[maze->ISLen - 1][0] && 
            maze->bfs_loc[1] == maze->intersection_stack[maze->ISLen - 1][1])
        {
            // sprintf(buffer, "Found path to %d,%d", maze->bfs_loc[0], maze->bfs_loc[1]);
            // log(buffer);

            // build path from current cell to intersection using bfs_parent
            int path[256][2];
            int plen = 0;

            // start from target (intersection) and walk parents back to start
            int tx = maze->intersection_stack[maze->ISLen - 1][0];
            int ty = maze->intersection_stack[maze->ISLen - 1][1];
            bool aborted = false;
            while (!(tx == maze->curr_cell[0] && ty == maze->curr_cell[1]))
            {
                path[plen][0] = tx;
                path[plen][1] = ty;
                plen++;
                int px = maze->cells[tx][ty].bfs_parent[0];
                int py = maze->cells[tx][ty].bfs_parent[1];
                // safety: if parent not set, abort
                if (px == -1 && py == -1)
                {
                    //log("bfs parent missing, aborting");
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
                        maze->cells[i][j].bfs_visited = false;
                maze->q_f = -1;
                maze->q_r = -1;
                return;
            }

            // finally add the starting cell
            path[plen][0] = maze->curr_cell[0];
            path[plen][1] = maze->curr_cell[1];
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
            //log("Planned path (start->target):");
            // for (int ii = 0; ii < plen; ii++)
            // {
            //     sprintf(buffer, "(%d,%d)", path[ii][0], path[ii][1]);
            //     log(buffer);
            // }

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
                if (need == 'N' && maze->cells[sx][sy].wallFront)
                    blocked = true;
                if (need == 'E' && maze->cells[sx][sy].wallRight)
                    blocked = true;
                if (need == 'S' && maze->cells[sx][sy].wallBack)
                    blocked = true;
                if (need == 'W' && maze->cells[sx][sy].wallLeft)
                    blocked = true;
                if (blocked)
                {
                    // sprintf(buffer, "Planned path tries to move through a wall at (%d,%d)", sx, sy);
                    // log(buffer);
                    // cleanup and abort
                    for (int ii = 0; ii < 16; ii++)
                        for (int jj = 0; jj < 16; jj++)
                            maze->cells[ii][jj].bfs_visited = false;
                    maze->q_f = -1;
                    maze->q_r = -1;
                    return;
                }

                // turn to required direction using minimal turns
                if (maze->orientation == need)
                {
                    // nothing
                }
                else if ((maze->orientation == 'N' && need == 'S') || (maze->orientation == 'S' && need == 'N') ||
                         (maze->orientation == 'E' && need == 'W') || (maze->orientation == 'W' && need == 'E'))
                {
                    turn_left(maze);
                    turn_left(maze);
                }
                else if ((maze->orientation == 'N' && need == 'W') || (maze->orientation == 'W' && need == 'S') ||
                         (maze->orientation == 'S' && need == 'E') || (maze->orientation == 'E' && need == 'N'))
                {
                    turn_left(maze);
                }
                else
                {
                    turn_right(maze);
                }

                // move forward one step
                move_forward(maze);
            }
            //log("Reached to prev intersection: ");
            log_coords(maze);

            // cleanup bfs_visited flags and queue pointers
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++)
                    maze->cells[i][j].bfs_visited = false;

            maze->q_f = -1;
            maze->q_r = -1;

            // pop this intersection from stack
            maze->n_ways = 0;
            maze->n_visited = 0;
            if (!API_wallFront())
            {
                maze->n_ways++;
                if (maze->cells[maze->front_cell[0]][maze->front_cell[1]].visited)
                    maze->n_visited++;
            }
            if (!API_wallLeft())
            {
                maze->n_ways++;
                if (maze->cells[maze->left_cell[0]][maze->left_cell[1]].visited)
                    maze->n_visited++;
            }
            if (!API_wallRight())
            {
                maze->n_ways++;
                if (maze->cells[maze->right_cell[0]][maze->right_cell[1]].visited)
                    maze->n_visited++;
            }
            if (maze->n_visited == maze->n_ways)
            {
                // sprintf(buffer, "Removing intersection: %d,%d", 
                //         maze->intersection_stack[maze->ISLen - 1][0], 
                //         maze->intersection_stack[maze->ISLen - 1][1]);
                // log(buffer);
                // sprintf(buffer, "%d", maze->n_ways);
                // log(buffer);
                maze->prev_removed[0] = maze->intersection_stack[maze->ISLen - 1][0];
                maze->prev_removed[1] = maze->intersection_stack[maze->ISLen - 1][1];
                maze->ISLen--;
            }
            return;
        }

        if (maze->bfs_loc[0] > 0 && !maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].wallLeft && 
            !maze->cells[maze->bfs_loc[0] - 1][maze->bfs_loc[1]].bfs_visited && 
            maze->cells[maze->bfs_loc[0] - 1][maze->bfs_loc[1]].visited)
        {
            maze->cells[maze->bfs_loc[0] - 1][maze->bfs_loc[1]].bfs_visited = true;
            maze->cells[maze->bfs_loc[0] - 1][maze->bfs_loc[1]].bfs_parent[0] = maze->bfs_loc[0];
            maze->cells[maze->bfs_loc[0] - 1][maze->bfs_loc[1]].bfs_parent[1] = maze->bfs_loc[1];
            maze->bfs_loc[0]--;
            enq(maze);
            maze->bfs_loc[0]++;
        }
        if (maze->bfs_loc[1] < 15 && !maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].wallFront && 
            !maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] + 1].bfs_visited && 
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] + 1].visited)
        {
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] + 1].bfs_visited = true;
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] + 1].bfs_parent[0] = maze->bfs_loc[0];
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] + 1].bfs_parent[1] = maze->bfs_loc[1];
            maze->bfs_loc[1]++;
            enq(maze);
            maze->bfs_loc[1]--;
        }
        if (maze->bfs_loc[0] < 15 && !maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].wallRight && 
            !maze->cells[maze->bfs_loc[0] + 1][maze->bfs_loc[1]].bfs_visited && 
            maze->cells[maze->bfs_loc[0] + 1][maze->bfs_loc[1]].visited)
        {
            maze->cells[maze->bfs_loc[0] + 1][maze->bfs_loc[1]].bfs_visited = true;
            maze->cells[maze->bfs_loc[0] + 1][maze->bfs_loc[1]].bfs_parent[0] = maze->bfs_loc[0];
            maze->cells[maze->bfs_loc[0] + 1][maze->bfs_loc[1]].bfs_parent[1] = maze->bfs_loc[1];
            maze->bfs_loc[0]++;
            enq(maze);
            maze->bfs_loc[0]--;
        }
        if (maze->bfs_loc[1] > 0 && !maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1]].wallBack && 
            !maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] - 1].bfs_visited && 
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] - 1].visited)
        {
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] - 1].bfs_visited = true;
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] - 1].bfs_parent[0] = maze->bfs_loc[0];
            maze->cells[maze->bfs_loc[0]][maze->bfs_loc[1] - 1].bfs_parent[1] = maze->bfs_loc[1];
            maze->bfs_loc[1]--;
            enq(maze);
            maze->bfs_loc[1]++;
        }
    }
    //log("not found");
}

void DFS_explore(struct Maze *maze)
{
    // API_setColor(maze->curr_cell[0], maze->curr_cell[1], 'G');

    while (!maze->exploration_done)
    {
        if (!maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].visited)
        {
            maze->cells[maze->curr_cell[0]][maze->curr_cell[1]].visited = true;
            // API_setColor(maze->curr_cell[0], maze->curr_cell[1], 'G');
        }
        update_adjacent_cells(maze);
        update_walls_at_cell(maze);

        //  Move ahead if no other route:
        if (!API_wallFront() && API_wallLeft() && API_wallRight())
        {
            move_forward(maze);
        }
        else if (API_wallLeft() && API_wallFront() && !API_wallRight())
        {
            turn_right(maze);
            move_forward(maze);
        }
        else if (!API_wallLeft() && API_wallFront() && API_wallRight())
        {
            turn_left(maze);
            move_forward(maze);
        }
        //  Dead End
        else if (API_wallFront() && API_wallLeft() && API_wallRight())
        {
            // Go back to previous intersection
            to_prev_intersection(maze);
        }
        //  Intersection found
        else
        {
            if ((!(maze->intersection_stack[maze->ISLen - 1][0] == maze->curr_cell[0] && 
                   maze->intersection_stack[maze->ISLen - 1][1] == maze->curr_cell[1])) && 
                (!(maze->curr_cell[0] == maze->prev_removed[0] && maze->curr_cell[1] == maze->prev_removed[1])))
            {
                maze->intersection_stack[maze->ISLen][0] = maze->curr_cell[0];
                maze->intersection_stack[maze->ISLen][1] = maze->curr_cell[1];
                maze->ISLen++;
                char buffer[200];
                // sprintf(buffer, "Added intersection: %d,%d", 
                //         maze->intersection_stack[maze->ISLen - 1][0], 
                //         maze->intersection_stack[maze->ISLen - 1][1]);
                // log(buffer);
                // //log("Intersection stack:");
                // for (int i = 0; i < maze->ISLen; i++)
                // {
                //     sprintf(buffer, "%d,%d", maze->intersection_stack[i][0], maze->intersection_stack[i][1]);
                //     log(buffer);
                // }
            }

            if (!API_wallLeft() && !maze->cells[maze->left_cell[0]][maze->left_cell[1]].visited)
            {
                turn_left(maze);
                move_forward(maze);
                continue;
            }
            else if (!API_wallFront() && !maze->cells[maze->front_cell[0]][maze->front_cell[1]].visited)
            {
                move_forward(maze);
                continue;
            }
            else if (!API_wallRight() && !maze->cells[maze->right_cell[0]][maze->right_cell[1]].visited)
            {
                turn_right(maze);
                move_forward(maze);
                continue;
            }
            else
            {
                // Go back to previous intersection
                to_prev_intersection(maze);
            }
        }
    }
}

// void log(char *text)
// {
//     fprintf(stderr, "%s\n", text);
//     fflush(stderr);
// }

int main(int argc, char *argv[])
{
    struct Maze maze;
    Maze(&maze);
    DFS_explore(&maze);
}
