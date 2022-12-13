#include <stdio.h>
#include "schedule.h"

typedef struct myprocess_{
	int pid;
	int begin_t;
	int burst_t;
	int remaining_t;
	int response_t;
	int finish_t;
	int state;
}MyProcess;

MyProcess proc_list[1024];

int proc_num, preemptive, m;
int min_burst = 2147483647;
int min_i = -1;

void read_proc_list(const char* file_name){
	FILE* fp = fopen(file_name, "r");
	fseek(fp, 0, SEEK_SET);
	fscanf(fp, "%d", &proc_num);
	for(int i=0;i<proc_num;i++){
		fscanf(fp, "%d %d %d", &proc_list[i].pid, &proc_list[i].begin_t, &proc_list[i].burst_t);
		proc_list[i].state = 0;
		proc_list[i].remaining_t = proc_list[i].burst_t;
	}
	fclose(fp);
}

void set_schedule(int method){
	m = method;
	if(m == 1 || m == 2) preemptive=0;
	else if(m == 3) preemptive=1;
}

int do_schedule(int tick){
	int pid = -1;

	if(m == 1){
		for(int i = 0; i<proc_num;i++){
			if(tick == proc_list[i].begin_t){
				proc_list[i].state = 1;  // ready
				break;
			}
		}
		for(int i = 0; i<proc_num;i++){
			if(proc_list[i].state == 1 && proc_list[i].remaining_t>0){
				proc_list[i].state = 2; // running
			}
			if (proc_list[i].state == 2 && proc_list[i].remaining_t>0){
				if(proc_list[i].remaining_t == proc_list[i].burst_t) proc_list[i].response_t = tick;
				proc_list[i].remaining_t--;
				if (proc_list[i].remaining_t == 0){
					proc_list[i].finish_t = tick+1;
					proc_list[i].state = 3; // terminate
				}
				
				return proc_list[i].pid;
			}
		}
		
		return -1;
	}
	
	
	else if(m == 2){
		for(int i = 0; i<proc_num;i++){
			if(tick == proc_list[i].begin_t){
				proc_list[i].state = 1;  // ready
			}
		}

		if (min_i== -1) {
			for(int i = 0; i<proc_num;i++){
				if(proc_list[i].state == 1){
					if(min_burst>proc_list[i].burst_t){
						min_burst = proc_list[i].burst_t;
						min_i = i;
					}
				}
			}

		}
		if (min_i >= 0) {
			proc_list[min_i].state = 2;
			if(proc_list[min_i].remaining_t == proc_list[min_i].burst_t)proc_list[min_i].response_t = tick;
			proc_list[min_i].remaining_t--;
			pid = proc_list[min_i].pid;
			if (proc_list[min_i].remaining_t == 0) {
				proc_list[min_i].finish_t = tick+1;
				proc_list[min_i].state = 3;
				min_i = -1;
				min_burst = 2147483647;
			}
			return pid;		
		}
		return -1;
	}
	else if(m == 3){
		for(int i = 0; i<proc_num;i++){
			if(tick == proc_list[i].begin_t){
				proc_list[i].state = 1;  // ready
				if(proc_list[min_i].remaining_t>0){
					proc_list[min_i].state = 1;
					min_i = -1;
					min_burst = 2147483647;
				}
			}
		}
		if (min_i== -1) {
			for(int i = 0; i<proc_num;i++){
				if(proc_list[i].state == 1){
					if(min_burst>proc_list[i].burst_t){
						min_burst = proc_list[i].burst_t;
						min_i = i;
					}
				}
			}

		}
		if (min_i >= 0) {
			proc_list[min_i].state = 2;
			if(proc_list[min_i].remaining_t == proc_list[min_i].burst_t)proc_list[min_i].response_t = tick;
			proc_list[min_i].remaining_t--;
			pid = proc_list[min_i].pid;
			if (proc_list[min_i].remaining_t == 0) {
				proc_list[min_i].finish_t = tick+1;
				proc_list[min_i].state = 3;
				min_i = -1;
				min_burst = 2147483647;
			}
			return pid;		
		}
		return -1;
	}	
}	
void print_performance(){
	int total_tat = 0;
	int total_wt = 0;
	int total_rt = 0;
	printf("========================================================================\n");
	printf(" PID   begin   finish   Turn around time   Waiting time   Response time \n");
	printf("========================================================================\n");
	for(int i=0;i<proc_num;i++){
		int tat = proc_list[i].finish_t - proc_list[i].begin_t;
		int wt = proc_list[i].finish_t - proc_list[i].begin_t - proc_list[i].burst_t;
		int rt = proc_list[i].response_t - proc_list[i].begin_t;
		
		total_tat += tat;
		total_wt += wt;
		total_rt += rt;
		
		printf("%3d %6d %8d %15d %13d %14d\n", 
			proc_list[i].pid, proc_list[i].begin_t, proc_list[i].finish_t, tat, wt, rt);      
	}
	printf("------------------------------------------------------------------------\n");
	printf("average:%25.2f %15.2f %15.2f\n", 
		((float)(total_tat)/(float)(proc_num)), ((float)(total_wt)/(float)(proc_num)), ((float)(total_rt)/(float)(proc_num)));	
}



