#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class QM {
private:
    int bit;
    // 문자열 첫번째는 minterm인지 don't care인지 판단
    // 이후 bit + 1번째 인덱스에 1의 갯수 저장
    vector<string> input_v;
    vector<string> minterm; // minterm 저장
    vector<string> prime; // Prime Implicant 저장
    vector<string> sorted_v;

public:
    ~QM() {}

    void
        LoadData(const char* path); // 생성자. 오픈 할 파일의 경로를 매개변수로 받음
    void step1();
    //vector<string> sort_vector(int len);
    vector<string> compare(vector<string>& temp1, vector<string>& temp2);
};

//  텍스트 파일에 저장된 값을 가져와서 벡터에 저장
void QM::LoadData(const char* path) {
    ifstream fin;
    string input;

    fin.open(path);

    if (!fin.is_open()) { // 제대로 파일이 열렸는지 확인
        cout << "invaild file path";
        return;
    }

    this->bit = fin.get() - '0';
    getline(fin, input); // 첫번째 줄 초기화

    // 한줄씩 받아와서 저장
    while (getline(fin, input)) { 
        input.erase(1, 1);
        input_v.push_back(input);
        if (input[0] == 'm')
            minterm.push_back(input.substr(1, bit)); // minterm은 따로 저장
    }

    fin.close();
}

void QM::step1() {
    char cnt = '0';
    int len = this->input_v.size();

    // 각 배열의 1의 갯수 카운팅
    for (int i = 0; i < len; i++) {
        for (char c : input_v[i])
            if (c == '1')
                cnt++;

        this->input_v[i].push_back(cnt); // 각 벡터 마지막에 1의 갯수 추가
        cnt = '0';
    }

    // 1의 개수에 따라 정렬 후 sorted_v에 저장
    for (cnt = '0'; cnt - '0' < bit + 1; cnt++) {
        for (int i = 0; i < len; i++) {
            if (this->input_v[i].back() == cnt)       // 1의 개수를 판단하여
                this->sorted_v.push_back(this->input_v[i].substr(1, this->bit + 1)); // 정렬된 벡터에 푸시
        }
    }


    bool repeat = true;
    vector<string> column = sorted_v;
    // 재귀적으로 다음 column이 생기지 않을 때까지 반복
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

            // hamming distance가 1인 묶음끼리 비교 후 다음 열 임시 벡터에 저장
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

        // 재귀함수 탈출부
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