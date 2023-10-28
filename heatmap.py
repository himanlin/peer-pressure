# -*- coding: utf-8 -*-
"""
Created on Wed Feb 15 15:08:18 2023

@author: mlchen
"""

import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

import seaborn as sns

df[["pre", "syn"]] = df[["pre", "syn"]].apply(pd.to_numeric)
sns.heatmap(df.pivot(index='Y', columns='X', values='Z'),cmap='inferno')
# Create a dataset
df=pd.read_csv('C:/Users/mlchen/Desktop/peer/optresult/Book2.csv')
df.index = [1,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1,0]

df=df.drop(['Unnamed: 0'], axis=1)
# plot using a color palette

fig, ax = plt.subplots(figsize=[10,10])
sns.set(font_scale=1.5) 

im=ax.imshow(df,
          origin='lower',
          cmap='inferno', 
      #    interpolation='bilinear'
          )
#ax.xaxis.set_ticks(values_list)

ax.set_xticks([0,2,4,6,8,10,12,14])
ax.set_xticklabels([1.5,2,2.5,3,3.5,4,4.5,5])
ax.set_yticks([0,2,4,6,8,10])
ax.set_yticklabels([0,0.2,0.4,0.6,0.8,1.0])
plt.xlabel('synergy, r')
plt.ylabel('peer pressure, $\mu$')
cb = fig.colorbar(im, shrink=0.4)
cb.ax.set_ylabel('fractions')
plt.show()


df2=pd.read_csv('C:/Users/mlchen/Desktop/peer/opt_sep/f1test/result_2t075_n_300_s_0.50_steps_5000000_randseed_8879.csv')
df3=pd.read_csv('C:/Users/mlchen/Desktop/peer/opt_sep/f1test/result_2t025_n_300_s_0.50_steps_5000000_randseed_8879.csv')

plt.figure(figsize=(12, 6))
ax = sns.heatmap(df2.pivot(index='pre', columns='syn', values='ave')[::-1], cmap='inferno', vmin=0, vmax=1)
sns.set(font_scale=1.8)
# Set the title
ax.set_title('threshold = 0.5')

# Set custom x and y-axis titles
ax.set_xlabel('synergy')
ax.set_ylabel('pressure')

y_labels = ax.get_yticklabels()
for i, label in enumerate(y_labels):
    if i % 2 != 0:  # Skip every other label (change the condition as needed)
        label.set_visible(False)
x_labels = ax.get_xticklabels()
for i, label in enumerate(x_labels):
    if i % 2 != 0:  # Skip every other label (change the condition as needed)
        label.set_visible(False)
# Show the plot
plt.show()

