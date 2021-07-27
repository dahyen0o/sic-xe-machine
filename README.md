# sic-xe-machine

## Project 1
이 프로그램은 어셈블러, 링커, 로더들을 실행하게 될 셸과 컴파일을 통해서 만들어진 object코드가 적재되고 실행될 메모리공간, mnemonic을 opcode값으로 변환하는 OPCODE 테이블과 관련 명령어들을 구현하는 프로그램이다. 따라서 프로그램을 실행하면 unix shell처럼 입력 프롬프트 상태가 되며, 원하는 명령어를 입력하면 해당하는 기능을 프로그램이 수행하게 된다. 이는 실제 머신이 아니라 임의로 만들어낸 가상 머신이다.
<br>해당 프로그램에는 크게 help, dir, quit, history, dump, edit, fill. reset, opcode, opcodelist로 10개의 명령어가 존재한다. 이 중 앞의 4개는 shell 관련 명령어, 그 뒤의 4 개는 메모리 관련 명령어, 마지막으로 남은 두 개는 opcode table 관련 명령어이다.

	- help: 명령어 목록을 보여주어 사용자에게 프로그램 사용에 도움을 준다.<br>
	- dir: 현재 디렉토리에 존재하는 모든 파일을 알려준다.<br>
	- quit: 프로그램을 종료시킨다.<br>
	- history: 올바르지 않은 입력값을 제외하고, 프로그램이 시작했을 때부터 입력해온 명령어들의 목록을 보여준다.<br>
	- dump: 입력값에 따라 알맞은 형식으로 메모리를 출력한다.<br>
	- edit: 메모리 값을 지정해 변경한다.<br>
	- fill: 연속된 메모리의 값을 변경한다.<br>
	- reset: 메모리 값을 0으로 모두 초기화한다.<br>
	- opcode: 해당 명령어에 해당하는 opcode를 출력한다.<br>
	- opcodelist: 저장되어 있는 opcode 목록을 알맞게 출력한다.

올바르지 않은 명령어를 콘솔창에 입력했을 때 화면에 부적절한 명령어로 에러가 났음을 표시하고 다음 명령어를 받기 위해 대기한다. 따라서 어떤 입력값을 주어도 강제로 프로그램이 종료되는 일은 없다. 따라서 종료를 원하면 quit 명령어를 입력하여 프로그램을 종료시킨다. 
<br>
</br>
## Project 2
저번 프로젝트 1 에서 구현한 셀(shell)에 assemble 기능을 추가하는 프로그램이다. SIC/XE의 assembly program source 파일(.asm)을 입력 받아서 object파일을 생성하고, 어셈블리 과정 중 생성된 symbol table과 결과물인 object 파일을 볼 수 있는 기능을 추가 구현한다.
<br>추가된 명령어는 assemble, symbol, type인데 assemble은 asm 파일을 받아서 알맞은 object 및 list 파일을 만들어 내는 명령어이다. 이 때 중간에 오류가 나면 object, list 파일은 생성되지 않으며 종료된다. 또한 symbol table도 정상적으로 assemble이 되었을 경우 생성되는데, 이는 symbol 명령어를 통해 심볼 알파벳의 오름차순으로 확인할 수 있다. type 명령어는 filename에 해당하는 파일을 현재 디렉터리에서 읽어서 화면에 출력시킨다.
<br>
</br>
## Project 3
프로젝트 1, 2에서 구현한 셀(shell)에 linking과 loading 기능을 추가하는 프로그램이다. 프로젝트 2에서 구현된 assemble 명령을 통해서 생성된 object 파일을 link시켜 메모리에 올리는 일을 수행한다.
<br>이번 프로그램은 크게 세 가지의 명령어를 구현하였다. 각각 loader, bp, run이다. 우선 loader 명령어를 통해 obj 파일을 link 시켜 메모리에 올렸다. 이 때 프로그램의 시작 주소는 progadd 명령어를 통해 설정할 수 있다. bp는 break point 관련 명령어로 break point를 생성, 삭제하며 현재 bp 목록을 콘솔창에 출력한다. 마지막으로 run은 loader 명령어로 메모리에 load된 프로그램을 실행하는 데, bp 명령어로 지정한 break point가 있으면 그 때까지만 실행된다.
