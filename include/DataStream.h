#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <chrono>
#include "B+.h"
using namespace std;

namespace yazi {
	namespace serialize {
		class DataStream {
		public:
			enum DataType
			{
				BOOL = 0,
				CHAR,
				INT32,
				INT64,
				FLOAT,
				DOUBLE,
				STRING,
				VECTOR,
				LIST,
				MAP,
				SET,
				BPLUSNODE
			};
			DataStream();
			~DataStream();
			void show() const;
			bool writeIntoFile();
			bool readFromFile(ifstream &ifs, int nums);
			void write(const char* data, int len);
			void write(bool value);
			void write(char value);
			void write(int32_t value);
			void write(int64_t value);
			void write(float value);
			void write(double value);
			void write(const char* value);
			void write(const string& value);
			void write(BPlusNode node, string strs);
			DataStream& operator << (bool value);
			DataStream& operator << (char value);
			DataStream& operator << (int32_t value);
			DataStream& operator << (int64_t value);
			DataStream& operator << (float value);
			DataStream& operator << (double value);
			DataStream& operator << (const char* value);
			DataStream& operator << (const string & value);
			// DataStream& operator << (BPlusNode* &node_ptr);

			bool read(bool &value);
			bool read(char &value);
			bool read(int32_t &value);
			bool read(int64_t &value);
			bool read(float &value);
			bool read(double &value);
			bool read(string& value);
			bool read(BPlusNode& node);
			DataStream& operator >> (bool &value);
			DataStream& operator >> (char &value);
			DataStream& operator >> (int32_t &value);
			DataStream& operator >> (int64_t &value);
			DataStream& operator >> (float &value);
			DataStream& operator >> (double &value);
			DataStream& operator >> (string& value);
			DataStream& operator >> (BPlusNode& node);

			void clear();
		private:
			vector<char> m_buf;
			int position;
			int num; // 一次性要写入的节点个数
			vector<int> node_num; // 每一个元素代表当前节点所占字节数
			int current_num = 0; // 记录当前有多少个节点
		private:
			void reserve(int len);
		};
		// 构造函数
		DataStream::DataStream():position(0), num(100){

		}
		// 析构函数
		DataStream::~DataStream() {

		}
		// 清除
		void DataStream::clear(){
			m_buf.clear();
			position = 0;
		}
		// 扩容
		void DataStream::reserve(int len) {
			int size = m_buf.size();
			int cap = m_buf.capacity();
			if (size + len > cap) {
				while (size + len > cap)
				{
					if (cap == 0) {
						cap = 1;
					}
					else
					{
						cap *= 2;
					}
				}
				m_buf.reserve(cap);
			}
		}

