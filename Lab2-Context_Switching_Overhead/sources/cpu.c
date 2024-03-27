#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdint.h>

#define ROW (100)
#define COL ROW

int play=1; // signal handling을 위한 변수

void signalHandler() {
   play=0;
} // main 함수의 signal함수의 function parameter로 넘겨주기 위한 함수로 play=0으로 만들어 프로그램 실행을 중단시킴

// RT-RR 스케줄링을 이용하기 위한 구조체와 함수의 선언
struct sched_attr {
   uint32_t size;
   uint32_t sched_policy;
   uint64_t sched_flags;
   int32_t  sched_nice;
   uint32_t sched_priority;
   uint64_t sched_runtime;
   uint64_t sched_deadline;
   uint64_t sched_period;
};

static int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
    return syscall(SYS_sched_setattr, pid, attr, flags);
}

// child process에서 행렬 연산을 담당하고 연산 결과를 print하는 함수 calc
int calc(int time, int cpu){
   int matrixA[ROW][COL];
   int matrixB[ROW][COL];
   int matrixC[ROW][COL];
   int i, j, k;

   int cpuid = cpu;
   int count = 0; // 행렬 연산 횟수 count
   int quantum = 1; // 1 quantum을 100ms로 하여 100ms 지날때마다 출력하기위해 사용할 변수 선언
   long elapsedTime = 0; // 현재 경과 시간 측정할 변수
   
   struct timespec start, end;
   clock_gettime(CLOCK_REALTIME, &start); // 시작 시간을 start에 저장
   while(1){
      for(i=0; i < ROW; i++) {
         for(j=0; j < COL; j++) {
            for(k=0; k < COL; k++) {
               matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
            }
         }
         count++; // 2중 for문이 돈 이후 count을 증가시켰음
         clock_gettime(CLOCK_REALTIME, &end);// count가 증가할때마다 현재 시각을 측정하여 end라는 변수에 넣고
         elapsedTime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000; // 초 단위에는 10^3을 곱하고 ns단위에는 10^6을 나누어 ms단위로 측정
         if(elapsedTime / 100 >= quantum){ // 100ms quantum 구간을 넘어갈때마다 현재 count를 출력하고 quantum을 증가시켜서 100ms 당 한 번씩만 출력되도록 구현
            ++quantum;
            printf("PROCESS #%02d count = %d 100\n",cpuid, count);
         }
         if(elapsedTime>=time*1000 || play==0){ // 입력된 시간이 초 단위이므로 1000을 곱하여 ms단위로 맞추고 비교하여 시간이 경과되었거나 signal handler에 의해 play가 0이 되었다면 종료문구 출력
            printf("DONE!! PROCESS #%02d : %06d %ld\n", cpuid, count, elapsedTime);
            return 0;
         }
      }
   }
   return 0;
}

int main(int argc, char **argv){
   int numProcess = atoi(argv[1]);
   int givenTime = atoi(argv[2]); // command line argument를 받아서 각각 int형으로 변경
   int parentPid = getpid(); // 현재 부모 프로세스의 pid 저장해놓음
   int status = 0; // 부모 process가 child process가 끝날때까지 대기하기 위해 사용할 변수
   pid_t wpid;

   signal(SIGINT, signalHandler); // ctrl+c입력 시 signal handling을 위한 함수

   int result;
   struct sched_attr attr;
   memset(&attr, 0, sizeof(attr));
   attr.size = sizeof(struct sched_attr);
   attr.sched_priority = 10;
   attr.sched_policy = SCHED_RR; // 스케줄링 정책을 변경하고 dmesg 출력할 process 구분을 위해 priority 10으로 설정
   result = sched_setattr(getpid(), &attr, 0);
   if (result == -1) {
       perror("Error calling sched_setattr.");
   } else { // 스케줄링 정책 setting에 error가 없다면
	for(int i=0; i<numProcess; i++){ // 입력받은 process 개수만큼 process를 for문을 돌며 fork()로 생성 
      		printf("Creating Process: #%d\n", i);
      		pid_t pid;
      		pid = fork();  
      		if(pid==0){ // 생성된 자식프로세스인 경우에만 calc함수 수행하고 break, 부모프로세스(pid!=0)인 경우 for문 이어 수행
         		calc(givenTime, getpid() - parentPid -1); // 수행 시간과 프로세스 번호를 parameter로 calc수행
         		break;
      		}
   	}
   }
   while ((wpid = wait(&status)) > 0); // 부모 프로세스는 child process가 전부 종료될때까지 대기한다 
   return 0;
}
