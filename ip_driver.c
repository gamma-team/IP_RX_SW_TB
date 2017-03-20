#include "ip_driver.h"

void set_cust_ip_reg(int baseaddr, int regnum, int value){
	Xil_Out32(baseaddr + regnum, value);
}

int get_cust_ip_reg(int baseaddr, int regnum){
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

void set_end_bit(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp | 0x00010000;
	set_cust_ip_reg(baseaddr, 0, temp);
}

void clr_end_bit(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 0);
	temp = temp & 0xFFFEFFFF;
	set_cust_ip_reg(baseaddr, 0, temp);
}

void set_header(int baseaddr, int hdr_1, int hdr_2, int hdr_3, int hdr_4, int hdr_5, int num_opt, ...){
	set_cust_ip_reg(baseaddr, 1, hdr_1);
	set_cust_ip_reg(baseaddr, 2, hdr_2);
	set_cust_ip_reg(baseaddr, 3, hdr_3);
	set_cust_ip_reg(baseaddr, 4, hdr_4);
	set_cust_ip_reg(baseaddr, 5, hdr_5);


	va_list valist;
	int i;

	/* initialize valist for num number of arguments */
	va_start(valist, num_opt);

	/* access all the arguments assigned to valist */
	for (i = 0; i < num_opt; i++) {
		set_cust_ip_reg(baseaddr, i+6, va_arg(valist, int));
	}

	/* clean memory reserved for valist */
	va_end(valist);
}

int get_header_length(int baseaddr){
	int temp;
	temp = get_cust_ip_reg(baseaddr, 1);
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
		set_cust_ip_reg(baseaddr, i+10, va_arg(valist, int));
	}

	/* clean memory reserved for valist */
	va_end(valist);

}

int get_return_status(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 256);
	temp = temp & 0x000F0000;
    if(temp>0){
    	return 1;
    } else {
    	return 0;
    }
}

int get_return_data_length(int baseaddr){
	int temp = 0;
	temp = get_cust_ip_reg(baseaddr, 256);
	temp = temp & 0x0000FFFF;
	return temp;
}
