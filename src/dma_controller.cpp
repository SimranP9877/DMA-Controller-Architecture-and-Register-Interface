#include "dma_controller.h"

DMAController::DMAController(sc_module_name name)
  : sc_module(name)
  , dma_cpu_target_sckt("dma_cpu_target_sckt")
  , dma_mem_init_sckt("dma_mem_init_sckt")
  , start_dma_event("start_dma_event")
{
  dma_cpu_target_sckt.register_b_transport(this,&DMAController::b_transport);
  SC_THREAD(dma_read_write_transaction);
}

//DMA func for cpu
void DMAController::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
  tlm::tlm_command cmd = trans.get_command();
  uint32_t         adr = trans.get_address();
  unsigned char*   ptr = trans.get_data_ptr();
  unsigned int     len = trans.get_data_length();

  if(len != 4)
  {
    trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
    return;
  }

  if(ptr == nullptr )
  {
    trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
	  return ;
  }

  if (trans.get_byte_enable_ptr() != nullptr)
  { 
    trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
    return;
  }

  if(cmd == tlm::TLM_WRITE_COMMAND)
  {
    dma_reg_write(adr,trans);
    delay += sc_time(DMA_REG_TRANS_DELAY, SC_NS);
  }
  else if(cmd == tlm::TLM_READ_COMMAND)
  {
    dma_reg_read(adr,trans);
    delay += sc_time(DMA_REG_TRANS_DELAY, SC_NS);
  }
  else
  {
    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  }
}


void DMAController::dma_reg_write(uint32_t adr,tlm::tlm_generic_payload& trans)
{
    unsigned char*   ptr = trans.get_data_ptr();
    uint32_t value;

    switch(adr)
    {
        case 0x00: 
        //Source address
        std::memcpy(&value,ptr,sizeof(value));
        SRC_ADDR_REG = value;
        std::cout <<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: SRC_ADDR_REG = 0x"<< std::hex << value << "\n";
        break;
    
        case 0x04:
        //Destination address
        std::memcpy(&value,ptr,sizeof(value));
        DST_ADDR_REG = value;
        std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: DST_ADDR_REG  = 0x"<< std::hex << value << "\n";
        break;

        case 0x08:
        //Total bytes to transfer
        std::memcpy(&value,ptr,sizeof(value));
        TRANSFER_SIZE_REG =value;
        std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: TRANSFER_SIZE_REG  = "<< value << "\n";
        break;

        case 0x0C:
        //Bytes per burst
        std::memcpy(&value,ptr,sizeof(value));
        BURST_SIZE_REG = value;
        std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: BURST_SIZE_REG  = "<< value << "\n";
        break;

        case 0x10:
        //DMA control
        std::memcpy(&value,ptr,sizeof(value));
        CONTROL_REG = value;
        //REset has a priority if both RESET and START come together
        if (value & RESET) 
        {
          std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: RESET requested"<<"\n";
          STATUS_REG = 0;
          INT_STATUS_REG =0;
          CONTROL_REG =0;

        }
        else 
        {
          if (value & START)
          {
            STATUS_REG = 1;
            start_dma_event.notify(SC_ZERO_TIME);
          }  

          if (value & INTERRUPT_ENABLE)
          {
            std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: INTERRUPT_ENABLE enabled"<<"\n";
          }
        }
        break;

        case 0x18:
        //Bytes per burst
        std::memcpy(&value,ptr,sizeof(value));
        INT_STATUS_REG = value;
        std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: INT_STATUS_REG updated = 0x"<< std::hex << value << "\n";
        break;

        case 0x14:
        //DMA status
        std::cout<<"DMA : Invalid status" <<"\n";
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;


        default:
        std::cout<<"DMA: Invalid register address"<<"\n";
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return;
    }

  trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

void DMAController::dma_reg_read(uint32_t adr,tlm::tlm_generic_payload& trans)
{
  unsigned char* ptr = trans.get_data_ptr();
    
  switch(adr)
  {
    case 0x00: 
    {//Source address
    uint32_t value = SRC_ADDR_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: reading done"<<"\n";
    break;
    }

    case 0x04:
    {//Destination address
    uint32_t value = DST_ADDR_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: = reading done"<<"\n";
    break;
    }

    case 0x08:
    {//Total bytes to transfer
    uint32_t value = TRANSFER_SIZE_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: = reading done"<<"\n";
    break;
    }

    case 0x0C:
    {//Bytes per burst
    uint32_t value = BURST_SIZE_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: = reading done"<<"\n";
    break;
    }

    case 0x10:
    {//DMA control
    uint32_t value = CONTROL_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: = reading done"<<"\n";
    break;
    }

    case 0x14:
    {// DMA status
    uint32_t value = STATUS_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA : STATUS = "<<value<<"\n";
    break;
    }

    case 0x18:
    {// interrupt status
    uint32_t value = INT_STATUS_REG;
    std::memcpy(ptr,&value,sizeof(value));
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    break;
    }

    default:
    trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
    break;
  }
}

//DMA to memory
void DMAController::dma_read_write_transaction()
{
  while(true)
  {
    wait(start_dma_event);

    if (TRANSFER_SIZE_REG != 4)
    {
      STATUS_REG = 3;
      std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA : STATUS = ERROR" <<"\n";
      return;
     }

    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: Starting DMA Transaction"<<"\n";

    STATUS_REG = 1;
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: STATUS = BUSY"<<"\n";

    unsigned char data[TRANSFER_SIZE_REG]{};

    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;

    //read
    trans.set_command(tlm::TLM_READ_COMMAND);
    trans.set_address(SRC_ADDR_REG);
    trans.set_data_ptr(data);
    trans.set_data_length(sizeof(data));
    trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA: Reading from memory address  0x"<<std::hex<< SRC_ADDR_REG<<"\n";
    dma_mem_init_sckt->b_transport(trans,delay);

    if (trans.is_response_error())
    {
      std::string error_msg = "Transaction failed with status: " + std::string(trans.get_response_string());
      std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA :" << error_msg.c_str()<<"\n";
      STATUS_REG = 3;
      std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA :" << "STATUS = ERROR"<<"\n";
      return;
    }

    //write
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(DST_ADDR_REG);
    trans.set_data_ptr(data);
    trans.set_data_length(sizeof(data));
    trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA:Writing to memory address 0x"<<std::hex<<DST_ADDR_REG<<"\n";
    dma_mem_init_sckt->b_transport(trans,delay);
    wait(delay);

    if (trans.is_response_error())
    {
      std::string error_msg = "Transaction failed with status: " + std::string(trans.get_response_string());
      STATUS_REG = 3;
      std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA :" << error_msg.c_str()<<"\n";
      std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA :" << "STATUS = ERROR"<<"\n";
      return;
    }
    
    STATUS_REG = 2;
    std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" DMA :" << "STATUS = COMPLETE"<<"\n";
  }
}