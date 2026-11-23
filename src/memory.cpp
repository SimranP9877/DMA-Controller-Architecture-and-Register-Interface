#include "memory.h"

Memory ::Memory(sc_module_name name)
  : sc_module(name)
  , memory_target_cpu_sckt("memory_target_cpu_sckt")
  , memory_target_dma_sckt("memory_target_dma_sckt")
{
    memory_target_cpu_sckt.register_b_transport(this,&Memory::b_transport);
	memory_target_dma_sckt.register_b_transport(this,&Memory::b_transport);  
}


void Memory::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
	tlm::tlm_command cmd = trans.get_command();
	uint32_t         adr = trans.get_address();
	unsigned char*   ptr   = trans.get_data_ptr();
	unsigned int     len   = trans.get_data_length();

    if(len == 0  || len > SIZE )
	{
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return ;
	}

	if(ptr == nullptr || adr >= SIZE || len > SIZE- adr)
	{
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		std::cout << "I  addr: " << adr << " size:" << SIZE << " len:" << len << "\n";
		return ;
	}

	if (trans.get_byte_enable_ptr() != nullptr)
    {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		return;
    }

	if(cmd == tlm::TLM_WRITE_COMMAND)
	{
		std::memcpy(&mem[adr],ptr,len);
		delay += sc_time(MEMORY_RD_WT_DELAY, SC_NS);
		uint32_t value = *reinterpret_cast<uint32_t*>(&mem[adr]);
		std::cout<<sc_time_stamp() <<":" <<sc_delta_count() <<" Memory : Write  at 0x"<<std::hex<< adr << " = 0x"<< std::hex<<value << "\n"<<"\n";
	}
	else if(cmd == tlm::TLM_READ_COMMAND)
	{
		std::memcpy(ptr, &mem[adr], len);	
		delay += sc_time(MEMORY_RD_WT_DELAY, SC_NS);
       	std::cout<<sc_time_stamp() <<":" <<sc_delta_count() << " Memory: Read  successful" <<"\n"<<"\n";
	}
	else
	{
		trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		return;
	}
			
	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}