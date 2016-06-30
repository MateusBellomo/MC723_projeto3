/******************************************************
 * This is the main file for the mips ArchC model     *
 * This file is automatically generated by ArchC      *
 * WITHOUT WARRANTY OF ANY KIND, either express       *
 * or implied.                                        *
 * For more information on ArchC, please visit:       *
 * http://www.archc.org                               *
 *                                                    *
 * The ArchC Team                                     *
 * Computer Systems Laboratory (LSC)                  *
 * IC-UNICAMP                                         *
 * http://www.lsc.ic.unicamp.br                       *
 ******************************************************/


const char *project_name="mips";
const char *project_file="mips.ac";
const char *archc_version="2.2";
const char *archc_options="-abi ";

#include  <iostream>
#include  <systemc.h>
#include  "ac_stats_base.H"
#include  "mips.H"


#include  "ac_tlm_mem.h"
#include  "ac_tlm_router.h"
#include  "ac_tlm_fpu.h"
#include  "ac_tlm_lock.h"

using user::ac_tlm_mem;
using user::ac_tlm_router;
using user::ac_tlm_fpu;
using user::ac_tlm_lock;

int sc_main(int ac, char *av[])
{
  if (argc < 5) {
    printf("Usage %s image_file preset number_of_procs accelerated\n", av[0]);
    return 1;
  }

  char filename[32];
  int preset;
  int nproc;
  int accel;

  FILE *parameterFile;
  parameterFile = fopen(av[1], "r");
  sscanf(av[1], "%s", filename);
  sscanf(av[2], "%d", &preset);
  sscanf(av[3], "%d", &nproc);
  sscanf(av[4], "%d", &accel);
  fclose(parameterFile);

  printf("filename: %s\n", filename);
  printf("preset: %d\n", preset);
  printf("number of processors: %d\n", nproc);
  printf("hardware accelerated: %d\n", accel);

  int argc = 6;
  char str_preset[10];
  char str_nproc[10];
  sprintf(str_preset, "%d", preset);
  sprintf(str_nproc, "%d", nproc);

  char *av1[] = {"mips.x", "--load=mandel.mips", filename, str_preset, "1", str_nproc};
  char *av2[] = {"mips.x", "--load=mandel.mips", filename, str_preset, "2", str_nproc};
  char *av3[] = {"mips.x", "--load=mandel.mips", filename, str_preset, "3", str_nproc};
  char *av4[] = {"mips.x", "--load=mandel.mips", filename, str_preset, "4", str_nproc};

  char *av1_h[] = {"mips.x", "--load=mandel_hard_1.mips", file, str_preset, "1", str_nproc};
  char *av2_h[] = {"mips.x", "--load=mandel_hard_2.mips", file, str_preset, "2", str_nproc};
  char *av3_h[] = {"mips.x", "--load=mandel_hard_3.mips", file, str_preset, "3", str_nproc};
  char *av4_h[] = {"mips.x", "--load=mandel_hard_4.mips", file, str_preset, "4", str_nproc};


  //!  ISA simulator
  mips mips_proc1("mips");
  mips *mips_proc2, *mips_proc3, *mips_proc4;
  if(nproc > 1){
    mips_proc2 = new mips("mips2");
  }
  if(nproc > 2){
    mips_proc3 = new mips("mips3");
    mips_proc4 = new mips("mips4");
  }

  ac_tlm_mem mem("mem", 100*1024*1024);
  ac_tlm_router router("router");
  ac_tlm_fpu fpu("fpu");
  ac_tlm_lock lock("lock");

  router.MEM_port(mem.target_export);
  router.FPU_port(fpu.target_export);
  router.LOCK_port(lock.target_export);

  mips_proc1.DM_port(router.target_export);
  if(nproc > 1){
    mips_proc2->DM_port(router.target_export);
  }
  if(nproc > 2){
    mips_proc3->DM_port(router.target_export);
    mips_proc4->DM_port(router.target_export);
  }



#ifdef AC_DEBUG
  ac_trace("mips_proc1.trace");
#endif

  if (accel == 1) { // Hardware Accelerated
    mips_proc1.init(argc, av1_h);
    if(nproc > 1){
      mips_proc2->init(argc, av2_h);
    }
    if(nproc > 2){
      mips_proc3->init(argc, av3_h);
      mips_proc4->init(argc, av4_h);
    }
  } else { // Non Hardware Accelerated
    mips_proc1.init(argc, av1);
    if(nproc > 1){
      mips_proc2->init(argc, av2);
    }
    if(nproc > 2){
      mips_proc3->init(argc, av3);
      mips_proc4->init(argc, av4);
    }
  }

  mips_proc1.set_instr_batch_size(1);
  if(nproc > 1){
    mips_proc2->set_instr_batch_size(1);
  }
  if(nproc > 2){
    mips_proc3->set_instr_batch_size(1);
    mips_proc4->set_instr_batch_size(1);
  }

  cerr << endl;

  sc_start();

  mips_proc1.PrintStat();
  if(nproc > 1){
    mips_proc2->PrintStat();
  }
  if(nproc > 2){
    mips_proc3->PrintStat();
    mips_proc4->PrintStat();
  }

  cerr << endl;

  fpu.print_counters(nproc);

#ifdef AC_STATS
  ac_stats_base::print_all_stats(std::cerr);
#endif

#ifdef AC_DEBUG
  ac_close_trace();
#endif

  int ret = mips_proc1.ac_exit_status;
  ret += (nproc > 1) ? mips_proc2->ac_exit_status : 0;
  ret += (nproc > 2) ? mips_proc3->ac_exit_status + mips_proc4->ac_exit_status : 0;
  return ret;
}
