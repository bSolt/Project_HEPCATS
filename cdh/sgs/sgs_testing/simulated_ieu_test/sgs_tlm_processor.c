void tlm_processor(char* buffer)
{	
	// Save off packet header and packet data field:
	memcpy(buffer+0,&pkt_hdr,6);
	memcpy(buffer+6,&pkt_dat_fld,14);

	uint16_t pkt_id;

	memcpy(&pkt_id,buffer,2);

	// Mask the last 3 bits:
	uint8_t version = pkt_id & 0x07;//0x07;

	// Shift by 0 bits:
	version = version >> 0;

	printf("%d\n",version);

	unsigned int mask=1<<((sizeof(uint8_t)<<3)-1);
    while(mask) {
        printf("%d", (version&mask ? 1 : 0));
        mask >>= 1;}
}