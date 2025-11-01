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

void log(char *text)
{
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    log("Running...");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "abc");
    while (1)
    {
        if (!API_wallLeft())
        {
            API_turnLeft();
        }
        while (API_wallFront())
        {
            API_turnRight();
        }
        API_moveForward();
    }
}
