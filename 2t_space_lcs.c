#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
void *malloc(size_t);
void free(void *);
char *splitFromAtoB(char *, int, int);
char *reverse(char *);
void exit(int);

char *g_xs; //input1
char *g_ys; //input2
int lcs_len; //length of lcs

typedef struct t_params{
	int *cur; // array of ints
	int x_beg, x_end;
	int y_beg, y_end;
	
} t_par;


void *vec_fwd(void * params)
{
        t_par *par = (t_par *)params;
        int x_beg = par->x_beg, x_end = par->x_end;
	int y_beg = par->y_beg, y_end = par->y_end;
	//
	char *xs = g_xs;
	char *ys = g_ys;
	//
        int len_y = 1 + y_end-y_beg;
        int *curr = malloc(sizeof(int) * len_y);
        int *prev = malloc(sizeof(int) * len_y);
        //
	int cur_at, x_i, y_i;
        for(cur_at = 0; cur_at < len_y; cur_at++)
                curr[cur_at] = 0, prev[cur_at] = 0;
	//
	for(x_i = x_beg; x_i < x_end; x_i++){
                prev[0] = curr[0]; 
		for(y_i = y_beg; y_i < y_end; y_i++){
			cur_at = y_i+1-y_beg;
                        prev[cur_at] = curr[cur_at]; 
                        if (xs[x_i] == ys[y_i]){
                                curr[cur_at] = prev[cur_at-1] + 1;
                        }
                        else{
                                curr[cur_at] = curr[cur_at-1];
                                if (curr[cur_at-1] < prev[cur_at]){
                                        curr[cur_at] = prev[cur_at];
                                }
                        }
                }
        }
        free(prev);
	par->cur = curr;
}
void *vec_bkwd(void * params)
{
        t_par *par = (t_par *)params;
        int x_beg = par->x_beg, x_end = par->x_end;
        int y_beg = par->y_beg, y_end = par->y_end;
	//
        char *xs = g_xs;
        char *ys = g_ys;
        //
        int len_y = 1 + y_end-y_beg;
        int *curr = malloc(sizeof(int) * len_y);
        int *prev = malloc(sizeof(int) * len_y);
        int *temp;
        int cur_at, x_i, y_i;
        for(cur_at = 0; cur_at < len_y; cur_at++)
                curr[cur_at] = 0, prev[cur_at] = 0;
	//
	for(x_i = x_end-1; x_beg<=x_i; x_i--){
                for(y_i = y_end-1; y_beg<=y_i; y_i--){
			cur_at = len_y-(y_i-y_beg)-1;
			prev[cur_at] = curr[cur_at];
                        if (xs[x_i] == ys[y_i]){
                                curr[cur_at] = prev[cur_at-1] + 1;
                        }
                        else{
				curr[cur_at]=curr[cur_at-1];
				if (curr[cur_at-1] < prev[cur_at]){
                                        curr[cur_at] = prev[cur_at];
                                }
                        }
                }
        }
        free(prev);
	par->cur = curr;
}

int top_lvl_t;

void *lcs(void *params)
{
	t_par *par = (t_par *)params;
        int x_beg = par->x_beg, x_end = par->x_end;
        int y_beg = par->y_beg, y_end = par->y_end;

	int x_len = x_end-x_beg;
	char *r="";
	if (x_len == 0){
		return r;
	}
	else if (x_len == 1){
		if(AcontainsB(g_ys,y_beg,y_end,g_xs,x_beg)){
			lcs_len++;
			r = malloc(sizeof(char)*2);
			strncpy(r,g_xs+x_beg,1);
			r[1]='\0'; 
			return r;		
		}
		return r;
	}
	else{
		int x_spl = (x_end+x_beg)/2;

		t_par param1 = {NULL, x_beg, x_spl,y_beg,y_end};
                t_par param2 = {NULL, x_spl, x_end,y_beg,y_end};

		pthread_t tids[2];
                pthread_create(&tids[0],NULL, vec_fwd, (void *)&param1);
                pthread_create(&tids[1],NULL, vec_bkwd, (void *)&param2);
                pthread_join(tids[0], NULL);
                pthread_join(tids[1], NULL);

                int *upper_m = param1.cur;
                int *lower_m = param2.cur;
		
		int y_len = y_end - y_beg +1;           
                int y_spl = maxColumn(upper_m, lower_m, y_len);
		
		t_par param3 = {NULL, x_beg, x_spl, y_beg, y_beg+y_spl};
                t_par param4 = {NULL, x_spl, x_end, y_beg+y_spl, y_end};
		
		lcs((void *)&param3);
		lcs((void *)&param4);
		
		return r;
	}
}

int main(int argc, char *argv [])
{	
	lcs_len = 0;
	char *word1 = argv[1];
	char *word2 = argv[2];
	g_xs = word1;
	g_ys = word2;	

	top_lvl_t = 1;
	t_par param1 = {NULL, 0, strlen(word1), 0, strlen(word2)};
	lcs((void *)&param1);
	
	printf("\n");
	printf("%d\n", lcs_len);

	return 0;
}
