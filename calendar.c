#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "calendar.h"

#include "event.h"


int init_calendar(const char * name, int days,
  int( * comp_func)(const void * ptr1,
    const void * ptr2),
  void( * free_info_func)(void * ptr), Calendar ** calendar) {
  int length;
  Calendar * p;
  if (name == NULL || calendar == NULL || days < 1) {
    return FAILURE;
  }

  length = strlen(name);

    /*Allocating memory for fields  and checking for null*/
  p = malloc(sizeof(Calendar));
  p -> name = malloc((length + 1));
  p -> events = calloc(days, sizeof(Event * ));
  if (p == NULL || p -> name == NULL || p -> events == NULL) {
    return FAILURE;
  }
  /* Allocating respective fields */
  strcpy(p -> name, name);
  p -> total_events = 0;
  p -> free_info_func = free_info_func;
  p -> comp_func = comp_func;
  p -> days = days;

  *calendar = p;
  
     return SUCCESS;
   } 
int print_calendar(Calendar * calendar, FILE * output_stream, int print_all) {
       FILE * parse;
         int i;
           int day = 0;
             Event * current;
               if (calendar == NULL || output_stream == NULL) {
                   return FAILURE;
                     }
                       /* Print all case and printing Events regardless */
  parse = output_stream;
  if (print_all == 1) {

    fprintf(parse, "Calendar's Name: \"%s\"\n", calendar -> name);
    fprintf(parse, "Days: %d\n", calendar -> days);
    fprintf(parse, "Total Events: %d\n", calendar -> total_events);
    fprintf(parse, "\n");
  }

  fprintf(parse, "%s", "**** Events ****\n");

  if (calendar -> total_events == 0) {
    return SUCCESS;
  }
    
    /* Traversing to get to each event */
  for (i = 0; i < calendar -> days; i++) {
    day++;
    fprintf(parse, "Day %d\n", day);

    current = calendar -> events[i];
    /*While loop to print each event's information */
    while (current != NULL) {
      fprintf(parse, "Event's Name: \"%s\"", current -> name);
      fprintf(parse, ", Start_time: %d,", current -> start_time);
      fprintf(parse, " Duration: %d\n", current -> duration_minutes);

      current = current -> next;
    }

  }

  return SUCCESS;
}

int add_event(Calendar * calendar,
  const char * name, int start_time,
    int duration_minutes, void * info, int day) {
  Event * add, * curr, * prev;
  if (calendar == NULL || name == NULL || start_time < 0 || start_time > 2400 || duration_minutes <= 0 || day < 1 || day > calendar -> days || find_event(calendar, name, & add) == SUCCESS) {
    return FAILURE;
  }

  /*Create event and allocate mem for event to be added*/
  add = malloc(sizeof(Event));
  add -> name = malloc(strlen(name) + 1);
  /*Failure if they are not able to be allocated*/
  if (add == NULL || add -> name == NULL) {
    return FAILURE;
  }

  /*Change variables to actually create the event*/
  strcpy(add -> name, name);
  add -> start_time = start_time;
  add -> duration_minutes = duration_minutes;
  add -> info = info;

  curr = calendar -> events[day - 1];
  prev = NULL;

  while (curr != NULL && calendar -> comp_func(add, curr) > 0) {
    /* Processing here */
    prev = curr; /* Adjusting prev to current node */
    curr = curr -> next; /* Moving to next node */
  }

  if (prev == NULL) {
    /* Insert before */
    add -> next = calendar -> events[day - 1];
    calendar -> events[day - 1] = add;
  } else {
    add -> next = curr;
    prev -> next = add;
  }

  calendar -> total_events++;

  return SUCCESS;

}

int find_event(Calendar * calendar,
  const char * name, Event ** event) {
  int i;
  Event *curr;

  if (calendar == NULL || name == NULL) {
    return FAILURE;
  }
/*Traverse with method to reuse code*/
  for (i = 1; i <= calendar -> days; i++) {
    if (find_event_in_day(calendar, name, i, event) == SUCCESS) {
      /**event  = curr;*/
      return SUCCESS;
    }

  }
  if (event == NULL) {
    return FAILURE;
  }
  return FAILURE;
}

int find_event_in_day(Calendar * calendar,
  const char * name, int day,
    Event ** event) {
  Event * curr;
  if (calendar == NULL || name == NULL || day < 1 || day > calendar -> days) {
    return FAILURE;
  }

  if (calendar -> total_events == 0) {
    return FAILURE;
  }
/*Traverse to find event*/
  curr = calendar -> events[day - 1];
  while (curr != NULL) {
    if (strcmp(curr -> name, name) == 0) {
      *event = curr;
            return SUCCESS;
                 } else {
                       curr = curr -> next;
                           }
                            }
                               return FAILURE;
                               }
      
int remove_event(Calendar * calendar,
   const char * name) {
 Event * curr, * prev;
      
if (calendar == NULL || name == NULL) {
return FAILURE;
 }
      
        /*Setup for while loop*/

  curr = calendar -> events[0];
  prev = NULL;
  /* While loop to remove event */
  while (curr != NULL) {
      /* Finds the equivalent name and processes*/
    if (strcmp(curr -> name, name) == 0) {
      if (prev != NULL) {
        prev -> next = curr -> next;
      } else {
        calendar -> events[0] = NULL;
      }
      free(curr -> name);
    /* Null case */
      if (calendar -> free_info_func != NULL) {
        if (curr -> info != NULL) {
          calendar -> free_info_func(curr -> info);
        }
      }

      free(curr);
      calendar -> total_events--;

      return SUCCESS;
    }

    prev = curr;
    curr = curr -> next;

  }

  return FAILURE;

}

void * get_event_info(Calendar * calendar,
  const char * name) {
  Event * current;
  int i;
/*Finds event, and returns info pointer */
  for (i = 0; i < calendar -> days; i++) {
    current = calendar -> events[i];
    while (current != NULL) {
      if (strcmp(current -> name, name) == 0) {
        return current -> info;
      }

      current = current -> next;
    }
  }

  return NULL;
}

int clear_calendar(Calendar * calendar) {
  int i;
  if (calendar == NULL) {
    return FAILURE;
  }
  /*Loop to clear each day */
  for (i = 1; i <= calendar -> days; i++) {
    clear_day(calendar, i);
  }
  return SUCCESS;
}

int clear_day(Calendar * calendar, int day) {
  Event * curr, * prev;
  if (calendar == NULL || day < 1 || day > calendar -> days) {
    return FAILURE;
  }
  /*Loop to clear one day, curr is set to the first day */
  curr = calendar -> events[day - 1];

  while (curr != NULL) {
    prev = curr -> next;

    free(curr -> name);

    if (calendar -> free_info_func != NULL) {
      if (curr -> info != NULL) {
        calendar -> free_info_func(curr -> info);
      }
    }
    free(curr);
    calendar -> total_events--;
    curr = prev;
  }
  calendar -> events[day - 1] = NULL;

  return SUCCESS;

}

int destroy_calendar(Calendar * calendar) {

  if (calendar == NULL) {
    return FAILURE;
  }
  /*Clearing in order */
  clear_calendar(calendar);
  free(calendar -> name);
  free(calendar -> events);
  free(calendar);

  return SUCCESS;
}
