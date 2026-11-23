#ifndef CPU_H
#define CPU_H

#include<systemc>
#include<tlm.h>
#include<tlm_utils/simple_initiator_socket.h>

using namespace sc_core;
using namespace sc_dt;

#define SRC_ADDR_REG_OFFSET    0x00
#define DST_ADDR_REG_OFFSET    0x04
#define TRANSFER_SZ_REG_OFFSET 0x08
#define BUS_SZ_REG_OFFSET      0x0C
#define CONTROL_REG_OFFSET     0x10
#define STATUS_REG_OFFSET      0x14
#define INT_STATUS_REG_OFFSET  0x18

class CPU : public sc_module
{
public:
  tlm_utils::simple_initiator_socket<CPU> cpu_init_mem_sckt;
  tlm_utils::simple_initiator_socket<CPU> cpu_init_dma_sckt;

public:
  SC_HAS_PROCESS(CPU);
  CPU(sc_module_name name);

  void run_read_write_test();
 
  //CPU to memory
  void transaction_read_memory(uint32_t address,int d_len,uint32_t value,tlm::tlm_command cmd);
  void transaction_write_memory(uint32_t address,uint32_t data,tlm::tlm_command cmd);

  //CPU to DMA
  void write_dma_reg(uint32_t address,unsigned int data,tlm::tlm_command cmd);
  void read_dma_reg(uint32_t address, unsigned int value,tlm::tlm_command cmd);
};

#endif