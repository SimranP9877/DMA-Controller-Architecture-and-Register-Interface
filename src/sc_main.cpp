#include <systemc.h>
#include "cpu.h"
#include "dma_controller.h"
#include "memory.h"

class Top : public sc_module
{
public:
	CPU cpu;
	DMAController dmaController;
	Memory memory;

public:
	SC_HAS_PROCESS(Top);

	Top(sc_module_name name)
	    : sc_module(name)
		, cpu("cpu")
		, memory("memory")
		, dmaController("dmaController")
   	{
	   	//bind
		cpu.cpu_init_mem_sckt.bind(memory.memory_target_cpu_sckt);
	   	cpu.cpu_init_dma_sckt.bind(dmaController.dma_cpu_target_sckt);
		dmaController.dma_mem_init_sckt.bind(memory.memory_target_dma_sckt);
	}
};

int sc_main(int argc, char* argv[])
{
	Top top("top");
	sc_start();
	return 0 ;
}
