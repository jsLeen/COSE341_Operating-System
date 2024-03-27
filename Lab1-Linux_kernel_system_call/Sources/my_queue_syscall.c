#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/linkage.h>

#define MAXSIZE 500

int queue[MAXSIZE]; // 큐를 구현하는 배열 선언
int front = 0; // 큐의 가장 앞 원소 위치를 의미하는 index
int rear = 0; // 큐의 가장 마지막 원소 한 칸뒤를 의미하는 index
int i, res = 0; // for문 iteration을 위한 변수 i와 dequeue시에 반환 값을 담을 변수 res 선언

SYSCALL_DEFINE1(oslab_enqueue, int , a){ // enqueue에 대한 system call 정의하는 함수
	if(rear >= MAXSIZE - 1){
		printk(KERN_INFO "[Error] - QUEUE IS FULL---------------\n");
		return -2; // queue에 남은 공간이 있는지 확인하고 없다면 error 메시지 출력과 -2를 반환
	} else { // 남은 공간이 있다면
		for(i = front; i < rear; i++) { // 큐의 앞부터 마지막까지 scan하면서
			if(queue[i] == a) { // 입력값으로 들어온 a가 이미 큐에 존재한다면
				printk(KERN_INFO "[Error] - %d is already existing value\n", a);
				return a; // error 메시지 출력과 이미 존재하는 값 a 반환
			}
		}
		queue[rear++] = a; // for문을 통과했다는 것은 입력값이 queue에 존재하지 않으므로 정상적인 enqueue가 가능하므로 rear의 index에 a를 넣고 값을 1 증가시킴				
	}
	printk(KERN_INFO "[System call] oslab_enqueue(); -----\n");
	printk("Queue Front---------------------\n");
	for (i = front; i < rear; i++) {
		printk("%d\n", queue[i]);
	} // enqueue 이후 현재 큐의 상태 for문으로 출력
	printk("Queue Rear---------------------\n");
	
	return a; // enqueue 된 값 반환
}

SYSCALL_DEFINE0(oslab_dequeue){ // dequeue에 대한 system call 정의하는 함수
	if(rear == front){
		printk(KERN_INFO "[Error] - EMPTY QUEUE---------------\n");
		return -2; // rear == front로 queue가 비어있는지 확인하고 비어있다면 error 메시지를 출력하고 -2 반환
	}
	res = queue[front]; // queue가 비어있지 않다면 dequeue 가능하므로 dequeue할 값을 res에 저장
	front++; // front가 기존 front의 다음 index를 가리키게 값을 1 증가시킴 
	printk(KERN_INFO "[System call] oslab_dequeue(); -----\n");
	printk("Queue Front---------------------\n");
	for (i = front; i < rear; i++) {
		printk("%d\n", queue[i]);
	} // dequeue 이후 현재 큐의 상태 for문으로 출력
	printk("Queue Rear---------------------\n");
	
	return res; // dequeue 된 값 반환
}
