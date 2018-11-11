gc_prompt: gc_prompt_interp.c gc_macro_cmd.c gc_cmd_str_interp.c gc_crt_telecmd_pkt.c gc_open_port.c gc_port_config.c gc_write_buffer.c get_doy.c
	gcc -o gc_prompt gc_prompt_interp.c gc_macro_cmd.c gc_cmd_str_interp.c gc_crt_telecmd_pkt.c gc_open_port.c gc_port_config.c gc_write_buffer.c get_doy.c
gc_rcv_tlm: gc_rcv_tlm.c gc_open_port.c gc_port_config.c gc_read_buffer.c gc_tlm_pkt_proc.c
	gcc -o gc_rcv_tlm gc_rcv_tlm.c gc_open_port.c gc_port_config.c gc_read_buffer.c gc_tlm_pkt_proc.c
clean:
	rm gc_prompt