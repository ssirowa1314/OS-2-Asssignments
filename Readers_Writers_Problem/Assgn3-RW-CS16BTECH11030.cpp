#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

int kw, kr, read_count=0, csseed, remseed;			//readcount tracks number of readers 
sem_t rw_mutex;			//semaphore for writer and readers(common)
sem_t mutex_;			//reader semaphore
double *avg_time_w, *avg_time_r;		//arrays to store times
FILE * ifile;					//file pointer for log files

void reader(int index){			//reader process
	int randcstime, randremtime;	//random sleep times
	time_t my_time;
	auto timeinfo=localtime(&my_time);

	for(int i=0; i<kr; i++){		//loop to run k instances
		time (&my_time);
		timeinfo = localtime (&my_time);
		auto start = std::chrono::system_clock::now();		//start time
		fprintf(ifile,"%dth CS request by Reader Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		sem_wait(&mutex_);			//requesting

		read_count++;
		if(read_count == 1)
			sem_wait(&rw_mutex);

		sem_post(&mutex_);			//got access

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS entry by Reader Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		/*reading is performed*/
		srand(csseed);
		randcstime =(random()%500)+1;
		usleep(randcstime);			//cs time sleep

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS exit by Reader Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		auto end = std::chrono::system_clock::now();		//end time
		std::chrono::duration<double> elapsed_seconds = end-start;		//calc time difference
		avg_time_w[index]+=elapsed_seconds.count();		//store time in array

		sem_wait(&mutex_);

		read_count--;
		if(read_count == 0)
			sem_post(&rw_mutex);

		sem_post(&mutex_);

		srand(remseed);
		randremtime=(random()%500)+1;
		usleep(randremtime);			//rem time sleep
	}

}

void writer(int index){			//writer process
	int randcstime, randremtime;		//random sleep times
	time_t my_time;
	auto timeinfo=localtime(&my_time);

	for(int i=0; i<kw; i++){		//loop to run k instances of thread
		time (&my_time);
		timeinfo = localtime (&my_time);
		auto start = std::chrono::system_clock::now();			//start time
		fprintf(ifile,"%dth CS request by Writer Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		sem_wait(&rw_mutex);		//requesting
									//got access
		
		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS entry by Writer Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		srand(csseed);
		randcstime =(random()%500)+1;
		timeinfo = localtime (&my_time);
		usleep(randcstime);			//cs time sleep

		sem_post(&rw_mutex);		//exit from cs(signal)

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS exit by Writer Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		auto end = std::chrono::system_clock::now();			//end time
		std::chrono::duration<double> elapsed_seconds = end-start;			//calc time difference
		avg_time_w[index]+=elapsed_seconds.count();		//store time in array

		srand(remseed);
		randremtime=(random()%500)+1;
		usleep(randremtime);			//rem time sleep
	}
}

int main(){

	sem_init(&rw_mutex,0,1);		//initialisations
	sem_init(&mutex_,0,1);
	vector <thread> writer_, reader_;	//thread initialisation
	int nw, nr, csseed, remseed;

	ifstream ofile;		//file object to read input file
	ofstream ofile2;		//file object to write to avg time file
	ofile.open("inp_params.txt");
	ofile2.open("Average_time.txt",ios::app);
	ifile=fopen("RW-log.txt", "a+");
	ofile>>nw>>nr>>kw>>kr>>csseed>>remseed;

	fprintf(ifile, "RW-Log of (nw,nr,kw,kr) --> (%d,%d,%d,%d)\n\n",nw,nr,kw,kr);
	ofile2<<"Average_time of RW-Log (nw,nr,kw,kr) --> ("<<nw<<","<<nr<<","<<kw<<","<<kr<<")\n";

	avg_time_w=new double[nw+1];			//creare dynamic array
	memset(avg_time_w,0.0,sizeof(double)*(nw+1));	//initalize	

	avg_time_r=new double[nr+1];			//create dynamic array
	memset(avg_time_r,0.0,sizeof(double)*(nr+1));		//initialize

	for(int i=0; i<nw; i++){
		writer_.emplace_back(writer,i+1);
	}

	for(int i=0; i<nr; i++){
		reader_.emplace_back(reader,i+1);
	}

	for (auto& v : writer_){
        v.join();
    }

    for (auto& v : reader_){
        v.join();
    }	

    sem_destroy(&mutex_);
    sem_destroy(&rw_mutex);


    for(int i=1; i<nw+1; i++)			//calculation of average time
    	avg_time_w[0]+=avg_time_w[i];
    for(int i=1; i<nr+1; i++)
    	avg_time_r[0]+=avg_time_r[i];
    double temp=(avg_time_r[0]+avg_time_w[0])/(nr+nw);

    
    ofile2<<temp<<"\n\n";
    fprintf(ifile, "\n\n" );
    fclose(ifile);
	ofile.close();
}