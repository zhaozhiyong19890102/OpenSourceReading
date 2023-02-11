# include <stdio.h>
# include <stdlib.h>
# include <vector>
# include <iostream>
using namespace std; 

#define MAX_CODE_LENGTH 10
#define vocab_size 6
#define vocab_max_size 10

struct vocab_word {
	long long cn; // 出现的次数
	int *point; // 从根结点到叶子节点的路径
	char *word, *code, codelen;// 分别对应着词，Huffman编码，编码长度
};

struct vocab_word *vocab;

void CreateBinaryTree() {
	long long a, b, i, min1i, min2i, pos1, pos2, point[MAX_CODE_LENGTH];
	char code[MAX_CODE_LENGTH];
	
	// 申请2倍的词的空间，（在这里完全没有必要申请这么多的空间）
	long long *count = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
	long long *binary = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
	long long *parent_node = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
	
	// 分成两半进行初始化
	for (a = 0; a < vocab_size; a++) count[a] = vocab[a].cn;// 前半部分初始化为每个词出现的次数
	for (a = vocab_size; a < vocab_size * 2; a++) count[a] = 1e15;// 后半部分初始化为一个固定的常数

	// 两个指针：
	// pos1指向前半截的尾部
	// pos2指向后半截的开始
	pos1 = vocab_size - 1;
	pos2 = vocab_size;

	// Following algorithm constructs the Huffman tree by adding one node at a time
	// 每次增加一个节点，构建Huffman树
	for (a = 0; a < vocab_size - 1; a++) {
		// First, find two smallest nodes 'min1, min2'
		// 选择最小的节点min1
		if (pos1 >= 0) {
			if (count[pos1] < count[pos2]) {
				min1i = pos1;
				pos1--;
			} else {
				min1i = pos2;
				pos2++;
			}
		} else {
			min1i = pos2;
			pos2++;
		}
		// 选择最小的节点min2
		if (pos1 >= 0) {
			if (count[pos1] < count[pos2]) {
				min2i = pos1;
				pos1--;
			} else {
				min2i = pos2;
				pos2++;
			}
		} else {
			min2i = pos2;
			pos2++;
		}

		count[vocab_size + a] = count[min1i] + count[min2i];
		// 设置父节点
		parent_node[min1i] = vocab_size + a;
		parent_node[min2i] = vocab_size + a;
		binary[min2i] = 1;// 设置一个子树的编码为1
	}
	// Now assign binary code to each vocabulary word
	// 为每一个词分配二进制编码，即Huffman编码
	for (a = 0; a < vocab_size*2; a++) {// 针对每一个词
		// 测试
		cout << a << ": count: " << count[a] << ", binary: " << binary[a] << ", parent_node: " << parent_node[a] << endl;
		/*b = a;
		i = 0;
		while (1) {
			code[i] = binary[b];// 找到当前的节点的编码
			point[i] = b;// 记录从叶子节点到根结点的序列
			i++;
			b = parent_node[b];// 找到当前节点的父节点
			if (b == vocab_size * 2 - 2) break;// 已经找到了根结点，根节点是没有编码的
		}
		vocab[a].codelen = i;// 词的编码长度
		vocab[a].point[0] = vocab_size - 2;// 根结点
		for (b = 0; b < i; b++) {
			vocab[a].code[i - b - 1] = code[b];// 编码的反转
			vocab[a].point[i - b] = point[b] - vocab_size;// 记录的是从根结点到叶子节点的路径
		}*/
	}
	free(count);
	free(binary);
	free(parent_node);
}

int main() {
	vector<int> cnt;
	cnt.push_back(8);
	cnt.push_back(5);
	cnt.push_back(4);
	cnt.push_back(3);
	cnt.push_back(1);
	cnt.push_back(1);
	vocab = (struct vocab_word *)calloc(vocab_max_size, sizeof(struct vocab_word));
	for (int i = 0; i < vocab_size; i++) {
		vocab[i].cn = cnt[i];
	}
	CreateBinaryTree();
	return 0;
}