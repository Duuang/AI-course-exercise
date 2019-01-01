######################################
#来源：https://cloud.tencent.com/developer/ask/31650
#    感谢！
#########################################、

import numpy as np
import tensorflow as tf
from random import randint
from collections import Counter
from tensorflow.examples.tutorials.mnist import input_data

mnist = input_data.read_data_sets("MNIST_data/")
X, y, k = mnist.test.images, mnist.test.labels, 10
#所以在这里X是我要聚类的数据(10000, 784)，y是真正的类别，代码如下：

# select random points as a starting position. You can do better by randomly selecting k points.
start_pos = tf.Variable(X[np.random.randint(X.shape[0], size=k),:], dtype=tf.float32)
centroids = tf.Variable(start_pos.initialized_value(), 'S', dtype=tf.float32)

# populate points
points           = tf.Variable(X, 'X', dtype=tf.float32)
ones_like        = tf.ones((points.get_shape()[0], 1))
prev_assignments = tf.Variable(tf.zeros((points.get_shape()[0], ), dtype=tf.int64))

# find the distance between all points: http://stackoverflow.com/a/43839605/1090562
p1 = tf.matmul(
    tf.expand_dims(tf.reduce_sum(tf.square(points), 1), 1),
    tf.ones(shape=(1, k))
)
p2 = tf.transpose(tf.matmul(
    tf.reshape(tf.reduce_sum(tf.square(centroids), 1), shape=[-1, 1]),
    ones_like,
    transpose_b=True
))
distance = tf.sqrt(tf.add(p1, p2) - 2 * tf.matmul(points, centroids, transpose_b=True))

# assign each point to a closest centroid
point_to_centroid_assignment = tf.argmin(distance, axis=1)

# recalculate the centers
total = tf.unsorted_segment_sum(points, point_to_centroid_assignment, k)
count = tf.unsorted_segment_sum(ones_like, point_to_centroid_assignment, k)
means = total / count

# continue if there is any difference between the current and previous assignment
is_continue = tf.reduce_any(tf.not_equal(point_to_centroid_assignment, prev_assignments))

with tf.control_dependencies([is_continue]):
    loop = tf.group(centroids.assign(means), prev_assignments.assign(point_to_centroid_assignment))

sess = tf.Session()
sess.run(tf.global_variables_initializer())

# do many iterations. Hopefully you will stop because of has_changed is False
has_changed, cnt = True, 0
while has_changed and cnt < 300:
    cnt += 1
    has_changed, _ = sess.run([is_continue, loop])

# see how the data is assigned
res = sess.run(point_to_centroid_assignment)
#现在是时候检查我们的聚类效果有多好了。要做到这一点，我们将把聚类中出现的所有实数组合在一起。在随机聚类的情况下，每个值将在组中大致相同地表示。

nums_in_clusters = [[] for i in range(10)]
for cluster, real_num in zip(list(res), list(y)):
    nums_in_clusters[cluster].append(real_num)

for i in range(10):
    print(Counter(nums_in_clusters[i]).most_common(3))