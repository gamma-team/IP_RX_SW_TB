#include "xil_io.h"
#include "stdarg.h"

void set_cust_ip_reg(int baseaddr, int regnum, int value);
int get_cust_ip_reg(int baseaddr, int regnum);
void set_start_bit(int baseaddr);
void clr_start_bit(int baseaddr);
void set_end_bit(int baseaddr);
void clr_end_bit(int baseaddr);
void set_header(int baseaddr, int hdr_1, int hdr_2, int hdr_3, int hdr_4, int hdr_5, int num_opt, ...);
int get_header_length(int baseaddr);
void set_packet_data(int baseaddr, int num_half_words, ...);
int get_return_status(int baseaddr);
int get_return_data_length(int baseaddr);

