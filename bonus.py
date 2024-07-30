from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
from sklearn import datasets

iris = datasets.load_iris()
inertia_arr = []

for K in range(1, 11):
    kmeans = KMeans(n_clusters=K, random_state=0).fit(iris.data)
    inertia_arr.append(kmeans.inertia_)

K_arr = [x for x in range(1,11)]

plt.plot(K_arr, inertia_arr)
plt.title('Elbow Method for selection of optimal K clusters')
plt.xlabel('K')
plt.ylabel('Average Dispersion')
plt.savefig('elbow.png')