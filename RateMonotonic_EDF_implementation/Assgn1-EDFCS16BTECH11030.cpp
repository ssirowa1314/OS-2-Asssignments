#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <limits.h>
using namespace std;

class Process{					//class to store data about processes
	int pid, p, k, t, kcopy, deadline, progress, index, start, waiting, turnaround, temp;		/*pid=process id
								t=processing time
								k=number of such processes
								p=period
								index is used for mapping later
							*/
	bool exist;
public:
	Process(int a, int b, int c, int d){			//constructor
		pid=a;
		t=b;
		deadline=p=c;
		kcopy=k=d;
		temp=turnaround=waiting=progress=0;

	}

	int get_deadline() const{				//function to return period
		return deadline;
	}

	int get_pid() const{
		return pid;
	}

	

	friend class Simulate;			//to allow Simulate to access private members of the class

};


bool operator<(const Process &a, const Process &b){			//overloaded operator to make a min_priority queue based on periods
  	if(a.get_deadline() != b.get_deadline())
  		return a.get_deadline() > b.get_deadline();
  	else{
  		return a.get_pid() > b.get_pid();
  	}
}

class Simulate{							//class to simulate the scheduling algorithm
	vector <Process> process;
	priority_queue <Process> ready_queue;
	int time_total, time, ksum, missed, completed;

public:
	Simulate(){
		time_total=-1;
		missed=completed=ksum=time=0;
	}

	void add_process(){
		int pid, t, p, k, n;

		ifstream ifile;
		ifile.open("inp-params.txt");
		ifile>>n;

		for(int i=1; i<=n; i++){
			ifile>>pid >>t >>p >>k;
			ksum+=k;
			if(time_total<p*k)
				time_total=p*k;
			process.emplace_back(pid,t,p,k);
		}
		for(int i=0; i<process.size(); i++)
			process[i].index=i;
	}

	

	void print(){
		vector <Process> pq;
		int x=ready_queue.size();
		for(int i=0; i<x; i++){
			pq.push_back(ready_queue.top());
			ready_queue.pop();
		}
		for(int i=0; i<x; i++){
			ready_queue.push(pq[i]);
		}

		//print2();
	}

