#ifndef CPU_H
#define CPU_H

#include<systemc>
#include<tlm.h>
#include<tlm_utils/simple_initiator_socket.h>

using namespace sc_core;
using namespace sc_dt;

class CPU : public sc_module
{
public:
  tlm_utils::simple_initiator_socket<CPU> cpu_init_sckt;

public:
  SC_HAS_PROCESS(CPU);
  CPU(sc_module_name name);

  void run_read_write_test();

  void transaction_for_write(uint32_t address,unsigned int data,tlm::tlm_command cmd);
  void transaction_for_read(uint32_t address, unsigned int value,tlm::tlm_command cmd);
};

#endif