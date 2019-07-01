#include <BCCH-DL-SCH-Message.h>
#include <PLMN-IdentityInfo.h>
#include <asn_SEQUENCE_OF.h>
#include <SchedulingInfo.h>
#include <SIB-Type.h>
#include <TDD-Config.h>
#include <stdio.h>


extern  int write_out(const void *buffer, size_t size, void *app_key);

int DL_SCH_MESSAGE_ENCODE_SIB_SIB1(int argc,char *argv[])
{


	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 
	int size,i; 
	char buf[1024] = {0};

	BCCH_DL_SCH_Message_t *bcch_message; 
	SystemInformationBlockType1_t *sib1; 

	//!<sib1中使用到的结构体
	PLMN_IdentityInfo_t  plmm_identity_info[1]; 

	SchedulingInfo_t schedulingInfo;
	SIB_Type_t sib_type; 
	MCC_MNC_Digit_t dummy_mcc[1][3]; 
	MCC_MNC_Digit_t dummy_mnc[1][3]; 


	fprintf(stdout, "-----start encode-------------\n");
	//!<生成文件
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
	//calloc memory
    bcch_message = CALLOC(sizeof(BCCH_DL_SCH_Message_t),1); 
	memset((void *)bcch_message,0,sizeof(BCCH_DL_SCH_Message_t)); 

	//!<填写IE 
	bcch_message->message.present=BCCH_DL_SCH_MessageType_PR_c1; 
	bcch_message->message.choice.c1.present = BCCH_DL_SCH_MessageType__c1_PR_systemInformationBlockType1; 
    sib1 = &bcch_message->message.choice.c1.choice.systemInformationBlockType1; 

    //!< plmn info list 
    //!<1)设置MCC 
    memset((void *)plmm_identity_info,0,1 * sizeof(plmm_identity_info)); 
	plmm_identity_info[0].plmn_Identity.mcc = CALLOC(sizeof(MCC_MNC_Digit_t),1); 
	memset((void *)plmm_identity_info[0].plmn_Identity.mcc, 0,sizeof (plmm_identity_info[0].plmn_Identity.mcc)); 

	asn_set_empty(&plmm_identity_info[0].plmn_Identity.mcc->list); //!<数组清空

	dummy_mcc[0][0] = 0; 
	dummy_mcc[0][1] = 0; 
	dummy_mcc[0][2] = 0; 

	ASN_SEQUENCE_ADD(&plmm_identity_info[0].plmn_Identity.mcc->list, &dummy_mcc[0][0]); 
	ASN_SEQUENCE_ADD(&plmm_identity_info[0].plmn_Identity.mcc->list, &dummy_mcc[0][1]); 
	ASN_SEQUENCE_ADD(&plmm_identity_info[0].plmn_Identity.mcc->list, &dummy_mcc[0][2]); 

	//!< 设置MNC
	plmm_identity_info[0].plmn_Identity.mnc.list.size = 0; 
	plmm_identity_info[0].plmn_Identity.mnc.list.count = 0; 

	dummy_mnc[0][0] = 0; 
	dummy_mnc[0][1] = 1; 
	//dummy_mnc[0][2] = 0xf;   //!< 只能是0-9的int 

	ASN_SEQUENCE_ADD(&plmm_identity_info[0].plmn_Identity.mnc.list, &dummy_mnc[0][0]); 
	ASN_SEQUENCE_ADD(&plmm_identity_info[0].plmn_Identity.mnc.list, &dummy_mnc[0][1]); 

	ASN_SEQUENCE_ADD(&(sib1->cellAccessRelatedInfo.plmn_IdentityList.list), &plmm_identity_info[0]); 
	//tracking area code 是bitstring 类型的， 16bit
	sib1->cellAccessRelatedInfo.trackingAreaCode.buf = MALLOC(2); 
	sib1->cellAccessRelatedInfo.trackingAreaCode.buf[0]=0x0; 
	sib1->cellAccessRelatedInfo.trackingAreaCode.buf[1]=0x1; 
	sib1->cellAccessRelatedInfo.trackingAreaCode.bits_unused = 0; 
	sib1->cellAccessRelatedInfo.trackingAreaCode.size = 2; 
	//sib1->cellAccessRelatedInfo.cellIdentity 是bit string ,28bit 
	sib1->cellAccessRelatedInfo.cellIdentity.buf = MALLOC(8); //!<测试一下是否可以是4

	sib1->cellAccessRelatedInfo.cellIdentity.buf[0]=0x00; 
	sib1->cellAccessRelatedInfo.cellIdentity.buf[1]=0x00; 
	sib1->cellAccessRelatedInfo.cellIdentity.buf[2]=0x00; 
	sib1->cellAccessRelatedInfo.cellIdentity.buf[3]=0x00; 

	sib1->cellAccessRelatedInfo.cellIdentity.size = 4; 
	sib1->cellAccessRelatedInfo.cellIdentity.bits_unused=4;

	//!<
	sib1->cellAccessRelatedInfo.cellBarred = 1; 
	sib1->cellAccessRelatedInfo.intraFreqReselection = 1; 

	//!<csg identity bit27
	sib1->cellAccessRelatedInfo.csg_Indication = 0;
	/*
	sib1->cellAccessRelatedInfo.csg_Identity = CALLOC(sizeof(CSG_Identity_t),1); 
    */

	//!< 
	sib1->cellSelectionInfo.q_RxLevMin = -40; //~<[-60 ~-13] 
	sib1->cellSelectionInfo.q_RxLevMinOffset=NULL;  //!<option
	(sib1->p_Max) = CALLOC(sizeof(P_Max_t),1);
	sib1->p_Max[0] = 23; 
	sib1->freqBandIndicator = 7; 

	//!< schedulingInfo 
	memset((void *)&schedulingInfo,0,sizeof(schedulingInfo));
	schedulingInfo.si_Periodicity = 0; //8sf
	sib_type = 2;
	ASN_SEQUENCE_ADD(&schedulingInfo.sib_MappingInfo.list,&sib_type); 
	ASN_SEQUENCE_ADD(&sib1->schedulingInfoList.list, &schedulingInfo); 

	//!< 
	sib1->tdd_Config = CALLOC(sizeof(TDD_Config_t),1); 
	sib1->tdd_Config->specialSubframePatterns = 4; 
	sib1->tdd_Config->subframeAssignment = 1; 
	sib1->systemInfoValueTag = 2; 
	sib1->si_WindowLength = 2;

     //!< 开始编码
     ec = asn_encode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_BCCH_DL_SCH_Message, bcch_message, write_out, fp); //涓ょ鏂瑰紡閮藉彲浠?
   // ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&BCCH_BCH_Message_t, bch_msg,encode_buffer,buffersize); 
	fclose(fp);
	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode SIB1 (at %s)\n",
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

	
    //打印XML格式
	//xer_fprint(stdout,&asn_DEF_BCCH_DL_SCH_Message,(void*)bcch_message); 
	asn_fprint(stdout,&asn_DEF_BCCH_DL_SCH_Message,(void*)bcch_message); 
	system("pause");
	return 0;

}

int DL_SCH_MESSAGE_DECODE_SIB_SIB1(int argc,char * argv[])
{

    BCCH_DL_SCH_Message_t  *bch_msg = NULL; 
	FILE *fp;
	asn_dec_rval_t ec; 
	const char *filename; 
    char buf[1024] = {0}; 
    int size ; 

	fprintf(stdout,"-----start decode-------------\n");
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
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_BCCH_DL_SCH_Message, (void **)&bch_msg, buf, size); 
   
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
    	asn_fprint(stdout, &asn_DEF_BCCH_DL_SCH_Message, bch_msg);  
    system("pause");
	return 0; /* Encoding finished successfully */
}

