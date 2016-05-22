/* Test Suite for libxmodem */

#include <stdint.h>
#include <iostream>
#include "XModemTests.h"



static bool is_inbound_empty()
{
   return inbound_empty;
}

static bool is_outbound_full()
{
   return outbound_full;
}

static bool read_data(const uint32_t requested_size, uint8_t *buffer, uint32_t *returned_size)
{
   requested_inbound_size = requested_size;
   memcpy(buffer, inbound_buffer, requested_size); // maybe swap requested_size with returned_size
   *returned_size = returned_inbound_size;
   return result_inbound_buffer;
}

static bool write_data(const uint32_t requested_size, uint8_t *buffer, uint32_t *returned_size)
{
   requested_outbound_size = requested_size;
   memcpy(outbound_buffer, buffer, requested_size);
   *returned_size = returned_outbound_size;
   return result_outbound_buffer;
}

TEST_F(XModemTests, XMODEM_VERIFY_PACKET)
{
   //bool xmodem_verify_packet(const xmodem_packet_t packet, uint8_t expected_packet_id)
}
  

TEST_F(XModemTests, XMODEM_TRANSMIT_TIMEOUT_WAIT_WRITE_BLOCK)
{
  
  EXPECT_EQ(false, xmodem_transmit_init(buffer, BUFFER_SIZE));
  EXPECT_EQ(XMODEM_TRANSMIT_UNKNOWN, xmodem_transmit_state());

  xmodem_set_callback_write(&write_data);
  xmodem_set_callback_read(&read_data);
  xmodem_set_callback_is_outbound_full(&is_outbound_full);
  xmodem_set_callback_is_inbound_empty(&is_inbound_empty);

  EXPECT_EQ(true, xmodem_transmit_init(buffer, BUFFER_SIZE));
  EXPECT_EQ(XMODEM_TRANSMIT_INITIAL, xmodem_transmit_state());

  for (int i = 0; i < 20; ++i)
  {  
    EXPECT_EQ(true, xmodem_transmit_process(0));
    EXPECT_EQ(XMODEM_TRANSMIT_WAIT_FOR_C, xmodem_transmit_state());
  }

  returned_inbound_size = 1; 
  inbound_buffer[0] = C;

  // attempt to send a SOH control character, but the outbound buffer is full 
  returned_outbound_size = 0;
  EXPECT_EQ(true, xmodem_transmit_process(1));
  EXPECT_EQ(XMODEM_TRANSMIT_WRITE_BLOCK, xmodem_transmit_state());

  EXPECT_EQ(true, xmodem_transmit_process(2));
 
  xmodem_cleanup(); 

}


TEST_F(XModemTests, XMODEM_TRANSMIT_WRITE_BLOCK)
{
  
  EXPECT_EQ(false, xmodem_transmit_init(buffer, BUFFER_SIZE));
  EXPECT_EQ(XMODEM_TRANSMIT_UNKNOWN, xmodem_transmit_state());

  xmodem_set_callback_write(&write_data);
  xmodem_set_callback_read(&read_data);
  xmodem_set_callback_is_outbound_full(&is_outbound_full);
  xmodem_set_callback_is_inbound_empty(&is_inbound_empty);

  EXPECT_EQ(true, xmodem_transmit_init(buffer, BUFFER_SIZE));
  EXPECT_EQ(XMODEM_TRANSMIT_INITIAL, xmodem_transmit_state());

  for (int i = 0; i < 20; ++i)
  {  
    EXPECT_EQ(true, xmodem_transmit_process(0));
    EXPECT_EQ(XMODEM_TRANSMIT_WAIT_FOR_C, xmodem_transmit_state());
  }

  returned_inbound_size  = 1; 
  inbound_buffer[0]      = C;
  outbound_full          = false;
  inbound_empty          = false;
  returned_outbound_size = 1;
  timer                  = 1;

  EXPECT_EQ(XMODEM_TRANSMIT_WAIT_FOR_C, xmodem_transmit_state());  
  EXPECT_EQ(true, xmodem_transmit_process(0));
  EXPECT_EQ(XMODEM_TRANSMIT_WRITE_BLOCK, xmodem_transmit_state());
  EXPECT_EQ(true, xmodem_transmit_process(0));
  EXPECT_EQ(XMODEM_TRANSMIT_WRITE_BLOCK, xmodem_transmit_state());
  EXPECT_EQ(outbound_buffer[0], SOH);
  EXPECT_EQ(outbound_buffer[1], packet_number);
  EXPECT_EQ(outbound_buffer[2], 0xFF - packet_number);
  EXPECT_EQ(0, memcmp(outbound_buffer+3, buffer+buffer_position, XMODEM_BLOCK_SIZE));


#if 0
  EXPECT_EQ(true, xmodem_transmit_process(timer));
  ++timer;
  EXPECT_EQ(outbound_buffer[0], SOH);
  EXPECT_EQ(outbound_buffer[1], packet_number);
  EXPECT_EQ(outbound_buffer[2], 0xFF - packet_number);
  EXPECT_EQ(0, memcmp(outbound_buffer+3, buffer+buffer_position, 128));

  // clear outbound buffer on each iteration
  memset(outbound_buffer, 0, OUTBOUND_BUFFER_SIZE);

  EXPECT_EQ(XMODEM_TRANSMIT_COMPLETE, xmodem_transmit_state());
#endif
  xmodem_cleanup(); 

}

