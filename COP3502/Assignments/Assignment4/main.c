//
//  main.c
//  Assignment4
//
//  Created by Alperen Yazmaci on 19.10.2023.
//

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define PI 3.14159265
#define max(x,y) (((x) >= (y)) ? (x) : (y))
#define min(x,y) (((x) <= (y)) ? (x) : (y))

//Struct Aliases
typedef struct Group Group;

//Struct Definitions
struct Group{
    double angle;
    int size;
};

//function for creating a Group struct, given angle and size
Group create_group(double angle, int size){
    Group group;
    group.angle = angle;
    group.size = size;
    return group;
}

// Input the array and the size of the array, and merge_sort the given array
void merge_sort(Group * arr, int n) {
    // Base case
    if (n <= 1) 
        return;
    // Compute the number of half the array
    int n2 = n / 2;
    merge_sort(arr, n2);
    merge_sort(arr + n2, n - n2);
    Group * tmp = (Group *) malloc(sizeof(Group) * n);
    int fptr = 0; // front of the first half
    int bptr = n2; // front of the back half
    for (int i = 0; i < n; i++) {
        if (fptr == n2) {
            // front is empty
            tmp[i] = arr[bptr];
            bptr++;
        } else if (bptr == n) {
            // back is empty
            tmp[i] = arr[fptr];
            fptr++;
        } else if (arr[fptr].angle < arr[bptr].angle) {
            // front was smaller than the back
            tmp[i] = arr[fptr];
            fptr++;
        } else {
            // back was good enough
            tmp[i] = arr[bptr];
            bptr++;
        }
    }
    // Move the temp values into the original array
    for (int i = 0; i < n; i++) {
        arr[i] = tmp[i];
    }
    // Remove the temp array
    free(tmp);
}
    
//function that stops the program if the arr is not sorted
void verify_sorted(Group * arr, int n) {
    // assert will kill the program if our sort was invalid
    for (int i = 0; i < n - 1; i++)
        assert(arr[i].angle <= arr[i + 1].angle);
}

int main(int argc, const char * argv[]) {
    
    //declaration of integers and doubles needed
    int num_of_groups = 0;
    int initial_angle = 0;
    int total_people = 0;
    int first_output;
    double second_output;
    
    //getting the first line input
    scanf("%d %d", &num_of_groups, &initial_angle);
    
    //allocating memory for a Group array
    Group * groups_arr = (Group *) calloc(num_of_groups, sizeof(Group));
    //getting the rest of the inputs and adding them to the groups_arr
    for(int i = 0; i < num_of_groups; i++){
        int x, y, size;
        scanf("%d %d %d", &x, &y, &size);
        total_people += size;
        
        double angle = atan2(y, x) * (180.0/PI);
        if(angle < 0)
            angle = 360 + angle;
        groups_arr[i] = create_group(angle, size);
    }
    
    //setting the first_output to total_people
    first_output = total_people;
    
    //sorting the groups_arr from lowest angle to highest angle
    merge_sort(groups_arr, num_of_groups);
    verify_sorted(groups_arr, num_of_groups);
    
    //calculating what is the largest angle possible that does not hurt any groups(second output)
    if(num_of_groups > 2){ //calculating for if there is more than 2 groups
        second_output = groups_arr[0].angle + 360 - groups_arr[num_of_groups-1].angle;
        for(int i = 0; i < num_of_groups-1; i++){
            double angle_between = groups_arr[i+1].angle - groups_arr[i].angle;
            if(angle_between > second_output)
                second_output = angle_between;
        }
    }else if(num_of_groups == 2){ //calculating for if there is 2 groups
        double angle_between = groups_arr[1].angle - groups_arr[0].angle;
        if(angle_between < 180)
            angle_between = groups_arr[0].angle - groups_arr[1].angle + 360;
        second_output = angle_between;
    }else{//calculating for if there is 1 groups
        second_output = 360;
    }
    
    if(num_of_groups > 2){//calculating for if there is more than 2 groups
        int front = 0;
        int back = 1;
        int sum = 0;
        for (int i = 0; i < num_of_groups; i++) {
            sum = 0;
            double angle = groups_arr[i].angle + initial_angle;
            if(angle >= 360){
                front++;
                for(int j = front; j < num_of_groups; j++){
                    sum += groups_arr[front].size;
                }
                while(angle >= 360){
                    angle -= 360;
                }
                back = -1;
                while(groups_arr[back + 1].angle < angle && back < num_of_groups){
                    sum += groups_arr[back + 1].size;
                    back++;
                }
            }else{
                front++;
                if(groups_arr[front].angle < angle)
                    sum += groups_arr[front].size;
                while(groups_arr[back + 1].angle < angle && back < num_of_groups){
                    if(front != back+1){
                        sum += groups_arr[back + 1].size;
                        back++;
                    }back++;
                }
            }
            first_output = min(sum, first_output);
        }
    }else if(num_of_groups == 2){//calculating for if there is 2 groups
        if((initial_angle < (groups_arr[1].angle - groups_arr[0].angle)) || (initial_angle < (groups_arr[0].angle + 360 - groups_arr[1].angle)))
            first_output = 0;
        else
            first_output = min(groups_arr[0].size, groups_arr[1].size);
    }else{//calculating for if there is 1 groups
        first_output = 0;
    }
    
    //printing the outputs
    printf("%d\n%.4f\n", first_output, second_output);
    
    //freeing the memory allocated for the groups_arr
    free(groups_arr);
    
    //exiting the program
    return 0;
}
