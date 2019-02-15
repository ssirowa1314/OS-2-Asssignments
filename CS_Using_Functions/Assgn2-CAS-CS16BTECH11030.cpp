#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;


int n, k, cseed, rseed;
atomic<int>locks(0);
ofstream ofile1;
FILE * ofile2;
double *avg_time;


void testCS(int tid){

	time_t my_time;
	auto timeinfo=localtime(&my_time);
	int randcstime, randremtime;

	for(int i=1; i<=k; i++){

		time (&my_time);
		timeinfo = localtime (&my_time);
		auto start = std::chrono::system_clock::now();
		fprintf(ofile2,"%dth CS request by Thread %d at %d:%d:%d\n",i,tid,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		
		while(1)
		{
			int x=0,y=1;
			if(locks.compare_exchange_weak(x,y)!=0) //compare and swap function
				break;
		}

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ofile2,"%dth CS entry by Thread %d at %d:%d:%d\n",i,tid,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

		srand(cseed);
		randcstime =(random()%50)+1;
		usleep(randcstime);
		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ofile2,"%dth CS exit by Thread %d at %d:%d:%d\n",i,tid,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

		locks=0;

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		avg_time[tid]+=elapsed_seconds.count();

		srand(cseed);
		randremtime=(random()%50)+1;
		usleep(randremtime);
	}
}



int main(){
// create 'n' testCS threads
	ifstream ifile;
	ifile.open("inp-params.txt");
	ofile1.open("Average_time.txt,",ios::app);
	ofile2=fopen("CAS-log.txt", "a+");
	ifile>>n>>k>>cseed>>rseed;
	fprintf(ofile2,"\n\nLog of Compare and Swap (n,k,csseed,remseed) -> (%d,%d,%d,%d)\n",n,k,cseed,rseed);
	avg_time=new double[n+1];
	memset(avg_time,0.0,sizeof(double)*(n+1));
	vector <thread> t;

	for(int i=0; i<n; i++){
		t.emplace_back(testCS,i+1);
	}
	for (auto& v : t){
        v.join();
    }

	ofile1<<"\n\nCompare and Swap Avg Times(n,k,csseed,remseed) -> ("<<n<<","<<k<<","<<cseed<<","<<rseed<<")\n";
    avg_time[0]=0.0;
    for(int i=1; i<=n; i++)
    	avg_time[0]+=avg_time[i];

    ofile1<<"\n\n"<<avg_time[0]/(double)n;
    ofile1<<"\n\n";

    fprintf(ofile2,"\n\n");

	ofile1.close();
	fclose(ofile2);
	ifile.close();
}

