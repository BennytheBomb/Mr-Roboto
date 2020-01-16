#include <stdlib.h>
#include <stdio.h>

const int histogram_size = 256;
const int max_brightness_value = 900;
const int number_of_colors = 3;
int histogram[histogram_size];
float thresholds[number_of_colors - 1];

typedef struct Extrema { 
   int* value; 
   struct Extrema *next; 
} Extremas;

void insert(Extremas** head, int* new_data) { 
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

bool check_minima(int* value, int* left_neighbor, int* right_neighbor) {
    return (*value >= *left_neighbor) && (*value > *right_neighbor);
}

bool check_maxima(int* value, int* left_neighbor, int* right_neighbor) {
    return (*value <= *left_neighbor) && (*value < *right_neighbor);
}

void calculate_extrema(int* curve, int curve_size, Extremas* minima_head, Extremas* maxima_head) {
    for (int i = 1; i < curve_size - 1; i++) {
        if (check_minima(&curve[i], &curve[i - 1], &curve[i + 1])) {
            // Add minima to extremas
            insert(&minima_head, &i);
        } else if (check_maxima(&curve[i], &curve[i - 1], &curve[i + 1])) {
            // Add maxima to extremas
            insert(&maxima_head, &i);
        }
    }
}

void sort_maximas(int* index_array, int length, int* value_array) {
    for (int i = length - 1; i > 1; i--) {
        for (int j = 0; j < i - 1; j++) {
            if (value_array[index_array[j]] > value_array[index_array[j + 1]]) {
                int temp = index_array[j + 1];
                index_array[j + 1] = index_array[j];
                index_array[j] = temp;
            }
        }
    }
}

int* pick_best_maxima(Extremas* maxima_head, int* curve, int max_brightness, int max_maximas) {
    int maximas[max_maximas];
    Extremas* ptr;
    ptr = maxima_head;
    while (ptr != NULL) {
        if (curve[*(ptr->value)] > curve[maximas[0]]) {
            maximas[0] = *(ptr->value);
            sort_maximas(maximas, max_maximas, curve);
        }
        ptr = ptr->next;
    }
    return maximas;
}

void sort_minimas(int* index_array, int length, int* value_array) {
    for (int i = length - 1; i > 1; i--) {
        for (int j = 0; j < i - 1; j++) {
            if (value_array[index_array[j]] > value_array[index_array[j + 1]]) {
                int temp = index_array[j + 1];
                index_array[j + 1] = index_array[j];
                index_array[j] = temp;
            }
        }
    }
}

int* pick_best_minima(Extremas* minima_head, int* maximas, int* curve, int max_maximas, int max_minimas) {
    int minimas[max_minimas];
    Extremas* ptr;
    ptr = minima_head;

    int max_index = maximas[0];
    int min_index = maximas[0];
    for (int i = 1; i < max_maximas; i++) {
        if (maximas[i] > max_index) {
            max_index = i;
        }
        if (maximas[i] < min_index) {
            min_index = i;
        }
    }

    int index;
    while (ptr != NULL) {
        index = *(ptr->value);
        if (index < max_index && index > min_index) {
            if (curve[index] < minimas[0]) {
                minimas[0] = index;
                sort_minimas(minimas, max_minimas, curve);
            }
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
    calculate_extrema(histogram, histogram_size, minima_head, maxima_head);
    int* best_maximas = pick_best_maxima(maxima_head, histogram, max_brightness_value, number_of_colors);
    int* best_minimas = pick_best_minima(minima_head, best_maximas, histogram, number_of_colors, number_of_colors - 1);

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