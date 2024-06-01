#include "page.hpp"
#include <iostream> 
#include <cstring> 

void put2byte(void *dest, uint16_t data){
	*(uint16_t*)dest = data;
}

uint16_t get2byte(void *dest){
	return *(uint16_t*)dest;
}

page::page(uint16_t type){
	hdr.set_num_data(0);
	hdr.set_data_region_off(PAGE_SIZE-1-sizeof(page*));
	hdr.set_offset_array((void*)((uint64_t)this+sizeof(slot_header)));
	hdr.set_page_type(type);
}

uint16_t page::get_type(){
	return hdr.get_page_type();
}

uint16_t page::get_record_size(void *record){
	uint16_t size = *(uint16_t *)record;
	return size;
}

char *page::get_key(void *record){//key의 주소값은 record의 주소 + 2byte
	char *key = (char *)((uint64_t)record+sizeof(uint16_t));
	return key;
}

uint64_t page::get_val(void *key){
	uint64_t val= *(uint64_t*)((uint64_t)key+(uint64_t)strlen((char*)key)+1);//value의 주소값은 key의 주소 + key의 길이 + 1
	return val;
}

void page::set_leftmost_ptr(page *p){
	leftmost_ptr = p;
}

page *page::get_leftmost_ptr(){
	return leftmost_ptr;	
}

uint64_t page::find(char *key){
	// Please implement this function in project 2.
	//printf("find start\n");
	uint64_t val = 0;
	uint32_t num = hdr.get_num_data();
	uint16_t *off = (uint16_t*)hdr.get_offset_array();
	uint16_t base = sizeof(slot_header)+sizeof(page*);
	int temp_cmp;



	for(uint32_t i = 0; i<hdr.get_num_data(); i++){// Record offset
		uint16_t record_offset = off[i];
		char* comp_key = get_key((char *)this+record_offset);
		// char* comp_key = get_key(data+record_offset-base);
		if(*comp_key == NULL) continue;

		temp_cmp=strcmp(comp_key,key);

		if(temp_cmp == 0){
			val = get_val(comp_key);//find 혹은 오른쪽ptr
			return val;
		}
		else if(temp_cmp == 1 && get_type() == INTERNAL) {// 내부가 더 큰 것을 만난 경우 왼쪽 ptr
			if(i==0) return (uint64_t)get_leftmost_ptr();
			char* prev_key = get_key((char *)this + off[i-1]);
			val = get_val(prev_key);
			return val;
		}else if(temp_cmp == -1 && get_type() == INTERNAL) {
			return (uint64_t)get_leftmost_ptr();
		}
	}


	return val;
	//
	// for(int i = 0; i<sizeof(data); i++) {
	// 	if(data[i] != NULL) {
	// 		printf(" %c", data[i]);
	// 	}
	// }
	//printf("find return %llu\n",val);

}

bool page::insert(char *key,uint64_t val){
	// printf("page addr: %p\n",(char *)this);
	// printf("data addr: %p\n",&data);
	// Please implement this function in project 2.
	//free space의 마지막 위치 갱신
	//printf("insert start\n");
	uint16_t size = strlen(key) + 1 + sizeof(val) + sizeof(uint16_t);

	if(is_full(size)) return false;

	uint16_t base = sizeof(slot_header)+sizeof(page*);

	uint16_t off = hdr.get_data_region_off() - size;//
	hdr.set_data_region_off(off);

	put2byte((void *)((char*)this + off), size);
	memcpy((void *)((char*)this + off + sizeof(size)), key, strlen(key)+1);
	memcpy((void *)((char*)this + off + sizeof(size) + strlen(key)+1), &val, sizeof(uint64_t));

	// put2byte((void *)(data + off  - base), size);
	// memcpy((void *)(data + off + sizeof(size)  - base), key, strlen(key)+1);
	// memcpy((void *)(data + off + sizeof(size) + strlen(key)+1  - base), &val, sizeof(uint64_t));

// data는 header에 들어갈 만큼의 크기를 빼서 사용하는 data_region_off와 사이즈가 맞지 않아 32(base)만큼을 빼고 계산한다.
// print()에서 this를 이용한 연산을 하여
	uint32_t prev_num = hdr.get_num_data();
	void *offset_array=hdr.get_offset_array();
	hdr.set_num_data(prev_num+1);

	void *new_record_offset = (void *)((uint64_t)offset_array + sizeof(uint16_t) * prev_num);

	memcpy(new_record_offset, &off, sizeof(uint16_t));
	//printf("insert success");
	return true;
}

page* page::split(char *key, uint64_t val, char** parent_key){
	// Please implement this function in project 3.
	page *new_page;
	return new_page;
}

bool page::is_full(uint64_t inserted_record_size){
	// Please implement this function in project 2.
	//printf("isfull start\n");
	uint32_t num = hdr.get_num_data();
	uint32_t base_size = sizeof(slot_header)+sizeof(page*);
	uint64_t total_size = base_size + num * sizeof(uint16_t) + inserted_record_size;
	if(total_size > hdr.get_data_region_off()) {
		//printf("isfull return true");
		return true;
	}
	else{
		//printf("isfull return false");
		return false;
	}
}

void page::defrag(){
	page *new_page = new page(get_type());
	int num_data = hdr.get_num_data();
	void *offset_array=hdr.get_offset_array();
	void *stored_key=nullptr;
	uint16_t off=0;
	uint64_t stored_val=0;
	void *data_region=nullptr;

	for(int i=0; i<num_data/2; i++){
		off= *(uint16_t *)((uint64_t)offset_array+i*2);	
		data_region = (void *)((uint64_t)this+(uint64_t)off);
		stored_key = get_key(data_region);
		stored_val= get_val((void *)stored_key);
		new_page->insert((char*)stored_key,stored_val);
	}	
	new_page->set_leftmost_ptr(get_leftmost_ptr());

	memcpy(this, new_page, sizeof(page));
	hdr.set_offset_array((void*)((uint64_t)this+sizeof(slot_header)));
	delete new_page;

}

void page::print(){
	uint32_t num_data = hdr.get_num_data();
	uint16_t off=0;
	uint16_t record_size= 0;
	void *offset_array=hdr.get_offset_array();
	void *stored_key=nullptr;
	uint64_t stored_val=0;


	printf("## slot header\n");
	printf("Number of data :%d\n",num_data);
	printf("offset_array : |");
	for(int i=0; i<num_data; i++){
		off= *(uint16_t *)((uint64_t)offset_array+i*2);	
		printf(" %d |",off);
	}
	printf("\n");

	void *data_region=nullptr;
	for(int i=0; i<num_data; i++){
		off= *(uint16_t *)((uint64_t)offset_array+i*2);	
		data_region = (void *)((uint64_t)this+(uint64_t)off);
		record_size = get_record_size(data_region);
		stored_key = get_key(data_region);
		stored_val= get_val((void *)stored_key);
		printf("==========================================================\n");
		printf("| data_sz:%u | key: %s | val :%lu | key_len:%lu\n",record_size,(char*)stored_key, stored_val,strlen((char*)stored_key));

	}
}






