// Command Packet Field Inputs Structure
struct telecmd_pkt_inputs
{
	// Packet Identification:
	uint16_t pkt_apid;

	// Packet Sequence Control:
	uint16_t pkt_name;
	
	// Packet Secondary Header:
	uint16_t pkt_t_year;
	uint16_t pkt_t_doy;
	uint8_t pkt_t_hour;
	uint8_t pkt_t_min;
	uint8_t pkt_t_sec;

	// Packet Data Field:
	uint8_t pkt_app_dat_atc_flg;
	uint8_t pkt_app_dat_cmd_arg;
};