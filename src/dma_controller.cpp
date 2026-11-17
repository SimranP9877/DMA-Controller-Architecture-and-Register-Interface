#include "dma_controller.h"

DMAController::DMAController(sc_module_name name):sc_module(name),dma_target_sckt("dma_target_sckt")
{
  dma_target_sckt.register_b_transport(this,&DMAController::b_transport);
}

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
    dma_transaction_write(adr,trans);
  }
  else if(cmd == tlm::TLM_READ_COMMAND)
  {
    dma_transaction_read(adr,trans);
  }
  else
  {
    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  }
}


void DMAController::dma_transaction_write(uint32_t adr,tlm::tlm_generic_payload& trans)
{
    unsigned char*   ptr = trans.get_data_ptr();
    uint32_t value;
    switch(adr)
    {
        case 0x00: 
        //Source address
        std::memcpy(&value,ptr,sizeof(value));
        SRC_ADDR = value;
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout << "DMA: SRC_ADDR updated = 0x"<< std::hex << value << "\n";
        break;
    
        case 0x04:
        //Destination address
        std::memcpy(&value,ptr,sizeof(value));
        DST_ADDR = value;
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: DST_ADDR updated = 0x"<< std::hex << value << "\n";
        break;

        case 0x08:
        //Total bytes to transfer
        std::memcpy(&value,ptr,sizeof(value));
        TRANSFER_SIZE =value;
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: TRANSFER_SIZE updated =  0x"<< std::hex << value << "\n";
        break;

        case 0x0C:
        //Bytes per burst
        std::memcpy(&value,ptr,sizeof(value));
        BURST_SIZE = value;
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: BURST_SIZE updated = 0x"<< std::hex << value << "\n";
        break;

        case 0x10:
        //DMA control
        std::memcpy(&value,ptr,sizeof(value));
        CONTROL = value;
        if (CONTROL & (1u << 0))
        {
            std::cout<<"DMA: START requested"<<"\n";
        }
        if (CONTROL & (1u << 1))
        {
            std::cout<<"DMA: RESET requested"<<"\n";
        }
        if (CONTROL & (1u << 2))
        {
            std::cout<<"DMA: INTERRUPT_ENABLE enabled"<<"\n";
        }
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        break;

        case 0x18:
        //Bytes per burst
        std::memcpy(&value,ptr,sizeof(value));
        INT_STATUS = value;
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: INT_STATUS updated = 0x"<< std::hex << value << "\n";
        break;

        case 0x14:
        //DMA status
        std::cout<<"DMA: Invalid "<<"\n";
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        break;


        default:
        std::cout<<"DMA: Invalid register address"<<"\n";
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        break;
    }
       
}

void DMAController::dma_transaction_read(uint32_t adr,tlm::tlm_generic_payload& trans)
{
    
    switch(adr)
    {
        case 0x00: 
        {//Source address
        uint32_t value = SRC_ADDR;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: reading done"<<"\n";
        break;
        }
    
        case 0x04:
        {//Destination address
        uint32_t value = DST_ADDR;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: = reading done"<<"\n";
        break;
        }

        case 0x08:
        {//Total bytes to transfer
        uint32_t value = TRANSFER_SIZE;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: = reading done"<<"\n";
        break;
        }

        case 0x0C:
        {//Bytes per burst
        uint32_t value = BURST_SIZE;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        std::cout<<"DMA: = reading done"<<"\n";
        break;
        }

        case 0x10:
        {//DMA control
        uint32_t value = CONTROL;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        break;
        }

        case 0x14:
        {// DMA status
        uint32_t value = STATUS;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        break;
        }

        case 0x18:
        {// interrupt status
        uint32_t value = INT_STATUS;
        std::memcpy(trans.get_data_ptr(),&value,sizeof(value));
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        break;
        }


        default:
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        break;
    }
       
}
