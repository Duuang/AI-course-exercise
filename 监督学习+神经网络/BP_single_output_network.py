###########################################
# ��Դ��httpsgithub.com527515025My-TensorFlow-tutorials
#     ��л��
# ԭ�汾�����������޸�
# ����������1����������10��������
#################################################

from __future__ import absolute_import  
from __future__ import division  
from __future__ import print_function  
  
import gzip  
import os  
import tempfile  
  
import numpy  
#from six.moves import urllib  
#from six.moves import xrange  # pylint disable=redefined-builtin  
import tensorflow as tf  
from tensorflow.contrib.learn.python.learn.datasets.mnist import read_data_sets

# -- coding utf-8 --
# ��д����ʶ��
# ׼ȷ�� 0.8802
import input_data  
import tensorflow as tf
mnist = read_data_sets(MNIST_data, one_hot=True)  

# ����񾭲�ĺ���def add_layer(),�����ĸ�����������ֵ������Ĵ�С������Ĵ�С�ͼ��������������趨Ĭ�ϵļ���������None��Ҳ�������Ժ���
def add_layer(inputs, in_size, out_size, activation_function=None)
	# ����Ȩ��,������һ���������
	# ��Ϊ�����ɳ�ʼ����ʱ���������(normal distribution) ���ȫ��Ϊ0Ҫ�úܶ࣬�������������weights ��һ�� in_size��, out_size�е������������   
    Weights = tf.Variable(tf.random_normal([in_size, out_size]))
	# �ڻ���ѧϰ�У�biases���Ƽ�ֵ��Ϊ0������������������0�����Ļ������ּ���0.1��
    biases = tf.Variable(tf.zeros([1, out_size]) + 0.1)
	# ����Wx_plus_b, ��������δ�����ֵ(Ԥ���ֵ)�����У�tf.matmul()�Ǿ���ĳ˷���
    Wx_plus_b = tf.matmul(inputs, Weights) + biases
	# activation_function �����������������������Ƿ����Է��̣�ΪNoneʱ(���Թ�ϵ)��������ǵ�ǰ��Ԥ��ֵ����Wx_plus_b��
	# ��ΪNoneʱ���Ͱ�Wx_plus_b����activation_function()�����еõ������
    if activation_function is None
        outputs = Wx_plus_b
    else
    	# �������
        outputs = activation_function(Wx_plus_b)
    return outputs

	# ���һ���񾭲�ĺ�������def add_layer()�Ͷ�����ˡ�


def compute_accuracy(v_xs,v_ys)
	# ���� prediction Ϊȫ�ֱ���
	global prediction
	# �� xs data �� prediction ������Ԥ��ֵ��Ԥ��ֵҲ��һ�� 110 �ľ��� ��ÿ��ֵ�ĸ��ʣ�������һ��0-9 ��ֵ����0-9 ÿ��ֵ�ĸ��� ������˵3���λ�õĸ�����ߣ���ôԤ��3�������ͼƬ��ֵ
	y_pre = sess.run(prediction, feed_dict={xs v_xs})
	# �Ա��ҵ�Ԥ��ֵy_pre ����ʵ���� v_ys �Ĳ��
	correct_prediction = tf.equal(tf.argmax(y_pre,1), tf.argmax(v_ys,1))
	# ��������һ���������ж��ٸ�Ԥ���ǶԵģ����ٸ��Ǵ��
	accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
	# result ��һ���ٷֱȣ��ٷֱ�Խ�ߣ�Ԥ��Խ׼ȷ
	result = sess.run(accuracy, feed_dict={xs v_xs, ys v_ys})
	return result   

xs = tf.placeholder(tf.float32,[None, 784]) #ͼ����������  ÿ��ͼƬ��784 ��28 ��28�� �����ص�
ys = tf.placeholder(tf.float32, [None,10]) #ÿ��������10 �����

prediction = add_layer(xs, 784, 10, activation_function=tf.nn.softmax)

#loss�����������Ż�Ŀ�꺯����ѡ�ý����غ�������������������Ԥ��ֵ����ʵֵ�����Ƴ̶ȣ������ȫ��ͬ�����ǵĽ����ص����� ,����loss ԽС ѧ�ĺ�
#����һ�㶼�� softmax�� cross_entropy
cross_entropy = tf.reduce_mean(-tf.reduce_sum(ys  tf.log(prediction),
reduction_indices=[1]))
# cross_entropy = -tf.reduce_sum(ystf.log(prediction))  

global_step = tf.Variable(0)
##############################################################
# ���˸�ѧϰ�ʵ�˥������ֹ�����
learning_rate = tf.train.exponential_decay(0.5, global_step, 10, 0.95, staircase=True)
add_global = global_step.assign_add(1)
#############################################################
#train���������Ż��㷨�������ݶ��½�����  �Ż��� ����û���ѧϰ��������׼ȷ�ʡ� tf.train.GradientDescentOptimizer()�е�ֵ��ѧϰ��Ч�ʣ�ͨ����С��1
train_step = tf.train.GradientDescentOptimizer(learning_rate).minimize(cross_entropy)
sess = tf.Session()

# ��ʼ������
init= tf.global_variables_initializer()
sess.run(init)

for i in range(10001)
	#��ʼtrain��ÿ��ֻȡ100��ͼƬ���������̫��ѵ��̫��
	batch_xs, batch_ys = mnist.train.next_batch(50)
	sess.run(train_step, feed_dict={xs batch_xs, ys batch_ys})
    ##########################################################################
	if i % 10 == 0
		_= sess.run(add_global)
    #########################################################################
	if i % 100 == 0
		print('step ' + repr(i) + ' ')
		
		print(compute_accuracy(
            mnist.test.images, mnist.test.labels))