# ContextAdaptiveHuffmanCoding

## ContextAdaptiveHuffmanCoding이란?
ContextAdaptiveHuffmanCoding 의 경우 파일을 읽은 뒤, 각 문자의 출현 빈도에 따라, huffman table을 구성하는 것으로 
이번 프로젝트의 경우에는 미리 training data를 사용하여 huffman table을 구성하고 이를 통해 다른 파일들을 encoding 하여
파일의 크기를 줄이는 것이 목표이다.
## 왜 ContextAdaptiveHuffmanCoding 인가?
일반적인 HuffmanCoding의 경우에는 Table과 Data 두가지 모두를 전해주어야 decoding이 가능하고 encoding 과정이 오래 걸리게 되는데
미리 만들어진 Huffman table을 사용하면, 더 빠르게 encoding을 진행할 수 있고 HuffmanCoding으로 인해 만들어진 데이터 또한 더 적은
용량을 가지게 된다.(일종의 코덱(미리 만들어진 Huffman table)을 미리 받아놓으면 되므로)
