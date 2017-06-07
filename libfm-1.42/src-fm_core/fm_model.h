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
// fm_model.h: Model for Factorization Machines
//
// Based on the publication(s):
// - Steffen Rendle (2010): Factorization Machines, in Proceedings of the 10th
//   IEEE International Conference on Data Mining (ICDM 2010), Sydney,
//   Australia.

#ifndef FM_MODEL_H_
#define FM_MODEL_H_

#include "../util/matrix.h"
#include "../util/fmatrix.h"

#include "fm_data.h"

// fm_model模型类
class fm_model {
  private:
		DVector<double> m_sum, m_sum_sqr;// 分别对应着交叉项的中的两项
	public: //fm模型中的参数
		double w0;// 常数项
		DVectorDouble w;// 一次项的系数
		DMatrixDouble v;// 交叉项的系数矩阵

	public:
		// 属性
		// the following values should be set:
		uint num_attribute;// 特征的个数
		
		bool k0, k1;// 是否包含常数项和一次项
		int num_factor;// 交叉项因子的个数
		
		double reg0;// 常数项的正则参数
		double regw, regv;// 一次项和交叉项的正则系数
		
		double init_stdev;// 初始化参数时的方差
		double init_mean;// 初始化参数时的均值
		
		// 函数
		fm_model();// 构造函数，主要完成参数的初始化
		void debug();// debug函数
		void init();// 初始化函数，主要用于生成各维度系数的初始值
		// 对样本进行预测
		double predict(sparse_row<FM_FLOAT>& x);
		double predict(sparse_row<FM_FLOAT>& x, DVector<double> &sum, DVector<double> &sum_sqr);
};

// fm_model类的构造函数
fm_model::fm_model() {
	num_factor = 0;// 交叉项中因子的个数
	init_mean = 0;// 初始化的均值
	init_stdev = 0.01;// 初始化的方差
	reg0 = 0.0;// 常数项的正则化参数
	regw = 0.0;// 一次项的正则化参数
	regv = 0.0;// 交叉项的正则化参数 
	k0 = true;// 是否包含常数项
	k1 = true;// 是否包含一次项
}

// debug函数，主要用于输出中间调试的结果
void fm_model::debug() {
	std::cout << "num_attributes=" << num_attribute << std::endl;
	std::cout << "use w0=" << k0 << std::endl;
	std::cout << "use w1=" << k1 << std::endl;
	std::cout << "dim v =" << num_factor << std::endl;
	std::cout << "reg_w0=" << reg0 << std::endl;
	std::cout << "reg_w=" << regw << std::endl;
	std::cout << "reg_v=" << regv << std::endl; 
	std::cout << "init ~ N(" << init_mean << "," << init_stdev << ")" << std::endl;
}

// 初始化fm模型的函数
void fm_model::init() {
	w0 = 0;// 常数项的系数
	w.setSize(num_attribute);// 设置一次项系数的个数
	v.setSize(num_factor, num_attribute);// 设置交叉项的矩阵大小
	w.init(0);// 初始化一次项系数为0
	v.init(init_mean, init_stdev);// 按照均值和方差初始化交叉项系数
	// 交叉项中的两个参数，设置其大小为num_factor
	m_sum.setSize(num_factor);
	m_sum_sqr.setSize(num_factor);
}

// 对样本进行预测，其中x表示的是一行样本
double fm_model::predict(sparse_row<FM_FLOAT>& x) {
	return predict(x, m_sum, m_sum_sqr);		
}

double fm_model::predict(sparse_row<FM_FLOAT>& x, DVector<double> &sum, DVector<double> &sum_sqr) {
	double result = 0;// 最终的结果
	// 第一部分
	if (k0) {// 常数项	
		result += w0;
	}
	
	// 第二部分
	if (k1) {// 一次项
		for (uint i = 0; i < x.size; i++) {// 对样本中的每一个特征
			assert(x.data[i].id < num_attribute);// 验证样本的正确性
			// w * x
			result += w(x.data[i].id) * x.data[i].value;
		}
	}
	
	// 第三部分
	// 交叉项，对应着公式，有两重循环
	for (int f = 0; f < num_factor; f++) {// 外层循环
		sum(f) = 0;
		sum_sqr(f) = 0;
		for (uint i = 0; i < x.size; i++) {
			double d = v(f,x.data[i].id) * x.data[i].value;
			sum(f) += d;
			sum_sqr(f) += d*d;
		}
		result += 0.5 * (sum(f)*sum(f) - sum_sqr(f));// 得到交叉项的值
	}
	return result;
}

#endif /*FM_MODEL_H_*/
