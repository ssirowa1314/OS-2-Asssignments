#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

int k, preseed, postseed, barCount, currentBarCount;
sem_t mutex_, mutex2_;
double *avg_time;
FILE * ifile;

/*currentBarCount:  keeps track of how many have threads have reached barrier
barCount: keeps thread of number of threads
avg_time: keeps track of average of threads
mutex_: semaphore for incrementing currentBarCount across threads
mutex2_: semophore for blocking threads before all of them reaches barrier_point*/

int barrier_point(){            //function to implement barrier point
    sem_wait(&mutex_);
    currentBarCount++;
    sem_post(&mutex_);

    if(currentBarCount < barCount)
        sem_wait(&mutex2_);
    else{
        for(int i=1; i<barCount; i++){
            sem_post(&mutex2_);
        }
        currentBarCount=0;
    }

    while(currentBarCount);    
}

int barrier_init(long num){     //function to intialise variables
    barCount=num;
    currentBarCount=0;
    sem_init(&mutex_,0,1);
    sem_init(&mutex2_,0,1);
    avg_time=new double[num+1];
    memset(avg_time,0.0,sizeof(double)*(num+1));        //array to store average time
}

void newBarr(int index){            //function to test barrier
    int randBeforeTime, randRemTime;
    time_t my_time;
    auto timeinfo=localtime(&my_time);

    for(int i=1; i<=k; i++){
        time (&my_time);
        timeinfo = localtime (&my_time);
        fprintf(ifile,"Going to sleep before the %dth instance of Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
        srand(preseed);
        randBeforeTime =(random()%50000)+1;
        usleep(randBeforeTime);     //spend some time
        time (&my_time);
        timeinfo = localtime (&my_time);
        fprintf(ifile,"Before the Barrier invocation for %dth instance of Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
       
        auto start = std::chrono::system_clock::now();      //start timer
        barrier_point();
        auto end = std::chrono::system_clock::now();        //end timer
        //printf("%d %d\n",i,index );

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
        usleep(randRemTime);        //spend some time   
    }

}

int main(){

    vector <thread> bar;
    int n;

    ifstream ofile;
    ofstream ofile2;
    ofile.open("inp-params.txt");
    ofile2.open("Average_time.txt",ios::app);
    ifile=fopen("new-barr-log.txt", "a+");
    ofile>>n>>k>>preseed>>postseed;
    fprintf(ifile, "New-Barr-Log of (n,k) --> (%d,%d)\n\n",n,k);
    ofile2<<"Average_time of New-Barr-Log (n,k) --> ("<<n<<","<<","<<k<<")\n";

    barrier_init(n);

    for(int i=0; i<n; i++){     //create threads and call newBarr()
        bar.emplace_back(newBarr,i+1);
    }

    for (auto& v : bar){
        v.join();
    }   

    sem_destroy(&mutex_);
    sem_destroy(&mutex2_);

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