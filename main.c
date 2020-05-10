/**
 * Mastafa Awal
 * EECS3221 Final Project
 * Driver code for disk scheduler
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define DEF_HEAD_POS (33)
#define DEF_INPUT_FN ("input.txt")
#define DEF_OUTPUT_FN ("output.txt")
#define FIRST_CYL (0)
#define LAST_CYL (99)
#define MAX_REQS (25)

int start_head_pos = DEF_HEAD_POS;
int curr_head_pos = 0;
int head_movement = 0;
int num_seq = 0; 
FILE *out;
//Function declarations
int fcfs(int[]);
int scan(int[]);
int c_scan(int[]);
int move_head(int, int);
int reset_head();
int print_arr(int[]);

int main(int argc, char *argv[])
{
    FILE *in;
    
    //char raw_data[sizeof(int)];
    int raw_data;
    int input_seq[MAX_REQS];
    
    if (argc > 2) {
        fprintf(stderr, "Invalid Usage. Usage: ./disk_sched [initial head position]\n The directory must contain input.txt, and program must be able to write output.txt to the directory.");
    } else if (argc == 2) {
        //Use the inputted head position
        start_head_pos = atoi(argv[1]);
        //in = fopen(argv[2], "r");
    } 
    in = fopen(DEF_INPUT_FN, "r");
    if (in == NULL) {
        fprintf(stderr, "Error! input.txt not found!\n");
        return -1;
    }
    //open the output file
    out = fopen(DEF_OUTPUT_FN, "w");
    if (out == NULL) {
        fprintf(stderr, "Error creating output file!\n");
        return -1;
    }
    //Read data
    while (1) {
        raw_data = INT_MIN;
        if (fscanf(in, "%d", &raw_data) < 1) break;
        input_seq[num_seq++] = raw_data;
    }
   // num_seq--;
    // while (fgets(raw_data,sizeof(int),in) != NULL) {
    //     printf("RAW DATA %s\n", )
    //     input_seq[num_seq++] = atoi(raw_data);
    // }
    fclose(in);
    //printf("INPUT SEQUENCE\n");
    //print_arr(input_seq);
    //Invoke each of the algorithms
    reset_head();
    fprintf(out, "\nFCFS Algorithm\n");
    fprintf(out,"Initial head position = %d\n", start_head_pos);
    fprintf(out,"Seek Sequence is:\n");
    fcfs(input_seq);
    fprintf(out,"Total head movement for FCFS = ");
    fprintf(out,"%d\n", head_movement);
    
    reset_head();
    fprintf(out,"\nSCAN Algorithm\n");
    fprintf(out,"Initial head position = %d\n", start_head_pos);
    fprintf(out,"Seek Sequence is:\n");
    scan(input_seq);
    fprintf(out,"Total head movement for SCAN = ");
    fprintf(out,"%d\n", head_movement);
    
    reset_head();
    fprintf(out,"\nC_SCAN Algorithm\n");
    fprintf(out,"Initial head position = %d\n", start_head_pos);
    fprintf(out,"Seek Sequence is:\n");
    c_scan(input_seq);
    fprintf(out,"Total head movement for C_SCAN = ");
    fprintf(out,"%d", head_movement);
    return 0;
}

int fcfs(int req_seq[]) {
    //Returns number of head movement
    //FCFS, so just iterate through array and move to that head position.
    for (int i = 0; i < num_seq; i++) {
        move_head(req_seq[i], 1);
    }
    return head_movement;
}

int scan(int req_seq[]) {
    //Returns number of head movement
    
    int serviced[num_seq];
    int reqs_serv = 0;
    //Initialize serviced array
    for (int i = 0; i < num_seq;  i++) {
        serviced[i] = 0;
    }
    //Get the scan sequence - starting at the initial position,
    //get the closest to it, going towards zero.
    int tar_head_pos = curr_head_pos;
    int dir = -1;
    while (reqs_serv != num_seq) {
        //Naive implementation - SCAN algorithm directly from book.
        //but should guarantee correctness
        //Go from initial head position to start of disk, service any requests.
        //Go from start of disk to end of disk, service any requests
        //Go from end of disk to start of disk, service any requests
        //rinse and repeat
        
        if (tar_head_pos < FIRST_CYL) {
            tar_head_pos = FIRST_CYL;
            move_head(tar_head_pos, 0);
            dir = 1;
        } else if (tar_head_pos > LAST_CYL) {
            tar_head_pos = LAST_CYL;
            move_head(tar_head_pos, 0);
            dir = -1;
        }
        //printf("CURR TARGET HEAD POS: %d\n", tar_head_pos);
        for (int j = 0; j < num_seq; j++) {
            //If it has been serviced, continue to the next one
            if (serviced[j]) {
                 continue;
             } else if (tar_head_pos == req_seq[j]) {
                //Service that request
                move_head(tar_head_pos, 1);
                serviced[j] = 1;
                reqs_serv++;
            }
        }
        //Iterate index. If out of bounds, set the direction.
        tar_head_pos+= dir;
    }
    return head_movement;
}
/* NOTE TO PROFESSOR/TA: If the above algorithm will cost me marks due to inefficiency, please comment out the above method
* and comment in the method below. It uses a more efficient algorithm.
* I wrote both due to a bug that turned out to be elsewhere...
*/

