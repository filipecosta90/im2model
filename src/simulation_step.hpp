/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef __SIMULATION_STEP_H__
#define __SIMULATION_STEP_H__

#include <string>

// project classes
// dr probe prms
#include "celslc_prm.hpp"                                 // for CELSLC_prm
#include "msa_prm.hpp"                                    // for MSA_prm
#include "wavimg_prm.hpp"                                 // for WAVIMG_prm
// others
#include "simgrid_steplength.hpp"                         // for SIMGRID_wav...
#include "unit_cell.hpp"                                  // for UnitCell
#include "super_cell.hpp"                                 // for Super_Cell

class Simulation_Step {
  private:
    int _step_id;
    std::string _step_descriptor;

    /** Unit Cell **/
    UnitCell* _unit_cell;

    /** Super Cell **/
    Super_Cell* _super_cell;
    
    /** Dr Probe parameters **/
    CELSLC_prm* _celslc_parameters;
    MSA_prm* _msa_parameters;
    WAVIMG_prm* _wavimg_parameters;

  public:
    Simulation_Step();
    Simulation_Step( int step_id, std::string step_descriptor );

    /** setters **/
    void set_unit_cell( UnitCell* unit_cell );
    void set_super_cell( Super_Cell* super_cell );

    /** getters **/
    UnitCell* get_unit_cell();
    Super_Cell* get_super_cell();
    CELSLC_prm* get_celslc_parameters();
    MSA_prm* get_msa_parameters();
    WAVIMG_prm* get_wavimg_parameters();
};

#endif
