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
    vector<string> sorted_v; // 0이 아닌 문자의 갯수를 카운트하여 정렬한 벡터
    int** covered;

public:
    QM() {}
    ~QM() {

    }

    void LoadData(const char* path); // 생성자. 오픈 할 파일의 경로를 매개변수로 받음
    void step1(); // 입력받은 값드로 Prime Implicant를 구하는 멤버함수
    vector<string> compare(vector<string>& temp1, vector<string>& temp2);
    void step2();
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

// 입력받은 값드로 Prime Implicant를 구하는 멤버함수
void QM::step1() {
    int cnt = 0;
    int len = this->input_v.size();

    // 각 배열의 1의 갯수 카운팅
    for (int i = 0; i < len; i++) {
        for (char c : input_v[i])
            if (c == '1')
                cnt++;

        this->input_v[i].push_back(cnt + '0'); // 각 벡터 마지막에 1의 갯수 추가
        cnt = 0;
    }

    // 1의 개수에 따라 정렬 후 sorted_v에 저장
    for (int i = 0; i <= bit; i++) {
        for (int j = 0; j < len; j++) {
            if (this->input_v[j].back() == i + '0') // 1의 개수를 판단하여
                this->sorted_v.push_back(this->input_v[j].substr(1, this->bit + 1)); // 정렬된 벡터에 푸시
        }
    }

    // 정렬된 벡터를 임시로 column 벡터에 할당
    vector<string> column = sorted_v;
    // 재귀적으로 다음 column이 생기지 않을 때까지 반복
    while (1) {
        int i = 0; // for문을 위한 인자 선언
        int j = 0;
        vector<string> temp1, temp2; // 임시 벡터 선언
        vector<string> next_column; // 임시 벡터 선언

        // 0이 아닌 값의 개수를 기준으로 temp1에 저장
        while (column[j].back() == i + '0')
            temp1.push_back(column[j++]);

        // column의 사이즈만큼 반복
        for (; j < column.size(); i++) {
            // 0이 아닌 값의 개수를 기준으로 temp2에 저장
            while (column[j].back() == i + 1 + '0') {
                temp2.push_back(column[j++]);
                if (j >= column.size()) // j가 column의 사이즈보다 크거나 작으면 반복문 탈출
                    break;
            }

            // hamming distance가 1인 묶음끼리 비교 후 다음 열 임시 벡터에 저장
            if (!temp1.empty()) {
                // 앞서 뽑아온 temp1, temp2를 compare함수에 전달
                for (string str : compare(temp1, temp2)) // hamming distance가 1인 문자열 반환받아서
                    next_column.push_back(str); // next_column 벡터에 저장

                // temp1의 크기만큼 반복
                for (int k = 0; k < temp1.size(); k++)
                    // compare함수에서 한번도 안쓰인 prime implicant인지
                    // 그리고 이미 prime implicant에 들어갔는지 판단 후
                    if (temp1[k][this->bit + 1] != 'x' && find(prime.begin(), prime.end(), temp1[k]) == prime.end())
                        this->prime.push_back(temp1[k].substr(0, this->bit)); // prime 벡터에 푸시
                    else // 아니면 다음 반복을 위해 추가한 x 삭제
                        temp1[k].erase(this->bit + 1); 
            }
                      
            temp1 = temp2; // temp1에 temp2 저장 후
            temp2.clear(); // temp2 삭제. 다음 벡터를 받기 위한 준비
        }

        // 반복문을 빠져나온 후 다시 한번 temp1의 요소가 prime implicant인지 판단 후
        for (int k = 0; k < temp1.size(); k++)
            if (temp1[k][this->bit + 1] != 'x' && find(prime.begin(), prime.end(), temp1[k]) == prime.end())
                prime.push_back(temp1[k].substr(0,this->bit)); // prime 벡터에 추가. temp1 재사용 안하므로 그대로 종료

        // 재귀함수 탈출부. 다음 판단할 열인 next_column이 없으면 무한반복문 탈출
        if (next_column.empty())            
            break;

        // 반복문 탈출이 안되면 다음 열 판단을 위해 임시로 저장한 next_column을 column에 전달
        column = next_column;
    }

    // 마지막으로 중복요소 제거 
    this->prime.erase(unique(this->prime.begin(), this->prime.end()), this->prime.end());
}

// 해밍거리가 1인 두 묶음의 비교함수
vector<string> QM::compare(vector<string>& temp1, vector<string>& temp2) {
    vector<string> return_v; // 반환할 벡터
    
    // 비교할 두 벡터 중 하나라고 원소가 없으면 그대로 탈출
    if (temp1.size() == 0 || temp2.size() == 0) 
        return return_v;

    // 삼중 반복문으로 값 비교
    for (int i = 0; i < temp1.size(); i++) { // temp1 요소에 접근할 반복문
        for (int j = 0; j < temp2.size(); j++) { // temp2 요소에 접근할 반복문
            // cnt: 동일한 이진수가 몇개나 있는지 세는 변수
            // index: 동일한 이진수가 어디 위치하는지 저장하는 변수
            int cnt = 0, index = 0;
            // 각각의 벡터에 저장된 문자열에서 각각의 문자에 접근하기 위한 반복문
            for (int k = 0; k < this->bit; k++) {            
                if (temp1[i][k] != temp2[j][k]) { // 같은 위치에 같은 문자가 있으면
                    cnt++; // cnt 증가 및
                    index = k; // index에 해당 인덱스 저장
                }
            }

            // 같은 문자가 하나만 있다면
            if (cnt == 1) {
                temp1[i].push_back('x'); // 사용되었단 의미로 뒤에 x 추가
                temp2[j].push_back('x'); // temp2도 마찬가지로 x 추가
                string str = temp1[i].substr(0, index); // 문자열 처리를 위한 임시 문자열. index까지 저장 후
                str.push_back('-'); // 동일한 문자가 있는 자리를 '-'로 표시 후 
                str = str + (temp1[i].substr(index + 1, this->bit - index - 1)); // 남은 자리 채우기
               
                str.push_back(temp2[j][this->bit]); // 0이 아닌 수가 몇개인지 뒤에 추가

                return_v.push_back(str); // 반환할 벡터에 해당 문자열 푸시
            }
        }
    }

    // 삼중 반복문 탈출 후 비교한 값 반환
    return return_v;
}

// minterm과 PI간에 겹치는 것 판단
void QM::step2() {
    // 행과 열이 minterm과 prime implicant의 개수인 배열 동적 할당
    int** x_loc = new int* [this->prime.size()];
    for (int i = 0; i < this->prime.size(); i++)
        x_loc[i] = new int[this->minterm.size()];

    // 0으로 초기화
    for (int i = 0; i < this->prime.size(); i++)
        for (int j = 0; j < this->minterm.size(); j++)
            x_loc[i][j] = 0;

    for (int i = 0; i < this->prime.size(); i++) {
        for (int j = 0; j < this->minterm.size(); j++) {
            bool covered = true; // pi가 minterm을 커버하는지 확인하는 기준

            for (int k = 0; k < this->bit; k++) {
                // prime에서 k번째가 '-'이면 항상 맞으므로 다음 루프
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


    // 동적할당 메모리 해제
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