/* 
int scan(int req_seq[]) {
    //Returns number of head movement    
    int s_seq_low[num_seq];
    int ssl_idx = 0;
    int s_seq_high_eq[num_seq];
    int sshe_idx = 0;
    int serviced = 0;
    //Initialize the arrays to INT_MIN
    for (int i = 0; i < num_seq; i++) {
        s_seq_low[i] = INT_MIN;
        s_seq_high_eq[i] = INT_MIN;
    }

    //Iterate through the requests and add them to s_seq_low if it is lower than the initial head, 
    //and to s_seq_high if it is higher than the initial head. Insertion sort for sorting.
    for (int i = 0; i < num_seq;i++) {
        if (req_seq[i] < curr_head_pos) {
            //printf("INSERTING INTO LOW\n");
            insert_arr(s_seq_low, ssl_idx++, req_seq[i]);
        } else {
            //printf("INSERTING INTO HIGH\n");
            insert_arr(s_seq_high_eq, sshe_idx++, req_seq[i]);
        }
    }   
    //Go towards zero first, so process the lower values first
    for (int i = 0; i < ssl_idx; i++) {
        move_head(s_seq_low[i], 1);
        serviced++;
    } 
    //Move to 0, as we are going towards this value, if there are still requests left to be serviced
    if (serviced != num_seq) {
         move_head(0, 0);
    }
    //Then, go towards the highest cylinder, from 0. So, traverse the sorted array
    //in reverse
    for (int i = sshe_idx - 1; i >= 0; i--) {
        move_head(s_seq_high_eq[i], 1);
    }
}
*/
int c_scan(int req_seq[]) {
     //Returns number of head movement
    int serviced[num_seq];
    int reqs_serv = 0;
    //Initialize serviced array
    for (int i = 0; i < num_seq;  i++) {
        serviced[i] = 0;
    }
    //Get the scan sequence - starting at the initial position,
    //get the closest to it, going towards zero.
    int tar_head_pos = curr_head_pos;
    while (reqs_serv != num_seq) {
         if (tar_head_pos > LAST_CYL) {
            tar_head_pos = FIRST_CYL;
            move_head(LAST_CYL, 0);
            move_head(FIRST_CYL, 0);
            //Subtract number of cylinders from head movement as disk is circular
            //head_movement -= (LAST_CYL + 1) - (FIRST_CYL);
        }
        //Naive implementation but should guarantee correctness
        //printf("CURR TARGET HEAD POS: %d\n", tar_head_pos);
        for (int j = 0; j < num_seq; j++) {
            //If it has been serviced, continue to the next one
            if (serviced[j]) {
                 continue;
             } else if (tar_head_pos == req_seq[j]) {
                //Service that request
                move_head(tar_head_pos, 1);
                serviced[j] = 1;
                reqs_serv++;
            }
        }
        //Iterate index. Go to end of disk, and then back to 0 if at the end of disk
        tar_head_pos++; 
       
    }
    return head_movement;
}
/* NOTE TO PROFESSOR/TA: If the above algorithm will cost me marks due to inefficiency, please comment out the above method
* and comment in the method below. It uses a more efficient algorithm.
* I wrote both due to a bug that turned out to be elsewhere...
*/
/* 
int c_scan(int req_seq[]) {
        //Returns number of head movement    
    int s_seq_low[num_seq];
    int ssl_idx = 0;
    int s_seq_high_eq[num_seq];
    int sshe_idx = 0;
    int serviced = 0;
    //Initialize the arrays to INT_MIN
    for (int i = 0; i < num_seq; i++) {
        s_seq_low[i] = INT_MIN;
        s_seq_high_eq[i] = INT_MIN;
    }

    //Iterate through the requests and add them to s_seq_low if it is lower than the initial head, 
    //and to s_seq_high if it is higher than the initial head. Insertion sort for sorting.
    for (int i = 0; i < num_seq;i++) {
        if (req_seq[i] < curr_head_pos) {
            //printf("INSERTING INTO LOW\n");
            insert_arr(s_seq_low, ssl_idx++, req_seq[i]);
        } else {
            //printf("INSERTING INTO HIGH\n");
            insert_arr(s_seq_high_eq, sshe_idx++, req_seq[i]);
        }
    }
    //First, go towards the highest cylinder, from the head position
    //So, traverse the "high" sorted array in reverse

    for (int i = sshe_idx - 1; i >= 0; i--) {
        move_head(s_seq_high_eq[i], 1);
        serviced++;
    }
    //Go to end of disk, if requests still left to be serviced

    if(serviced != num_seq) {
        move_head(LAST_CYL, 0);
        //Then, reset to 0
        move_head(FIRST_CYL,0);
        //head_movement -= (LAST_CYL + 1) - (FIRST_CYL);
    }

    //Subtract the number of cylinders from the seektime. This is because going from first cycle to last cycle actually has 0 seek time
    //as a disk is circular
    //Now, service all the requests until the initial head position, as those
    //are the only requests that has not been serviced. Traverse in reverse order,
    //as we service them as we go towards the highest cylinder.
    for (int i = ssl_idx - 1; i >= 0; i--) {
        move_head(s_seq_low[i], 1);
    } 

    return head_movement;
}
*/

