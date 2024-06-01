#include "btree.hpp"
#include <cstring>
#include <iostream> 

btree::btree(){
	root = new page(LEAF);
	height = 1;
};

void btree::insert(char *key, uint64_t val){
	// Please implement this function in project 3.
	page* stack[1000];
	int top=-1;
	page* C = root;
	while (C->get_type() == INTERNAL) {
		char* split_key = nullptr;
		top++;
		stack[top] = C;
		C = (page *)C->find(key);

	}

	if(!C->is_full(strlen(key) + 1 + sizeof(val) + sizeof(uint16_t))) {
		C->insert(key, val);//빈공간 있으면 그냥 삽입
	}else {
		//가득차서 새로운 node가 필요
		char* new_parent_key = nullptr;
		page* new_leaf = C->split(key, val, &new_parent_key);

		if(C == root) {
			//split 해야하는게 root일때 새로운 root 필요
			page* new_root = new page(INTERNAL);
			new_root->insert(new_parent_key, (uint64_t)new_leaf);
			new_root->set_leftmost_ptr(C);
			root = new_root;
			height++;
		}else {
			page* parent = stack[top];
			parent->insert(new_parent_key, (uint64_t)new_leaf);
			top--;
		}
	}
}

uint64_t btree::lookup(char *key){
	// Please implement this function in project 3.

	page* C = root;
	while (C->get_type() == INTERNAL) {
		C = (page *)C->find(key);
	}

	return C->find(key);
}



// uint16_t record_size = 0;
//
// void *stored_key = nullptr;
// uint64_t stored_val = 0;
//
// printf("## slot header\n");
// printf("Number of data :%d\n", num_data);
// printf("offset_array : |");
//
// printf("\n");