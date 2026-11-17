#include "cpu.h"
#include <iostream>


void CPU::run_read_write_test()
{
  //milestone1:
  transaction_for_write(0x00,0x100,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n"<<"\n";

  transaction_for_write(0x04,0x500,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n"<<"\n";

  transaction_for_write(0x08,16,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n"<<"\n";

  transaction_for_write(0x0C,4,tlm::TLM_WRITE_COMMAND);
  std::cout<<"\n"<<"\n";

  std::cout<<"CPU: Reading DMA registers"<<"\n";
  transaction_for_read(0x00,0x100,tlm::TLM_READ_COMMAND);
  std::cout<<"\n"<<"\n";

  std::cout<<"CPU: Testing invalid command "<<"\n";
  transaction_for_write(0x00,0x87,tlm::TLM_IGNORE_COMMAND);
  std::cout<<"\n"<<"\n";

  std::cout<<"CPU: Testing invalid register access"<<"\n";
  transaction_for_read(0x000987,0x987,tlm::TLM_READ_COMMAND);
  std::cout<<"\n"<<"\n";

  std::cout<<"CPU:Reading STATUS"<<"\n";
  transaction_for_read(0x14,0,tlm::TLM_READ_COMMAND);

  std::cout<<"\n"<<"\n";


}

void CPU::transaction_for_write(uint32_t address,uint32_t data,tlm::tlm_command cmd)
{
  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  //write
  trans.set_command(cmd);
  trans.set_address(address);
  trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));
  trans.set_data_length(sizeof(data));
  trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

  std::cout<<"CPU: Writing data = 0x"<<std::hex << data <<" to address =0x"<<std::hex << address<<"\n";
  
  cpu_init_sckt->b_transport(trans,delay);

  if(trans.is_response_error())
  {
    std::string error_msg ="Transcation failed with status: " +std::string(trans.get_response_string());
    std::cout<<"CPU: "  <<error_msg.c_str()<<"\n";
  }

}

void CPU::transaction_for_read(uint32_t address, uint32_t value,tlm::tlm_command cmd)
{
  uint32_t data{};

  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  trans.set_command(cmd);
  trans.set_address(address);
  trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));
  trans.set_data_length(sizeof(data));
  trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);


  cpu_init_sckt->b_transport(trans,delay);
  
  
  if (trans.is_response_error())
  {
    std::string error_msg = "Transaction failed with status: " + std::string(trans.get_response_string());
    std::cout<<"CPU :" << error_msg.c_str()<<"\n";

    if(trans.get_response_status() == tlm::TLM_ADDRESS_ERROR_RESPONSE || trans.get_response_status() == tlm::TLM_COMMAND_ERROR_RESPONSE) 
      return;
  } 

  if(address == 0x14)
  {
    if (data == 0)
    {
      std::cout<<"STATUS = IDLE"<<"\n";
    }
    else if (data == 1)
    {
      std::cout<<"STATUS = BUSY"<<"\n";
    }
    else if ( data == 2)
    {
      std::cout<<"STATUS = COMPLETE"<<"\n";
    }
    else 
    {
      std::cout<<"STATUS = ERROR"<<"\n";
    }

  }
  else
  {
  if (data == value )
  {
    std::cout<<"CPU: Register verification successful"<<"\n";
  }
  else
  {
    SC_REPORT_ERROR("CPU:", "Verification failed");
  }
  }

  
  

}