TEST_F(XModemTests, XMODEM_TRANSMIT_WRITE_DOCUMENT)
{
  
  EXPECT_EQ(false, xmodem_transmit_init(buffer, BUFFER_SIZE));
  EXPECT_EQ(XMODEM_TRANSMIT_UNKNOWN, xmodem_transmit_state());

  xmodem_set_callback_write(&write_data);
  xmodem_set_callback_read(&read_data);
  xmodem_set_callback_is_outbound_full(&is_outbound_full);
  xmodem_set_callback_is_inbound_empty(&is_inbound_empty);

  EXPECT_EQ(true, xmodem_transmit_init(buffer, BUFFER_SIZE));
  EXPECT_EQ(XMODEM_TRANSMIT_INITIAL, xmodem_transmit_state());

  for (int i = 0; i < 20; ++i)
  {  
    EXPECT_EQ(true, xmodem_transmit_process(0));
    EXPECT_EQ(XMODEM_TRANSMIT_WAIT_FOR_C, xmodem_transmit_state());
  }

  returned_inbound_size  = 1; 
  inbound_buffer[0]      = C;
  outbound_full          = false;
  inbound_empty          = false;
  result_outbound_buffer = true;
  result_inbound_buffer  = true;
  returned_outbound_size = sizeof(xmodem_packet_t);
  timer                  = 1;

  EXPECT_EQ(true, xmodem_transmit_process(0));
  EXPECT_EQ(XMODEM_TRANSMIT_WRITE_BLOCK, xmodem_transmit_state());

 
     EXPECT_EQ(true, xmodem_transmit_process(timer));

  while (xmodem_transmit_state() != XMODEM_TRANSMIT_COMPLETE &&
         xmodem_transmit_state() != XMODEM_TRANSMIT_ABORT)
  {
     ++timer;
     EXPECT_EQ(outbound_buffer[0], SOH);
     EXPECT_EQ(outbound_buffer[1], packet_number);
     EXPECT_EQ(outbound_buffer[2], 0xFF - packet_number);
     EXPECT_EQ(0, memcmp(outbound_buffer+3, buffer+buffer_position, 1));
     buffer_position = buffer_position + XMODEM_BLOCK_SIZE;

     // clear outbound buffer on each iteration
     memset(outbound_buffer, 0, OUTBOUND_BUFFER_SIZE);
     ++packet_number;

     EXPECT_EQ(true, xmodem_transmit_process(timer));
  }

  EXPECT_EQ(true, xmodem_transmit_process(timer));
  EXPECT_EQ(XMODEM_TRANSMIT_COMPLETE, xmodem_transmit_state());
  EXPECT_EQ(outbound_buffer[0], EOT); 
  xmodem_cleanup(); 

}



#if 0

TEST_F(XModemTests, XMODEM_TIMEOUT_TRANSFER_ACK)
{

  EXPECT_EQ(false, xmodem_init());
  EXPECT_EQ(XMODEM_UNKNOWN, xmodem_state());

  xmodem_set_callback_write(&write_data);
  xmodem_set_callback_read(&read_data);
  xmodem_set_callback_is_outbound_full(&is_outbound_full);
  xmodem_set_callback_is_inbound_empty(&is_inbound_empty);

  EXPECT_EQ(true, xmodem_init());
  EXPECT_EQ(XMODEM_INITIAL, xmodem_state());

  for (int i = 0; i < 20; ++i)
  {  
    EXPECT_EQ(true, xmodem_process(0));
    EXPECT_EQ(XMODEM_WAIT_FOR_NACK, xmodem_state());
  }

  returned_inbound_size = 1; 
  inbound_buffer[0] = NACK;

   
  // attempt to send a SOH control character, but the outbound buffer is full 
  returned_outbound_size = 0;
  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_SEND_REQUEST_FOR_TRANSFER, xmodem_state());

  // check that a SOH control character is sent to the receiver
  returned_outbound_size = 1;
  outbound_full = false;
  tmp = SOH;
  
  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(0, memcmp(outbound_buffer, &tmp, 1));

  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());

  EXPECT_EQ(true, xmodem_process(2));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(3));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(59000));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(60001));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(60002));
  EXPECT_EQ(XMODEM_TIMEOUT_TRANSFER_ACK, xmodem_state());

  EXPECT_EQ(true, xmodem_process(60003));
  EXPECT_EQ(XMODEM_ABORT_TRANSFER, xmodem_state());
  EXPECT_EQ(true, xmodem_process(60004));
 
  tmp = CAN; 
  EXPECT_EQ(0, memcmp(outbound_buffer, &tmp, 1));
  EXPECT_EQ(true, xmodem_cleanup());

}



