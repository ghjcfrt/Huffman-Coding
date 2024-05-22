#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>

using namespace std;

// 哈夫曼树中节点的结构
struct Node
{
    char data;
    int frequency;
    Node *left;
    Node *right;

    Node(char data, int frequency)
    {
        this->data = data;
        this->frequency = frequency;
        left = right = nullptr;
    }
};

// 根据频率比较两个节点的函数
struct Compare
{
    bool operator()(Node *left, Node *right)
    {
        return left->frequency > right->frequency;
    }
};

// 对输入数据进行频率分析的函数
unordered_map<char, int> performFrequencyAnalysis(const string &data)
{
    unordered_map<char, int> frequencyMap;
    for (char c : data)
    {
        frequencyMap[c]++;
    }
    return frequencyMap;
}

// 构建哈夫曼树的函数
Node *buildHuffmanTree(const unordered_map<char, int> &frequencyMap)
{
    priority_queue<Node *, vector<Node *>, Compare> pq;
    for (const auto &pair : frequencyMap)
    {
        pq.push(new Node(pair.first, pair.second));
    }
    while (pq.size() > 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        Node *newNode = new Node('$', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }
    return pq.top();
}

// 为每个字符生成哈夫曼编码的函数
void generateHuffmanCodes(Node *root, const string &code, unordered_map<char, string> &huffmanCodes)
{
    if (root == nullptr)
    {
        return;
    }
    if (root->data != '$')
    {
        huffmanCodes[root->data] = code;
    }
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// 使用哈夫曼编码压缩输入数据的函数
string compressData(const string &data, const unordered_map<char, string> &huffmanCodes)
{
    string compressedData;
    for (char c : data)
    {
        compressedData += huffmanCodes.at(c);
    }
    return compressedData;
}

// 使用哈夫曼编码解压输入数据的函数
string decompressData(const string &compressedData, Node *root)
{
    string decompressedData;
    Node *currentNode = root;
    for (char c : compressedData)
    {
        if (c == '0')
        {
            currentNode = currentNode->left;
        }
        else
        {
            currentNode = currentNode->right;
        }
        if (currentNode->left == nullptr && currentNode->right == nullptr)
        {
            decompressedData += currentNode->data;
            currentNode = root;
        }
    }
    return decompressedData;
}

int main()
{
    // 从文件中读取输入数据
    ifstream inputFile("input.txt");
    string inputData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    // 执行频率分析
    unordered_map<char, int> frequencyMap = performFrequencyAnalysis(inputData);

    // 构建哈夫曼树
    Node *root = buildHuffmanTree(frequencyMap);

    // 生成哈夫曼编码
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // 压缩数据
    string compressedData = compressData(inputData, huffmanCodes);

    // 解压数据
    string decompressedData = decompressData(compressedData, root);

    // 将压缩后的数据保存到文件
    ofstream compressedFile("compressed.txt");
    compressedFile << compressedData;
    compressedFile.close();

    // 将解压后的数据保存到文件
    ofstream decompressedFile("decompressed.txt");
    decompressedFile << decompressedData;
    decompressedFile.close();

    return 0;
}
