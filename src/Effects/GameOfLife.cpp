#if 0
std::string pulsar_data = 
    ".................."
    ".................."
    "....OOO...OOO....."
    ".................."
    "..O....O.O....O..."
    "..O....O.O....O..."
    "..O....O.O....O..."
    "....OOO...OOO....."
    ".................."
    "....OOO...OOO....."
    "..O....O.O....O..."
    "..O....O.O....O..."
    "..O....O.O....O..."
    ".................."
    "....OOO...OOO....."
    ".................."
    ".................."
    "..................";

void Update(uint8_t *front, uint8_t *back)
{
    // bool gridTwo[GRID_SIZE + 1][GRID_SIZE + 1] = {};
    // compareGrid(gridOne, gridTwo);
    memcpy(front, back, (GRID_SIZE+2) * (GRID_SIZE+2) * sizeof(uint8_t));

    for (int a = 1; a < GRID_SIZE+1; a++)
    {
        for (int b = 1; b < GRID_SIZE+1; b++)
        {
            int alive = 0;

            uint32_t n00 = (a - 1) * (GRID_SIZE + 2) + (b - 1);
            alive += (back[n00] > 0) ? 1 : 0;

            uint32_t n01 = (a - 1) * (GRID_SIZE + 2) + (b);
            alive += (back[n01] > 0) ? 1 : 0;

            uint32_t n02 = (a - 1) * (GRID_SIZE + 2) + (b + 1);
            alive += (back[n02] > 0) ? 1 : 0;

            uint32_t n10 = (a) * (GRID_SIZE + 2) + (b - 1);
            alive += (back[n10] > 0) ? 1 : 0;

            // uint32_t n11 = 0;
            uint32_t n12 = (a) * (GRID_SIZE + 2) + (b + 1);
            alive += (back[n12] > 0) ? 1 : 0;

            uint32_t n20 = (a + 1) * (GRID_SIZE + 2) + (b - 1);
            alive += (back[n20] > 0) ? 1 : 0;

            uint32_t n21 = (a + 1) * (GRID_SIZE + 2) + (b);
            alive += (back[n21] > 0) ? 1 : 0;

            uint32_t n22 = (a + 1) * (GRID_SIZE + 2) + (b + 1);
            alive += (back[n22] > 0) ? 1 : 0;


            if ((alive < 2) || (alive > 3))
            {
                front[a * (GRID_SIZE + 2) + b] = 0;
            }
            else if(alive == 3)
            {
                front[a * (GRID_SIZE + 2) + b] = 1;
            }
        }
    }
}
#endif