#include<unistd.h>
#include<stdio.h>

#define my_queue_enqueue 335
#define my_queue_dequeue 336

int main(){
	int a = 0; // 시스템 콜 호출 반환 값을 받을 변수 a 선언
	int i; // iteration을 위한 변수 i 선언

	for (i = 1; i <= 3; i++) {
		a = syscall(my_queue_enqueue, i);
		printf("Enqueue : ");
		printf("%d\n", a);
	} // enqueue를 i에 1,2,3을 넣어서 한 번씩 호출

	a = syscall(my_queue_enqueue, 3);
	printf("Enqueue : ");
	printf("%d\n", a);
	// 이미 queue에 존재하는 값인 3을 한번 더 넣어서 중복 처리에 대한 기능 체크

	for (i = 1; i <= 3; i++) {
		a = syscall(my_queue_dequeue);
		printf("Dequeue : ");
		printf("%d\n", a);
	}
	// dequeue 3번 호출
	return 0;
}

