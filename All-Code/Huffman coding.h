// include "Huffman coding.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <sstream>

using namespace std;

// 定义一个结构体表示哈夫曼树的节点
struct Node
{
    char data;   // 节点存储的字符
    int freq;    // 节点的频率
    Node *left;  // 左子节点
    Node *right; // 右子节点

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {} // 构造函数
};

// 定义一个比较结构体用于优先级队列的排序
struct Compare
{
    bool operator()(Node *left, Node *right) // 重载()运算符
    {
        return left->freq > right->freq; // 频率小的优先级高
    }
};

// 从文件读取内容
void readFile(const string &filename, string &content)
{
    ifstream file(filename); // 打开文件
    if (file.is_open())      // 如果文件打开成功
    {
        string line;                // 存储每一行的内容
        while (getline(file, line)) // 逐行读取文件内容
        {
            // 替换每个换行符为 \\n
            size_t pos = 0;                                      // 查找位置
            while ((pos = line.find("\n", pos)) != string::npos) // 查找换行符
            {
                line.replace(pos, 1, "\\n"); // 替换为 \\n
                pos += 2;                    // 移动到下一个字符，跳过已经替换的 \\n
            }
            content += line; // 添加到内容中

            // 检查是否为最后一行，如果不是，则添加换行符
            if (!file.eof()) // 如果不是最后一行
            {
                content += "\n"; // 添加换行符
            }
        }
        file.close(); // 关闭文件
        MessageBox(NULL, "文件已读取。", "信息", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        // 文件写入失败时弹出错误消息框
        MessageBox(NULL, ("无法打开文件: " + filename).c_str(), "错误", MB_OK | MB_ICONERROR);
    }
}
// 打开 JSON 文件
bool openJsonFile(const string &filename, ifstream &file)
{
    file.open(filename); // 打开文件
    if (file.is_open())  // 如果文件打开成功
    {
        return true; // 文件成功打开
    }
    else
    {
        // 弹出文件打开失败的错误消息框
        MessageBox(NULL, ("无法打开文件: " + filename).c_str(), "错误", MB_OK | MB_ICONERROR);
        return false; // 文件打开失败
    }
}

// 计算每个字符出现的频率
unordered_map<char, int> calculateFrequencies(const string &content)
{
    unordered_map<char, int> frequencies; // 用于存储字符及其频率
    for (char c : content)                // 遍历每个字符
    {
        frequencies[c]++; // 统计每个字符的出现次数
    }
    return frequencies;
}

// 构建哈夫曼树
Node *buildHuffmanTree(const unordered_map<char, int> &frequencies)
{
    // 定义一个优先级队列，用于构建哈夫曼树
    priority_queue<Node *, vector<Node *>, Compare> pq; // 优先级队列，用于构建哈夫曼树
    for (const auto &pair : frequencies)                // 遍历每个字符及其频率
    {
        pq.push(new Node(pair.first, pair.second)); // 把每个字符及其频率作为一个节点加入优先级队列
    }

    // 构建哈夫曼树
    while (pq.size() > 1) // 当优先级队列中的节点数量大于1时
    {
        // 从优先级队列中取出频率最小的两个节点
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();

        // 创建一个新节点作为父节点，频率为两个子节点频率之和
        Node *parent = new Node('$', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        pq.push(parent); // 把新节点加入优先级队列
    }

    return pq.top(); // 返回哈夫曼树的根节点
}

// 生成哈夫曼编码
void generateCodes(Node *root, const string &code, unordered_map<string, string> &codes)
{
    // 递归生成哈夫曼编码
    if (root == nullptr) // 如果节点为空，则返回
    {
        return;
    }

    // 如果是叶子节点，则存储实际字符及其哈夫曼编码
    if (root->data != '$')
    {                                        // 叶子节点存储实际字符
        codes[string(1, root->data)] = code; // 存储字符对应的哈夫曼编码
    }

    // 递归生成哈夫曼编码
    generateCodes(root->left, code + "0", codes);  // 递归左子树
    generateCodes(root->right, code + "1", codes); // 递归右子树
}

// 压缩数据
string compressData(const string &content, const unordered_map<string, string> &codes)
{
    string compressedData; // 用于存储压缩后的数据

    // 遍历每个字符，使用哈夫曼编码进行压缩
    for (char c : content) // 遍历每个字符
    {
        string key(1, c);                // 使用字符本身作为键
        compressedData += codes.at(key); // 使用哈夫曼编码
    }
    return compressedData;
}

// 解压缩数据
string decompressData(const string &encryptedText, const unordered_map<string, string> &huffmanCodes)
{
    string decryptedText; // 用于存储解密后的文本
    string buffer;        // 用于存储临时编码

    // 遍历加密后的文本，使用哈夫曼编码进行解密
    for (char c : encryptedText) // 遍历加密后的文本
    {
        buffer += c;                          // 添加字符到缓冲区
        for (const auto &code : huffmanCodes) // 遍历哈夫曼编码
        {
            if (buffer == code.second) // 如果缓冲区中的编码与哈夫曼编码相同
            {
                if (code.first == "\\n") // 如果是换行符
                {
                    decryptedText += '\n'; // 将解码后的 "\\n" 转换为换行符
                }
                else
                {
                    decryptedText += code.first; // 添加解码后的字符
                }
                buffer.clear(); // 清空缓冲区
                break;
            }
        }
    }

    return decryptedText;
}

// 保存数据到文件
void saveToFile(const string &filename, const string &data)
{
    ofstream file(filename); // 打开文件
    if (file.is_open())      // 如果文件打开成功
    {
        file << data; // 写入数据
        file.close(); // 关闭文件
    }
    else
    {
        // 文件写入失败时弹出错误消息框
        MessageBox(NULL, ("无法打开文件: " + filename).c_str(), "错误", MB_OK | MB_ICONERROR);
    }
}

// 保存哈夫曼编码到json
void saveHuffmanCodesToJson(const unordered_map<string, string> &codes, const string &filename)
{
    ofstream file(filename); // 打开文件
    if (file.is_open())      // 如果文件打开成功
    {
        stringstream ss; // 创建一个字符串流

        // 添加哈夫曼编码到字符串流
        ss << "{" << endl;             // 添加左花括号
        for (const auto &pair : codes) // 遍历哈夫曼编码
        {
            if (pair.first == "\\n") // 如果是换行符
            {
                ss << "\"\\n\": \"" << pair.second << "\", " << endl; // 如果是换行符，则使用\n表示
            }
            else
            {
                ss << "\"" << pair.first << "\": \"" << pair.second << "\", " << endl; // 添加键值对到字符串流
            }
        }

        // 生成JSON格式的字符串
        string str = ss.str();                 // 获取字符串流的内容
        str = str.substr(0, str.length() - 3); // 删除最后一个逗号和空格
        file << str << endl;                   // 写入文件
        file << "}";                           // 添加右花括号
        file.close();                          // 关闭文件
    }
    else
    {
        // 文件写入失败时弹出错误消息框
        MessageBox(NULL, ("无法打开文件: " + filename).c_str(), "错误", MB_OK | MB_ICONERROR);
    }
}

// 加载json为哈弗曼编码
unordered_map<string, string> loadHuffmanCodesFromJson(ifstream &file)
{
    unordered_map<string, string> codes; // 用于存储哈夫曼编码
    string line;                         // 用于存储每一行的内容

    while (getline(file, line)) // 逐行读取文件内容
    {
        if (line.empty() || line.find("//") == 0) // 忽略空行和注释行
        {
            continue; // 跳过空行和注释行
        }

        // 查找冒号用以分割键值对
        size_t colonPos = line.find(":"); // 查找键值对中的冒号
        if (colonPos != string::npos)     // 如果找到冒号
        {
            // 提取键和值
            string key = line.substr(0, colonPos);    // 提取键
            string value = line.substr(colonPos + 1); // 提取值

            // 去除键和值中的空格
            key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());         // 去除键中的空格
            value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end()); // 去除值中的空格

            // 去除键和值中的双引号
            key.erase(remove(key.begin(), key.end(), '"'), key.end());         // 去除键中的双引号
            value.erase(remove(value.begin(), value.end(), '"'), value.end()); // 去除值中的双引号

            // 去除值中的逗号
            value.erase(remove(value.begin(), value.end(), ','), value.end()); // 去除值中的逗号

            // 添加键值对到哈希表
            if (!key.empty()) // 检查键是否为空
            {
                codes[key] = value; // 添加键值对到哈希表
            }
        }
    }

    return codes;
}
