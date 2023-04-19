#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class QM {
private:
    int bit; // ���Ʈ���� �����ϴ� ����
    // ���ڿ� ù��°�� minterm���� don't care���� �Ǵ�
    // ���� bit + 1��° �ε����� 1�� ���� ����
    vector<string> input_v;
    vector<string> minterm; // minterm ����
    vector<string> prime; // Prime Implicant ����
    vector<string> sorted_v; // 0�� �ƴ� ������ ������ ī��Ʈ�Ͽ� ������ ����
    int checked_minterm; // ����Լ� Ż���� ���� ����. ��� minterm�� ���� �Ǵ��� �������� ����
    vector<string> output; // ��°� ���� ����
    bool no_nextcol = true;

public:
    QM() { // �����ڿ��� �ʱ�ȭ. ���ʹ� �ʱ�ȭ ���ʿ�
        checked_minterm = 0;
    }
    ~QM() {} // private�� �����Ҵ��� ���� �����Ƿ� �Ҹ��� ���ʿ�

    void LoadData(const char* intput_path); // ������. ���� �� ������ ��θ� �Ű������� ����
    void step1(); // �Է¹��� ������ Prime Implicant�� ���ϴ� ����Լ�
    // �� ���͸� ���Ͽ� �ع� ���Ͻ��� 1�� ���͸� ��ȯ�ϴ� ����Լ�
    vector<string> compare(vector<string>& temp1, vector<string>& temp2); 
    void step2(); // PI�� minterm ���� ��ġ�� �� üũ�ϴ� ����Լ�
    void step3(int** x_loc); // ����Լ��� PIM ���ϴ� �Լ�
    void saveEPI(const char* output_path); // ���� ��¹� Ʈ�������� ����� ���� �Լ�
};

//  �ؽ�Ʈ ���Ͽ� ����� ���� �����ͼ� ���Ϳ� ����
void QM::LoadData(const char* input_path) {
    ifstream fin;
    string input;

    fin.open(input_path);

    if (!fin.is_open()) { // ����� ������ ���ȴ��� Ȯ��
        cout << "invaild file path";
        return;
    }

    getline(fin, input); // ù��° �� �ʱ�ȭ
    this->bit = stoi(input);

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
    int index_prime = 0;
    // ��������� ���� column�� ������ ���� ������ �ݺ�
    while (1) {
        int i = 0; // for���� ���� ���� ����
        int j = 0;
        vector<string> temp1, temp2; // �ӽ� ���� ����
        vector<string> next_column; // �ӽ� ���� ����

        // 0�� �ƴ� ���� ������ �������� temp1�� ����
        while (column[j].back() == i + '0') {
            temp1.push_back(column[j++]);
            if (j > column.size() - 1)
                break;
        }

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
                    if (temp1[k][this->bit + 1] != 'x') // x�� �ڿ� ������                        
                            this->prime.push_back(temp1[k].substr(0, this->bit)); // prime ���Ϳ� Ǫ��                    
                    else // �ƴϸ� ���� �ݺ��� ���� �߰��� x ����
                        temp1[k].erase(this->bit + 1); 
            }
                      
            temp1 = temp2; // temp1�� temp2 ���� ��
            temp2.clear(); // temp2 ����. ���� ���͸� �ޱ� ���� �غ�
        }

        // �ݺ����� �������� �� �ٽ� �ѹ� temp1�� ��Ұ� prime implicant���� �Ǵ� ��
        for (int k = 0; k < temp1.size(); k++)
            if (temp1[k][this->bit + 1] != 'x') // x�� �ڿ� ������              
                prime.push_back(temp1[k].substr(0,this->bit)); // prime ���Ϳ� �߰�. temp1 ���� ���ϹǷ� �״�� ����

        // ����Լ� Ż���. ���� �Ǵ��� ���� next_column�� ������ ���ѹݺ��� Ż��
        if (next_column.empty())            
            break;

        // �ݺ��� Ż���� �ȵǸ� ���� �� �Ǵ��� ���� �ӽ÷� ������ next_column�� column�� ����
        column = next_column;
    }
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
                    if (cnt > 1)
                        break;
                }
            }

            // ���� ���ڰ� �ϳ��� �ִٸ�
            if (cnt == 1) {
                if (!temp1[i][this->bit + 1])
                    temp1[i].push_back('x'); // ���Ǿ��� �ǹ̷� �ڿ� x �߰�
                if (!temp2[j][this->bit + 1])
                    temp2[j].push_back('x'); // temp2�� ���������� x �߰�
                string str = temp1[i].substr(0, index); // ���ڿ� ó���� ���� �ӽ� ���ڿ�. index���� ���� ��
                str.push_back('-'); // ������ ���ڰ� �ִ� �ڸ��� '-'�� ǥ�� �� 
                str = str + (temp1[i].substr(index + 1, this->bit - index - 1)); // ���� �ڸ� ä���
               
                str.push_back(temp2[j][this->bit]); // 0�� �ƴ� ���� ����� �ڿ� �߰�

                // �ߺ�Ȯ��
                bool repeat = false;
                for (int l = 0; l < return_v.size(); l++) {
                    if (return_v[l] == str)
                        repeat = true;
                }

                if (repeat) // �ߺ������� ���� ���� ����
                    continue;
                else
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

    // �����迭 0���� �ʱ�ȭ
    for (int i = 0; i < this->prime.size(); i++)
        for (int j = 0; j < this->minterm.size(); j++)
            x_loc[i][j] = 0;

    for (int i = 0; i < this->prime.size(); i++) {
        for (int j = 0; j < this->minterm.size(); j++) {
            bool covered = true; // PI�� minterm�� Ŀ���ϴ��� Ȯ���ϴ� ����

            for (int k = 0; k < this->bit; k++) {
                // prime���� k��°�� '-'�̸� �׻� �����Ƿ� ���� ����
                if (this->prime[i][k] == '-')
                    continue;

                // prime�� minterm���� ���� ��ġ�� ���� ���ڰ� ������
                if (prime[i][k] != minterm[j][k]) {
                    covered = false; // covered�� false�� �ٲٰ�
                    break; // �ݺ��� Ż��
                }
            }
            // EPI�̸� �ش� ��ġ 1 ����
            if (covered) {
                x_loc[i][j] = 1;
                this->no_nextcol = false;
            }
        }
    }

    // Essential Prime Implicant ���� ��� �Լ� ȣ��
    if (!(this->no_nextcol))
        step3(x_loc);
    else
        this->output = this->prime;

    // �����Ҵ� �޸� ����
    for (int i = 0; i < this->prime.size(); i++)
        delete[] x_loc[i];
    delete[] x_loc;
}

