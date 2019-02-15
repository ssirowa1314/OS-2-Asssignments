#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

int kw, kr, csseed, remseed;
sem_t in;		//semaphore common to both reader and writer
sem_t out;		//semaphore common to both reader and writer
sem_t wrt;		//semaphore common to both reader and writer
int ctrin=0, ctrout=0;		//keeps track of readers count
bool wait=false;			//this becomes true if a writer arives when readers are reading
double *avg_time_w, *avg_time_r;			//arrray to store avg times
FILE * ifile;			//file pointer to write log files

void reader(int index){			//reader process
	int randcstime, randremtime;		//ramdom times
	time_t my_time;
	auto timeinfo=localtime(&my_time);
	for(int i=0; i<kr; i++){		//loop to run k times thread
		time (&my_time);
		timeinfo = localtime (&my_time);
		auto start = std::chrono::system_clock::now();		//start time
		fprintf(ifile,"%dth CS request by Reader Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

		sem_wait(&in);
		ctrin++;
		sem_post(&in);

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS entry by Reader Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		/*reading is performed*/
		srand(csseed);
		randcstime =(random()%500)+1;
		usleep(randcstime);			//CS time sleep

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS exit by Reader Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		auto end = std::chrono::system_clock::now();	//end time
		std::chrono::duration<double> elapsed_seconds = end-start;		//cal difference of times
		avg_time_w[index]+=elapsed_seconds.count();			//store time taken

		sem_wait(&out);
		ctrout++;
		if(wait == 1 && ctrin == ctrout)
			sem_post(&wrt);
		sem_post(&out);

		srand(remseed);
		randremtime=(random()%500)+1;
		usleep(randremtime);			//rem time sleep
	}
}

void writer(int index){					//writer process
	int randcstime, randremtime;			//random times
	time_t my_time;
	auto timeinfo=localtime(&my_time);

	for(int i=0; i<kw; i++){			//loop to run a thread k times
		time (&my_time);
		timeinfo = localtime (&my_time);
		auto start = std::chrono::system_clock::now();			//start time
		fprintf(ifile,"%dth CS request by Writer Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

		sem_wait(&in);
		sem_wait(&out);
		if(ctrin == ctrout)
			sem_post(&out);
		else{
			wait=1;
			sem_post(&out);
			sem_wait(&wrt);
			wait=0;
		}

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS entry by Writer Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		srand(csseed);
		randcstime =(random()%500)+1;
		timeinfo = localtime (&my_time);
		usleep(randcstime);			//CS tiem sleep

		sem_post(&in);

		time (&my_time);
		timeinfo = localtime (&my_time);
		fprintf(ifile,"%dth CS exit by Writer Thread %d at %d:%d:%d\n",i,index,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		auto end = std::chrono::system_clock::now();		//end time
		std::chrono::duration<double> elapsed_seconds = end-start;		//calc time
		avg_time_w[index]+=elapsed_seconds.count();			//store time

		srand(remseed);
		randremtime=(random()%500)+1;
		usleep(randremtime);		//rem time sleep


	}
}

int main(){

	sem_init(&in,0,1);		//semaphore initialisations
	sem_init(&out,0,1);
	sem_init(&wrt,0,0);
	vector <thread> writer_, reader_;		//thread initialisation
	int nw, nr, csseed, remseed;

	ifstream ofile;		//file object for reading input file
	ofstream ofile2;		//file object for writing avg time file
	ofile.open("inp_params.txt");
	ofile2.open("Average_time.txt",ios::app);
	ifile=fopen("FairRW-log.txt", "a+");
	ofile>>nw>>nr>>kw>>kr>>csseed>>remseed;

	fprintf(ifile, "FairRW-Log of (nw,nr,kw,kr) --> (%d,%d,%d,%d)\n\n",nw,nr,kw,kr);
	ofile2<<"Average_time of FairRW-Log (nw,nr,kw,kr) --> ("<<nw<<","<<nr<<","<<kw<<","<<kr<<")\n";

	avg_time_w=new double[nw+1];			//create dynamic array
	memset(avg_time_w,0.0,sizeof(double)*(nw+1));		//initialise with zero

	avg_time_r=new double[nr+1];		//create dynamic array
	memset(avg_time_r,0.0,sizeof(double)*(nr+1));		//initialise with xero

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

    sem_destroy(&wrt);
    sem_destroy(&in);
    sem_destroy(&out);


    for(int i=1; i<nw+1; i++)					//calculation of avg time
    	avg_time_w[0]+=avg_time_w[i];
    for(int i=1; i<nr+1; i++)
    	avg_time_r[0]+=avg_time_r[i];
    double temp=(avg_time_r[0]+avg_time_w[0])/(nr+nw);


    ofile2<<temp<<"\n\n";
    fprintf(ifile, "\n\n" );
    fclose(ifile);
	ofile.close();
}