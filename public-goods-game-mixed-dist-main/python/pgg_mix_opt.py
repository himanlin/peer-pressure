import csv
import numpy as np
import matplotlib.pyplot as plt

class PGG_Grid():
    DEFT = 0
    COOP = 1

    def __init__(self, n: int, s: float, r: float, u0: float, u1: float, random_seed: int):
        """
        n: grid size
        s: segregation level
        r: synergy level
        u0: peer pressure for group 0
        u1: peer pressure for group 1
        random_seed: random seed for each experiment
        """
        np.random.seed(random_seed)

        self.n = n
        self.r = r
        self.u = [u0, u1]
        self.size = n*n
        self.behavior = np.random.choice([PGG_Grid.DEFT, PGG_Grid.COOP], self.size)
        self.group = self.build_group(n, s)

        coop_num_g0 = np.sum(np.dot(np.ones(self.group.shape, dtype=int) - self.group, self.behavior))
        coop_num_g1 = np.sum(np.dot(self.group, self.behavior))
        self.coop_num = [coop_num_g0, coop_num_g1]

        self.z = 0
        
    @staticmethod
    def build_group(n: int, s: float) -> np.ndarray:
        up = np.array([0]*int(np.ceil(n*n*(1-s)/2)) + [1]*int(n*n*s/2))
        down = np.array([0]*int(n*n*s/2) + [1]*int(np.ceil(n*n*(1-s)/2)))
        np.random.shuffle(up)
        np.random.shuffle(down)
        group = np.concatenate((up, down))
        return group
    
    def get_coop_num(self) -> int:
        return self.coop_num[0] + self.coop_num[1]
    
    def get_majority(self, group: int) -> int:
        return 1 if self.coop_num[group] / (self.size/2) > 0.5 else 0

    def is_majority(self, x: int) -> bool:
        return self.behavior[x] == self.get_majority(self.group[x])

    def get_neighbor(self, x: int) -> list:
        return [x,
                (x-self.n+self.size)%self.size,
                (x+self.n)%self.size,
                (x-1+self.size)%self.size,
                (x+1)%self.size]
    
    def payoff_center(self, x: int) -> float:
        y = np.mean([self.behavior[nei] for nei in self.get_neighbor(x)]) * self.r
        return y
    
    def payoff(self, x: int) -> float:
        y = np.sum([self.payoff_center(nei) for nei in self.get_neighbor(x)])
        y -= self.behavior[x] * 5

        if not self.is_majority(x):
            y *= 1 - self.u[self.group[x]]
        y = round(y, 3)
        return y
    
    def step(self) -> None:
        coop_num = self.get_coop_num()
        if coop_num / self.size < 0.01:
            self.z += 1
        
        if coop_num != 0 and coop_num != self.size:
            ag1 = np.random.choice(self.size)
            ag2 = -1

            payoff1 = self.payoff(ag1)
            for nei in self.get_neighbor(ag1):
                if self.behavior[ag1] != self.behavior[nei]:
                    ag2 = nei
                    break;
            
            if ag2 != -1:
                payoff2 = self.payoff(ag2)
                im = 1 / (1 + np.exp((payoff2-payoff1)/0.5))

                # ag2 imitate ag1
                if np.random.uniform() < im:
                    if self.behavior[ag1] == 0:
                        self.behavior[ag2] = 0
                        self.coop_num[self.group[ag2]] -= 1
                    if self.behavior[ag1] == 1:
                        self.behavior[ag2] = 1
                        self.coop_num[self.group[ag2]] += 1
        else:
            self.z = 1000

        


if __name__ == "__main__":
    n = 100
    s = 0.5
    r_list = [3.5, 3.75, 4.0, 4.25, 4.5, 4.75, 5.0]
    u0 = 0.0
    u1 = 0.0
    steps = 500000
    n_trail = 5
    random_seed = 4789
    
    f = open("result_mix.csv", 'w', newline='')
    writer = csv.writer(f)
    writer.writerow(['n', 'steps', 's', 'u0', 'u1', 'r'] + ["trail{}".format(t) for t in range(n_trail)])

    for r in r_list:
        coop_ratio_list = list()
        for trail in range(n_trail):
            pgg = PGG_Grid(n, s, r, u0, u1, random_seed + trail)
            # print("step 0 | coop_num = {}".format(pgg.get_coop_num()))
            for step in range(steps):
                pgg.step()
                if pgg.z >= 1000:
                    break
                # print("step {} | coop_num = {}".format(step, pgg.get_coop_num()))
            print("trail {} | r = {} | coop_ratio = {}".format(trail, r, pgg.get_coop_num()/pgg.size))
            coop_ratio_list.append(pgg.get_coop_num()/pgg.size)
        writer.writerow([n, steps, s, u0, u1, r] + coop_ratio_list)
            
        
        