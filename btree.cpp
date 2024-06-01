#include "btree.hpp"
#include <iostream> 

btree::btree(){
	root = new page(LEAF);
	height = 1;
};

void btree::insert(char *key, uint64_t val){
	// Please implement this function in project 3.

}

uint64_t btree::lookup(char *key){
	// Please implement this function in project 3.
	uint64_t val = 0;
	page* C = root;
	while (C->get_type() == INTERNAL) {
		//val = C->find(key);
		uint32_t num_data = hdr.get_num_data();
		uint16_t off = 0;
		void *offset_array = hdr.get_offset_array();
		for (int i = 0; i < num_data; i++) {
			off = *(uint16_t *)((uint64_t)offset_array + i * 2);
			if (key < off) {
				//왼쪽 포인터 타기
				
			}
			else if (key == off) {
				//오른쪽 포인터 타기
			}

			//돌아가기

		}
		
	}
	val = C->find(key);
	return val;
}



uint16_t record_size = 0;

void *stored_key = nullptr;
uint64_t stored_val = 0;

printf("## slot header\n");
printf("Number of data :%d\n", num_data);
printf("offset_array : |");

printf("\n");