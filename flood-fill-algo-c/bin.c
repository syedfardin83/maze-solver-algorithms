#include <stdio.h>

struct Maze{
    int number_of_cells;
};

void Maze(struct Maze* maze){
    maze->number_of_cells = 256;
}

int main(){
    struct Maze maze;
    Maze(&maze);

    printf("Number of cells = %d",maze.number_of_cells);

    return 0;
}