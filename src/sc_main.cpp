#include <systemc.h>
#include "cpu.h"
#include "dma_controller.h"

class Top : public sc_module
{
public:
	CPU cpu;
	DMAController dmaController;

public:
	SC_HAS_PROCESS(Top);

	Top(sc_module_name name)
	    : sc_module(name)
		, cpu("cpu")
		, dmaController("dmaController")
   	{
	   	//bind
	   	cpu.cpu_init_sckt.bind(dmaController.dma_target_sckt);
	}
};

int sc_main(int argc, char* argv[])
{
	Top top("top");
	sc_start();
	return 0 ;
}
