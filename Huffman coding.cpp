#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>

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
            content += line + "\n"; // 读取文件每一行并添加换行符
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
void generateCodes(Node *root, const string &code, unordered_map<char, string> &codes)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->data != '$')
    {                             // 叶子节点存储实际字符
        codes[root->data] = code; // 存储字符对应的哈夫曼编码
    }

    generateCodes(root->left, code + "0", codes);  // 递归左子树
    generateCodes(root->right, code + "1", codes); // 递归右子树
}

// 压缩数据
string compressData(const string &content, const unordered_map<char, string> &codes)
{
    string compressedData;
    for (char c : content)
    {
        compressedData += codes.at(c); // 把每个字符替换为哈夫曼编码
    }
    return compressedData;
}

// 解压缩数据
string decompressData(const string &compressedData, Node *root)
{
    string decompressedData;
    Node *current = root;
    for (char c : compressedData)
    {
        if (c == '0')
        {
            current = current->left; // 遇到'0'则移动到左子节点
        }
        else
        {
            current = current->right; // 遇到'1'则移动到右子节点
        }

        if (current->left == nullptr && current->right == nullptr)
        {
            decompressedData += current->data; // 到达叶子节点，记录字符
            current = root;                    // 回到根节点继续处理下一个编码
        }
    }
    return decompressedData;
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
// 从json构建哈夫曼树
Node *buildHuffmanTreeFromJson(const string &json)
{
    Node *root = nullptr;
    Node *current = nullptr;
    for (char c : json)
    {
        if (c == '0')
        {
            if (current->left == nullptr)
            {
                current->left = new Node('$', 0);
            }
            current = current->left;
        }
        else
        {
            if (current->right == nullptr)
            {
                current->right = new Node('$', 0);
            }
            current = current->right;
        }

        if (current->left == nullptr && current->right == nullptr)
        {
            if (root == nullptr)
            {
                root = current;
            }
            current = root;
        }
    }
    return root;
}
// 保存哈夫曼树到json
string saveHuffmanTreeToJson(Node *root)
{
    string json;
    queue<Node *> q;
    q.push(root);
    while (!q.empty())
    {
        Node *current = q.front();
        q.pop();
        if (current->left != nullptr)
        {
            q.push(current->left);
            json += "0";
        }
        else
        {
            json += "1";
        }
        if (current->right != nullptr)
        {
            q.push(current->right);
            json += "0";
        }
        else
        {
            json += "1";
        }
    }
    return json;
}

int main()
{
    string filename = "D:\\code\\1.5\\test huffman.txt"; // 输入文件路径
    string content;
    readFile(filename, content); // 读取文件内容

    unordered_map<char, int> frequencies = calculateFrequencies(content); // 计算字符频率

    Node *root = buildHuffmanTree(frequencies); // 构建哈夫曼树

    unordered_map<char, string> codes;
    generateCodes(root, "", codes); // 生成哈夫曼编码

    string compressedData = compressData(content, codes); // 压缩数据

    // 压缩文件路径
    string compressedFilename = "D:\\code\\1.5\\compressed.txt";
    // 如果文件已经存在，则添加序号
    int index = 1;
    while (ifstream(compressedFilename).good())
    {
        compressedFilename = "D:\\code\\1.5\\compressed" + to_string(index) + ".txt";
        index++;
    }

    saveToFile(compressedFilename, compressedData); // 保存压缩数据到文件

    string decompressedData = decompressData(compressedData, root); // 解压缩数据

    // 解压缩文件路径
    string decompressedFilename = "D:\\code\\1.5\\decompressed.txt";
    // 如果文件已经存在，则添加序号
    index = 1;
    while (ifstream(decompressedFilename).good())
    {
        decompressedFilename = "D:\\code\\1.5\\decompressed" + to_string(index) + ".txt";
        index++;
    }

    // 获取父路径
    string parentPath = filename.substr(0, filename.find_last_of("\\/"));

    // 根据父路径生成储存路径
    string storagePath = parentPath + "\\storage.txt";

    // 保存文件到储存路径
    saveToFile(storagePath, content);

    saveToFile(decompressedFilename, decompressedData); // 保存解压数据到文件

    string huffmanTreeJson = saveHuffmanTreeToJson(root);            // 保存哈夫曼树到json
    string huffmanTreeFilename = "D:\\code\\1.5\\huffman_tree.json"; // 哈夫曼树文件路径

    saveToFile(huffmanTreeFilename, huffmanTreeJson); // 保存哈夫曼树到文件

    system("pause");
    return 0;
}
