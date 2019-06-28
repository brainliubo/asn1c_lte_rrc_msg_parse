#include <stdio.h>
#include <sys/types.h>
#include <PHICH-Config.h>
#include <BCCH-BCH-Message.h>


/* Write the encoded output into some FILE stream. */
static int write_out(const void *buffer, size_t size, void *app_key) {
	FILE *out_fp = app_key;
	size_t wrote = fwrite(buffer, 1, size, out_fp);
	return (wrote == size) ? 0 : -1;
}


int main(int argc, char *argv[])
{
	BCCH_BCH_Message_t  *bch_msg; 
	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 

    char sfn = 8; 
	uint16_t *spare = calloc(1, sizeof(uint16_t));



	if(argc < 2) 
	{
		fprintf(stderr, "Specify filename for BER output\n");
	} else 
	{
		filename = argv[1];
		fp = fopen(filename, "wb");
		/* for BER output */
		if(!fp) {
			perror(filename);
			exit(1);
	    }
	}
 	
	/* Allocate the Rectangle_t */
	bch_msg = calloc(1, sizeof(BCCH_BCH_Message_t)); /* not malloc! */
	if(!bch_msg) {
		perror("calloc() failed");
		exit(1);
	}

    bch_msg->message.dl_Bandwidth = 5;
    bch_msg->message.phich_Config.phich_Duration = 1; 
	bch_msg->message.phich_Config.phich_Resource = 2; 
    bch_msg->message.schedulingInfoSIB1_BR_r13 = 0; 
	
    bch_msg->message.systemFrameNumber.bits_unused = 0;
    bch_msg->message.systemFrameNumber.buf = &sfn; 
    bch_msg->message.systemFrameNumber.size = 1;
	
    bch_msg->message.spare.bits_unused = 1;
    bch_msg->message.spare.buf = (char *)spare; 
    bch_msg->message.spare.size = 1;

    //!<encode BCH 
    ec = asn_encode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_BCCH_BCH_Message, bch_msg, write_out, fp); //两种方式都可�?
   // ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&BCCH_BCH_Message_t, bch_msg,encode_buffer,buffersize); 
	fclose(fp);
	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode Rectangle (at %s)\n",
		ec.failed_type ? ec.failed_type->name : "unknown");
		exit(1);
	} else {
		printf("ec.encoded = %d bytes\n",ec.encoded); 
		fprintf(stderr, "Created %s with PER encoded success\n", filename);
	}

	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_Rectangle, rectangle); //!<
	asn_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg);  //!< 在不完全编码时，也可以打印，比xer_fprint更适合于调�?
    system("pause");
	return 0; /* Encoding finished successfully */

}
