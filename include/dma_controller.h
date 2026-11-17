#ifndef DMA_CONTROLLER_H
#define DMA_CONTROLLER_H

#include <systemc>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace sc_dt;

class DMAController : public sc_module
{   
public:
  tlm_utils::simple_target_socket<DMAController> dma_target_sckt;

private:
  //memory mapped registers
  uint32_t SRC_ADDR{};
  uint32_t DST_ADDR{};
  uint32_t TRANSFER_SIZE{};
  uint32_t BURST_SIZE{};
  uint32_t CONTROL{};
  uint32_t STATUS{};
  uint32_t INT_STATUS{};

public:
  SC_HAS_PROCESS(DMAController);
  DMAController(sc_module_name name);
  void b_transport(tlm::tlm_generic_payload& trans,sc_time& delay);
  void dma_transaction_write(uint32_t adr,tlm::tlm_generic_payload& trans);
  void dma_transaction_read(uint32_t adr,tlm::tlm_generic_payload& trans);
};

#endif

