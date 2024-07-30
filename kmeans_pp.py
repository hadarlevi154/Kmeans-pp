import numpy as np
import pandas as pd
import sys
import mykmeanssp as km


def distance_data_points(x1, x2):
    distance = 0
    for k in range(len(x1)):
        distance += (x1[k] - x2[k]) ** 2
    return distance


def find_min_distance(x_i, centroids_arr, length):
    min_dis = distance_data_points(x_i, centroids_arr[0])
    for m in range(length):
        curr_dis = distance_data_points(x_i, centroids_arr[m])
        if curr_dis < min_dis:
            min_dis = curr_dis

    return min_dis


def print_centroids(cents):

    cents = np.round(cents, decimals= 4)
    for j in range(len(cents)):
        for v in range(len(cents[0])):
            if v == len(cents[0]) - 1:
                print(str(cents[j][v]), end='')
            else:
                print(str(cents[j][v]) + ",", end='')
        print("")


# default value
max_iter = 300

# check that command line arguments are ok
if len(sys.argv) < 4 or len(sys.argv) > 5:
    print("illegal arguments")
    exit()
try:
    K = int(sys.argv[1])
    if len(sys.argv) == 5:
        max_iter = int(sys.argv[2])
        file_name_1 = sys.argv[3]
        file_name_2 = sys.argv[4]

    else:
        file_name_1 = sys.argv[2]
        file_name_2 = sys.argv[3]


except ValueError:
    print("illegal arguments")
    exit()
if K <= 0 or max_iter <= 0:
    print("illegal arguments")
    exit()

if not file_name_1.endswith(".txt") and not file_name_1.endswith(".csv"):
    print("illegal arguments")
    exit()

if not file_name_2.endswith(".txt") and not file_name_2.endswith(".csv"):
    print("illegal arguments")
    exit()

data_points_1 = pd.read_csv(file_name_1, header=None)
data_points_2 = pd.read_csv(file_name_2, header=None)

data_points_df = pd.merge(data_points_1, data_points_2, on=0)
data_points_df.set_index(0, inplace=True)
data_points_df.sort_index(inplace=True)

d = len(data_points_df.columns)
N = len(data_points_df.index)

# check if K is ok
if K >= N:
    print("illegal arguments")
    exit()

data_points_arr = data_points_df.to_numpy()
centroids = np.zeros((K, d))

# find random index for first centroid
np.random.seed(0)
rand_index = np.random.choice([x for x in range(N - 1)])
print(str(rand_index)+",", end='')
centroids[0] = data_points_arr[rand_index]

for z in range(1, K):

    D_arr = np.zeros(N)
    for i in range(N):
        D_arr[i] = find_min_distance(data_points_arr[i], centroids, z)

    sum_D_arr = np.sum(D_arr)
    prob = D_arr / sum_D_arr
    rand_index = np.random.choice(N, p=prob)

    if z == K - 1:
        print(rand_index)
    else:
        print(str(rand_index)+",", end='')

    centroids[z] = data_points_arr[rand_index]

centroids = km.fit(K, N, d, max_iter, data_points_arr.tolist(), centroids.tolist())
print_centroids(centroids)
