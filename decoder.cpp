#include<iostream>
#include<fstream>
#include<string>
#include<vector>

#define DSIZE 3122 //데이터 파일 크기 
#define NSIZE 182  //일반 호프만 테이블 크기
#define CSIZE 1724 //Adaptive 호프만 테이블 크기
using namespace std;

unsigned char binaryToChar(string binary);	//2진수를 문자로 변환하는 함수	
void processData(string& data, int size);	//데이터 전처리 함수
void readData(string& nTable, string& cTable, string& data); //데이터를 읽어오는 함수					
void processTable(string& nTable, vector<string>& table, string& cTable, vector<vector<string>>& adaptiveTable); //테이블 불러오는 함수 


void decode(string& data, vector<string>& table, vector<vector<string>>& adaptiveTable); // 압축 해제




int main() {
	string data;			//데이터
	string nTable;			//테이블 데이터 
	string cTable;			//Adaptive 테이블 데이터 
	vector<string> table(128);					//테이블
	vector<vector<string>> adaptiveTable(128);	//Adaptive 테이블

	readData(nTable, cTable, data);

	processData(data, DSIZE);
	processData(nTable, NSIZE);
	processData(cTable, CSIZE);

	processTable(nTable, table, cTable, adaptiveTable);

	decode(data, table, adaptiveTable);
	return 0;
}
void readData(string& nTable, string& cTable, string& data) {
	ifstream dataFile("encoded.hbs", ios::in | ios::binary);				//데이터 불러오기 
	ifstream nTableFile("HuffmanTable.hbs", ios::in | ios::binary);			//테이블 데이터
	ifstream cTableFile("AdaptiveHuffmanTable.hbs", ios::in | ios::binary);	//Adaptive 테이블 데이터
	
	if (nTableFile.is_open() && cTableFile.is_open() && dataFile.is_open()) {
		data.resize(DSIZE);
		nTable.resize(NSIZE);
		cTable.resize(CSIZE);
		dataFile.read(&data[0], DSIZE);
		nTableFile.read(&nTable[0], NSIZE);
		cTableFile.read(&cTable[0], CSIZE);
	}
	dataFile.close();
	nTableFile.close();
	cTableFile.close();
	return;
}

void processData(string& data ,int size) {	//비트 단위로 파일 표시
	string tmp;							
	for (int i = 0; i < size; i++) {
		for (int j = 7; j >= 0; j--) {
			if ((data[i] >> j) & 1) tmp += '1';
			else tmp += '0';
		}
	}
	data = tmp;
	return;
}

void decode(string& data, vector<string>& table, vector<vector<string>>& adaptiveTable) {
	ofstream decodeFile("output.txt", ios::out);
	string originalText;
	string code;
	code.resize(0);
	int eod = 0;
	int tmp = 0; 
	int idx = 0;
	int flag = 0;
	
	for (int i = 0; i < 13; i++) {
		code += data[i];
		idx++;
		for (int j = 0; j < 128; j++) {// 일반 호프만 부호로 되어있는 첫번째 변환
			if (table[j] == code) {
				tmp = j;
				break;
			}
		}
		if (table[tmp] == code) {
			break;
		}
	}
	code.clear();
	originalText += (char)tmp;
	while (true) {
		for (int i = 0; i < 13; i++, idx++) {
			if(idx < data.size()) code.push_back(data.at(idx)); //code 를 통해 Adaptive 테이블로 변환
			for (int j = 0; j < 128; j++) {
				if (code == adaptiveTable[tmp][j]) {
					flag = 1;
					if (j == 3) { // EOD 판별
						eod = 1;
						break; 
					}
	
					idx++;
					tmp = j;
					originalText += (char)tmp;
					break;
				}
			}
			if (flag || eod) break;

		}
		
		if (eod) break;
		flag = 0;
		code.clear();
	}
	decodeFile << originalText;
	decodeFile.close();
}

void processTable( string& nTable, vector<string>& table, string& cTable, vector<vector<string>>& adaptiveTable) { 
	for (int i = 0; i < nTable.length();) {//일반 테이블
		int len, Ascii;
		string ascii;
		string clen;
		string code;

		for (int j = 0; j < 8; i++, j++) {  // ascii 데이터 read
			if (i < nTable.length())
				ascii += nTable[i];
		}

		for (int j = 0; j < 8; i++, j++) {  // 코드 길이 데이터 read
			if (i < nTable.length())
				clen += nTable[i];
		}

		Ascii = binaryToChar(ascii);
		len = binaryToChar(clen);

		for (int j = 0; j < len; i++, j++) { // 코드 데이터 read
			if (i < nTable.length()) code += nTable[i];
		}

		if (i < nTable.length()) table[Ascii] = code;
		else break;
	}

	int num;
	int count = 0;

	string cont;

	for (int i = 0; i < 8; i++) cont += cTable[i], count++; // Adaptive 테이블 
	num = binaryToChar(cont);

	for (int i = 0; i < num; i++) {
		string cnt;
		for (int j = count; j < count + 8 && j < cTable.length(); j++) { //테이블 크기 판별
			cnt += cTable[j];
		}
		count += 8;
		int tmp = binaryToChar(cnt);
		adaptiveTable[tmp].resize(128);
		for (int j = count; j < cTable.length();) {
			int len, Ascii;
			string ascii;
			string clen;
			string code;
			string EOD;
			for (int k = 0; k < 13; k++) { // EOD 판별
				if (j < cTable.length()) {
					EOD += cTable[j + k];
				}
			}
			if (EOD == table[3]) {
				count += 13;
				break;
			}

			for (int k = 0; k < 8; j++, k++) {  // ascii 데이터 read

				if (j < cTable.length())
					ascii += cTable[j];
				count++;
			}

			for (int k = 0; k < 8; j++, k++) {  // 코드 길이 데이터 read
				if (j < cTable.length())
					clen += cTable[j];
				count++;
			}

			Ascii = binaryToChar(ascii);
			len = binaryToChar(clen);
			
			for (int k = 0; k < len; j++, k++) { // 코드 데이터 read
				if (j < cTable.length()) code += cTable[j];
				count++;
			}

			if (j < cTable.length()) adaptiveTable[tmp][Ascii] = code;
			else break;
		}
	}
	
	return;
}
unsigned char binaryToChar(string binary) { // 2진수를 문자열로 변환 
	unsigned char result = 0;
	for (int i = 0; i < binary.length(); i++) {
		if (binary[i] == '1') result += (int)pow(2, binary.length() - i - 1);
	}
	return result;
}