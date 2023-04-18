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
    vector<string> sorted_v; // 0�� �ƴ� ������ ������ ī��Ʈ�Ͽ� ������ ����
    int** covered;

public:
    QM() {}
    ~QM() {

    }

    void LoadData(const char* path); // ������. ���� �� ������ ��θ� �Ű������� ����
    void step1(); // �Է¹��� ����� Prime Implicant�� ���ϴ� ����Լ�
    vector<string> compare(vector<string>& temp1, vector<string>& temp2);
    void step2();
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

// �Է¹��� ����� Prime Implicant�� ���ϴ� ����Լ�
void QM::step1() {
    int cnt = 0;
    int len = this->input_v.size();

    // �� �迭�� 1�� ���� ī����
    for (int i = 0; i < len; i++) {
        for (char c : input_v[i])
            if (c == '1')
                cnt++;

        this->input_v[i].push_back(cnt + '0'); // �� ���� �������� 1�� ���� �߰�
        cnt = 0;
    }

    // 1�� ������ ���� ���� �� sorted_v�� ����
    for (int i = 0; i <= bit; i++) {
        for (int j = 0; j < len; j++) {
            if (this->input_v[j].back() == i + '0') // 1�� ������ �Ǵ��Ͽ�
                this->sorted_v.push_back(this->input_v[j].substr(1, this->bit + 1)); // ���ĵ� ���Ϳ� Ǫ��
        }
    }

    // ���ĵ� ���͸� �ӽ÷� column ���Ϳ� �Ҵ�
    vector<string> column = sorted_v;
    // ��������� ���� column�� ������ ���� ������ �ݺ�
    while (1) {
        int i = 0; // for���� ���� ���� ����
        int j = 0;
        vector<string> temp1, temp2; // �ӽ� ���� ����
        vector<string> next_column; // �ӽ� ���� ����

        // 0�� �ƴ� ���� ������ �������� temp1�� ����
        while (column[j].back() == i + '0')
            temp1.push_back(column[j++]);

        // column�� �����ŭ �ݺ�
        for (; j < column.size(); i++) {
            // 0�� �ƴ� ���� ������ �������� temp2�� ����
            while (column[j].back() == i + 1 + '0') {
                temp2.push_back(column[j++]);
                if (j >= column.size()) // j�� column�� ������� ũ�ų� ������ �ݺ��� Ż��
                    break;
            }

            // hamming distance�� 1�� �������� �� �� ���� �� �ӽ� ���Ϳ� ����
            if (!temp1.empty()) {
                // �ռ� �̾ƿ� temp1, temp2�� compare�Լ��� ����
                for (string str : compare(temp1, temp2)) // hamming distance�� 1�� ���ڿ� ��ȯ�޾Ƽ�
                    next_column.push_back(str); // next_column ���Ϳ� ����

                // temp1�� ũ�⸸ŭ �ݺ�
                for (int k = 0; k < temp1.size(); k++)
                    // compare�Լ����� �ѹ��� �Ⱦ��� prime implicant����
                    // �׸��� �̹� prime implicant�� ������ �Ǵ� ��
                    if (temp1[k][this->bit + 1] != 'x' && find(prime.begin(), prime.end(), temp1[k]) == prime.end())
                        this->prime.push_back(temp1[k].substr(0, this->bit)); // prime ���Ϳ� Ǫ��
                    else // �ƴϸ� ���� �ݺ��� ���� �߰��� x ����
                        temp1[k].erase(this->bit + 1); 
            }
                      
            temp1 = temp2; // temp1�� temp2 ���� ��
            temp2.clear(); // temp2 ����. ���� ���͸� �ޱ� ���� �غ�
        }

        // �ݺ����� �������� �� �ٽ� �ѹ� temp1�� ��Ұ� prime implicant���� �Ǵ� ��
        for (int k = 0; k < temp1.size(); k++)
            if (temp1[k][this->bit + 1] != 'x' && find(prime.begin(), prime.end(), temp1[k]) == prime.end())
                prime.push_back(temp1[k].substr(0,this->bit)); // prime ���Ϳ� �߰�. temp1 ���� ���ϹǷ� �״�� ����

        // ����Լ� Ż���. ���� �Ǵ��� ���� next_column�� ������ ���ѹݺ��� Ż��
        if (next_column.empty())            
            break;

        // �ݺ��� Ż���� �ȵǸ� ���� �� �Ǵ��� ���� �ӽ÷� ������ next_column�� column�� ����
        column = next_column;
    }

    // ���������� �ߺ���� ���� 
    this->prime.erase(unique(this->prime.begin(), this->prime.end()), this->prime.end());
}

