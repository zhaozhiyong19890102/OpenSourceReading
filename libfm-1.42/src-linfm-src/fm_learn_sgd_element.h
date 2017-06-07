// Copyright (C) 2010, 2011, 2012, 2013, 2014 Steffen Rendle
// Contact:   srendle@libfm.org, http://www.libfm.org/
//
// This file is part of libFM.
//
// libFM is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libFM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libFM.  If not, see <http://www.gnu.org/licenses/>.
//
//
// fm_learn_sgd.h: Stochastic Gradient Descent based learning for
// classification and regression
//
// Based on the publication(s):
// - Steffen Rendle (2010): Factorization Machines, in Proceedings of the 10th
//   IEEE International Conference on Data Mining (ICDM 2010), Sydney,
//   Australia.

#ifndef FM_LEARN_SGD_ELEMENT_H_
#define FM_LEARN_SGD_ELEMENT_H_

#include "fm_learn_sgd.h"

// 继承了fm_learn_sgd
class fm_learn_sgd_element: public fm_learn_sgd {
	public:
		// 初始化
		virtual void init() {
			fm_learn_sgd::init();
			// 日志输出
			if (log != NULL) {
				log->addField("rmse_train", std::numeric_limits<double>::quiet_NaN());
			}
		}
		// 利用SGD训练FM模型
		virtual void learn(Data& train, Data& test) {
			fm_learn_sgd::learn(train, test);// 输出参数信息

			std::cout << "SGD: DON'T FORGET TO SHUFFLE THE ROWS IN TRAINING DATA TO GET THE BEST RESULTS." << std::endl; 
			// SGD
			for (int i = 0; i < num_iter; i++) {// 开始迭代，每一轮的迭代过程
				double iteration_time = getusertime();// 记录开始的时间
				for (train.data->begin(); !train.data->end(); train.data->next()) {// 对于每一个样本
					double p = fm->predict(train.data->getRow(), sum, sum_sqr);// 得到样本的预测值
					double mult = 0;// 损失函数的导数
					if (task == 0) {// 回归
						p = std::min(max_target, p);
						p = std::max(min_target, p);
						// loss=(y_ori-y_pre)^2
						mult = -(train.target(train.data->getRowIndex())-p);// 对损失函数求导
					} else if (task == 1) {// 分类
						// loss
						mult = -train.target(train.data->getRowIndex())*(1.0-1.0/(1.0+exp(-train.target(train.data->getRowIndex())*p)));
					}
					// 利用梯度下降法对参数进行学习
					SGD(train.data->getRow(), mult, sum);					
				}				
				iteration_time = (getusertime() - iteration_time);// 记录时间差
				// evaluate函数是调用的fm_learn类中的方法
				double rmse_train = evaluate(train);// 对训练结果评估
				double rmse_test = evaluate(test);// 将模型应用在测试数据上
				std::cout << "#Iter=" << std::setw(3) << i << "\tTrain=" << rmse_train << "\tTest=" << rmse_test << std::endl;
				// 日志输出
				if (log != NULL) {
					log->log("rmse_train", rmse_train);
					log->log("time_learn", iteration_time);
					log->newLine();
				}
			}		
		}
		
};

#endif /*FM_LEARN_SGD_ELEMENT_H_*/
