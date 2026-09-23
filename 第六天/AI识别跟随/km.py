from scipy.optimize import linear_sum_assignment
import numpy as np

# 代价矩阵
cost =np.array([[0.9,0.6,0,0],[0,0.3,0.9,0],[0.5,0.9,0,0],[0,0,0.2,0]])


# 匹配结果：该方法的目的是代价最小，这里是求最大匹配，所以将cost取负数
row_ind,col_ind=linear_sum_assignment(-cost)

#对应的行索引
print("行索引：\n{}".format(row_ind))
#对应行索引的最优指派的列索引
print("列索引：\n{}".format(col_ind))
#提取每个行索引的最优指派列索引所在的元素，形成数组
print("匹配度：\n{}".format(cost[row_ind,col_ind]))
