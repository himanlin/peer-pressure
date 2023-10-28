#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include <cassert>
using namespace std;

#define RANDOM_SEED 9975

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
     * Generate a random double within [0, 1), i.e. uniform distribution.
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
    vector<int> behavior;
    vector<int> group;
    vector<int> coop_num_grp;
    int z;

    Grid(int _n, double _s, double _r, double _u0, double _u1, int random_seed)
    {
        srand(random_seed);
        n = _n;
        s = _s;
        r = _r;
        u[0] = _u0;
        u[1] = _u1;
        size = n * n;
        behavior = init_behavior();
        group = init_group();
        coop_num_grp = init_coop_num();
        z = 0;

        neighbors_buf1 = vector<int>(5);
        neighbors_buf2 = vector<int>(5);
        ag1 = -1;
        ag2 = -1;
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
            ag1 = rand_int(0, size - 1);
            ag2 = -1;

            payoff1 = payoff(ag1);
            set_neighbors(neighbors_buf1, ag1);
            for (auto neighbor : neighbors_buf1)
            {
                if (behavior[ag1] != behavior[neighbor])
                {
                    ag2 = neighbor;
                    break;
                }
            }

            if (ag2 != -1)
            {
                payoff2 = payoff(ag2);
                im = 1.0 / (1.0 + exp((payoff2 - payoff1) / 0.5));

                // ag2 imitate ag1
                if (rand_uniform() < im)
                {
                    if (behavior[ag1] == 0)
                    {
                        behavior[ag2] = 0;
                        coop_num_grp[group[ag2]] -= 1;
                    }
                    else if (behavior[ag1] == 1)
                    {
                        behavior[ag2] = 1;
                        coop_num_grp[group[ag2]] += 1;
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
    vector<int> neighbors_buf1;
    vector<int> neighbors_buf2;
    int ag1, ag2;
    double payoff1, payoff2;
    double im;

    vector<int> init_behavior()
    {
        // defect = 0, cooperator = 1
        vector<int> res(size);
        for (int i = 0; i < size; ++i)
        {

            if ((i >= n / 2) && (i / n >= n / 2))
            {
                res[i] = 1;
            }
            else if ((i < n / 2) && (i / n < n / 2))
            {
                res[i] = 1;
            }
            else
            {
                res[i] = 0;
            }
        }

        return res;
    }

    vector<int> init_group()
    {
        vector<int> res(size);
        random_device rd;
        mt19937 gen(rd());
        bernoulli_distribution d(s);
        bernoulli_distribution b(1 - s);
        for (int i = 0; i < n; ++i)
        {
            if (i < size / 2)
                res[i] = d(gen);
            else
                res[i] = b(gen);
            i++;
        }

        return res;
    }

    vector<int> init_coop_num()
    {
        vector<int> res(2);
        res[0] = 0;
        res[1] = 0;
        for (int i = 0; i < size; ++i)
        {
            if (behavior[i] == 1)
            {
                res[group[i]] += 1;
            }
        }

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

    bool is_majority(int x)
    {
        return behavior[x] == get_majority(group[x]);
    }

    void set_neighbors(vector<int> &buf, int x)
    {
        buf[0] = x;
        buf[1] = (x - n + size) % size;
        buf[2] = (x + n + size) % size;
        buf[3] = (x - 1 + size) % size;
        buf[4] = (x + 1 + size) % size;
    }

    double payoff_centor(int x)
    {
        double res = 0;
        set_neighbors(neighbors_buf2, x);
        for (auto neighbor : neighbors_buf2)
        {
            res += (double)behavior[neighbor];
        }
        return res / 5 * r;
    }

    double payoff(int x)
    {
        double res = 0;
        set_neighbors(neighbors_buf1, x);
        for (auto neighbor : neighbors_buf1)
        {
            res += payoff_centor(neighbor);
        }
        res -= behavior[x] * 5;

        if (!is_majority(x))
        {
            res *= (double)(1 - u[group[x]]);
        }
        return res;
    }
};

int main()
{
    const int n = 600;
    const double s = 0.5;
    const double r = 3.0;
    const double u0_list[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1};
    const double u1_list[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1};
    const int steps = 400000000;
    const int n_trails = 8;

    char filename[128];
    snprintf(filename, sizeof(filename), "result_mix_ranb_n_%d_s_%.2f_steps_%d_randseed_%d.csv",
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
                Grid pgg_grid = Grid(n, s, r, u0, u1, RANDOM_SEED + i);
                for (step = 0; step < steps; step++)
                {
                    pgg_grid.step();
                    if (pgg_grid.z >= 1000)
                        break;
                }
                fprintf(stderr, "trail %d | u0 = %f | u1 = %f | coop_ratio = %f\n", i, u0, u1, pgg_grid.get_coop_ratio());
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