void QM::step3(int** x_loc) {
    // ����Լ� Ż���. minterm���� ��� Ŀ�� �Ǹ� Ż��
    if (this->minterm.size() == this->checked_minterm)
        return;

    // �� ���� ��Ҹ� ���Ǹ鼭 minterm�� pi�� ��ġ�� ���� 
    int num_x = 0, index = -1;
    for (int i = 0; i < this->prime.size(); i++) {
        int temp_x = 0;
        for (int j = 0; j < this->minterm.size(); j++) {            
            if (x_loc[i][j] == 1)
                temp_x++;
        }
        // minterm�� pi�� ��ġ�°� �� ū ���� ã����
        if (temp_x > num_x) {
            num_x = temp_x; // num_x�� �����ϰ�
            index = i; // �ش� ���� ����
        }
    }

    // ������� ��ġ�°� ���� ���� pi ����
    this->output.push_back(prime[index]);
    // minterm�� ������ŭ �ݺ��ϸ�
    for (int j = 0; j < this->minterm.size(); j++) {
        // ��ġ�� ���� minterm�� ��� -1�� ����
        if (x_loc[index][j] == 1) {        
            this->checked_minterm++; // ����Լ� Ż���� ���� ���� 1 ����
            for (int k = 0; k < this->prime.size(); k++)
                x_loc[k][j] = -1;
        }
    }

    // ����Լ� ȣ��
    step3(x_loc);
}

// ���Ͽ� 
void QM::saveEPI(const char* output_path) {
    ofstream fout;
    int transistor = 0;
    int AND = 0, OR = 0, INVERTER = 0;

    fout.open(output_path);
    
    // invertor ���� ī��Ʈ
    for (int i = 0; i < this->bit; i++) {
        // ��� EPI���� �ش� ��° ��� �� �ϳ��� 0�� ������
        bool inverse = false;
        for (int j = 0; j < this->output.size(); j++) {
            if (this->output[j][i] == '0') {
                inverse = true;
                break;
            }
        }
        // inverter �ʿ��ϴٰ� �Ǵ��Ͽ� Ʈ�������� 2�� ����
        if (inverse) 
            INVERTER++;        
    }

    // AND gate ���� ī����
    for (int i = 0; i < this->output.size(); i++) {
        transistor += 2;
        for (int j = 0; j < this->bit; j++) {
            if (output[i][j] != '-') {
                transistor += 2;
                AND++;
            }
        }
    }

    // OR gate �� ���� Ʈ�������� ���� �߰�
    OR = this->output.size();
    
    transistor += ((INVERTER * 2) + (OR * 2 + 2)); // Ʈ�������� ���� ���

    // EPI ���� �Էº�
    for (string str : this->output) 
        fout << str << endl;

    // Ʈ�������� ���� ���� ��º�
    fout << endl << "Cost(# of transistors): " << transistor;
}

int main() {
    const char* input_path = "input_minterm.txt"; // �Է°��� ����� ���ϸ�
    const char* output_path = "result.txt"; // ��°��� ������ ���ϸ�
    QM* qm = new QM; // QM ��ü �����Ҵ�

    qm->LoadData(input_path); // ���Ϸκ��� ������ �о����
    qm->step1();
    qm->step2();
    qm->saveEPI(output_path);

    delete qm; // QM��ü �����Ҵ� ����

    return 0;
}