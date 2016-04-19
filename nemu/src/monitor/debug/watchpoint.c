#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;
/******************************/
uint32_t expr(char *e, bool *success) ;
/******************************/
void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}
/* TODO: Implement the functionality of watchpoint */
// ******** src/monitor/cpu-exec.c
bool check_wp(){
	WP * curr = head;
	bool if_success;
	bool if_stop = false;
	while( curr != NULL ){
		curr->new_val = expr( curr->args, &if_success );
		if( !if_success ){
			printf("Expression Error!\n");
			if_stop = true;
		}
		if( curr->old_val != curr->new_val ){
			printf( "%s:\tHit WatchPoints!\n", curr->args );
			printf("Old Value = %d\nNew Value = %d\n", curr->old_val, curr->new_val );
			curr->old_val = curr->new_val;
			if_stop = true;
		}
		curr = curr->next;
	}
	return if_stop;
}
// ******** src/monitor/debug/ui.c
// ***************** static int cmd_w( char * args )
WP * new_wp(){
	if( free_ == NULL ) assert(0);
	WP * nwp = free_;
	free_ = free_->next;
	nwp->next = head;
	head = nwp;
	return nwp;
}
// ***************** static int cmd_d( char * args )
void free_wp( WP * prev ){
	WP * tmp = prev->next;
	prev->next = tmp->next;
	tmp ->next = free_;
	free_ = tmp;
}
bool delete_wp( int num ){
	if( num > 31 || num < 0 )
		return false;
	WP * prev = head;
	int i;
	if( num == 0 ){
		WP * tmp = head;
		head = head->next;
		tmp->next = free_;
		free_ = tmp;
	}
	else{
		for( i = 0; i < num-1; i++ )  {prev = prev->next;}
		free_wp( prev );
	}
	return true;
}
// ***************** static int cmd_info( char * args )
void query_wp(){
	WP * curr = head;
	int i = 0;
	while( curr != NULL ){
		printf("%2d\t%s:\t%d\n", i, curr->args, curr->old_val);
		curr = curr->next;
		i++;
	}return;
}