#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class QM {
private:
    int bit;

public:
    ~QM() {

    }

    void LoadData();

};

void QM::LoadData() {
    ifstream fin;
    string input;

    fin.open("input_minterm.txt");
    this->bit = fin.get() - '0';
    


    fin.close();
}

int main() {
    ifstream fin;
    int bit;
    string input;
    vector<vector<string>> input_v, implicant_v;
    vector<string> temp_v;

    fin.open("input_minterm.txt");

    if (!fin.is_open()) {
        cout << "invaild file path";
        return 0;
    }

    bit = fin.get() - '0'; // ù��° �Է°����� ��Ʈ�� ����
    fin.get();
    // text������ �����͸� 2���� ���� �迭�� ����
    while (getline(fin, input)) { // text���Ͽ��� ���پ� input ����
        temp_v.push_back(input.substr(0, 1));   // d�� m ���Ϳ� ����
        temp_v.push_back(input.substr(1, bit)); // bit����ŭ ���Ϳ� ����
        temp_v.clear();                         // �ʱ�ȭ

        input_v.push_back(temp_v);
    }

    // 2���� ���� �Ҵ� ����

    // temp_v �Ҵ�����
    temp_v.clear();
    temp_v.shrink_to_fit();

    return 0;
}