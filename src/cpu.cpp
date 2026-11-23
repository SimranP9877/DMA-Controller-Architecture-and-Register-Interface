#include "cpu.h"

CPU::CPU(sc_module_name name)
  : sc_module(name)
  , cpu_init_mem_sckt("cpu_init_mem_sckt")
  , cpu_init_dma_sckt("cpu_init_dma_sckt")
{
    SC_THREAD(run_read_write_test);
}