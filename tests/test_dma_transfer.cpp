#include "cpu.h"
#include <iostream>


void CPU::run_read_write_test()
{
  //milestone2:
  std::cout<<"\n";
  transaction_write_memory(0x100,0x12345678,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n"<<"\n";

  //milestone1:
  std::cout<<"CPU: configuring DMA"<<"\n";
  write_dma_reg(SRC_ADDR_REG_OFFSET,0x100,tlm::TLM_WRITE_COMMAND);
  write_dma_reg(DST_ADDR_REG_OFFSET,0x300,tlm::TLM_WRITE_COMMAND);
  write_dma_reg(TRANSFER_SZ_REG_OFFSET,4,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n"<<"\n";

  //milestone2
  std::cout<<"CPU: starting DMA"<<"\n";
  write_dma_reg(CONTROL_REG_OFFSET,1,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n";
  
  //Verify destination data
  transaction_read_memory(0x300,4,0x12345678,tlm::TLM_READ_COMMAND);

  std::cout<<"current status"<<"\n";
  read_dma_reg(STATUS_REG_OFFSET,0,tlm::TLM_READ_COMMAND);
  std::cout<<"\n"<<"\n";

  std::cout<<"CPU: Testing invalid command "<<"\n";
  write_dma_reg(0x00,0x87,tlm::TLM_IGNORE_COMMAND);
  std::cout<<"\n"<<"\n";

  std::cout<<"CPU: Testing invalid register access"<<"\n";
  read_dma_reg(0x000987,0x987,tlm::TLM_READ_COMMAND);
  std::cout<<"\n"<<"\n";

  // std::cout<<"Testing invalid status"<<"\n";
  // read_dma_reg(STATUS_REG_OFFSET,0,tlm::TLM_READ_COMMAND);
  // std::cout<<"\n"<<"\n";

  std::cout<<"\n"<<"\n";
}

void CPU::transaction_write_memory(uint32_t address,uint32_t data,tlm::tlm_command cmd)
{
  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  //write
  trans.set_command(cmd);
  trans.set_address(address);
  trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));
  trans.set_data_length(sizeof(data));
  trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

  std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<"  CPU: Initializing source memory"<<"\n";
  cpu_init_mem_sckt->b_transport(trans,delay);
  wait(delay);

  if(trans.is_response_error())
  {
    std::string error_msg ="Transcation failed with status: " +std::string(trans.get_response_string());
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU: "  <<error_msg.c_str()<<"\n";
  }
}

void CPU::transaction_read_memory(uint32_t address,int d_len,uint32_t value,tlm::tlm_command cmd)
{
  unsigned char data[d_len]{};

  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  trans.set_command(cmd);
  trans.set_address(address);
  trans.set_data_ptr(data);
  trans.set_data_length(sizeof(data));
  trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

  std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU: Reading destination memory"<<"\n";
  cpu_init_mem_sckt->b_transport(trans,delay);
  wait(delay);

  if (trans.is_response_error())
  {
    std::string error_msg = "Transaction failed with status: " + std::string(trans.get_response_string());
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU :" << error_msg.c_str()<<"\n";
    return;
  } 
  
  if(d_len == 0 )
  {
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU : Cant verify"<<"\n";
  }
  else if (std::memcmp(data,&value, d_len) == 0)
  {
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU : Verification successful"<<"\n";
  }
  else
  {
    SC_REPORT_ERROR("TLM-2", "data verification failed");
  }
}

void CPU::write_dma_reg(uint32_t address,uint32_t data,tlm::tlm_command cmd)
{
  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  //write
  trans.set_command(cmd);
  trans.set_address(address);
  trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));
  trans.set_data_length(sizeof(data));
  trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
 
  cpu_init_dma_sckt->b_transport(trans,delay);
  wait(delay);

  if(trans.is_response_error())
  {
    std::string error_msg ="Transcation failed with status: " +std::string(trans.get_response_string());
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU: "  <<error_msg.c_str()<<"\n";
  }
}

void CPU::read_dma_reg(uint32_t address, uint32_t value,tlm::tlm_command cmd)
{
  uint32_t data{};

  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  trans.set_command(cmd);
  trans.set_address(address);
  trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));
  trans.set_data_length(sizeof(data));
  trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

  cpu_init_dma_sckt->b_transport(trans,delay);
  wait(delay);
  
  if (trans.is_response_error())
  {
    std::string error_msg = "Transaction failed with status: " + std::string(trans.get_response_string());
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" CPU :" << error_msg.c_str()<<"\n";
  } 
}
  






