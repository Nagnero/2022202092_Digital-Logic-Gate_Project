#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class QM {
private:
    int bit;
    // ���ڿ� ù��°�� minterm���� don't care���� �Ǵ�
    // ���� bit + 1��° �ε����� 1�� ���� ����
    vector<string> input_v;
    vector<string> minterm; // minterm ����
    vector<string> prime; // Prime Implicant ����
    vector<string> sorted_v;

public:
    ~QM() {}

    void
        LoadData(const char* path); // ������. ���� �� ������ ��θ� �Ű������� ����
    void step1();
    //vector<string> sort_vector(int len);
    vector<string> compare(vector<string>& temp1, vector<string>& temp2);
};

//  �ؽ�Ʈ ���Ͽ� ����� ���� �����ͼ� ���Ϳ� ����
void QM::LoadData(const char* path) {
    ifstream fin;
    string input;

    fin.open(path);

    if (!fin.is_open()) { // ����� ������ ���ȴ��� Ȯ��
        cout << "invaild file path";
        return;
    }

    this->bit = fin.get() - '0';
    getline(fin, input); // ù��° �� �ʱ�ȭ

    // ���پ� �޾ƿͼ� ����
    while (getline(fin, input)) { 
        input.erase(1, 1);
        input_v.push_back(input);
        if (input[0] == 'm')
            minterm.push_back(input.substr(1, bit)); // minterm�� ���� ����
    }

    fin.close();
}

void QM::step1() {
    char cnt = '0';
    int len = this->input_v.size();

    // �� �迭�� 1�� ���� ī����
    for (int i = 0; i < len; i++) {
        for (char c : input_v[i])
            if (c == '1')
                cnt++;

        this->input_v[i].push_back(cnt); // �� ���� �������� 1�� ���� �߰�
        cnt = '0';
    }

    // 1�� ������ ���� ���� �� sorted_v�� ����
    for (cnt = '0'; cnt - '0' < bit + 1; cnt++) {
        for (int i = 0; i < len; i++) {
            if (this->input_v[i].back() == cnt)       // 1�� ������ �Ǵ��Ͽ�
                this->sorted_v.push_back(this->input_v[i].substr(1, this->bit + 1)); // ���ĵ� ���Ϳ� Ǫ��
        }
    }


    bool repeat = true;
    vector<string> column = sorted_v;
    // ��������� ���� column�� ������ ���� ������ �ݺ�
    while (repeat) { 
        char i = '0';
        int j = 0;
        vector<string> temp1, temp2;
        vector<string> next_column;

        while (column[j].back() == i)
            temp1.push_back(column[j++]);

        for (; j < column.size(); i++) {
            while (column[j].back() == i + 1) {
                temp2.push_back(column[j++]);
                if (j >= column.size())
                    break;
            }

            // hamming distance�� 1�� �������� �� �� ���� �� �ӽ� ���Ϳ� ����
            if (temp1.size() != 0) {
                for (string str : compare(temp1, temp2))
                    next_column.push_back(str);

                for (int k = 0; k < temp1.size(); k++)
                    if (!(temp1[k][this->bit + 1])) {
                        cout << temp1[k] << endl;
                        prime.push_back(temp1[k]);
                    }
                    else
                        temp1[k].erase(this->bit + 1);
            }

            temp1 = temp2;
            temp2.clear();
        }

        // ����Լ� Ż���
        if (next_column.size() == 0) {
            for (int k = 0; k < temp1.size(); k++)
                if (!(temp1[k][this->bit + 1])) {
                    cout << temp1[k] << endl;
                    prime.push_back(temp1[k]);
                }
                else
                    temp1[k].erase(this->bit + 1);
            break;
        }

        column = next_column;


    }
}

vector<string> QM::compare(vector<string>& temp1, vector<string>& temp2) {
    vector<string> return_v;
    
    if (temp1.size() == 0 || temp2.size() == 0) {
        return return_v;
    }

    for (int i = 0; i < temp1.size(); i++) {
        for (int j = 0; j < temp2.size(); j++) {
            int cnt = 0, index = 0;
            for (int k = 0; k < this->bit; k++) {            
                if (temp1[i][k] != temp2[j][k]) {
                    cnt++;
                    index = k;
                }

            }

            if (cnt == 1) {
                temp1[i].push_back('x');
                temp2[j].push_back('x');
                string str = temp1[i].substr(0, index);
                str.push_back('-');
                if (index != this->bit - 1)
                    str = str + (temp1[i].substr(index + 1, this->bit - index - 1));
               
                str.push_back(temp2[j][this->bit]);

                return_v.push_back(str);
            }
        }
    }

    return_v.erase(unique(return_v.begin(), return_v.end()), return_v.end());

    return return_v;
}

int main() {
    const char* path = "input_minterm.txt";
    QM* qm = new QM;

    qm->LoadData(path);
    qm->step1();

    return 0;
}