#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>

#define SIZE 7302

using namespace std;
class bitstream { //비트 단위로 출력하기 위한 클래스
	std::streambuf* sbuf;
	int             count = 0;
	unsigned char   byte = 0;
public:
	bitstream(std::ostream& out) : sbuf(out.rdbuf()) {}
	~bitstream() {
		if (this->count) {
			this->sbuf->sputc(this->byte);
		}
	}
	bitstream& operator<< (bool b) { //연산자 오버라이딩을 통해 버퍼에 비트 조작
		this->byte = (this->byte << 1) | b;
		if (++this->count == 8) {
			this->sbuf->sputc(this->byte);
			this->count = 0;
		}
		return *this;
	}
};
typedef struct data { // 데이터 구조체
	int amount = 0;
	unsigned int data;
}Data;

typedef struct node { //노드 구조체
	bool bit;
	Data data;
	struct node* parent;
	struct node* child[2];
}Node;

bool compare(Node* node1, Node* node2); // sort 함수 비교 함수

void readFile(string& txtData);			// 파일 읽는 함수
void dataProcess(string txtData, vector<vector<Node*>>& cData, vector<Node*>& nData); // 데이터 전처리

void makeTable(vector<Node*>& Data, vector<string>& tableCode); // 테이블 생성 함수 

void makeNTableFile(vector<string> table);									//파일에 일반 테이블 입력
void makeCTableFile(vector<vector<string>> table, vector<string>nTable);	//파일에 Adaptive 테이블 입력
void Encode(vector<vector<string>> table, vector<string>nTable);			//부호화를 진행하는 함수

int main() {
	string txtData;							//텍스트 데이터
	vector<Node*> ref;						//할당 해제를 위한 레퍼런스
	vector<Node*> nData(128);				//일반 테이블 데이터 
	vector<string> nTable(128);				//일반 테이블 
	vector<vector<Node*>> cData(128);		//Adaptive 테이블 데이터 
	vector<vector<string>> tableCode(128);	//Adaptive 테이블
	for (int i = 0; i < 128; i++) {			//노드 할당
		nData[i] = new Node;
		ref.push_back(nData[i]);
	}
	for (int i = 0; i < 128; i++) {			//Adaptive 테이블 전처리
		cData[i].resize(128);
		for (int j = 0; j < 128; j++) {
			cData[i][j] = new Node;
			ref.push_back(cData[i][j]);
		}
	}
	readFile(txtData);
	dataProcess(txtData, cData, nData);
	for (int i = 0; i < 128; i++) {			//테이블을 하나씩 만들어서 넣어줌
		cout << i << endl;
		makeTable(cData[i], tableCode[i]);
	}
	makeTable(nData, nTable);				//일반 테이블 생성 
	makeNTableFile(nTable);					//테이블 저장
	makeCTableFile(tableCode, nTable);
	Encode(tableCode, nTable);
	for (int i = 0; i < ref.size(); i++) {	//할당 해제
		delete ref[i];
	}
}

