# -*- coding: utf-8 -*-
"""
Created on Sun Oct 23 13:58:54 2022

"""

import datetime
import random
import numpy as np
import pandas as pd
from pandas.core.frame import DataFrame
from scipy import signal
import matplotlib.pyplot as plt


class PGG_Grid():
    def __init__(self, n, s):
        # 1:cooperate, 0:defect, choice->strategy of player
        choices=[0,1]
        probs_b=[0.5,0.5]
        self.array=np.random.choice(choices, (n,n), p=probs_b)

        self.fraction=[sum(self.array.flatten())]
        self.z=0
        # s: spatial segregation level of group0 & group1  (0: fully segregated, 0.5: fully mixed)
        up=[0]*int(np.ceil(n*n*(1-s)/2))+[1]*int(n*n*s/2)
        random.shuffle(up)
        upar = np.array(up)
        upar=upar.reshape(int(n/2),n)
        down=[0]*int(n*n*s/2)+[1]*int(np.ceil(n*n*(1-s)/2))
        random.shuffle(down)
        downar = np.array(down)
        downar =downar.reshape(int(n/2),n)
        self.group=np.concatenate((upar,downar), axis=0)
    
    # r: synergy, u: peer pressure [u1,u2]
    def step(self, r, n, u):
        
        choice=self.array.flatten()
        
        # calculating % of cooperators in each group
        
        # (Jun): this is the part that make this code slow.
        # Each step it takes n*n calculation steps, the size of the network, to obtain the # of cooperator
        # You can instead count the # of cooperator at initialization and for each following step
        # update the # when an agent imitates its neighbor, which takes only 1 step
        group=self.group.flatten()
        group0=np.dot(np.ones(n*n, dtype=int)-group,choice)
        cop0=group0/(n*n-sum(group))
        group1=np.dot(group,choice)
        cop1=group1/sum(group)

        # calculate payoff given center player is x
        def payoff_center(x, n):
            nn=n*n
            Neighbour=[x,(x-n+nn)%nn, (x+n)%nn, (x+1)%nn, (x-1+nn)%nn]
            y=0
            for i in Neighbour:
                y+=choice[i]
            y=round(y*r/5,2)
            return y
        
        # calculate payoff given player is x with it five pgg with its neighbors
        def payoff(x, n, pressure):
            nn=n*n
            Nei=[x,(x-n+nn)%nn, (x+n)%nn, (x+1)%nn, (x-1+nn)%nn]
            y=0
            for i in Nei:
                y+=payoff_center(i,n)
            y=y-choice[x]*5

            #payoff would time 1-pressure if its strategy is not the majority in its group
            if(group[x]==0):
                if(choice[x]==1 and cop0<0.5):
                    y*=1-pressure[0]
                elif(choice[x]==0 and cop0>=0.5):
                    y*=1-pressure[0]
                    
            elif(group[x]==1):
                if(choice[x]==1 and cop1<0.5):
                    y*=1-pressure[1]
                elif(choice[x]==0 and cop1>=0.5):
                    y*=1-pressure[1]

            y=round(y,3)
            
            return y

            
            
        nn=n*n
        f=sum(choice)

        # if coop ratio is smaller than 0.01 for over 1000 rounds, exp end
        if f/(n^2)<0.01:
            self.z+=1
        
        #cooperator ratio
        self.fraction.append(sum(choice))

        #list of cooperator
        coop=np.where(choice==1)[0]

        if(len(coop)!=0 and len(coop)!=nn):
            # choose an agent
            i1=np.random.choice(nn)
            i2=-1

            payoff1=payoff(i1,n, u)

            # choose a neighbor with different strategy
            Neighbour=[(i1-n+nn)%nn, (i1+n)%nn, (i1+1)%nn, (i1-1+nn)%nn]
            for j in Neighbour:
                if (i1 in coop and j not in coop):
                    i2=j
                    break;
                elif(i1 not in coop and j in coop):
                    i2=j
                    break;

            #imitation with fermi function probability
            if(i2!=-1):
                x2=i2%n
                y2=i2//n
                payoff2=payoff(i2,n,u)
                im=1/(1+np.exp( (payoff2-payoff1)/0.5 ) )
                if np.random.binomial(size=1,n=1,p=im )[0]==1:
                    if i1 in coop:
                        self.array[y2][x2]=1
                    else:
                        self.array[y2][x2]=0

        else:
            self.z=1000

        
if __name__ == "__main__":
    start_time = datetime.datetime.now() 

    #testing different parameters and saved the result to csv file
    nn=100
    syn=[4]
    u1=0.1
    u2=0.1
    df=DataFrame()

    for syne in syn:
        print(u1,u2,syne)
        result=[u1,u2,syne]
        for j in range(1):
            print(j)
            grid=PGG_Grid(nn, 0.5)
            for i in range(10000):
                if(i%10000==0):
                    print(i)
                if (grid.z>=1000):
                    break;
                grid.step(r=syne,n=nn,u=[u1,u2])
            result.append(grid.fraction[-1]/(nn*nn))
        #  plt.plot(grid.fraction)
        #  plt.show()
        resultd=DataFrame(result)
        df = pd.concat([df,resultd.T],axis=0)
    df.to_csv("mix_result.csv")
    print(datetime.datetime.now()-start_time)
