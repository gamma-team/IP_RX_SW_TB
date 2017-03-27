#include "ip_driver.h"



void set_cust_ip_reg(int baseaddr, int regnum, int value){
	Xil_Out32(baseaddr + regnum, value);
}

unsigned int get_cust_ip_reg(int baseaddr, int regnum){
	int temp = 0;
	int addr=baseaddr+regnum*4;
	temp = Xil_In32(addr);
	return (temp);
}

void set_start_bit(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp | 0x00000001;
	set_cust_ip_reg(baseaddr, 0, temp);
}

void clr_start_bit(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp & 0xFFFFFFFE;
	set_cust_ip_reg(baseaddr, 0, temp);
}

void set_single_exec(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp & 0xFFFFFFFD;
	set_cust_ip_reg(baseaddr, 0, temp);
}

void set_multiple_exec(int baseaddr, int numExec){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp | 0x00000002;
	set_cust_ip_reg(baseaddr, 0, temp);
	set_cust_ip_reg(baseaddr, 1, numExec);
}

void set_payload_seq(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp & 0xFFFFFFFB;
	set_cust_ip_reg(baseaddr, 0, temp);
}

void set_payload_red(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp | 0x00000004;
	set_cust_ip_reg(baseaddr, 0, temp);
}

unsigned int get_count_in(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 2);
	return temp;
}

unsigned int get_count_out(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 3);
	return temp;
}

unsigned int get_output_length(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 4);
	temp = temp & 0x000007FC;
	temp = temp >> 2;
	return temp;
}

unsigned int is_output_started(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 4);
	temp = temp & 0x00000001;
	return temp;
}

unsigned int is_output_finished(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 4);
	temp = temp & 0x00000002;
	temp = temp >> 1;
	return temp;
}

void set_header(int baseaddr, int hdr_1, int hdr_2, int hdr_3, int hdr_4, int hdr_5, int num_opt, ...){
	set_cust_ip_reg(baseaddr, 5, hdr_1);
	set_cust_ip_reg(baseaddr, 6, hdr_2);
	set_cust_ip_reg(baseaddr, 7, hdr_3);
	set_cust_ip_reg(baseaddr, 8, hdr_4);
	set_cust_ip_reg(baseaddr, 9, hdr_5);


	va_list valist;
	int i;

	/* initialize valist for num number of arguments */
	va_start(valist, num_opt);

	/* access all the arguments assigned to valist */
	for (i = 0; i < num_opt; i++) {
		set_cust_ip_reg(baseaddr, i+10, va_arg(valist, int));
	}

	/* clean memory reserved for valist */
	va_end(valist);
}

unsigned int get_header_length(int baseaddr){
	int temp;
	temp = get_cust_ip_reg(baseaddr, 5);
	temp = temp & 0x0F000000;
	temp = temp >> 24;
	return (temp);
}


void set_packet_data(int baseaddr, int num_half_words, ...){
	va_list valist;
	int i;

	/* initialize valist for num number of arguments */
	va_start(valist, num_half_words);

	/* access all the arguments assigned to valist */
	for (i = 0; i < num_half_words; i++) {
		set_cust_ip_reg(baseaddr, i+14, va_arg(valist, int));
	}

	/* clean memory reserved for valist */
	va_end(valist);

}

void get_output_data(int baseaddr, int* rtn){
	int  i =0;
	int size = get_output_length(baseaddr);

	int temp_data[size];
	int temp_valid[size];
	union temp32bitint {
	    unsigned int integer;
	    unsigned char byte[4];
	} byte_data[size];

	int j = 0;
	for (i = 0; i < size; ++i) {
		temp_data[i] = get_cust_ip_reg(baseaddr, i+517);
		temp_valid[i] = get_cust_ip_reg(baseaddr, i+1029);
		if( ((temp_valid[i] >> 3) & 0x1) == 1 ) {
			byte_data[j/4].byte[j%4] = (temp_data[i] >> 24) & 0xFF;
			++j;
		}
		if( ((temp_valid[i] >> 2) & 0x1) == 1 ) {
			byte_data[j/4].byte[j%4] = (temp_data[i] >> 16) & 0xFF;
			++j;
		}
		if( ((temp_valid[i] >> 1) & 0x1) == 1 ) {
			byte_data[j/4].byte[j%4] = (temp_data[i] >> 8) & 0xFF;
			++j;
		}
		if( (temp_valid[i] & 0x1) == 1 ) {
			byte_data[j/4].byte[j%4] = temp_data[i] & 0xFF;
			++j;
		}
	}

	for (i = 0; i < j/4 + 1; ++i){
		rtn[i] = byte_data[j/4].integer;
	}



}
