#include <stdlib.h>
#include <stdio.h>

//YOUR CODE
const int histogram_size = 256;
const int max_brightness_value = 900;
const int number_of_colors = 3;
int histogram[histogram_size];
float thresholds[number_of_colors - 1];

typedef struct Extrema { 
   int value; 
   struct Extrema *next; 
} Extremas;

void insert(Extremas** head, int new_data) { 
   Extremas* new_node = (Extremas*) malloc(sizeof(Extremas)); 
   new_node->value = new_data; 
   new_node->next = *head; 
   *head = new_node; 
}

void display(Extremas* head) { 
   Extremas* ptr;
   ptr = head;
   while (ptr != NULL) { 
      printf("%d\n", ptr->value);
      ptr = ptr->next; 
   }
} 

int map_value(int value, int max_value, int max_target_value) {
    return (value * max_target_value) / max_value;
}

bool check_minima(int value, int left_neighbor, int right_neighbor) {
    return (value <= left_neighbor) && (value < right_neighbor);
}

bool check_maxima(int value, int left_neighbor, int right_neighbor) {
    return (value >= left_neighbor) && (value > right_neighbor);
}

void calculate_extrema(int* curve, int curve_size, Extremas** minima_head, Extremas** maxima_head) {
    for (int i = 1; i < curve_size - 1; i++) {
        if (check_minima(curve[i], curve[i - 1], curve[i + 1])) {
            // Add minima to extremas
            insert(minima_head, i);
        } else if (check_maxima(curve[i], curve[i - 1], curve[i + 1])) {
            // Add maxima to extremas
            insert(maxima_head, i);
        }
    }
}

void sort_maximas(int* index_array, int length, int* value_array) {
    for (int i = length; i > 1; i--) {
        for (int j = 0; j < i - 1; j++) {
            if (value_array[index_array[j]] > value_array[index_array[j + 1]]) {
                int temp = index_array[j + 1];
                index_array[j + 1] = index_array[j];
                index_array[j] = temp;
            }
        }
    }
}

int* pick_best_maxima(Extremas* maxima_head, int* curve, int max_maximas) {
    int maximas[max_maximas];
    // WHY CANT I WRITE int array[length] = {0} INSTEAD!!!!!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < max_maximas; i++) {
        maximas[i] = 0;
    }
    Extremas* ptr;
    ptr = maxima_head;
    while (ptr != NULL) {
        if (curve[ptr->value] > curve[maximas[0]]) {
            maximas[0] = ptr->value;
            sort_maximas(maximas, max_maximas, curve);
        }
        ptr = ptr->next;
    }
    
    return maximas;
}

void sort_minimas(int* index_array, int length, int* value_array) {
    for (int i = length; i > 1; i--) {
        for (int j = 0; j < i - 1; j++) {
            if (value_array[index_array[j]] < value_array[index_array[j + 1]]) {
                int temp = index_array[j + 1];
                index_array[j + 1] = index_array[j];
                index_array[j] = temp;
            }
        }
    }
}

int* pick_best_minima(Extremas* minima_head, int* curve, int max_minimas) {
    int minimas[max_minimas];
    Extremas* ptr;
    ptr = minima_head;
    int counter = 0;

    while (ptr != NULL) {
        if (counter < max_minimas) {
            minimas[counter] = ptr->value;
            counter = counter + 1;
            if (counter >= max_minimas) {
                sort_minimas(minimas, max_minimas, curve);
            }
        } else if (curve[ptr->value] < minimas[0]) {
            minimas[0] = ptr->value;
            sort_minimas(minimas, max_minimas, curve);
        }
        ptr = ptr->next;
    }
    return minimas;
}

//input:
// int[] data : array of data including previous data so far, with length of num_prev_data.
// num_prev_data : number of data above

int predict(int* data, int num_prev_data){
    //your new data should be data[num_prev_data-1];
    int value = data[num_prev_data-1];

    // KDE (https://en.wikipedia.org/wiki/Kernel_density_estimation) or Meanshift (https://stackoverflow.com/questions/4831813/image-segmentation-using-mean-shift-explained/17912660#17912660)
    int mapped_value = map_value(abs(value), max_brightness_value, histogram_size - 1);
    // printf("Value: %d, Mapped Value: %d\n", value, mapped_value);
    histogram[mapped_value] = histogram[mapped_value] + 1;
    Extremas* minima_head = NULL;
    Extremas* maxima_head = NULL;
    calculate_extrema(histogram, histogram_size, &minima_head, &maxima_head);
    int* best_maximas = pick_best_maxima(maxima_head, histogram, number_of_colors);
    int* best_minimas = pick_best_minima(minima_head, histogram, number_of_colors - 1);

    //output: 0 for black, 1 for gray and 2 for white.
    for (int i = 0; i < number_of_colors - 1; i++) {
        if (mapped_value < best_minimas[i]) {
            return i;
        }
    }
    return number_of_colors - 1;
    // if(value > 300)return 2;
    // if(value > 200)return 1;
    // else return 0;
}

//////////////////////////////////////////////////////////
//////////////////DO NOT MODIFY BELOW/////////////////////
//////////////////////////////////////////////////////////

int main(void) {
    // Extrema* minima_head = NULL;
    // Extrema* maxima_head = NULL;
    // const int max_brightness = 1000;
    // const int number_of_colors = 3;
    // int curve[] = { 1, 2, 4, 4, 0, 3, 1, 2, 5, 4, 3, 3, 5, 1, 6, 1 };
    // const int curve_size = sizeof(curve)/sizeof(curve[0]);
    // calculate_extrema(curve, curve_size, &minima_head, &maxima_head);
    // display(minima_head);
    // display(maxima_head);
    // int* maximas = pick_best_maxima(maxima_head, curve, number_of_colors);
    // for (int i = 0; i < number_of_colors; i++) {
    //     printf("%d: %d = %d\n", i, maximas[i], curve[maximas[i]]);
    // }
    // int* minimas = pick_best_minima(minima_head, curve, number_of_colors - 1);
    // // int minimas[number_of_colors - 1];
    // // Extremas* ptr;
    // // ptr = minima_head;
    // // int counter = 0;

    // // while (ptr != NULL) {
    // //     if (counter < (number_of_colors - 1)) {
    // //         minimas[counter] = ptr->value;
    // //         counter = counter + 1;
    // //         if (counter >= (number_of_colors - 1)) {
    // //             sort_minimas(minimas, number_of_colors - 1, curve);
    // //         }
    // //     } else if (curve[ptr->value] < curve[minimas[0]]) {
    // //         minimas[0] = ptr->value;
    // //         sort_minimas(minimas, number_of_colors - 1, curve);
    // //     }
    // //     ptr = ptr->next;
    // // }
    // for (int i = 0; i < number_of_colors - 1; i++) {
    //     printf("%d: %d = %d\n", i, minimas[i], curve[minimas[i]]);
    // }

    float average_score = 0;
    for(int f=1; f <= 4; f++){
        char filename[10];
        sprintf(filename, "data%d.txt", f);
        FILE* fp = fopen(filename, "r");
        
        int N;
        fscanf(fp, "%d", &N);
        int data[N];
        int score = 0;
        for(int i=0; i < N; i++){
            int d, label;
            fscanf(fp, "%d %d", &d, &label);
            data[i] = d;
            int pred = predict(data, i+1);
            if(pred == label)score++;
        }
        printf("round %d: your score: %f\n",f, score / float(N));
        average_score += score/float(N);
        //score the prediction
        fclose(fp);
    }
    average_score /= 4;
    printf("average score: %f\n", average_score);
}