TEST_F(XModemTests, XMODEM_TIMEOUT_WAIT_READ_BLOCK)
{
  xmodem_set_callback_write(&write_data);
  xmodem_set_callback_read(&read_data);
  xmodem_set_callback_is_outbound_full(&is_outbound_full);
  xmodem_set_callback_is_inbound_empty(&is_inbound_empty);

  EXPECT_EQ(true, xmodem_init());
  EXPECT_EQ(XMODEM_INITIAL, xmodem_state());

  EXPECT_EQ(true, xmodem_process(0));
  EXPECT_EQ(XMODEM_WAIT_FOR_NACK, xmodem_state());

  returned_inbound_size = 1; 
  inbound_buffer[0] = NACK;

  EXPECT_EQ(true, xmodem_process(0));
  EXPECT_EQ(XMODEM_SEND_REQUEST_FOR_TRANSFER, xmodem_state());

  // check that a SOH control character is sent to the receiver
  returned_outbound_size = 1;
  outbound_full = false;
  tmp = SOH;
  
  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(0, memcmp(outbound_buffer, &tmp, 1));

  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(3));

  /* setup mock receive buffer */
  inbound_empty         = false; 
  result_inbound_buffer = true;
  returned_inbound_size = 1;
  inbound_buffer[0]     = ACK;
  
  EXPECT_EQ(true, xmodem_process(3));
  EXPECT_EQ(XMODEM_TRANSFER_ACK_RECEIVED, xmodem_state());
  EXPECT_EQ(true, xmodem_process(3));
  EXPECT_EQ(XMODEM_READ_BLOCK, xmodem_state());   
  EXPECT_EQ(true, xmodem_process(60001));
  EXPECT_EQ(XMODEM_READ_BLOCK, xmodem_state());   
  EXPECT_EQ(true, xmodem_process(60002));
  EXPECT_EQ(XMODEM_TIMEOUT_WAIT_READ_BLOCK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(60004));
  EXPECT_EQ(XMODEM_ABORT_TRANSFER, xmodem_state());  
  EXPECT_EQ(true, xmodem_process(60005));
 
  tmp = CAN; 
  EXPECT_EQ(0, memcmp(outbound_buffer, &tmp, 1));
  EXPECT_EQ(true, xmodem_cleanup());

}

#endif


#if 0

TEST_F(XModemTests, XMODEM_SUCCESS_READ_BLOCK)
{
  xmodem_set_callback_write(&write_data);
  xmodem_set_callback_read(&read_data);
  xmodem_set_callback_is_outbound_full(&is_outbound_full);
  xmodem_set_callback_is_inbound_empty(&is_inbound_empty);

  EXPECT_EQ(true, xmodem_init());
  EXPECT_EQ(XMODEM_INITIAL, xmodem_state());

  EXPECT_EQ(true, xmodem_process(0));
  EXPECT_EQ(XMODEM_WAIT_FOR_NACK, xmodem_state());

  returned_inbound_size = 1; 
  inbound_buffer[0] = NACK;

  EXPECT_EQ(true, xmodem_process(0));
  EXPECT_EQ(XMODEM_SEND_REQUEST_FOR_TRANSFER, xmodem_state());

  // check that a SOH control character is sent to the receiver
  returned_outbound_size = 1;
  outbound_full = false;
  tmp = SOH;
  
  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(0, memcmp(outbound_buffer, &tmp, 1));

  EXPECT_EQ(true, xmodem_process(1));
  EXPECT_EQ(XMODEM_WAIT_FOR_TRANSFER_ACK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(3));

  /* setup mock receive buffer */
  inbound_empty         = false; 
  result_inbound_buffer = true;
  returned_inbound_size = 1;
  inbound_buffer[0]     = ACK;
  
  EXPECT_EQ(true, xmodem_process(3));
  EXPECT_EQ(XMODEM_TRANSFER_ACK_RECEIVED, xmodem_state());
  EXPECT_EQ(true, xmodem_process(3));
  EXPECT_EQ(XMODEM_READ_BLOCK, xmodem_state());   
  EXPECT_EQ(true, xmodem_process(60001));
  EXPECT_EQ(XMODEM_READ_BLOCK, xmodem_state());   
  EXPECT_EQ(true, xmodem_process(60002));

#if 0
  EXPECT_EQ(XMODEM_TIMEOUT_WAIT_READ_BLOCK, xmodem_state());
  EXPECT_EQ(true, xmodem_process(60004));
  EXPECT_EQ(XMODEM_ABORT_TRANSFER, xmodem_state());  
  EXPECT_EQ(true, xmodem_process(60005));
 
  tmp = CAN; 
  EXPECT_EQ(0, memcmp(outbound_buffer, &tmp, 1));
#endif

  EXPECT_EQ(true, xmodem_cleanup());

}


#endif

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

void yyerror(char *s) {fprintf (stderr, "%s", s);}


