#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
using namespace std;

#define RANDOM_SEED 9974

int rand_int(int min, int max)
{
    /**
     * Generate a random integer within [min, max].
     */
    return rand() % (max - min + 1) + min;
}

double rand_uniform()
{
    /**
     * Generate a random float within [0, 1), i.e. uniform distribution.
     */
    return (double)rand() / (RAND_MAX + 1.0);
}

class Grid
{

public:
    int n;
    double s;
    double r;
    double u;
    int size;
    int filter_size;
    int radius;
    vector<vector<int>> behavior;
    int coop_num;
    vector<vector<int>> filter;
    int filter_sum;
    int z;

    Grid(int _n, double _s, double _r, double _u, int _filter_size, int random_seed)
    {
        srand(random_seed);
        n = _n;
        s = _s;
        r = _r;
        u = _u;
        size = n * n;
        filter_size = _filter_size;
        radius = _filter_size / 2;
        behavior = init_behavior();
        coop_num = init_coop_num();
        filter = init_filter();
        filter_sum = init_filter_sum();
        z = 0;

        neighbors_buf1 = init_neighbor_buf();
        neighbors_buf2 = init_neighbor_buf();
        nei1 = 0, nei2 = 0;
        ag1_x = -1;
        ag1_y = -1;
        ag2_x = -1;
        ag2_y = -1;
        payoff1 = 0.0;
        payoff2 = 0.0;
        im = 0.0;
    }

    double get_coop_ratio()
    {
        return coop_num / (double)size;
    }

    void step()
    {
        if ((double)coop_num / (double)size < 0.01)
            z += 1;

        if (coop_num != 0 || coop_num != size)
        {
            ag1_x = rand_int(0, n - 1);
            ag1_y = rand_int(0, n - 1);
            ag2_x = -1;
            ag2_y = -1;

            payoff1 = payoff(ag1_x, ag1_y);
            set_neighbors(neighbors_buf1, ag1_x, ag1_y);
            for (nei1 = 0; nei1 < 5; nei1++)
            {
                int nei_x = neighbors_buf1[nei1][0];
                int nei_y = neighbors_buf1[nei1][1];
                if (behavior[ag1_x][ag1_y] != behavior[nei_x][nei_y])
                {
                    ag2_x = nei_x;
                    ag2_y = nei_y;
                    break;
                }
            }

            if (ag2_x != -1)
            {
                payoff2 = payoff(ag2_x, ag2_y);
                im = 1.0 / (1.0 + exp((payoff2 - payoff1) / 0.5));

                // ag2 imitate ag1
                if (rand_uniform() < im)
                {
                    if (behavior[ag1_x][ag1_y] == 0)
                    {
                        behavior[ag2_x][ag2_y] = 0;
                        coop_num--;
                    }
                    else if (behavior[ag1_x][ag1_y] == 1)
                    {
                        behavior[ag2_x][ag2_y] = 1;
                        coop_num++;
                    }
                }
            }
        }
        else
        {
            z = 1000; // the experiment ends
        }
    }

private:
    // some buffer
    int conv;
    int i, j;

    vector<vector<int>> neighbors_buf1;
    vector<vector<int>> neighbors_buf2;
    int nei1, nei2;
    int ag1_x, ag1_y;
    int ag2_x, ag2_y;
    double payoff1, payoff2;
    double im;

