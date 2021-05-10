#include<iostream>
#include<fstream>
#include<string>
#include<vector>

#define DSIZE 3122 //������ ���� ũ�� 
#define NSIZE 182  //�Ϲ� ȣ���� ���̺� ũ��
#define CSIZE 1724 //Adaptive ȣ���� ���̺� ũ��
using namespace std;

unsigned char binaryToChar(string binary);	//2������ ���ڷ� ��ȯ�ϴ� �Լ�	
void processData(string& data, int size);	//������ ��ó�� �Լ�
void readData(string& nTable, string& cTable, string& data); //�����͸� �о���� �Լ�					
void processTable(string& nTable, vector<string>& table, string& cTable, vector<vector<string>>& adaptiveTable); //���̺� �ҷ����� �Լ� 


void decode(string& data, vector<string>& table, vector<vector<string>>& adaptiveTable); // ���� ����




int main() {
	string data;			//������
	string nTable;			//���̺� ������ 
	string cTable;			//Adaptive ���̺� ������ 
	vector<string> table(128);					//���̺�
	vector<vector<string>> adaptiveTable(128);	//Adaptive ���̺�

	readData(nTable, cTable, data);

	processData(data, DSIZE);
	processData(nTable, NSIZE);
	processData(cTable, CSIZE);

	processTable(nTable, table, cTable, adaptiveTable);

	decode(data, table, adaptiveTable);
	return 0;
}
void readData(string& nTable, string& cTable, string& data) {
	ifstream dataFile("encoded.hbs", ios::in | ios::binary);				//������ �ҷ����� 
	ifstream nTableFile("HuffmanTable.hbs", ios::in | ios::binary);			//���̺� ������
	ifstream cTableFile("AdaptiveHuffmanTable.hbs", ios::in | ios::binary);	//Adaptive ���̺� ������
	
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

void processData(string& data ,int size) {	//��Ʈ ������ ���� ǥ��
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
		for (int j = 0; j < 128; j++) {// �Ϲ� ȣ���� ��ȣ�� �Ǿ��ִ� ù��° ��ȯ
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
			if(idx < data.size()) code.push_back(data.at(idx)); //code �� ���� Adaptive ���̺�� ��ȯ
			for (int j = 0; j < 128; j++) {
				if (code == adaptiveTable[tmp][j]) {
					flag = 1;
					if (j == 3) { // EOD �Ǻ�
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
	for (int i = 0; i < nTable.length();) {//�Ϲ� ���̺�
		int len, Ascii;
		string ascii;
		string clen;
		string code;

		for (int j = 0; j < 8; i++, j++) {  // ascii ������ read
			if (i < nTable.length())
				ascii += nTable[i];
		}

		for (int j = 0; j < 8; i++, j++) {  // �ڵ� ���� ������ read
			if (i < nTable.length())
				clen += nTable[i];
		}

		Ascii = binaryToChar(ascii);
		len = binaryToChar(clen);

		for (int j = 0; j < len; i++, j++) { // �ڵ� ������ read
			if (i < nTable.length()) code += nTable[i];
		}

		if (i < nTable.length()) table[Ascii] = code;
		else break;
	}

	int num;
	int count = 0;

	string cont;

	for (int i = 0; i < 8; i++) cont += cTable[i], count++; // Adaptive ���̺� 
	num = binaryToChar(cont);

	for (int i = 0; i < num; i++) {
		string cnt;
		for (int j = count; j < count + 8 && j < cTable.length(); j++) { //���̺� ũ�� �Ǻ�
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
			for (int k = 0; k < 13; k++) { // EOD �Ǻ�
				if (j < cTable.length()) {
					EOD += cTable[j + k];
				}
			}
			if (EOD == table[3]) {
				count += 13;
				break;
			}

			for (int k = 0; k < 8; j++, k++) {  // ascii ������ read

				if (j < cTable.length())
					ascii += cTable[j];
				count++;
			}

			for (int k = 0; k < 8; j++, k++) {  // �ڵ� ���� ������ read
				if (j < cTable.length())
					clen += cTable[j];
				count++;
			}

			Ascii = binaryToChar(ascii);
			len = binaryToChar(clen);
			
			for (int k = 0; k < len; j++, k++) { // �ڵ� ������ read
				if (j < cTable.length()) code += cTable[j];
				count++;
			}

			if (j < cTable.length()) adaptiveTable[tmp][Ascii] = code;
			else break;
		}
	}
	
	return;
}
unsigned char binaryToChar(string binary) { // 2������ ���ڿ��� ��ȯ 
	unsigned char result = 0;
	for (int i = 0; i < binary.length(); i++) {
		if (binary[i] == '1') result += (int)pow(2, binary.length() - i - 1);
	}
	return result;
}