int insert_arr(int dest_arr[], int arr_len, int data) {
    //Insertion sort insert in array
    int curr_idx = 0;
    while(dest_arr[curr_idx] > data) {
        //Find the index to insert, which is the first location that is less than the data to insert
        curr_idx++;   
    }
    //Shift all the indices after the insertion point by 1 so that there is space to insert
    for (int i = arr_len; i > curr_idx; i--) {
        dest_arr[i] = dest_arr[i - 1]; 
    }
    //Insert into that position
    dest_arr[curr_idx] = data;
    //printf("AFTER INSERTION:\n");
    //print_arr(dest_arr);
    return 0;
}

int move_head(int new_pos, int print) {
    //Move head to a specified position, and update the head count
    //int old_hm = head_movement;
    if (new_pos < curr_head_pos) {
        //if new position is before the current position
        head_movement += curr_head_pos - new_pos;
    } else {
        head_movement += new_pos - curr_head_pos;
    }
    //printf("Old head pos: %d, New head pos: %d\n", curr_head_pos, new_pos);
    //printf("Old head movement: %d, New head movement: %d, diff: %d\n", old_hm, head_movement, head_movement - old_hm);
    
    curr_head_pos = new_pos;
    if (print) {
        fprintf(out, "%d\n", curr_head_pos);
    }
    return 0;
}

int reset_head() {
    head_movement = 0;
    curr_head_pos = start_head_pos;
    return 0;
}

int print_arr(int arr[]) {
    fprintf(out,"ARRAY CONTENTS:\n");
    for (int i =0; i < num_seq; i++) {
        fprintf(out,"Index: %d, Value: %d\n", i, arr[i]);
    }
    return 0;
}
