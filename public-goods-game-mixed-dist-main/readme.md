# pgg-mixed

The code experiments with the spatial public good game with peer pressure.

The public goods game (PGG) participants are arranged on the nodes of n x n lattices, which is the conceptualization of the social structure of agents.
Those agents are defined as having von Neumann neighborhoods, in which their distance is the sum of the absolute differences of their coordinates, which equals 1.
Accordingly, they have four neighbors located up, down, left, and right of them.
Every lattice point is occupied by a player. As a result, there are n x n agents in total.
Besides, all of the population is divided into two groups with equal numbers of players.
First, the agents on the upper half of the grids belong to the first group, and those on the bottom half are in the other group.
Then, we will tweak the spatial distribution of two groups with parameter s stands for spatial segregation level (0: fully segregated, 0.5: fully mixed) to examine the impact of social segregation and compare the outcomes of asymmetry in group size or group pressure.

The agent is either a cooperator or a defector in a PGG with its four closest neighbors.
Their initial behavior is randomly assigned. At the beginning of each iteration,
each cooperator contributes c_i=1 to the common pool when defectors do not endow anything.
The accumulated contributions are then multiplied by the synergy level r and distributed equally to the 5 participants.
Besides, the majority of behavior within the group would be recorded.
If the agent’s behavior is differed from most of the member in its group, it would face the peer pressure μ which discount on its payoff.
In other word, the player’s payoff would times (1-μ) if it encounters social pressure from its group.
Thus, when the pressure increases, the minority would gain less payoff.

Furthermore, in an iteration of simulation, there is also the potential for behavioral change in the Monte Carlo procedure.
There would be one random player with the potential for behavioral change in each step.
The change in behavior is based on the imitation process, in which the probability of a player x adopting the strategy of a randomly selected neighbor y is based on the difference in their payoffs W(s_x,s_y )=1/(1+exp[((P_x-P_y ))⁄K] ), which is a Fermi function (Szabó & Tőke, 1998) where K=0.5 represents the noise factor,
P_x and P_y are the payoffs of player x and player y. And, during each simulation, there will be about 1 million steps in the interaction process; if all the agents choose the same strategy, the simulation will come to an end.

# Run Experiments

## C++
```bash
# replace `g++` with `clang++` if your're using Mac

cd cpp

# pgg_mix
g++ -o pgg_mix_opt.out pgg_mix_opt.cpp -Wall -std=c++11
./pgg_mix_opt.out

# pgg_dist
g++ -o pgg_dist_opt.out pgg_dist_opt.cpp -Wall -std=c++11
./pgg_dist_opt.out
```

## Python
Just run the .py file with `python` or `python3`
```bash
cd python

python3 pgg_mix_opt.py
python3 pgg_dist_opt.py
```