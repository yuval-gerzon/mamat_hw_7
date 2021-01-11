#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked-list.h"
#include "grades.h"

/* User function, clone “element” to “output”. Returns 0 on success */
typedef int(*element_clone_t)(void *element, void **output);

/* User function, Destroy “element”. always succeed */
typedef void(*element_destroy_t)(void *element);

/**
 * @brief an element in the list
 * @param int used fot id or grade
 * @param pointer to char for students/coure name
 * @param pointer used for another list
 * @returns A pointer to the list, or NULL on error
 */

typedef struct list_element_ts_t {
               int number;
               char *name_;
               void *sub_list;/*used only for main list*/
} list_element_t;

struct grades {
	   struct list *linked_list;
};


int clone (void *element, void **output);
void destroy (void *element);
struct grades* grades_init();
int grades_add_student(struct grades *grades, const char *name, int id);
void grades_destroy(struct grades *grades);
int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade);
float grades_calc_avg(struct grades *grades, int id, char **out);
int grades_print_student(struct grades *grades, int id);
int grades_print_all(struct grades *grades);



/*copying the data from the pointer and setting the output*/
int clone (void *element, void **output){
	list_element_t *copy_elem = (list_element_t*) element;
	list_element_t *temp = malloc(sizeof(list_element_t));
	if (!temp){
		free(temp);
		return -1;
	}
	temp->number = copy_elem->number;
	temp->name_ = malloc(sizeof(char)*(strlen(copy_elem->name_)+1));//copy_elem->name_;
	if (!temp->name_){
		free(temp->name_);
		free(temp);
		return -1;
	}
	strcpy(temp->name_,copy_elem->name_);
	temp->sub_list = copy_elem->sub_list;
	*output = temp;
	return 0;	
}


/*freeing every element we get, to avoid memory leaks*/
void destroy (void *element){
	list_element_t *copy_elem = (list_element_t*) element;
	if (!copy_elem){
		return;
	}
	free(copy_elem->name_);
	free(copy_elem);
}


struct grades* grades_init(){
//printf(" this is init fu and it works\n");
	struct grades *grades_return;
	grades_return->linked_list=list_init(clone,
	                   destroy);
	return grades_return;
}


/*three elements for every student:
1.id 2.name (pointer) 3.pointer to personal list*/
int grades_add_student(struct grades *grades, const char *name, int id){

	list_element_t *temp;
	list_element_t *temp_elem;
	list_element_t element;
	temp = &element;
	struct node *it;
	int i = 0;
	it = list_begin(grades->linked_list);
	temp_elem = (list_element_t*) list_get(it);
		while (i<list_size(grades->linked_list)){ //finding the student
	       	if (temp_elem->number == id ){
				return -1;
			}
			it= list_next(it);
			temp_elem = (list_element_t*) list_get(it);
			i++;
	}
	
	temp->name_=(char*)malloc(sizeof(char)*(strlen(name) +1));
	if (temp->name_ == NULL ) {
		free (temp->name_);
		return -1;
	}
	temp->number = id;/*adding the ID to the students list*/
	strcpy(temp->name_,name);/*adding the *pointer* of NAME to the students list*/
    temp->sub_list = list_init(clone, destroy);                   
	list_push_back(grades->linked_list,temp); /*adds students list to the main list (grades)*/
	free (temp->name_);
	return 0;
}


void grades_destroy(struct grades *grades){ /*destroys every
sub list, and the main one*/
	struct node *it_main;
	list_element_t *temp_elem;
	
	it_main = list_begin(grades->linked_list);
	temp_elem = (list_element_t*) list_get(it_main);
	int i = 0;
	while (i<list_size(grades->linked_list)){
		list_destroy(temp_elem->sub_list);
		it_main = list_next(it_main);
		temp_elem = (list_element_t*) list_get(it_main);
		i++;
	}
	list_destroy(grades->linked_list);
	/* after destroying every sub_list, destroy the main list.*/
}


