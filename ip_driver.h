#include "xil_io.h"
#include "stdarg.h"

void set_cust_ip_reg(int baseaddr, int regnum, int value);
unsigned int get_cust_ip_reg(int baseaddr, int regnum);
void set_start_bit(int baseaddr);
void clr_start_bit(int baseaddr);
void set_single_exec(int baseaddr);
void set_multiple_exec(int baseaddr, int numExec);
void set_payload_seq(int baseaddr);
void set_payload_red(int baseaddr);
unsigned int get_count_in(int baseaddr);
unsigned int get_count_out(int baseaddr);
unsigned int get_output_length(int baseaddr);
unsigned int is_output_started( int baseaddr);
unsigned int is_output_finished(int baseaddr);

void set_header(int baseaddr, int hdr_1, int hdr_2, int hdr_3, int hdr_4, int hdr_5, int num_opt, ...);
unsigned int get_header_length(int baseaddr);
void set_packet_data(int baseaddr, int num_half_words, ...);
void get_output_data(int baseaddr, int* rtn);