// �عְŸ��� 1�� �� ������ ���Լ�
vector<string> QM::compare(vector<string>& temp1, vector<string>& temp2) {
    vector<string> return_v; // ��ȯ�� ����
    
    // ���� �� ���� �� �ϳ���� ���Ұ� ������ �״�� Ż��
    if (temp1.size() == 0 || temp2.size() == 0) 
        return return_v;

    // ���� �ݺ������� �� ��
    for (int i = 0; i < temp1.size(); i++) { // temp1 ��ҿ� ������ �ݺ���
        for (int j = 0; j < temp2.size(); j++) { // temp2 ��ҿ� ������ �ݺ���
            // cnt: ������ �������� ��� �ִ��� ���� ����
            // index: ������ �������� ��� ��ġ�ϴ��� �����ϴ� ����
            int cnt = 0, index = 0;
            // ������ ���Ϳ� ����� ���ڿ����� ������ ���ڿ� �����ϱ� ���� �ݺ���
            for (int k = 0; k < this->bit; k++) {            
                if (temp1[i][k] != temp2[j][k]) { // ���� ��ġ�� ���� ���ڰ� ������
                    cnt++; // cnt ���� ��
                    index = k; // index�� �ش� �ε��� ����
                }
            }

            // ���� ���ڰ� �ϳ��� �ִٸ�
            if (cnt == 1) {
                temp1[i].push_back('x'); // ���Ǿ��� �ǹ̷� �ڿ� x �߰�
                temp2[j].push_back('x'); // temp2�� ���������� x �߰�
                string str = temp1[i].substr(0, index); // ���ڿ� ó���� ���� �ӽ� ���ڿ�. index���� ���� ��
                str.push_back('-'); // ������ ���ڰ� �ִ� �ڸ��� '-'�� ǥ�� �� 
                str = str + (temp1[i].substr(index + 1, this->bit - index - 1)); // ���� �ڸ� ä���
               
                str.push_back(temp2[j][this->bit]); // 0�� �ƴ� ���� ����� �ڿ� �߰�

                return_v.push_back(str); // ��ȯ�� ���Ϳ� �ش� ���ڿ� Ǫ��
            }
        }
    }

    // ���� �ݺ��� Ż�� �� ���� �� ��ȯ
    return return_v;
}

// minterm�� PI���� ��ġ�� �� �Ǵ�
void QM::step2() {
    // ��� ���� minterm�� prime implicant�� ������ �迭 ���� �Ҵ�
    int** x_loc = new int* [this->prime.size()];
    for (int i = 0; i < this->prime.size(); i++)
        x_loc[i] = new int[this->minterm.size()];

    // 0���� �ʱ�ȭ
    for (int i = 0; i < this->prime.size(); i++)
        for (int j = 0; j < this->minterm.size(); j++)
            x_loc[i][j] = 0;

    for (int i = 0; i < this->prime.size(); i++) {
        for (int j = 0; j < this->minterm.size(); j++) {
            bool covered = true; // pi�� minterm�� Ŀ���ϴ��� Ȯ���ϴ� ����

            for (int k = 0; k < this->bit; k++) {
                // prime���� k��°�� '-'�̸� �׻� �����Ƿ� ���� ����
                if (this->prime[i][k] == '-')
                    continue;

                if (prime[i][k] != minterm[j][k]) {
                    covered = false;
                    break;
                }
                else {
                    continue;
                }
            }
            if (covered)
                x_loc[i][j] = 1;
        }
    }


    // �����Ҵ� �޸� ����
    for (int i = 0; i < this->prime.size(); i++)
        delete[] x_loc[i];
    delete[] x_loc;

}

int main() {
    const char* path = "input_minterm.txt";
    QM* qm = new QM;

    qm->LoadData(path);
    qm->step1();
    qm->step2();


    delete qm;
    return 0;
}