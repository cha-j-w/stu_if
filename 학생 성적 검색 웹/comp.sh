#!/bin/bash

echo "1. ds/mystr.c 개별 컴파일 중 (uint 매크로 주입)..."
x86_64-w64-mingw32-gcc -Duint="unsigned int" -O2 -I. -c ds/mystr.c -o ds/mystr.o

echo "2. 나머지 소스 파일 컴파일 중..."
x86_64-w64-mingw32-gcc -O2 -I. -c main.c -o main.o
x86_64-w64-mingw32-gcc -O2 -I. -c API.c -o API.o
x86_64-w64-mingw32-gcc -O2 -I. -c cJSON.c -o cJSON.o
x86_64-w64-mingw32-gcc -O2 -I. -c html.c -o html.o
x86_64-w64-mingw32-gcc -O2 -I. -c mongoose.c -o mongoose.o
x86_64-w64-mingw32-gcc -O2 -I. -c ds/hashmap.c -o ds/hashmap.o
x86_64-w64-mingw32-gcc -O2 -I. -c ds/linked_list.c -o ds/linked_list.o
x86_64-w64-mingw32-gcc -O2 -I. -c ds/tree.c -o ds/tree.o

echo "3. 오브젝트 파일들을 묶어 student_web_server.exe 생성 중..."
x86_64-w64-mingw32-gcc -o student_web_server.exe main.o API.o cJSON.o html.o mongoose.o ds/hashmap.o ds/linked_list.o ds/mystr.o ds/tree.o -lws2_32

echo "4. 임시 오브젝트(.o) 파일 정리 중..."
rm -f *.o ds/*.o

echo "컴파일 완료! student_web_server.exe가 생성되었습니다."
