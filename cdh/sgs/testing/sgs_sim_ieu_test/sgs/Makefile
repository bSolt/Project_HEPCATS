gc_prompt: gc_interp_prompt.c gc_macro_cmd.c gc_interp_cmd_str.c gc_crt_telecmd_pkt.c gc_open_port.c gc_config_port.c gc_write_port.c
	gcc -Wall -o gc_prompt gc_interp_prompt.c gc_macro_cmd.c gc_interp_cmd_str.c gc_crt_telecmd_pkt.c gc_open_port.c gc_config_port.c gc_write_port.c
gc_rcv_tlm: gc_rcv_tlm.c gc_open_port.c gc_config_port.c gc_read_port.c gc_proc_tlm_pkt.c
	gcc -Wall -o gc_rcv_tlm gc_rcv_tlm.c gc_open_port.c gc_config_port.c gc_read_port.c gc_proc_tlm_pkt.c
clean:
	rm gc_prompt
	rm gc_rcv_tlm