import tensorflow as tf
import numpy as np
from tensorflow.examples.tutorials.mnist import input_data

tf.logging.set_verbosity(tf.logging.INFO)

# 设置提示信息的等级 menu['1','2','3'] 默认'1'显示所有信息,'2'只显示 warning 和 Error,'3'只显示 Error
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

# 神经网络神经元个数的参数
#
num_of_output_neuros = 10
num_of_hidden_neuros = 200
# 设置tensorflow对GPU的使用按需分配
config = tf.ConfigProto(allow_soft_placement=True)

# 定义数据集
mnist = input_data.read_data_sets("MNIST_data/", one_hot=True)                        # 将外部数据读取到data里面

# 权值初始化
def weight_variable(shape):
    # 用正态分布来初始化权值
    initial = tf.truncated_normal(shape, stddev=0.1)
    return tf.Variable(initial)

def bias_variable(shape):
    # 本例中用relu激活函数，所以用一个很小的正偏置较好
    initial = tf.constant(0.1, shape=shape)
    return tf.Variable(initial)


# input_layer
X_ = tf.placeholder(tf.float32, [None, 784])
y_ = tf.placeholder(tf.float32, [None, num_of_output_neuros])

# FC1
W_fc1 = weight_variable([784, num_of_hidden_neuros])
b_fc1 = bias_variable([num_of_hidden_neuros])
h_fc1 = tf.nn.relu(tf.matmul(X_, W_fc1) + b_fc1)

# FC2
W_fc2 = weight_variable([num_of_hidden_neuros, num_of_output_neuros])
b_fc2 = bias_variable([num_of_output_neuros])
y_pre = tf.nn.softmax(tf.matmul(h_fc1, W_fc2) + b_fc2)


# 1.损失函数：cross_entropy
cross_entropy = -tf.reduce_sum(y_ * tf.log(y_pre))
# 2.优化函数：AdamOptimizer, 优化速度要比 GradientOptimizer 快很多
train_step = tf.train.GradientDescentOptimizer(0.01).minimize(cross_entropy)

# 3.预测结果评估
#　预测值中最大值（１）即分类结果，是否等于原始标签中的（１）的位置。argmax()取最大值所在的下标
correct_prediction = tf.equal(tf.argmax(y_pre, 1), tf.argmax(y_, 1))  
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))


# 启动图 (graph)
with tf.Session(config=config) as sess:
  # 初始化变量
  sess.run(tf.global_variables_initializer())
  # 这大概迭代了不到 10 个 epoch， 训练准确率已经达到了0.98
  for i in range(50000):
    x, y = mnist.train.next_batch(20)
    train_step.run(feed_dict={X_: x, y_: y})
    if (i+1) % 200 == 0 :
        train_accuracy = accuracy.eval(feed_dict={X_: mnist.train.images, y_: mnist.train.labels})
        print('step:',i+1, '   ', 'training acc:', train_accuracy)
    #if (i+1) % 1000 == 0:
    #    test_accuracy = accuracy.eval(feed_dict={X_: mnist.test.images, y_: mnist.test.labels})
    #    print('step:',i+1, '   ', 'training acc:', test_accuracy)