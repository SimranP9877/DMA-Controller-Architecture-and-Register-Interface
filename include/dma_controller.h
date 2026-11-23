#ifndef DMA_CONTROLLER_H
#define DMA_CONTROLLER_H

#include <systemc>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;
using namespace sc_dt;

#define DMA_REG_TRANS_DELAY 10

class DMAController : public sc_module
{   
public:
  tlm_utils::simple_target_socket<DMAController> dma_cpu_target_sckt;
  tlm_utils::simple_initiator_socket<DMAController> dma_mem_init_sckt;

private:
  //memory mapped registers
  uint32_t SRC_ADDR_REG{};
  uint32_t DST_ADDR_REG{};
  uint32_t TRANSFER_SIZE_REG{};
  uint32_t BURST_SIZE_REG{};
  uint32_t CONTROL_REG{};
  uint32_t STATUS_REG{};
  uint32_t INT_STATUS_REG{};

  //control
  unsigned int START            = (1u << 0);
  unsigned int RESET            = (1u << 1);
  unsigned int INTERRUPT_ENABLE = (1u << 2);
  
  //event
  sc_event start_dma_event;

public:
  SC_HAS_PROCESS(DMAController);
  DMAController(sc_module_name name);

  //dma target to cpu
  void b_transport(tlm::tlm_generic_payload& trans,sc_time& delay);
  
  //dma to memory
  void dma_reg_write(uint32_t adr,tlm::tlm_generic_payload& trans);
  void dma_reg_read(uint32_t adr,tlm::tlm_generic_payload& trans);
  
  void dma_read_write_transaction();
};

#endif

