#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

// 定义一个结构体表示哈夫曼树的节点
struct Node
{
    char data;   // 节点存储的字符
    int freq;    // 节点的频率
    Node *left;  // 左子节点
    Node *right; // 右子节点

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// 定义一个比较结构体用于优先级队列的排序
struct Compare
{
    bool operator()(Node *left, Node *right)
    {
        return left->freq > right->freq; // 频率小的优先级高
    }
};

// 从文件读取内容
void readFile(const string &filename, string &content)
{
    ifstream file(filename);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            // 替换每个换行符为 \\n
            size_t pos = 0;
            while ((pos = line.find("\n", pos)) != string::npos)
            {
                line.replace(pos, 1, "\\n");
                pos += 2; // 移动到下一个字符，跳过已经替换的 \\n
            }
            content += line; // 添加到内容中

            // 检查是否为最后一行，如果不是，则添加换行符
            if (!file.eof())
            {
                content += "\n";
            }
        }
        file.close();
    }
    else
    {
        cout << "Unable to open file: " << filename << endl;
    }
}

// 计算每个字符出现的频率
unordered_map<char, int> calculateFrequencies(const string &content)
{
    unordered_map<char, int> frequencies;
    for (char c : content)
    {
        frequencies[c]++; // 统计每个字符的出现次数
    }
    return frequencies;
}

// 构建哈夫曼树
Node *buildHuffmanTree(const unordered_map<char, int> &frequencies)
{
    priority_queue<Node *, vector<Node *>, Compare> pq;
    for (const auto &pair : frequencies)
    {
        pq.push(new Node(pair.first, pair.second)); // 把每个字符及其频率作为一个节点加入优先级队列
    }

    while (pq.size() > 1)
    {
        Node *left = pq.top();
        pq.pop(); // 取出频率最小的两个节点
        Node *right = pq.top();
        pq.pop();

        Node *parent = new Node('$', left->freq + right->freq); // 创建一个新节点作为父节点，频率为两个子节点频率之和
        parent->left = left;
        parent->right = right;

        pq.push(parent); // 把新节点加入优先级队列
    }

    return pq.top(); // 返回哈夫曼树的根节点
}

// 生成哈夫曼编码
void generateCodes(Node *root, const string &code, unordered_map<string, string> &codes)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->data != '$')
    {                                        // 叶子节点存储实际字符
        codes[string(1, root->data)] = code; // 存储字符对应的哈夫曼编码
    }

    generateCodes(root->left, code + "0", codes);  // 递归左子树
    generateCodes(root->right, code + "1", codes); // 递归右子树
}

// 压缩数据
string compressData(const string &content, const unordered_map<string, string> &codes)
{
    string compressedData;
    for (char c : content)
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
    for (char c : encryptedText)
    {
        buffer += c;
        for (const auto &code : huffmanCodes)
        {
            if (buffer == code.second)
            {
                if (code.first == "\\n")
                {
                    decryptedText += '\n'; // 将解码后的 "\\n" 转换为换行符
                }
                else
                {
                    decryptedText += code.first;
                }
                buffer.clear();
                break;
            }
        }
    }

    return decryptedText;
}

// 保存数据到文件
void saveToFile(const string &filename, const string &data)
{
    ofstream file(filename);
    if (file.is_open())
    {
        file << data; // 写入数据
        file.close();
    }
    else
    {
        cout << "Unable to open file: " << filename << endl;
    }
}
// 保存哈夫曼编码到json
void saveHuffmanCodesToJson(const unordered_map<string, string> &codes, const string &filename)
{
    ofstream file(filename);
    if (file.is_open())
    {
        stringstream ss;
        ss << "{" << endl;
        for (const auto &pair : codes)
        {
            if (pair.first == "\\n")
            {
                ss << "\"\\n\": \"" << pair.second << "\", " << endl; // 如果是换行符，则使用\n表示
            }
            else
            {
                ss << "\"" << pair.first << "\": \"" << pair.second << "\", " << endl;
            }
        }
        string str = ss.str();
        str = str.substr(0, str.length() - 3); // 删除最后一个逗号和空格
        file << str;
        file << endl
             << "}";
        file.close();
    }
    else
    {
        cout << "Unable to open file: " << filename << endl;
    }
}

// 加载json为哈弗曼编码
unordered_map<string, string> loadHuffmanCodesFromJson(ifstream &file)
{
    unordered_map<string, string> codes;

    string line;
    while (getline(file, line))
    {
        // 忽略空行和注释行
        if (line.empty() || line.find("//") == 0)
        {
            continue;
        }

        // 查找键值对中的冒号
        size_t colonPos = line.find(":");
        if (colonPos != string::npos)
        {
            // 提取键和值
            string key = line.substr(0, colonPos);
            string value = line.substr(colonPos + 1);

            // 去除键和值中的空格
            key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
            value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());

            // 去除键和值中的双引号
            key.erase(remove(key.begin(), key.end(), '"'), key.end());
            value.erase(remove(value.begin(), value.end(), '"'), value.end());

            // 去除值中的逗号
            value.erase(remove(value.begin(), value.end(), ','), value.end());

            /*// 替换值中的转义字符
            size_t found = key.find("\\\\");
            while (found != string::npos)
            {
                key.replace(found, 2, "\\");
                found = key.find("\\\\", found + 1);
            }*/

            // 添加键值对到哈希表
            if (!key.empty())
            { // 检查键是否为空
                codes[key] = value;
            }
        }
    }

    return codes;
}
