# BOOST/ASIO를 이용한 서버 구현

## 빌드방법

    사실 컴파일을 다 해놓았기에 바로 ./server 명령어를 통해서 결과를 확인하실수 있습니다.
    빌드 환경은 g++ -o server server.cpp -lsqlite3 -std=c++11 입니다.
    c++11을 택한 이유는 boost 라이브러리에 템플릿 부분이 > >와 같이 띄어쓰기 하나만 차이나도 인식을 못하는 경우가 있어서 일부로 버젼을 낮춰서 빌드 했습니다.

## GENERATOR

    사용법은 다음과 같습니다.
    ./server controller "생성하고자 하는 프로그램명" 
    이렇게 입력하면 제가 구현해두었던 controler.h 코드의 재사용 가능한 부분을 묶어서 
    정리해둔 generator.h 코드가 복사되어 생성됩니다. 

## DB

    앞서 웹서빙 기능에서 설명되었어야 하지만 패킷 생성이 실패한 관계로 따로 설명드립니다.
    패킷 생성에만 실패한 것이라 DB와는 정상작동을 합니다. 따라서 웹 요청의 GET, POST, DELETE, PATCH, PUT까지 DB에 작동하는 것을 확인했습니다. DB를 직접 확인하셔도 좋고 콘솔창을 통해서도 확인이 가능합니다.
    처음부터 높은 수준의 DB를 설계할 것이 아니였기에 DB구조는 매우 간단합니다.
    
     ID    NAME      AGE
    INT   STRING     INT 의 형태로 저장되어 있습니다.
    
## GET Request

    1. /table            =>     테이블의 모든 주소를 반환

    2. /table/{pk}       =>     {pk}에 해당하는 Id를 가진 컬럼 조회

## POST Request

    (name, age)를 모두 받아야지만 DB에 새로 추가됨
    
## PUT/PATCH

    /table/{pk} 에서 {pk}에 해당하는 id값을 가지고 있으면서
    (name, age)를 둘다 가지거나 둘중에 하나만 데이터로 들어와도 UPDATE를 실행함

## DELETE

    /table/{pk} 에서 {pk}에 해당하는 id값을 가진 컬럼 제거