/*finds the student in main_list ->goes to his grade list
-> adds course -> adds grade*/
int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade){
	struct node *it_main;
	struct list *stud_list_point;
	list_element_t *temp_elem;
	list_element_t *temp;
	list_element_t element;
	temp = &element;
	int i=0;
	it_main = list_begin(grades->linked_list);
	temp_elem = (list_element_t*) list_get(it_main);
	/*finding the student:*/
	while (i<list_size(grades->linked_list)&& (temp_elem->number != id )){
		it_main = list_next(it_main);
		temp_elem = (list_element_t*) list_get(it_main);
		i++;
	}
	if((i==list_size(grades->linked_list)&& (temp_elem->number != id ))||grade>100||grade<0){
		return -1;
	}
	
	stud_list_point =(struct list*) temp_elem->sub_list;
	temp->name_=(char*)malloc(sizeof(char)*(strlen(name) +1));
	if (temp->name_ == NULL ) {
		free (temp->name_);
		return -1;
	}
	
	i = 0;
	it_main = list_begin(stud_list_point);
	temp_elem = (list_element_t*) list_get(it_main);
	while ( i < list_size(stud_list_point)){
		if (strcmp (temp_elem->name_, name) == 0){
			free (temp->name_);
			return -1;
		}
		it_main = list_next(it_main);
		temp_elem = (list_element_t*) list_get(it_main);
		i++;
	}
	
	temp->number = grade;
	strcpy(temp->name_,name);
	list_push_back(stud_list_point,temp);
	free (temp->name_);
	return 0;
 }


float grades_calc_avg(struct grades *grades, int id, char **out){
	
	struct node *it_main;
	struct list *stud_list_point;
	list_element_t *temp_elem;
	int i=0;
	it_main = list_begin(grades->linked_list);
	temp_elem = (list_element_t*) list_get(it_main);
	/*finding the student:*/
	while (i<list_size(grades->linked_list)&& (temp_elem->number != id )){
		it_main = list_next(it_main);
		temp_elem = (list_element_t*) list_get(it_main);
		i++;
	}
	if((i==list_size(grades->linked_list))){
		out = NULL; 
		return -1;
	}
	*out = (char*)malloc(sizeof(char)*(strlen(temp_elem->name_)+1));
	if (!(*out)) {
		return -1;
	}
	strcpy(*out, temp_elem->name_);
	stud_list_point =(struct list*) temp_elem->sub_list;
		
	struct node *it_grade;
	
	it_grade = list_begin(stud_list_point);
	int j = 0;
	float total = 0;
	if (list_size(stud_list_point) == 0 ) {
		return 0;
	}
	while (j<list_size(stud_list_point)){ 
		temp_elem = (list_element_t*) list_get(it_grade);
		total = total + temp_elem->number;
		it_grade = list_next(it_grade);
        j++;
	}
	return total/j ;
	
}

	
int grades_print_student(struct grades *grades, int id){
	struct node *it_main;
	struct list *stud_list_point;
	list_element_t *temp_elem;
	int i=0;
	it_main = list_begin(grades->linked_list);
	temp_elem = (list_element_t*) list_get(it_main);
	/*finding the student:*/
	while (i<list_size(grades->linked_list)&& (temp_elem->number != id )){
		it_main = list_next(it_main);
		temp_elem = (list_element_t*) list_get(it_main);
		i++;
	}
	if((i==list_size(grades->linked_list)&& (temp_elem->number != id ))){
		return -1;
	}

	stud_list_point =(struct list*) temp_elem->sub_list;
	printf("%s %d:", temp_elem->name_, temp_elem->number);
	struct node *it_grade;
	
	it_grade = list_begin(stud_list_point);
	int j = 0;
	if (list_size(stud_list_point) == 0 ) {
	    printf("\n");
	    return 0;
	}
	while (j<list_size(stud_list_point)){ 
		temp_elem = (list_element_t*) list_get(it_grade);
			if (list_next(it_grade)==NULL) {
					printf (" %s %d", temp_elem->name_, temp_elem->number);
			}
			else {
					printf (" %s %d,",temp_elem->name_, temp_elem->number);
			}	
		it_grade = list_next(it_grade);
        j++;
	}
	printf("\n");
	return 0 ;
}


int grades_print_all(struct grades *grades){
	int i = 0;
	list_element_t *temp_elem;
	struct node *it_main;
	
	it_main = list_begin(grades->linked_list);
	while (i<list_size(grades->linked_list)){		
		temp_elem = (list_element_t*) list_get(it_main);
		if (grades_print_student(grades, temp_elem->number) == -1 ){
			return -1;
		}
		i++;
		it_main = list_next(it_main);
	}	
	return 0;
}
