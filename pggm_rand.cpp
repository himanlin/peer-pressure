#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include <cassert>
using namespace std;

#define RANDOM_SEED 8879

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
    double u[2];
    int size;
    int filter_size;
    int radius;
    vector<vector<int>> group;
    vector<int> coop_num_grp;
    vector<vector<int>> behavior;
    int coop_num;
    vector<vector<int>> filter;
    int filter_sum;
    int z;

    Grid(int _n, double _s, double _r, double _u0, double _u1, int _filter_size, int random_seed)
    {
        srand(random_seed);
        n = _n;
        s = _s;
        r = _r;
        u[0] = _u0;
        u[1] = _u1;
        size = n * n;
        filter_size = _filter_size;
        radius = _filter_size / 2;
        behavior = init_behavior();
        group = init_group();
        coop_num_grp = init_coop_num();
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
        return get_coop_num() / (double)size;
    }

    void step()
    {
        int coop_num = get_coop_num();
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
                        coop_num_grp[group[ag2_x][ag2_y]] -= 1;
                    }
                    else if (behavior[ag1_x][ag1_y] == 1)
                    {
                        behavior[ag2_x][ag2_y] = 1;
                        coop_num_grp[group[ag2_x][ag2_y]] += 1;
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
                if ((i >= n / 2) && (j >= n / 2))
                {
                    res[i][j] = 1;
                }
                else if ((i < n / 2) && (j < n / 2))
                {
                    res[i][j] = 1;
                }
                else
                {
                    res[i][j] = 0;
                }
            }
        }
        return res;
    }

    vector<vector<int>> init_group()
    {
        vector<vector<int>> res2d(n, vector<int>(n, 0));
        double h = 0.5;
        int i = 0;
        random_device rd;
        mt19937 gen(rd());
        bernoulli_distribution d(s);
        bernoulli_distribution b(1 - s);
        int sum = 0;
        for (int x = 0; x < n; ++x)
        {
            for (int y = 0; y < n; ++y)
            {
                if (i < size / 2)
                    res2d[y][x] = d(gen);
                else
                    res2d[y][x] = b(gen);
                i++;
            }
        }
        return res2d;
    }

    vector<int> init_coop_num()
    {
        vector<int> res(2);
        res[0] = 0;
        res[1] = 0;
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (behavior[i][j] == 1)
                {
                    res[group[i][j]] += 1;
                }
            }
        }

        return res;
    }

    vector<vector<int>> init_neighbor_buf()
    {
        vector<vector<int>> res(5, vector<int>(2));
        return res;
    }

    int get_coop_num()
    {
        return coop_num_grp[0] + coop_num_grp[1];
    }

    int get_majority(int group)
    {
        return (coop_num_grp[group] > size / 4) ? 1 : 0;
    }

    bool is_majority(int x, int y)
    {
        return behavior[x][y] == get_majority(group[x][y]);
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
            res *= (double)(1 - u[group[x][y]]);
        }
        return res;
    }
};

int main()
{
    const int n = 600;
    const double s = 0.5;
    const double r = 3.0; //{3.50, 3.55, 3.60, 3.65, 3.70, 3.75, 3.80, 3.85, 3.90, 3.95, 4.00, 4.05, 4.10, 4.15, 4.20, 4.25, 4.30, 4.35, 4.40, 4.45, 4.50, 4.55, 4.60, 4.65, 4.70, 4.75, 4.80, 4.85, 4.90, 4.95, 5.00};
    const double u0_list[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1};
    const double u1_list[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1}; //{0.000, 0.025, 0.050, 0.075, 0.100, 0.125, 0.150, 0.175, 0.200, 0.225, 0.250, 0.275, 0.300, 0.325, 0.350, 0.375, 0.400, 0.425, 0.450, 0.475, 0.500};
    const int filter_size = 11;
    const int steps = 32000000;
    const int n_trails = 8;

    char filename[128];
    snprintf(filename, sizeof(filename), "result_b_mix_n_%d_s_%.2f_steps_%d_randseed_%d.csv",
             n, s, steps, RANDOM_SEED);
    FILE *result_file = fopen(filename, "w");

    fprintf(result_file, "n, steps, s, u0, u1, r");
    for (int i = 0; i < n_trails; i++)
    {
        fprintf(result_file, ", n_trail%d", i);
    }
    fprintf(result_file, "\n");

    int i, step;
    double coop_ratio_res[n_trails];
    for (auto u0 : u0_list)
    {
        for (auto u1 : u1_list)
        {
            for (i = 0; i < n_trails; i++)
            {
                Grid pgg_grid = Grid(n, s, r, u0, u1, filter_size, RANDOM_SEED + i);
                for (step = 0; step < steps; step++)
                {
                    pgg_grid.step();
                    if (pgg_grid.z >= 1000)
                        break;
                }
                fprintf(stderr, "trail %d | r = %f | u0 = %f | u1 = %f| coop_ratio = %f\n", i, r, u0, u1, pgg_grid.get_coop_ratio());
                coop_ratio_res[i] = pgg_grid.get_coop_ratio();
            }
            fprintf(result_file, "%d, %d, %.2f, %.2f, %.2f, %.2f", n, steps, s, u0, u1, r);
            for (i = 0; i < n_trails; i++)
            {
                fprintf(result_file, ", %.4f", coop_ratio_res[i]);
            }
            fprintf(result_file, "\n");
        }
    }

    return 0;
}