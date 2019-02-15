#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

int k, preseed, postseed;
double *avg_time;		//to store avg time
FILE * ifile;

pthread_barrier_t mybarrier;		//barrier variable

void pthreadBarr(int index){		//function to test barrier
	int randBeforeTime, randRemTime;
	time_t my_time;
	auto timeinfo=localtime(&my_time);

	for(int i=1; i<=k; i++){
        time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"Going to sleep before the %dth instance of Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
        srand(preseed);
		randBeforeTime =(random()%50000)+1;
        usleep(randBeforeTime);				//spend some time
        time (&my_time);
		timeinfo = localtime (&my_time);
        fprintf(ifile,"Before the Barrier invocation for %dth instance of Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
        
        auto start = std::chrono::system_clock::now();		//start timer
        pthread_barrier_wait(&mybarrier);					//barrier_point
        auto end = std::chrono::system_clock::now();		//end timer
        
        time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"After the Barrier invocation for %dth instance of Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
        
		std::chrono::duration<double> elapsed_seconds = end-start;
		avg_time[index]+=elapsed_seconds.count();

        time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"Going to sleep after the %dth instance of Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
        srand(postseed);
		randRemTime=(random()%50000)+1;
		usleep(randRemTime);				//spend some time	
	}

}

int main(){

	vector <thread> bar;
	int n;

	ifstream ofile;
	ofstream ofile2;
	ofile.open("inp-params.txt");
	ofile2.open("Average_time.txt",ios::app);
	ifile=fopen("pthread-barr-log.txt", "a+");
	ofile>>n>>k>>preseed>>postseed;
	fprintf(ifile, "Pthread-Barr-Log of (n,k) --> (%d,%d)\n\n",n,k);
	ofile2<<"Average_time of Pthread-Barr-Log (n,k) --> ("<<n<<","<<","<<k<<")\n";

	avg_time=new double[n+1];
	memset(avg_time,0.0,sizeof(double)*(n+1));

    pthread_barrier_init(&mybarrier, NULL, n);			//initialise barrier

	for(int i=0; i<n; i++){		//create threads and call the function
		bar.emplace_back(pthreadBarr,i+1);
	}

    for (auto& v : bar){
        v.join();
    }	

    pthread_barrier_destroy(&mybarrier);

    for(int i=1; i<n+1; i++){
        ofile2<<"Thread "<<i<<" "<<avg_time[i]/(double)k<<"\n";
    	avg_time[0]+=avg_time[i];
    }
    
    double temp=(avg_time[0])/(n);
    ofile2<<"All Threads "<<temp<<"\n\n";
    fprintf(ifile, "\n\n" );
    fclose(ifile);
	ofile.close();
}