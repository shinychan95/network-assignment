# UDP file transfer server/client

## About Code
- socket(PF_INET, SOCK_DGRAM, 0);
   - PF_INET: IPv4 인터넷 프로토콜을 사용합니다.
   - SOCK_DGRAM: UDP/IP 프로토콜을 이용합니다.
   - (protocol) 0: 통신에 있어 특정 프로토콜을 사용을 지정하기 위한 변수이며, 보통 0 값을 사용합니다.
