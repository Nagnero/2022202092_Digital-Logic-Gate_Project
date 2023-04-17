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

    bit = fin.get() - '0'; // 첫번째 입력값으로 비트수 저장
    fin.get();
    // text파일의 데이터를 2차원 벡터 배열에 대입
    while (getline(fin, input)) { // text파일에서 한줄씩 input 대입
        temp_v.push_back(input.substr(0, 1));   // d나 m 벡터에 삽입
        temp_v.push_back(input.substr(1, bit)); // bit수만큼 벡터에 삽입
        temp_v.clear();                         // 초기화

        input_v.push_back(temp_v);
    }

    // 2차원 벡터 할당 해제

    // temp_v 할당해제
    temp_v.clear();
    temp_v.shrink_to_fit();

    return 0;
}