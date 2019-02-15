#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;


int n, k, cseed, rseed;
bool *request;
atomic_flag locks = ATOMIC_FLAG_INIT;
ofstream ofile1;
FILE * ofile2;
double *avg_time;


void testCS(int tid){

	bool key;
	time_t my_time;
	auto timeinfo=localtime(&my_time);
	int randcstime, randremtime;

	for(int i=1; i<=k; i++){

		time (&my_time);
		timeinfo = localtime (&my_time);
		auto start = std::chrono::system_clock::now();
		fprintf(ofile2,"%dth CS request by Thread %d at %d:%d:%d\n",i,tid,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		
		request[tid]=true;
		key=true;
		while(request[tid] && key){
			key=atomic_flag_test_and_set(&locks);

		}
		request[tid]=false;
		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ofile2,"%dth CS entry by Thread %d at %d:%d:%d\n",i,tid,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

		srand(cseed);
		randcstime =(random()%50)+1;
		usleep(randcstime);
		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ofile2,"%dth CS exit by Thread %d at %d:%d:%d\n",i,tid,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		avg_time[tid]+=elapsed_seconds.count();

		int j=(tid)%n+1;
		while((j!=tid) && request[j] == false){
			j=j%n+1;
		}
		if(j==tid)
			atomic_flag_clear(&locks);
		else
			request[j]=false;

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
	ofile2=fopen("TAS_bounded-log.txt", "a+");
	ifile>>n>>k>>cseed>>rseed;
	fprintf(ofile2,"\n\nLog of Test and Set Bounded (n,k,csseed,remseed) -> (%d,%d,%d,%d)\n",n,k,cseed,rseed);
	request=new bool[n+1];
	avg_time=new double[n+1];
	memset(request,false,sizeof(bool)*(n+1));
	memset(avg_time,0.0,sizeof(double)*(n+1));
	vector <thread> t;
	for(int i=0; i<n; i++){
		t.emplace_back(testCS,i+1);
	}
	for (auto& v : t){
        v.join();
    }
    ofile1<<"\n\nTest and Set Bounded Avg Times(n,k,csseed,remseed) -> ("<<n<<","<<k<<","<<cseed<<","<<rseed<<")\n";
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

