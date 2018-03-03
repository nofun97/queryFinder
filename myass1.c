/* COMP10002 Assignment 1, this programs take in texts and find similarities
 * based on the query
 *
 * Novan Allanadi, September 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define MAX_CHARACTER 1000
#define MAX_SIZE (MAX_CHARACTER + 1)
#define MAX_STORED_LINE 5
#define SMALLEST_SCORE (MAX_STORED_LINE - 1)
#define TRUE 1
#define FALSE 0

/****************************************************************/

typedef struct{
    char line_content[MAX_SIZE];
    int line_num;
    double score;
} line_info;

/****************************************************************/

void query_scan(int query_count, char **queries);
void clear_array(line_info *top_line);
void line_process(int query_count, char **queries, line_info *top_line);
double line_scoring(char sentence[], char **queries, int word_count,
                    int query_count);
int mygetchar();
int cmp(line_info *line1, line_info *line2);
void score_sort(line_info top_line[], double *min_score);
void high_score_print(line_info *top_line);

/****************************************************************/

/* main function call the functions
 */

int main(int argc, char **argv){

    /* scan the queries */
    query_scan(argc, argv);

    /* setting the scores and line number of the array to 0 */
    line_info top_line[MAX_STORED_LINE];
    clear_array(top_line);

    /* process the line */
    line_process(argc, argv, top_line);

    /* prints the top scoring line */
    high_score_print(top_line);
    return 0;
}

/****************************************************************/

/* checking for the right queries
 */

void query_scan(int query_count, char **queries){
    int wrong_queries = 0;

    /* should there be no query, the program exits with error code */
    if (query_count == 1){
        printf("S1: No query specified, must provide at least one word\n");
        exit(EXIT_FAILURE);
    }

    /* printing the query */
    printf("S1: query =");
    int i, j;
    for (i = 1; i < query_count; i++){
        printf(" %s", queries[i]);
    }

    printf("\n");

    /* should there be wrong query, it will be printed */
    for (i = 1; i < query_count; i++){
        for (j = 0; queries[i][j] != '\0'; j++){
            if (!isalnum(queries[i][j]) || isupper(queries[i][j])){
                printf("S1: %s: invalid character(s) in query\n", queries[i]);
                wrong_queries++;
                break;
            }
        }
    }

    /* the program will stop should there be wrong queries */
    if (wrong_queries){
        exit(EXIT_FAILURE);
    }
}

/****************************************************************/

/* initializing array values to 0
 */

void clear_array(line_info *top_line){
    int i;
    for (i = 0; i < MAX_STORED_LINE; i++){
        top_line[i].line_num = 0;
        top_line[i].score = 0;
    }
}

/****************************************************************/

/* counting lines, bytes, and words of each line
 */

void line_process(int query_count, char **queries, line_info *top_line){
    int line = 1, bytes = 0, words = 0, letters = 0;
    double min_score = 0;
    char chars, text_line[MAX_SIZE];

    while ((chars = mygetchar()) != EOF){
        /* storing the line character by character */
        text_line[bytes] = chars;

        /* if chars is not a new line, then bytes is added */
        bytes += (chars != '\n');

        /* should there isn't any characters in a line, it will be skipped */
        if (!bytes){
            line++;
            continue;
        }

        /* if chars is an alphanumeric character, letters will be added */
        letters += (isalnum(chars) != 0);

        /* if chars a non alphanumeric character and there are letters before
         * chars, word is added and letters is reset
         */
        if ((!isalnum(chars) && letters)){
            words++;
            letters = 0;
        }

        /* should chars be a new line information regarding the line will be
         * printed and next line will be processed
         */
        if (chars == '\n'){
            text_line[bytes] = '\0';
            printf("---\n");
            printf("%s\n", text_line);
            printf("S2: line = %d, bytes = %d, words = %d\n", line, bytes,
                   words);
            double line_score = line_scoring(text_line, queries, words,
                                             query_count);
            printf("S3: line = %d, score = %.3lf\n", line, line_score);

            /* should the score of the current line be larger than the smallest
             * score in the array, then the line will be processed
             */
            if (line_score > min_score){
                memcpy(top_line[SMALLEST_SCORE].line_content, text_line,
                       strlen(text_line) + 1);
                top_line[SMALLEST_SCORE].score = line_score;
                top_line[SMALLEST_SCORE].line_num = line;
                score_sort(top_line, &min_score);
            }
            line++;
            bytes = 0;
            words = 0;
            letters = 0;
        }
    }
    printf("------------------------------------------------\n");

}

/****************************************************************/

/* scoring the lines based on the query
 */

double line_scoring(char sentence[], char **queries, int word_count,
                    int query_count){
    double sum = 0;
    int i, j;
    for (i = 1; i < query_count; i++){
        int query_length = strlen(queries[i]);
        int index = 0, matches = 0;

        /* iterate through the sentence */
        for (j = 0; j < strlen(sentence); j++){

            /* should the starting character of a sentence be the same as the
             * query's starting character, it will start checking the prefix
             */
            if (queries[i][index] == tolower(sentence[j])){
                index++;

                /* if index is the same as the query length, that means the
                 * prefix is the same as the query
                 */
                if (index == query_length){
                    matches++;
                    index = 0;
                    // skip to the next word
                    for (; isalnum(sentence[j]); j++);
                }
            }
                /* skipping to the next word should the query and prefix do not
                 * match
                 */
            else{
                index = 0;
                for (; isalnum(sentence[j]); j++);
            }
        }

        /* if there are matches, it will be added to the sum to count the
         * nominator
         */
        if(matches > 0){
            sum += (log(1.0 + matches)/log(2.0));
        }
    }
    return sum/(log(8.5 + word_count)/log(2.0));
}

/****************************************************************/

/* replacing getchar function
 */

int mygetchar(){
    /* adapted from
     * http://people.eng.unimelb.edu.au/ammoffat/teaching/10002/ass1/
     */
    int c;
    while ((c=getchar())=='\r') {
    }
    return c;
}

/****************************************************************/

/* function returns true should the values be swapped
 */

int cmp(line_info *line1, line_info *line2){
    if (line2->score != line1->score){
        return (line2->score > line1->score) ? TRUE : FALSE;
    }
    return (line1->line_num > line2->line_num) ? TRUE : FALSE;
}

/****************************************************************/

/* sorting the array
 */

void score_sort(line_info top_line[], double *min_score){
    int index;
    /* as only the last element is not sorted only it will be to be moved
     * to the right index
     */
    for (index = SMALLEST_SCORE-1; cmp(&top_line[index], &top_line[index+1])
                                   && index >= 0; index--){
        line_info temp = top_line[index];
        top_line[index] = top_line[index+1];
        top_line[index+1] = temp;
    }

    /* changing the value of the minimum score */
    *min_score = top_line[SMALLEST_SCORE].score;
}

/****************************************************************/

/* printing the the lines with top scores
 */

void high_score_print(line_info top_line[]){
    int i;
    /* prints the top scoring line */
    for (i = 0; i < MAX_STORED_LINE; i++){
        if (top_line[i].score != 0) {
            printf("S4: line = %d, score = %.3f\n", top_line[i].line_num,
                   top_line[i].score);
            printf("%s\n", top_line[i].line_content);
            printf("---\n");
        }
    }
}

/* Algorithms are fun */