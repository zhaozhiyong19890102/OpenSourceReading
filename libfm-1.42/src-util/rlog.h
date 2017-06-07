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
// rlog.h: Logging into R compatible files

#ifndef RLOG_H_
#define RLOG_H_
#include <iostream>
#include <fstream>
#include <assert.h>
#include <map>

// 日志类
class RLog {
	private:
		std::ostream* out;// 输出流
		std::vector<std::string> header;// 保存日志需要记录的项
		std::map<std::string,double> default_value;// 需要记录的项与值
		std::map<std::string,double> value;// 需要记录的项与值
	public:
		// 构造方法
		RLog(std::ostream* stream) { 
			out = stream;// 初始化输出流
			// 分别清空三个容器
			header.clear();
			default_value.clear();
			value.clear();
		};	
		
		// 在value中记录相应的日志内容
		void log(const std::string& field, double d) {
			value[field] = d;
		}
		
		// 初始化
		void init() {
			if (out != NULL) {// 正确打开输出流
				// 对header中的内容进行输出
				for (uint i = 0; i < header.size(); i++) {
					*out << header[i];
					if (i < (header.size()-1)) {
						*out << "\t";
					} else {
						*out << "\n";
					}
				}			
				out->flush();// 刷新
			}
			// 将default_value中的内容转存到value中
			for (uint i = 0; i < header.size(); i++) {
				value[header[i]] = default_value[header[i]];	
			}
		}
		
		// 增加新的field
		void addField(const std::string& field_name, double def) {
			//std::cout << field_name << std::endl; std::cout.flush();
			// 在header中查找field_name
			std::vector<std::string>::iterator i = std::find(header.begin(), header.end(), field_name);
			if (i != header.end()) {// 存在field_name
				throw "the field " + field_name + " already exists";
			}
			// 不存在
			header.push_back(field_name);// 将field_name添加到header中
			default_value[field_name] = def;// 添加到default_value中
		}
		
		// 将当前value中的内容输出，同时，在value中增加新的一行日志
		void newLine() {
			if (out != NULL) {// 正确打开输出流
				// 对value中的内容进行输出
				for (uint i = 0; i < header.size(); i++) {
					*out << value[header[i]];
					// 输出标志位
					if (i < (header.size()-1)) {
						*out << "\t";
					} else {
						*out << "\n";
					}
				}
				out->flush();// 刷新
				value.clear();// 清空value	
				// 将default_value中的内容转存到value中
				for (uint i = 0; i < header.size(); i++) {
					value[header[i]] = default_value[header[i]];	
				}
			}
		}
};
	

#endif /*RLOG_H_*/