bool compare(Node* node1, Node* node2) {
	return node1->data.amount < node2->data.amount; //데이터릐 양에 따라 정렬
}
void readFile(string& txtData) {
	ifstream File("input_data.txt");				//입력 데이터 파일
	if (File.is_open()) {
		txtData.resize(SIZE);
		File.read(&txtData[0], SIZE);
		txtData += (char)3;							//EOD 데이터도 포함시키기 위해 포함
	}
	File.close();
	return;
}
void dataProcess(string txtData, vector<vector<Node*>>& cData, vector<Node*>& nData) {
	int txtlen = txtData.length();

	for (int i = 0; i < txtlen - 1; i++) {
		nData[txtData[i]]->data.amount++;								//일반 테이블 파일용 데이터
		nData[txtData[i]]->data.data = txtData[i];						
		cData[txtData[i]][txtData[i + 1]]->data.amount++;				//Adaptive 테이블 파일용 데이터
		cData[txtData[i]][txtData[i + 1]]->data.data = txtData[i + 1];
	}

	nData[txtData[txtlen - 1]]->data.data = txtData[txtlen - 1];		
	nData[txtData[txtlen - 1]]->data.amount++;

}
void makeTable(vector<Node*>& Data, vector<string>& tableCode) {
	int check = 0;

	Node* current;
	vector<Node*> ref;
	vector<Node*> trace(128);
	sort(Data.begin(), Data.end(), compare);
	vector<Node*>::iterator iter = Data.begin();
	for (int i = 0; i < 128; i++) {
		if (Data[i]->data.amount == 0) {
			check++;
			delete Data[i];
		}
	}
	Data.erase(iter, iter + check);
	iter = Data.begin();
	for (int i = 0; i < Data.size(); i++) {
		trace[Data[i]->data.data] = Data[i];
	}
	if (Data.size() > 0) {
		int scale = 0;
		Node* tmp;
		while (Data.size() != 1) { //호프만 트리 작성
			tmp = new Node;
			ref.push_back(tmp);

			tmp->bit = 0;
			tmp->data.data = 0;
			tmp->parent = NULL;

			Data[0]->bit = 0;
			Data[1]->bit = 1;
			tmp->child[0] = Data[0];
			tmp->child[1] = Data[1];
			tmp->data.amount = Data[0]->data.amount + Data[1]->data.amount;

			Data[0]->parent = tmp;
			Data[1]->parent = tmp;

			Data.erase(iter, iter + 2), iter = Data.begin();
			Data.insert(iter, tmp), iter = Data.begin();
			sort(Data.begin(), Data.end(), compare);
		}
		for (int i = 0; i < 128; i++) {
			if (trace[i] != NULL)
				scale++;
		}
		string code;	// 코드 삽입
		current = NULL;
		tableCode.resize(128);
		if (scale > 1) {
			for (int i = 0; i < trace.size(); i++) {
				code = "";
				current = trace[i];
				while (current && current->parent) {
					if (current->bit) {
						code = "1" + code;
					}
					else {
						code = "0" + code;
					}
					current = current->parent;
				}
				tableCode[i] = code;
			}
		}
		else {
			for (int i = 0; i < 128; i++) {
				if (trace[i] != NULL) {
					tableCode[i] = "0";
				}
			}
		}
		for (int i = 0; i < ref.size(); i++) {
			delete ref[i];
		}
	}
	return;
}
void makeNTableFile(vector<string> table) {
	int check = 0;
	ofstream tableFile("HuffmanTable.hbs", ios::out | ios::binary);
	bitstream bitFile(tableFile);

	for (int i = 0; i < 128; i++) {
		if (table[i].length() != 0) {
			for (int j = 7; j >= 0; --j) {// 8비트 단위로 입력(ascii 데이터)
				int word = i >> j & 1;
				bitFile << word;
				check++;
			}
			for (int j = 7; j >= 0; --j) {// 8비트 단위로 입력(코드 길이 데이터)
				int word = table[i].length() >> j & 1;
				bitFile << word;
				check++;
			}
			for (int j = 0; j < table[i].length(); j++) { // 코드를 비트로 입력
				if (table[i][j] == '1') bitFile << true;
				else bitFile << false;
				check++;
			}
		}

	}
	if (check % 8) {						//byte align 을 위한 stuffing bit 추가
		for (int i = 0; i < 8 - check % 8; i++) bitFile << false;
	}
	tableFile.close();
	return;
}
void makeCTableFile(vector<vector<string>> table, vector<string>nTable) {
	int check = 0;
	ofstream tableFile("AdaptiveHuffmanTable.hbs", ios::out | ios::binary);
	bitstream bitFile(tableFile);
	
	for (int k = 7; k >= 0; --k) {	// 8비트 단위로 입력(테이블 크기 입력)
		int word = (char)table.size() >> k & 1;
		bitFile << word;
		check++;
	}
	for (int i = 0; i < 128; i++) {
		if (table[i].size() != 0) {
			for (int k = 7; k >= 0; --k) {	// 선행문자 입력
				int word = char(i) >> k & 1;
				bitFile << word;
				check++;
			}
			for (int j = 0; j < 128; j++) {
				if (table[i][j].length() != 0) {
					for (int k = 7; k >= 0; --k) {	// 8비트 단위로 입력(ascii 데이터)
						int word = j >> k & 1;
						bitFile << word;
						check++;
					}
					for (int k = 7; k >= 0; --k) {	// 8비트 단위로 입력(코드 길이 데이터)
						int word = table[i][j].length() >> k & 1;
						bitFile << word;
						check++;

					}
					for (int k = 0; k < table[i][j].length(); k++) { // 코드를 비트로 입력
						if (table[i][j][k] == '1') bitFile << true;
						else bitFile << false;
						check++;
					}
				}
			}
			for (int j = 0; j < nTable[3].size(); j++) {	// EOD 코드 입력
				if (nTable[3][j] == '1')bitFile << true;
				else bitFile << false;
				check++;
			}
		}
	}
	if (check % 8) {						//byte align 을 위한 stuffing bit 추가
		for (int j = 0; j < 8 - check % 8; j++) bitFile << false;
	}
	tableFile.close();
}

void Encode(vector<vector<string>> table, vector<string>nTable) {
	int check = 0;

	ifstream dataFile("input_data.txt");
	ofstream File("encoded.hbs", ios::out | ios::binary);

	bitstream bitFile(File);

	string txtData;
	if (dataFile.is_open()) {
		txtData.resize(SIZE);
		dataFile.read(&txtData[0], SIZE);
		txtData += (char)3;							//데이터 마지막에 EOD 삽입
	}
	dataFile.close();
	for (int i = 0; i < nTable[txtData[0]].size(); i++) {// 첫 데이터는 일반 테이블로 입력
		if (nTable[txtData[0]][i] == '1') bitFile << true;
		else bitFile << false;
		check++;
	}
	for (int i = 1; i < txtData.size(); i++) {		//데이터를 Adaptive 테이블에 따라 입력
		for (int j = 0; j < table[txtData[i - 1]][txtData[i]].size(); j++) {
			if (table[txtData[i - 1]][txtData[i]][j] == '1') bitFile << true;
			else bitFile << false;
			check++;
		}
	}
	if (check % 8) {								//byte align 을 위한 stuffing bit 추가
		for (int i = 0; i < 8 - check % 8; i++) bitFile << false;
	}
	File.close();
	return;
}
