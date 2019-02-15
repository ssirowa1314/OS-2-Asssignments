1) Download the zip file
2) Extract the zip file
3) Go the extracted directory
4) Open terminal in the directory where you extracted the file

5) The programs take input from inp-params.txt file.
	If you want to change the input, write n,k,csseed, remseed each on a separate line.

6) issue the folowing command to run the files
	g++ -std=c++11 -pthread Assgn2-TAS-CS16BTECH11030.cpp -lrt
	./a.out
	g++ -std=c++11 -pthread Assgn2-CAS-CS16BTECH11030.cpp -lrt
	./a.out
	g++ -std=c++11 -pthread Assgn2-TAS_Fair-CS16BTECH11030.cpp -lrt
	./a.out


7) Following files are created/updated after this 
	TAS-log.txt
	CAS-log.txt
	TAS_Bounded-log.txt
	Average_time.txt