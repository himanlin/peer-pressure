import csv
import numpy as np
import matplotlib.pyplot as plt

class PGG_Grid():
    DEFT = 0
    COOP = 1
    def __init__(self, n: int, s: float, r: float, u: float, random_seed: int, filter_size=11):
        """
        n: grid size
        s: segregation level
        r: synergy level
        u: peer pressure
        random_seed: random seed for each experiment
        """
        np.random.seed(random_seed)

        self.n = n
        self.r = r
        self.u = u 
        self.filter_size = filter_size
        self.size = n*n
        self.behavior = np.random.choice([PGG_Grid.DEFT, PGG_Grid.COOP], (self.n, self.n))
        self.filter = self.construct_filter(filter_size)
        self.filter_sum = np.sum(self.filter)

        self.coop_num = np.sum(self.behavior)

        self.z = 0
        
    @staticmethod
    def construct_filter(size):
        center_x = center_y = int(size/2)
        center_val = int((size+1)/2)
        ar = np.array([[max(0, center_val - abs(i-center_x) - abs(j-center_y)) for j in range(size)] for i in range(size)])
        ar[center_x][center_y] = 0.0
        return ar
    
    def get_coop_num(self) -> int:
        return self.coop_num
    
    @staticmethod
    def single_point_convolve(arr: np.ndarray, filter: np.ndarray, x: int, y: int) -> int:
        size = filter.shape[0]
        radius = int(size/2)

        conv = 0
        for i in range(x-radius, x+radius+1):
            for j in range(y-radius, y+radius+1):
                conv += arr[i%n][j%n] * filter[i-(x-radius)][j-(y-radius)]
        return conv
    
    def get_majority(self, x, y) -> int:
        return PGG_Grid.COOP if self.single_point_convolve(self.behavior, self.filter, x, y) / self.filter_sum > 0.5 else PGG_Grid.DEFT

    def is_majority(self, x, y) -> bool:
        return self.behavior[x][y] == self.get_majority(x, y)

    def get_neighbor(self, x, y) -> list:
        return [(x, y),
                ((x-1)%self.n, y),
                ((x+1)%self.n, y),
                (x, (y-1)%self.n),
                (x, (y+1)%self.n)]
    
    def payoff_center(self, x, y) -> float:
        payoff = np.mean([self.behavior[nei_x][nei_y] for (nei_x, nei_y) in self.get_neighbor(x, y)]) * self.r
        return payoff
    
    def payoff(self, x, y):
        payoff = np.sum([self.payoff_center(nei_x, nei_y) for (nei_x, nei_y) in self.get_neighbor(x, y)])
        payoff -= self.behavior[x][y] * 5

        if not self.is_majority(x, y):
            payoff *= 1 - self.u
        payoff = round(payoff, 3)
        return payoff
    
    def step(self):
        coop_num = self.get_coop_num()
        if coop_num / self.size < 0.01:
            self.z += 1
        
        if coop_num != 0 and coop_num != self.size:
            ag1_x, ag1_y = np.random.choice(self.n, 2)
            ag2_x, ag2_y = -1, -1

            payoff1 = self.payoff(ag1_x, ag1_y)
            for (nei_x, nei_y) in self.get_neighbor(ag1_x, ag1_y):
                if self.behavior[ag1_x][ag1_y] != self.behavior[nei_x][nei_y]:
                    ag2_x, ag2_y = nei_x, nei_y
                    break;
            
            if ag2_x != -1:
                payoff2 = self.payoff(ag2_x, ag2_y)
                im = 1 / (1 + np.exp((payoff2-payoff1)/0.5))

                # ag2 imitate ag1
                if np.random.uniform() < im:
                    if self.behavior[ag1_x][ag1_y] == 0:
                        self.behavior[ag2_x][ag2_y] = 0
                        self.coop_num -= 1
                    if self.behavior[ag1_x][ag1_y] == 1:
                        self.behavior[ag2_x][ag2_y] = 1
                        self.coop_num += 1
        else:
            self.z = 1000

        

if __name__ == "__main__":
    n = 100
    s = 0.5
    r_list = [3.5, 3.75, 4.0, 4.25, 4.5, 4.75, 5.0]
    u = 0.0
    steps = 500000
    n_trail = 5
    random_seed = 4789

    f = open("result_dist.csv", 'w', newline='')
    writer = csv.writer(f)
    writer.writerow(["n", "steps", "s", "u", "r"] + ["trail{}".format(t) for t in range(n_trail)])

    for r in r_list:
        coop_ratio_list = list()
        for trail in range(n_trail):
            pgg = PGG_Grid(n, s, r, u, random_seed + trail)
            # print("step 0 | coop_num = {}".format(pgg.get_coop_num()))
            for step in range(steps):
                pgg.step()
                if pgg.z >= 1000:
                    break
                # print("step {} | coop_num = {}".format(step, pgg.get_coop_num()))
            print("trail {} | r = {} | coop_ratio = {}".format(trail, r, pgg.get_coop_num()/pgg.size))
            coop_ratio_list.append(pgg.get_coop_num()/pgg.size)
        writer.writerow([n, steps, s, u, r] + coop_ratio_list)
    