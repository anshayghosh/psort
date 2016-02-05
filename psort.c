#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include "helper.h"
#include <math.h>
#define REC_SIZE sizeof(struct rec)



int main(int argc, char **argv){
	//initializing most of the variables
	int i;
	int j;
	int n;//variable for the specified number of processes to make
	int check;
	int num_rec;//variable for the number of the records in the file
	int interval;
	int file_size;
	//file pointers for IO
	FILE *infile;
	FILE *outfile;

	// this while loop gets the command line arguments and processes each of the arguments
	while ((check = getopt(argc, argv, "n:f:o:")) != -1){
		switch(check){
			case 'n':
				if((n = strtol(optarg, NULL, 10))==0){
					perror("strtol");
					exit(1);
				}
				break;

			case 'f':
				file_size = get_file_size(optarg);
				num_rec = file_size / REC_SIZE;
				interval = ceil((float) num_rec / (float) n);
				if ((infile = fopen(optarg, "r")) == NULL){
					perror("fopen");
					exit(1);
				}
				break;
  
			case 'o':
				if ((outfile = fopen(optarg, "w")) == NULL){
					perror("fopen");
					exit(1);
				} 
				break;
		}
	}
	

	struct rec rec_array[interval]; //each child process will read 1/n of the file into this array
	struct rec pipe_array[n]; //at the end this contains the next element from each pipe
	//initialize the time calulating variables
	struct timeval starttime, endtime;
    double timediff;

    //initialize the starting time value to begin calculation of the time
        
    if ((gettimeofday(&starttime, NULL)) == -1) {
        perror("gettimeofday");
        exit(1);
    }

    //begin piping

	int fd[n][2];
	for(i = 0; i < n; i++){
		pipe(fd[i]);
		int r = fork();
		if (r == 0){
			// this loop is needed because all pipes are copied with each fork call
			for (j = 0; j <= i; j++){
				if (close(fd[j][0]) != 0){
					perror("closing the read end of pipe");
					exit(1);
				}				
			}
			
			fseek(infile, REC_SIZE*(interval*i), SEEK_SET);
			fread(&rec_array, REC_SIZE, interval, infile);
			fclose(infile);


			//these print loops aren't necessary, just for checking that everything is working
			printf("UNSORTED: \n");
			j = 0;
			while(j < interval && ((i*interval)+j) < num_rec){
					printf("Reading %s %d\n", rec_array[j].word, rec_array[j].freq);
				j++;
			}

			qsort(rec_array, sizeof(rec_array)/REC_SIZE, REC_SIZE, compare_freq);

			// printf("SORTED: \n");
			// j = 0;
			// while(j < interval && ((i*interval)+j) < num_rec){
			// 	printf("Reading %d %s\n", rec_array[j].freq, rec_array[j].word);
			// 	j++;
			// }
			
			//writes the array to the pipe
			j = 0;
			while(j < interval && ((i*interval)+j) < num_rec){
				write(fd[i][1], &rec_array[j], REC_SIZE);
				j++;
			}
			close(fd[i][1]);
			exit(0);
		}
		else if (r > 0){
			close(fd[i][1]);
		}

	}

// This part is illustrated in the assignment page
	//makes the initial pipe array
	for (j = 0; j < n; j++){
			read(fd[j][0], &pipe_array[j], REC_SIZE);
		}

	//not a necessary loop just want to see what's in the array
	// for (j = 0; j < n; j++){
	// 		printf("%d \n", pipe_array[j].freq);
	// 	}

	i = 0;
	while(i < num_rec){
		struct rec min = pipe_array[0];
		int pos = 0;
		for (j = 1; j < n; j++){
			if (min.freq == -1){
				min = pipe_array[j];
				pos = j;
			}
			else if (min.freq > pipe_array[j].freq && pipe_array[j].freq != -1){
				min = pipe_array[j];
				pos = j;
			}
		}
		printf("%d %s \n", min.freq, min.word);
		fwrite(&min, REC_SIZE, 1, outfile);
		if (read(fd[pos][0], &pipe_array[pos], REC_SIZE) == 0){
			pipe_array[pos].freq = -1;
		} 
		i++;
	}
	//closes the files and the pipes
	fclose(outfile);
	for(int i =0;i<n;i++){
		close(fd[i][0]);
	}

	//calculates the end of the time of the processes

	if ((gettimeofday(&endtime, NULL)) == -1) {
            perror("gettimeofday");
            exit(1);
        }
    //subtracts endtime from start time to recieve total time taken and prints the value. 
    timediff = (endtime.tv_sec - starttime.tv_sec) +
            (endtime.tv_usec - starttime.tv_usec) / 1000000.0;
    fprintf(stdout, "%.4f\n", timediff);
    

}








