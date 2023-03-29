# ContextAdaptiveHuffmanCoding

## ContextAdaptiveHuffmanCoding이란?
ContextAdaptiveHuffmanCoding 의 경우 파일을 읽은 뒤, 각 문자의 출현 빈도에 따라, huffman table을 구성하되, 앞 문자와의 출현 빈도를 고려하여 Huffman Coding을 진행하는 것으로 일반 HuffmanCoding에 비해 더 큰 비율로 파일의 크기를 줄일 수 있다.
이번 프로젝트의 경우에는 미리 training data를 사용하여 context adaptive huffman table을 구성하고 이를 통해 다른 파일들을 encoding 하여 파일의 크기를 줄이는 것이 목표이다.
## 왜 ContextAdaptiveHuffmanCoding 인가?
일반적인 Huffman Coding의 경우에는 문자의 출현 빈도만 고려하여 작성되는 반면, Context Adaptive Huffman Coding의 경우에는 앞 문자와의 관계를 따진 conditional probability를 사용하기 때문에 더 효과적(크기를 많이 줄이는 방향)으로 encoding을 진행할 수 있다. 하지만 encoding 과정에서 일반 Huffman Coding에 비해 더 큰 시간 소모가 이루어지고, Huffman Table의 크기다 더 커진다는 단점이 존재하기 때문에 경우에 따라 더 많은 사항을 고려하여 적용되어야 한다.
