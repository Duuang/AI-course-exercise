import tensorflow as tf
import numpy as np
from tensorflow.examples.tutorials.mnist import input_data

tf.logging.set_verbosity(tf.logging.INFO)

# ������ʾ��Ϣ�ĵȼ� menu['1','2','3'] Ĭ��'1'��ʾ������Ϣ,'2'ֻ��ʾ warning �� Error,'3'ֻ��ʾ Error
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

# ��������Ԫ�����Ĳ���
#
num_of_output_neuros = 10
num_of_hidden_neuros = 200
# ����tensorflow��GPU��ʹ�ð������
config = tf.ConfigProto(allow_soft_placement=True)

# �������ݼ�
mnist = input_data.read_data_sets("MNIST_data/", one_hot=True)                        # ���ⲿ���ݶ�ȡ��data����

# Ȩֵ��ʼ��
def weight_variable(shape):
    # ����̬�ֲ�����ʼ��Ȩֵ
    initial = tf.truncated_normal(shape, stddev=0.1)
    return tf.Variable(initial)

def bias_variable(shape):
    # ��������relu�������������һ����С����ƫ�ýϺ�
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


# 1.��ʧ������cross_entropy
cross_entropy = -tf.reduce_sum(y_ * tf.log(y_pre))
# 2.�Ż�������AdamOptimizer, �Ż��ٶ�Ҫ�� GradientOptimizer ��ܶ�
train_step = tf.train.GradientDescentOptimizer(0.01).minimize(cross_entropy)

# 3.Ԥ��������
#��Ԥ��ֵ�����ֵ�����������������Ƿ����ԭʼ��ǩ�еģ�������λ�á�argmax()ȡ���ֵ���ڵ��±�
correct_prediction = tf.equal(tf.argmax(y_pre, 1), tf.argmax(y_, 1))  
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))


# ����ͼ (graph)
with tf.Session(config=config) as sess:
  # ��ʼ������
  sess.run(tf.global_variables_initializer())
  # ���ŵ����˲��� 10 �� epoch�� ѵ��׼ȷ���Ѿ��ﵽ��0.98
  for i in range(50000):
    x, y = mnist.train.next_batch(20)
    train_step.run(feed_dict={X_: x, y_: y})
    if (i+1) % 200 == 0 :
        train_accuracy = accuracy.eval(feed_dict={X_: mnist.train.images, y_: mnist.train.labels})
        print('step:',i+1, '   ', 'training acc:', train_accuracy)
    #if (i+1) % 1000 == 0:
    #    test_accuracy = accuracy.eval(feed_dict={X_: mnist.test.images, y_: mnist.test.labels})
    #    print('step:',i+1, '   ', 'training acc:', test_accuracy)