#include "stdint.h"
#include <stdio.h> 
//#include <lwip/err.h>

extern uint16_t tcp_test_stat;
//err_t tcptest_init(void);
uint32_t tcptest_generate_request(uint8_t *txbuff);

#pragma pack(1)
typedef struct sensors_state_t4
{

  uint8_t key12;
  float rarh10;
   uint8_t key13;
  float radb10;
   uint8_t key10;
  float sarh10;
   uint8_t key11;
  float sadb10;
   uint8_t key8;
  float oarh10;
   uint8_t key9;
  float oadb10;
   uint8_t key16;
  float sltc11;
   uint8_t key21;
  float sltc21;
   uint8_t key26;
  float sltc31;
   uint8_t key31;
  float sltc41;
   uint8_t key17;
  float lltc11;
   uint8_t key22;
  float lltc21;
   uint8_t key27;
  float lltc31;
   uint8_t key32;
  float lltc41;
   uint8_t key14;
  float spc11;
   uint8_t key19;
  float spc21;
   uint8_t key24;
  float spc31;
   uint8_t key29;
  float spc41;
   uint8_t key15;
  float llpc11;
   uint8_t key20;
  float llpc21;
   uint8_t key25;
  float llpc31;
   uint8_t key30;
  float llpc41;
   uint8_t key3;
  float iUei1;
   uint8_t key4;
  float iUei2;
   uint8_t key5;
  float iUei3;
   uint8_t key0;
  float uUei1;
   uint8_t key1;
  float uUei2;
   uint8_t key2;
  float uUei3;
   uint8_t key6;
  float powerFactor;
   uint8_t key7;
  float activeWatts;
  uint8_t key34;
  float wire1_orange;
  uint8_t key35;
  float wire2_white;
  uint8_t key36;
  float wire3_yellow;
}sensors_state_t4;



#pragma pack(1)
typedef struct sensors_state_t3
{

  uint8_t key12;
  float rarh10;
   uint8_t key13;
  float radb10;
   uint8_t key10;
  float sarh10;
   uint8_t key11;
  float sadb10;
   uint8_t key8;
  float oarh10;
   uint8_t key9;
  float oadb10;
   uint8_t key16;
  float sltc11;
   uint8_t key21;
  float sltc21;
   uint8_t key26;
  float sltc31;
   uint8_t key17;
  float lltc11;
   uint8_t key22;
  float lltc21;
   uint8_t key27;
  float lltc31;
   uint8_t key14;
  float spc11;
   uint8_t key19;
  float spc21;
   uint8_t key24;
  float spc31;
   uint8_t key15;
  float llpc11;
   uint8_t key20;
  float llpc21;
   uint8_t key25;
  float llpc31;
   uint8_t key3;
  float iUei1;
   uint8_t key4;
  float iUei2;
   uint8_t key5;
  float iUei3;
   uint8_t key0;
  float uUei1;
   uint8_t key1;
  float uUei2;
   uint8_t key2;
  float uUei3;
   uint8_t key6;
  float powerFactor;
   uint8_t key7;
  float activeWatts;
  uint8_t key34;
  float wire1_orange;
  uint8_t key35;
  float wire2_white;
  uint8_t key36;
  float wire3_yellow;
}sensors_state_t3;

#pragma pack(1)
typedef struct sensors_state_t2
{

  uint8_t key12;
  float rarh10;
   uint8_t key13;
  float radb10;
   uint8_t key10;
  float sarh10;
   uint8_t key11;
  float sadb10;
   uint8_t key8;
  float oarh10;
   uint8_t key9;
  float oadb10;
   uint8_t key16;
  float sltc11;
   uint8_t key21;
  float sltc21;
   uint8_t key17;
  float lltc11;
   uint8_t key22;
  float lltc21;
   uint8_t key14;
  float spc11;
   uint8_t key19;
  float spc21;
   uint8_t key15;
  float llpc11;
   uint8_t key20;
  float llpc21;
   uint8_t key3;
  float iUei1;
   uint8_t key4;
  float iUei2;
   uint8_t key5;
  float iUei3;
   uint8_t key0;
  float uUei1;
   uint8_t key1;
  float uUei2;
   uint8_t key2;
  float uUei3;
   uint8_t key6;
  float powerFactor;
   uint8_t key7;
  float activeWatts;
  uint8_t key34;
  float wire1_orange;
  uint8_t key35;
  float wire2_white;
  uint8_t key36;
  float wire3_yellow;
}sensors_state_t2;



#pragma pack(1)
typedef struct sensors_state_t1
{

  uint8_t key12;
  float rarh10;
   uint8_t key13;
  float radb10;
   uint8_t key10;
  float sarh10;
   uint8_t key11;
  float sadb10;
   uint8_t key8;
  float oarh10;
   uint8_t key9;
  float oadb10;
   uint8_t key16;
  float sltc11;
   uint8_t key17;
  float lltc11;
   uint8_t key14;
  float spc11;
   uint8_t key15;
  float llpc11;
   uint8_t key3;
  float iUei1;
   uint8_t key4;
  float iUei2;
   uint8_t key5;
  float iUei3;
   uint8_t key0;
  float uUei1;
   uint8_t key1;
  float uUei2;
   uint8_t key2;
  float uUei3;
   uint8_t key6;
  float powerFactor;
   uint8_t key7;
  float activeWatts;
  uint8_t key34;
  float wire1_orange;
  uint8_t key35;
  float wire2_white;
  uint8_t key36;
  float wire3_yellow;
}sensors_state_t1;






#define BLE_SENSORS_DESCRIPTOR_STR "rarh10;radb10;sarh10;sadb10;oarh10;oadb10;dlt10;dlt20;dlt30;dlt40;sltc11;sltc21;sltc31;sltc41;lltc11;lltc21;lltc31;lltc41;spc11;spc21;spc31;spc41;llpc11;llpc21;llpc31;llpc41;iUei1;iUei2;iUei3;uUei1;uUei2;uUei3;powerFactor;activeWatts;apparentWatts;reactiveWatts;atmpres;"

extern char sId[32];
extern sensors_state_t1 sensors_state1;
extern sensors_state_t2 sensors_state2;
extern sensors_state_t3 sensors_state3;
extern sensors_state_t4 sensors_state4;