		void DataStream::show() const {
			int size = m_buf.size();
			cout << "data size = " << size << endl;
			int i = 0;
			while (i < size)
			{
				switch ((DataType)m_buf[i])
				{
				case DataType::BOOL:
					if ((int)m_buf[++i] == 0) {
						cout << "false" << endl;
					}
					else {
						cout << "true" << endl;
					}
					i++;
					break;
				case DataType::CHAR:
					cout << m_buf[++i] << endl;
					i++;
					break;
				case DataType::INT32:
					cout << *((int32_t*)(&m_buf[++i])) << endl;
					i += 4;
					break;
				case DataType::INT64:
					cout << *((int64_t*)(&m_buf[++i])) << endl;
					i += 8;
					break;
				case DataType::FLOAT:
					cout << *((float*)(&m_buf[++i])) << endl;
					i += 4;
					break;
				case DataType::DOUBLE:

					cout << *((double*)(&m_buf[++i])) << endl;
					i += 8;
					break;
				case DataType::STRING:
					if ((DataType)m_buf[++i] == DataType::INT32) {
						int len = *((int32_t *)(&m_buf[++i]));
						i += 4;
						cout << string(&m_buf[i], len) << endl;
						i += len;
					}
					else {
						throw logic_error("parse string error");
					}
					break;
				default:
					break;
				}
			}
		}
		bool DataStream::writeIntoFile() {
			string file_path_name = "../doc/node.out";
			ofstream ofs;
			ofs.open(file_path_name, ios::app | ios::binary);
			if (!ofs.is_open()) {
				return false;
			}
			// 把m_buf的数据写入到文件中
			int pos = 0; // 记录位置
			int count = 1;
			for(vector<int>::iterator it = node_num.begin(); it != node_num.end(); it++){
				int j =  0;
				for (j; j < (*it); j++){
					ofs.put(m_buf[pos + j]);
				}
				pos += j;
			}
			// 所有节点写完之后，清空数据
			node_num.clear();
			m_buf.clear();
			current_num = 0;
			// 关闭文件
			ofs.close();
			return true;
		}
		bool DataStream::readFromFile(ifstream &ifs, int nums) { // 读取nums个字符
			// string file_path_name = "./binary.out";
			string buf;
			int i = 0;
			char c;
			while (ifs.get(c))
			{

				buf.append(1, c);
				i++;
				if (i == nums) {
					break;
				}
			}
			write(buf.data(), buf.size());
			/*for_each(V.begin(), V.end(), printVector);
			cout << endl;*/
			return true;
		}
		// 把数据写入vector中
		void DataStream::write(const char* data, int len) {
			double s = *(data);
			reserve(len);
			int size = m_buf.size();
			// cout << "size = " << size << endl;
			m_buf.resize(size + len);
			memcpy(&m_buf[size], data, len);
		}
		// 对bool类型的数据编码
		void DataStream::write(bool value) {
			char type = DataType::BOOL;
			write((char*)&type, sizeof(char));
			write((char*)&value, sizeof(char));
		}
		// 对char数据类型编码
		void DataStream::write(char value) {
			char type = DataType::CHAR;
			write((char*)&type, sizeof(char));
			write((char*)&value, sizeof(char));
		}
		// 对int32位的类型编码
		void DataStream::write(int32_t value) {
			char type = DataType::INT32;
			write((char*)&type, sizeof(char));
			write((char*)&value, sizeof(int32_t));
		}
		// 对int64位的类型编码
		void DataStream::write(int64_t value) {
			char type = DataType::INT64;
			write((char*)&type, sizeof(char));
			write((char*)&value, sizeof(int64_t));
		}
		// 对float的类型编码
		void DataStream::write(float value) {
			char type = DataType::FLOAT;
			write((char*)&type, sizeof(char));
			write((char*)&value, sizeof(float));
		}
		// 对double的类型编码
		void DataStream::write(double value) {
			char type = DataType::DOUBLE;
			write((char*)&type, sizeof(char));
			write((char*)&value, sizeof(double));
		}
		// 对c风格的字符串编码
		void DataStream::write(const char* value) {
			char type = DataType::STRING;
			write((char*)&type, sizeof(char));
			int len = strlen(value);
			write(len);
			write(value, len);
		}
		// 对c++风格的字符串编码
		void DataStream::write(const string& value) {
			char type = DataType::STRING;
			write((char*)&type, sizeof(char));
			int len = value.size();
			write(len);
			write(value.data(), len);
		}
		// 对自定义的节点类进行序列化编码
		void DataStream::write(BPlusNode node, string strs){
			// char type = DataType::INT32;
			// write((char*)&type, sizeof(char));
			if (strs == "push"){
				writeIntoFile();
			}else{
				current_num++;
				int size = node.getSize();
				write(size);
				vector<int> v = node.getNode();
				for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
					write(*it);
				}
				write(node.getFlag());
				node_num.push_back((size + 2) * 5);
				if (current_num > num){ // 当前节点数已经超过阈值，将数据全部写入到文件中
					writeIntoFile();
				}
			}
		}
		DataStream& DataStream::operator << (bool value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (char value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (int32_t value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (int64_t value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (float value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (double value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (const char* value) {
			write(value);
			return *this;
		}
		DataStream& DataStream::operator << (const string& value) {
			write(value);
			return *this;
		}
		// DataStream& DataStream::operator << (BPlusNode* &node_ptr) {
		// 	write(node_ptr);
		// 	return *this;
		// }

		bool DataStream::read(bool &value) {
			if ((DataType)m_buf[position] != DataType::BOOL) {
				return false;
			}
			position++;
			value = (bool)m_buf[position];
			position++;
			return true;
		}
		bool DataStream::read(char &value) {
			if ((DataType)m_buf[position] != DataType::CHAR) {
				return false;
			}
			position++;
			value = (char)m_buf[position];
			position++;
			return true;
		}
		bool DataStream::read(int32_t &value) {
			if ((DataType)m_buf[position] != DataType::INT32) {
				return false;
			}
			position++;
			value = *((int32_t *)(&m_buf[position]));
			position += 4;
			return true;
		}
		bool DataStream::read(int64_t &value) {
			if ((DataType)m_buf[position] != DataType::INT64) {
				return false;
			}
			position++;
			value = *((int64_t *)(&m_buf[position]));
			position += 8;
			return true;
		}
		bool DataStream::read(float &value) {
			if ((DataType)m_buf[position] != DataType::FLOAT) {
				return false;
			}
			position++;
			value = *((float *)(&m_buf[position]));
			position += 4;
			return true;
		}
		bool DataStream::read(double &value) {
			if ((DataType)m_buf[position] != DataType::DOUBLE) {
				return false;
			}
			position++;
			value = *((double *)(&m_buf[position]));
			position += 8;
			return true;
		}
		bool DataStream::read(string& value) {
			if ((DataType)m_buf[position] != DataType::STRING) {
				return false;
			}
			position++;
			int len;
			read(len);
			if (len < 0) {
				return false;
			}
			value.assign((char *)&m_buf[position], len);
			position += len;
			return true;
		}
		bool DataStream::read(BPlusNode& node){
			int len = node.getSize();
			node.setSize(0);
			// read(len);
			int i = 0;
			int value;
			for(i; i < len; i++){
				read(value);
				node.addNodeAnyway(value);
			}
			read(value);
			node.setFlag(value);
			return true;
		}
		DataStream& DataStream::operator >> (bool& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (char& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (int32_t& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (int64_t& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (float& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (double& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (string& value) {
			read(value);
			return *this;
		}
		DataStream& DataStream::operator >> (BPlusNode& node){
			read(node);
			return *this;
		}
	}
}