	void schedule(){
		int current=process[0].index, min_deadline=INT_MAX, c=0, p_left=process.size(), check=0, check1=0, check2=0;
		
		ofstream ifile;
		ifile.open("EDF-Log.txt");

		while(time<=time_total){

			for(int i=0; i<ready_queue.size(); i++){
				//cout<<"a\n";
				if(time != 0 && ready_queue.top().deadline < time){
					ready_queue.pop();
					if(ready_queue.size())
						current=ready_queue.top().index;
				}
				else{
					break;
				}
			}

			min_deadline=INT_MAX;

			for(int i=0; i<process.size(); i++){
				if(time != 0 && process[i].deadline <= time && current != process[i].index && process[i].deadline != 0 && process[i].progress <process[i].t){
					
					ifile<<"Process "<<process[i].pid<<" misses deadline at t="<<time<<" units\n";
					missed++;
					process[i].k--;
					process[i].turnaround+=process[i].p;
					process[i].waiting+=time-process[i].temp;
					process[i].exist=false;
					check2=1;
					if(process[i].k == 0){
						process[i].deadline=INT_MAX;
						p_left--;
						process[i].k--;
					}
				}
				if(time % process[i].p == 0 && process[i].k>0 && check1 == 0){
					if(process[i].k == 1 && check2 == 0 && process[i].exist == true  /*|| process[i].k == 1 && process[i].deadline == time && process[i].exist == true*/){
						if(process[i].deadline < min_deadline){
							min_deadline=process[i].deadline;
							c=i;
						}
						continue;
					}
					process[i].exist=true;
					process[i].temp=process[i].start=time;
					process[i].progress=0;
					if(time != 0)
						process[i].deadline=time+process[i].p;
					ready_queue.push(process[i]);
					
						ifile<<"Process "<<process[i].pid<<" joined the system at t="<<time<<" units\n";
				}
				if(process[i].deadline < min_deadline){
					min_deadline=process[i].deadline;
					c=i;
				}
				check2=0;
			}
			check2=0;
			if(time == 0){
				ifile<<"Process "<<ready_queue.top().pid<<" starts execution at t="<<time<<" units\n";
				current=ready_queue.top().index;
			}
			if(time != 0 && ready_queue.size() && ready_queue.top().index != current && process[current].progress < process[current].t){
				process[current].temp=time;
				ifile<<"Process "<<process[current].pid<<" preempted by Process "<<process[ready_queue.top().index].pid<<" at t="<<time<<" units; Remaining time="<<process[current].t-process[current].progress<<"\n";
				current=ready_queue.top().index;
				time++;
				process[current].progress++;
				ifile<<"Process "<<process[current].pid<<" starts execution at t="<<time<<" units\n";
				process[current].waiting+=time-process[current].temp-1;
				process[current].temp=time-1;
				continue;
			}
			else if(time != 0 && ready_queue.size() && ready_queue.top().index != current && process[current].progress == process[current].t){
				process[current].turnaround+=(time-process[current].start);
				ifile<<"Process "<<process[current].pid<<" finishes execution at t="<<time<<" units"<<endl;
				if(time % process[current].p != 0)
					process[current].exist=false;
				process[current].deadline=INT_MAX;
				process[current].k--;
				time++;
				current=ready_queue.top().index;
				process[current].progress++;
				ifile<<"Process "<<process[current].pid<<" starts execution at t="<<time<<" units"<<endl;
				process[current].waiting+=time-1-process[current].temp;
				process[current].temp=time-1;
				continue;
			}

			

			if(process[current].progress == process[current].t){
				process[current].turnaround+=(time-process[current].start);
				process[current].k--;
				ready_queue.pop();
				ifile<<"Process "<<process[current].pid<<" finishes excecution at t="<<time<<" units\n";
				if(time % process[current].p != 0){
					process[current].exist=false;
				}
				process[current].deadline=INT_MAX;
				check=1;
				completed++;
			}
			else if(ready_queue.top().deadline <= time){
				process[current].k--;
				process[current].turnaround+=process[current].p;
				ready_queue.pop();
				ifile<<"Process "<<process[current].pid<<" terminated as it misses deadline at t="<<time<<" units\n";
				if(time % process[current].p != 0)
					process[current].exist=false;
				process[current].deadline=INT_MAX;
				check=1;
				missed++;
			}

			if(process[current].k == 0){
				p_left--;
				process[current].deadline=INT_MAX;
			}

			time++;

			//

			for(int i=0; i<ready_queue.size(); i++){
				if(time != 0 && ready_queue.top().deadline < time){
					ready_queue.pop();
					if(ready_queue.size())
						current=ready_queue.top().index;
				}
				else
					break;
			}
			if(ready_queue.size() == 0){
				if(p_left <= 0){
					break;
				}
				ifile<<"System remains idle till t="<<min_deadline<<" units"<<endl;
				time=min_deadline;
				c=-1;
			}
			if(check==1){
				for(int i=0; i<process.size(); i++){
					if(time % process[i].p == 0 && process[i].k>0){
					
						process[i].exist=true;
						process[i].temp=process[i].start=time;
						process[i].progress=0;
						process[i].deadline=time+process[i].p;
						ready_queue.push(process[i]);
						ifile<<"Process "<<process[i].pid<<" joined the system at t="<<time<<" units\n";
						check1=1;
					}
				}
				current=ready_queue.top().index;
				if(process[current].progress == 0){ 
					process[current].waiting+=time-process[current].temp-1;
					ifile<<"Process "<<process[current].pid<<" starts execution at t="<<time<<" units\n";
				}
				else{
					process[current].waiting+=time-process[current].temp-1;
					ifile<<"Process "<<process[current].pid<<" resumes execution at t="<<time<<" units\n";
				}
				check=0;
			}
			if(c!=-1)
				process[current].progress++;
		}
		ifile.close();

		
	}

	void make_stats(){
		ofstream ifile;
		ifile.open("EDF-Stats.txt");

		ifile<<"number of processes came into system="<<ksum<<endl;
		ifile<<"number of processes successfully completed="<<completed<<endl;
		ifile<<"number of processes missed deadline="<<missed<<endl;

		for(int i=0; i<process.size(); i++){
			ifile<<"Average waiting time for process "<<process[i].pid<<"="<<((float)process[i].waiting/(float)process[i].kcopy)<<endl;
		}

		ifile.close();
	}

	void avg_time(){
		ofstream ifile;
		ifile.open("Average_times.txt", ios::app);

		int w=0,t=0;

		for(int i=0; i<process.size(); i++){
			w+=process[i].waiting;
			t+=process[i].turnaround;
		}

		ifile<<"Average waiting time for Earliest Deadline First algorithm="<<(float)w/(float)process.size()<<endl;
		ifile<<"Average turnaround time for Earliest Deadline First algorithm="<<(float)t/(float)process.size()<<endl<<endl;

		ifile.close();
	}


};

int main(){
	Simulate sim;

	sim.add_process();

	sim.schedule();

	sim.make_stats();

	sim.avg_time();
}