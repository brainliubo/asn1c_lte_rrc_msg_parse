﻿#include <stdio.h>
#include <sys/types.h>
#include <PHICH-Config.h>
#include <BCCH-BCH-Message.h>

extern int DL_SCH_MESSAGE_ENCODE_SIB_SIB1(int argc,char * argv[]);
extern int DL_SCH_MESSAGE_DECODE_SIB_SIB1(int argc,char * argv[]);

/* Write the encoded output into some FILE stream. */
int write_out(const void *buffer, size_t size, void *app_key) {
	FILE *out_fp = app_key;
	size_t wrote = fwrite(buffer, 1, size, out_fp);
	return (wrote == size) ? 0 : -1;
}







int BCCH_BCH_MESSAGE_ENCODE(int argc,char * argv[])
{

    BCCH_BCH_Message_t  *bch_msg; 
	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 

    char sfn = 8; 
	uint16_t *spare = calloc(1, sizeof(uint16_t));
	unsigned char buf[1024]; 
	int  size ; 
	int i; 

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
	bch_msg->message.phich_Config.phich_Resource = 3; 
    bch_msg->message.schedulingInfoSIB1_BR_r13 = 0; 
	
    bch_msg->message.systemFrameNumber.bits_unused = 0;
    bch_msg->message.systemFrameNumber.buf = &sfn; 
    bch_msg->message.systemFrameNumber.size = 1;
	
    bch_msg->message.spare.bits_unused = 1;
    bch_msg->message.spare.buf = (char *)spare; 
    bch_msg->message.spare.size = 1;

    //!<encode BCH 
    ec = asn_encode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_BCCH_BCH_Message, bch_msg, write_out, fp); //涓ょ鏂瑰紡閮藉彲浠?
   // ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&BCCH_BCH_Message_t, bch_msg,encode_buffer,buffersize); 
	fclose(fp);
	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode Rectangle (at %s)\n",
		ec.failed_type ? ec.failed_type->name : "unknown");
		exit(1);
	} else {
		printf("ec.encoded = %d bytes\n",ec.encoded); 
		fprintf(stderr, "Created %s with PER encoded success\n", filename);

		
		fp = fopen(filename,"rb"); 
		size = fread(buf, 1, sizeof(buf), fp); 
		if (size !=0)
		{
			printf("encode output byte: ");
		    for (i = 0; i < size;i++)
			{
			   fprintf(stdout,"%x", buf[i]); 
			}
			printf("  \n");
		}
	}
	fclose(fp);
	
	asn_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); 
    system("pause");
	return 0; /* Encoding finished successfully */
}


int BCCH_BCH_MESSAGE_DECODE(int argc,char * argv[])
{

    BCCH_BCH_Message_t  *bch_msg = NULL; 
	FILE *fp;
	asn_dec_rval_t ec; 
	const char *filename; 
    char buf[1024] = {0}; 
    int size ; 


	if(argc < 2) 
	{
		fprintf(stderr, "Specify filename for BER output\n");
	} else 
	{
		filename = argv[1];
		fp = fopen(filename, "rb");
		/* for BER output */
		if(!fp) {
			perror(filename);
			exit(1);
	    }
	}


	size = fread(buf, 1, sizeof(buf), fp); //!<灏嗘暟鎹産it浠庢枃浠朵腑璇诲叆鍒癇uffer涓紝杩斿洖size 
	fclose(fp);
	if(!size) {
		fprintf(stderr, "%s: Empty or broken\n", filename);
		exit(1);
	}

	
	

    //!<encode BCH 
	//!<注意这里的第四个参数，是（void **）,对指针再&地址
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_BCCH_BCH_Message, (void **)&bch_msg, buf, size); 
   
	fclose(fp);
	if(ec.code != RC_OK) {
		fprintf(stderr, "%s: Broken  decoding at byte %ld\n", filename,
		(long)ec.consumed);
		exit(1);
	} else {
		printf("ec.consumed = %d bytes\n",ec.consumed); 
		fprintf(stderr, " %s with PER decode success\n", filename);
	} 
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); //!<
    	asn_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg);  
    system("pause");
	return 0; /* Encoding finished successfully */
}




int main(int argc, char *argv[])
{
	//BCCH_BCH_MESSAGE_ENCODE(argc,argv); 
	//BCCH_BCH_MESSAGE_DECODE(argc,argv);
	DL_SCH_MESSAGE_ENCODE_SIB_SIB1(argc,argv);
	DL_SCH_MESSAGE_DECODE_SIB_SIB1(argc,argv);

}
