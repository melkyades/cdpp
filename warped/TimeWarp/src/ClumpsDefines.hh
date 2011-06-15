#ifndef CLUMPS_DEFINES_HH
#define CLUMPS_DEFINES_HH

/********   flags ALWAYS applicable  ***********/
#define MAX_MSG_SIZE            768


/********  ICM flags  ***********/
#define RECV_BUF_SIZE           15000
#define SEND_BUF_SIZE           20000

/********  CCM flags  ***********/
#define CCM_MAX_SEND_LIMIT      5
#define CCM_MAX_RECV_LIMIT      200


/********  DPC flags  ***********/
#define   MAX_RND_VAL            4000
#define   DPC_SEM_NUM            1024
#define   DPC_SEM_MAG_FACTOR     1
#define   DPC_SEM_INIT_VAL       1
#define   DPC_MAX_TW_INIT_MSGS      100
#define   DPC_INFREQ_RETR_FILE   "DPC_infreq_retr_infile"


/********  AGGR_ISCM flags  ***********/
#define MAX_AGG_SIZE            65536
#define MAX_MSGS_IN_AGG         20


// checking for illegal or incomplete flag defintions
#ifdef CLUMPS_ICM

#ifndef _REENTRANT
#error "_REENTRANT not defined. This is needed for multithreading"
#endif

#if defined(CLUMPS_ICM_IRCM) && defined(CLUMPS_ICM_ISCM)
#error "CLUMPS_ICM_IRCM & CLUMPS_ICM_ISCM defined. These flags are exclusive"
#endif

#endif

#endif