    vector<vector<int>> init_behavior()
    {
        // defect = 0, cooperator = 1
        vector<vector<int>> res(n, vector<int>(n));
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                res[i][j] = rand_int(0, 1);
            }
        }
        return res;
    }

    int init_coop_num()
    {
        int res = 0;
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (behavior[i][j] == 1)
                    res++;
            }
        }
        return res;
    }

    vector<vector<int>> init_filter()
    {
        vector<vector<int>> res(filter_size, vector<int>(filter_size));
        int center_x = filter_size / 2;
        int center_y = filter_size / 2;
        int center_val = (filter_size + 1) / 2;
        for (int i = 0; i < filter_size; ++i)
        {
            for (int j = 0; j < filter_size; ++j)
            {
                res[i][j] = max(0, center_val - abs(i - center_x) - abs(j - center_y));
            }
        }
        return res;
    }

    int init_filter_sum()
    {
        int res = 0;
        for (int i = 0; i < filter_size; ++i)
        {
            for (int j = 0; j < filter_size; ++j)
            {
                res += filter[i][j];
            }
        }
        return res;
    }

    vector<vector<int>> init_neighbor_buf()
    {
        vector<vector<int>> res(5, vector<int>(2));
        return res;
    }

    int single_point_convolve(int x, int y)
    {
        conv = 0;
        for (i = x - radius; i < x + radius + 1; ++i)
        {
            for (j = y - radius; j < y + radius + 1; ++j)
                conv += behavior[(i + n) % n][(j + n) % n] * filter[i - (x - radius)][j - (y - radius)];
        }
        return conv;
    }

    int get_majority(int x, int y)
    {
        return ((double)single_point_convolve(x, y) / (double)filter_sum > 0.5) ? 1 : 0;
    }

    bool is_majority(int x, int y)
    {
        return behavior[x][y] == get_majority(x, y);
    }

    void set_neighbors(vector<vector<int>> &buf, int x, int y)
    {
        buf[0][0] = x;
        buf[0][1] = y;
        buf[1][0] = (x - 1 + n) % n;
        buf[1][1] = y;
        buf[2][0] = (x + 1) % n;
        buf[2][1] = y;
        buf[3][0] = x;
        buf[3][1] = (y - 1 + n) % n;
        buf[4][0] = x;
        buf[4][1] = (y + 1) % n;
    }

    double payoff_centor(int x, int y)
    {
        double res = 0;
        set_neighbors(neighbors_buf2, x, y);
        for (nei2 = 0; nei2 < 5; nei2++)
        {
            res += (double)behavior[neighbors_buf2[nei2][0]][neighbors_buf2[nei2][1]];
        }
        return res / 5 * r;
    }

    double payoff(int x, int y)
    {
        double res = 0;
        set_neighbors(neighbors_buf1, x, y);
        for (nei1 = 0; nei1 < 5; nei1++)
        {
            res += payoff_centor(neighbors_buf1[nei1][0], neighbors_buf1[nei1][1]);
        }
        res -= behavior[x][y] * 5;

        if (!is_majority(x, y))
        {
            res *= u;
        }
        return res;
    }
};

int main()
{
    const int n = 200;
    const double s = 0.5;
    const double r = 3.5;
    const double u_list[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    const int filter_size = 11;
    const int steps = 5000000;
    const int n_trails = 1;

    char filename[128];
    snprintf(filename, sizeof(filename), "result_dist_n_%d_s_%.2f_r_%.2f_steps_%d_randseed_%d.csv",
             n, s, r, steps, RANDOM_SEED);
    FILE *result_file = fopen(filename, "w");

    fprintf(result_file, "n, steps, s, u, r");
    for (int i = 0; i < n_trails; i++)
    {
        fprintf(result_file, ", n_trail%d", i);
    }
    fprintf(result_file, "\n");

    int i, step;
    double coop_ratio_res[n_trails];
    for (auto u : u_list)
    {
        for (i = 0; i < n_trails; i++)
        {
            Grid pgg_grid = Grid(n, s, r, u, filter_size, RANDOM_SEED + i);
            for (step = 0; step < steps; step++)
            {
                pgg_grid.step();
                if (pgg_grid.z >= 1000)
                    break;
            }
            fprintf(stderr, "trail %d | r = %f | coop_ratio = %f\n", i, r, pgg_grid.get_coop_ratio());
            coop_ratio_res[i] = pgg_grid.get_coop_ratio();
        }
        fprintf(result_file, "%d, %d, %.2f, %.2f, %.2f", n, steps, s, u, r);
        for (i = 0; i < n_trails; i++)
        {
            fprintf(result_file, ", %.4f", coop_ratio_res[i]);
        }
        fprintf(result_file, "\n");
    }

    return 0;
}