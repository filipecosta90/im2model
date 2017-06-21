#include "td_map.hpp"

/* base constructor */
TDMap::TDMap( boost::process::ipstream& ostream_celslc_buffer,
    boost::process::ipstream& ostream_msa_buffer,
    boost::process::ipstream& ostream_wavimg_buffer,
    boost::process::ipstream& ostream_simgrid_buffer,
    Image_Crystal *image_crystal_ptr )
: _sim_tdmap_celslc_ostream_buffer( ostream_celslc_buffer ) ,
  _sim_tdmap_msa_ostream_buffer( ostream_msa_buffer ),
  _sim_tdmap_wavimg_ostream_buffer( ostream_wavimg_buffer ),
  _sim_tdmap_simgrid_ostream_buffer( ostream_simgrid_buffer )
{
  _core_image_crystal_ptr = image_crystal_ptr;
  _tdmap_celslc_parameters = new CELSLC_prm( ostream_celslc_buffer );
  _tdmap_msa_parameters = new MSA_prm( ostream_msa_buffer );
  _tdmap_wavimg_parameters = new WAVIMG_prm( ostream_wavimg_buffer );
  _td_map_simgrid = new SIMGRID_wavimg_steplength( ostream_simgrid_buffer );

  /* ******
   *
   */
  _ignore_edge_pixels_sim_images = 0;
  nx_ny_switch = false;

  _flag_defocus_lower_bound = false;
  _flag_defocus_upper_bound = false;
  _flag_defocus_samples = false;
  _flag_defocus_period = false;

  /////////////
  // only for debug. need to add this options like in im2model command line
  /////////////
  super_cell_size_a = 2.0;
  _flag_super_cell_size_a = true;
  super_cell_size_b = 2.0;
  _flag_super_cell_size_b = true;
  super_cell_size_c = 16.0;
  slc_file_name_prefix = "test";
  _flag_slc_file_name_prefix = true;

  wave_function_name = "wave";
  _flag_wave_function_name = true;
  wavimg_prm_name = "temporary_wavimg_im2model.prm";
  _flag_wavimg_prm_name = true;
  file_name_output_image_wave_function = "image";
  _flag_file_name_output_image_wave_function = true;


  /* ******
   * wavimg static settings
   */
  // setters line 1
  _tdmap_wavimg_parameters->set_file_name_input_wave_function( wave_function_name );
  // setters line 2
  // SIMULATION DEPENDENT
  // setters line 3
  // SIMULATION DEPENDENT
  // setters line 4
  // SIMULATION DEPENDENT
  // setters line 5
  // setters line 6
  _tdmap_wavimg_parameters->set_file_name_output_image_wave_function( file_name_output_image_wave_function );
  // setters line 7
  // SIMULATION DEPENDENT
  // setters line 8
  _tdmap_wavimg_parameters->set_image_data_type( 0 );
  _tdmap_wavimg_parameters->set_image_vacuum_mean_intensity( 3000.0f );
  _tdmap_wavimg_parameters->set_conversion_rate( 1.0f );
  _tdmap_wavimg_parameters->set_readout_noise_rms_amplitude( 0.0f );
  // setters line 9
  // NOT USED
  // setters line 10
  // NOT USED
  // setters line 11
  // NOT USED
  // setters line 12
  // NOT USED
  // setters line 13
  _tdmap_wavimg_parameters->set_switch_coherence_model( 1 );
  // setters line 14
  _tdmap_wavimg_parameters->set_partial_temporal_coherence_switch( 1 );
  _tdmap_wavimg_parameters->set_partial_temporal_coherence_focus_spread( 4.0f );
  // setters line 15
  _tdmap_wavimg_parameters->set_partial_spacial_coherence_switch( 1 );
  _tdmap_wavimg_parameters->set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
  // setters line 16
  // SIMULATION DEPENDENT
  // setters line 17
  _tdmap_wavimg_parameters->set_simulation_image_spread_envelope_switch( 0 );
  _tdmap_wavimg_parameters->set_isotropic_one_rms_amplitude( 0.03 ); // colocar a zero
  // setters line 18
  // SIMULATION DEPENDENT
  // setters line 19
  // SIMULATION DEPENDENT
  // setters line 19 + aberration_definition_index_number
  _tdmap_wavimg_parameters->set_objective_aperture_radius( 5500.0f );
  // setters line 20 + aberration_definition_index_number
  _tdmap_wavimg_parameters->set_center_x_of_objective_aperture( 0.0f );
  _tdmap_wavimg_parameters->set_center_y_of_objective_aperture( 0.0f );
  // setters line 21 + aberration_definition_index_number
  // SIMULATION DEPENDENT
  _tdmap_wavimg_parameters->set_prm_file_name("temporary_wavimg_im2model.prm");


}

TDMap::TDMap( boost::process::ipstream& ostream_celslc_buffer,
    boost::process::ipstream& ostream_msa_buffer,
    boost::process::ipstream& ostream_wavimg_buffer,
    boost::process::ipstream& ostream_simgrid_buffer,
    Image_Crystal *image_crystal_ptr,
    ApplicationLog::ApplicationLog* app_logger ) :
  TDMap::TDMap( ostream_celslc_buffer, ostream_msa_buffer, ostream_wavimg_buffer, ostream_simgrid_buffer,  image_crystal_ptr ) {
    logger = app_logger;
    _flag_logger = true;
    _tdmap_celslc_parameters->set_application_logger( app_logger );
  }

void TDMap::set_group_options( group_options* celslc_step, group_options* msa_step, group_options* wavimg_step, group_options* simgrid_step ){
  celslc_step_group_options = celslc_step;
  msa_step_group_options = msa_step;
  wavimg_step_group_options = wavimg_step;
  simgrid_step_group_options = simgrid_step;
}


bool TDMap::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for TDMap class." );
  _core_image_crystal_ptr->set_application_logger( app_logger );
  _tdmap_celslc_parameters->set_application_logger( app_logger );
  _tdmap_msa_parameters->set_application_logger( app_logger );
  _tdmap_wavimg_parameters->set_application_logger( app_logger );
  _td_map_simgrid->set_application_logger(app_logger);
  return true;
}

bool TDMap::set_base_dir_path( boost::filesystem::path path ){
  base_dir_path = path;
  _flag_base_dir_path = true;
  _core_image_crystal_ptr->set_base_dir_path( path );
  _tdmap_celslc_parameters->set_base_dir_path( path );
  _tdmap_msa_parameters->set_base_dir_path( path );
  _tdmap_wavimg_parameters->set_base_dir_path( path );
  _td_map_simgrid->set_base_dir_path(path);
  std::stringstream message;
  message << "TDMap baseDirPath: " << path.string();
  logger->logEvent( ApplicationLog::notification, message.str() );
  return true;
}

void  TDMap::set_sim_tdmap_ostream( std::ostream* stream ){
  _sim_tdmap_ostream = stream;
  _flag_sim_tdmap_ostream = true;
}

bool TDMap::set_number_slices_to_max_thickness_from_nz_simulated_partitions(){
  bool status = false;
  if ( _tdmap_celslc_parameters->_is_nz_simulated_partitions_defined() ){
    number_slices_to_max_thickness = _tdmap_celslc_parameters->get_nz_simulated_partitions();
    status = true;
  }
  return status;
}

bool TDMap::set_number_slices_to_load_from_nz_simulated_partitions(){
  bool status = false;
  if ( _tdmap_celslc_parameters->_is_nz_simulated_partitions_defined() ){
    slices_load = _tdmap_celslc_parameters->get_nz_simulated_partitions();
    status = true;
  }
  return status;
}

bool TDMap::calculate_simulated_image_sampling_rate_and_size(){

  bool status = false;

  if( _core_image_crystal_ptr->_is_sampling_rate_experimental_defined() &&
      _is_super_cell_size_a_defined() &&
      _is_super_cell_size_b_defined()
    ){
    const double sampling_rate_experimental_x_nm_per_pixel = _core_image_crystal_ptr->get_sampling_rate_experimental_x_nm_per_pixel();
    const double sampling_rate_experimental_y_nm_per_pixel = _core_image_crystal_ptr->get_sampling_rate_experimental_y_nm_per_pixel();

    if ( nx_ny_switch ){
      sampling_rate_super_cell_x_nm_pixel = super_cell_size_a / nx_simulated_horizontal_samples;
      sampling_rate_super_cell_y_nm_pixel = super_cell_size_b / ny_simulated_vertical_samples;
      _flag_sampling_rate_super_cell_x_nm_pixel = true;
      _flag_sampling_rate_super_cell_y_nm_pixel = true;
    }
    else{
      nx_simulated_horizontal_samples = (int) ( super_cell_size_a / sampling_rate_experimental_x_nm_per_pixel );
      ny_simulated_vertical_samples = (int) ( super_cell_size_b / sampling_rate_experimental_y_nm_per_pixel );
      sampling_rate_super_cell_x_nm_pixel = sampling_rate_experimental_x_nm_per_pixel;
      sampling_rate_super_cell_y_nm_pixel = sampling_rate_experimental_y_nm_per_pixel;
      std::cout << "automatic nx and ny. -nx " << nx_simulated_horizontal_samples << " -ny " << ny_simulated_vertical_samples << std::endl;
      _flag_sampling_rate_super_cell_x_nm_pixel = true;
      _flag_sampling_rate_super_cell_y_nm_pixel = true;
      _flag_nx_simulated_horizontal_samples = true;
      _flag_ny_simulated_vertical_samples = true;
    }

    initial_simulated_image_width = nx_simulated_horizontal_samples - ( 2 * _ignore_edge_pixels_sim_images );
    initial_simulated_image_height = ny_simulated_vertical_samples   - ( 2 * _ignore_edge_pixels_sim_images );
    reshaped_simulated_image_width = initial_simulated_image_width;
    reshaped_simulated_image_height = initial_simulated_image_height;

    ignore_edge_pixels_rectangle.x = _ignore_edge_pixels_sim_images;
    ignore_edge_pixels_rectangle.y = _ignore_edge_pixels_sim_images;
    ignore_edge_pixels_rectangle.width = initial_simulated_image_width;
    ignore_edge_pixels_rectangle.height = initial_simulated_image_height;

    status = true;
  }
  return status;
}

bool  TDMap::calculate_simulation_defocus_period(){
  if( _flag_defocus_lower_bound && _flag_defocus_upper_bound && _flag_defocus_samples ){
    defocus_period = ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
    _flag_defocus_period = true;
  }
  std::cout << "defocus period " <<  defocus_period << std::endl;
  return _flag_defocus_period;
}

bool TDMap::calculate_thickness_range_upper_bound_slice_from_nm(){
  bool result = false;
  if ( _is_thickness_range_upper_bound_defined() ){
    slices_upper_bound = _tdmap_celslc_parameters->get_slice_number_from_nm_floor( nm_upper_bound );
    std::cout << "Calculated slice # " << slices_upper_bound << " as the upper bound for the maximum thickness of: " << nm_upper_bound << " nm" << std::endl;
    _flag_thickness_upper_bound_slice = true;
    result = true;
  }
  return result;
}

bool TDMap::calculate_thickness_range_lower_bound_slice_from_nm(){
  bool result = false;
  if( _flag_logger ){
    std::stringstream message;
    message << "Inside calculate_thickness_range_lower_bound_slice_from_nm()";
    logger->logEvent( ApplicationLog::notification , message.str() );
    message = std::stringstream();
    message << "_tdmap_celslc_parameters->_is_nz_simulated_partitions_defined() " << std::boolalpha << _tdmap_celslc_parameters->_is_nz_simulated_partitions_defined();
    logger->logEvent( ApplicationLog::notification , message.str() );
    message = std::stringstream();
    message << "_is_thickness_range_lower_bound_defined() " << std::boolalpha << _is_thickness_range_lower_bound_defined();
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  if ( _is_thickness_range_lower_bound_defined() && _tdmap_celslc_parameters->_is_nz_simulated_partitions_defined()  ){
    slices_lower_bound = _tdmap_celslc_parameters->get_slice_number_from_nm_ceil( nm_lower_bound );
    std::cout << "Calculated slice # " << slices_lower_bound << " as the lower bound for the minimum thickness of: " << nm_lower_bound << " nm" << std::endl;
    if (slices_lower_bound == 0){
      std::cout << "WARNING: Defined slice lower bound as 0. Going to define slice lower bound as: 1" << std::endl;
      slices_lower_bound = 1;
    }
    _flag_thickness_lower_bound_slice = true;
    result = true;
  }
  return result;
}


double TDMap::get_spherical_aberration(){
  return _tdmap_wavimg_parameters->get_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 1 );
}

bool TDMap::get_spherical_aberration_switch(){
  return _tdmap_wavimg_parameters->get_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::SphericalAberration );
}

bool TDMap::set_spherical_aberration ( std::string _string_cs ){
  const double _cs = boost::lexical_cast<double>( _string_cs );
  _tdmap_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 2 , 0.0f );
  return _tdmap_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 1 , _cs );
}

bool TDMap::set_spherical_aberration_switch( bool cs_switch ){
  return _tdmap_wavimg_parameters->set_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::SphericalAberration, cs_switch );
}

cv::Mat TDMap::get_simulated_image_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_in_grid(  x,  y );
}

double TDMap::get_simulated_image_match_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_match_in_grid(  x,  y );
}

double TDMap::get_simulated_image_thickness_nm_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_thickness_nm_in_grid(  x,  y );
}

int TDMap::get_simulated_image_thickness_slice_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_thickness_slice_in_grid(  x,  y );
}

double TDMap::get_simulated_image_defocus_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_defocus_in_grid(  x,  y );
}

bool TDMap::get_run_celslc_switch(){
  return _run_celslc_switch;
}

bool TDMap::get_run_msa_switch(){
  return _run_msa_switch;
}

bool TDMap::get_run_wavimg_switch(){
  return _run_wavimg_switch;
}

bool TDMap::get_run_simgrid_switch(){
  return _run_simgrid_switch;
}

bool TDMap::_is_sim_tdmap_celslc_ostream_buffer_active(){
  return _tdmap_celslc_parameters->get_flag_io_ap_pipe_out();
}


bool TDMap::_is_sim_tdmap_msa_ostream_buffer_active(){
  return _tdmap_msa_parameters->get_flag_io_ap_pipe_out();
}


bool TDMap::_is_sim_tdmap_wavimg_ostream_buffer_active(){
  return _tdmap_wavimg_parameters->get_flag_io_ap_pipe_out();
}

bool TDMap::_is_sim_tdmap_simgrid_ostream_buffer_active(){
  return _td_map_simgrid->get_flag_io_ap_pipe_out();
}

bool TDMap::calculate_thickness_range_slice_period(){
  bool result = false;
  if ( _is_thickness_range_lower_bound_slice_defined() && _is_thickness_range_upper_bound_slice_defined() ){
    int slice_interval = slices_upper_bound - slices_lower_bound;
    std::div_t divresult;
    divresult = div (slice_interval, (slice_samples -1) );
    slice_period = divresult.quot;
    assert(slice_period >= 1);
    std::cout << "Calculated slice period of " << slice_period << std::endl;
    const int remainder_slices = divresult.rem;
    const int slices_to_period = (slice_samples -1) - remainder_slices;
    if ( remainder_slices > 0 ){
      std::cout << "WARNING: an adjustment needs to be made in the slices lower or upper bound." << std::endl;
      const int increase_top_range = slices_lower_bound + (slice_samples * slice_period );
      const int decrease_top_range = slices_lower_bound + ((slice_samples-1) * slice_period );
      const int decrease_bot_range = slices_lower_bound-slices_to_period + (slice_samples * slice_period );

      if ( increase_top_range <= number_slices_to_max_thickness ){
        std::cout << "Increasing top range to slice #" << increase_top_range << std::endl;
        std::cout << "Going to use one more sample than the requested " << slice_samples << " samples. Using " << (slice_samples+1) << " samples." << std::endl;
        slices_upper_bound = increase_top_range;
        slice_samples++;
      }
      else{
        if( decrease_bot_range >= 1 ){
          std::cout << "Decreasing bot range to slice #" << decrease_bot_range << std::endl;
          slices_lower_bound -= slices_to_period;
          slice_samples++;
        }
        else{
          std::cout << "Decreasing top range to slice #" << decrease_top_range << std::endl;
          slices_upper_bound = decrease_top_range;
        }
      }
    }
    _flag_thickness_period_slice = true;
    result = true;
  }
  return result;
}

std::vector<std::string> TDMap::get_test_run_config_errors(){
  return test_run_config_errors;
}

std::vector<std::string> TDMap::get_test_clean_run_env_warnings(){
  return test_clean_run_env_warnings;
}

bool TDMap::test_clean_run_env(){

  test_clean_run_env_warnings.clear();
  bool result = true;
  /*
     if( _run_celslc_switch ){
     const bool celslc_res = _tdmap_celslc_parameters->check_clean_run_env();
     if( celslc_res == false ){
     const std::vector<std::string> warnings_list = _tdmap_celslc_parameters->get_run_env_warnings();
     for(size_t i=0; i < warnings_list.size(); i++){
     test_clean_run_env_warnings.push_back(warnings_list.at(i));
     }
     }
     result &= celslc_res;
     }

     if( _run_msa_switch ){
     const bool msa_res = _tdmap_msa_parameters->check_clean_run_env();
     if( msa_res == false ){
     const std::vector<std::string> warnings_list = _tdmap_msa_parameters->get_run_env_warnings();
     for(size_t i=0; i < warnings_list.size(); i++){
     test_clean_run_env_warnings.push_back(warnings_list.at(i));
     }
     }
     result &= msa_res;
     }*/
  if( _run_wavimg_switch ){
    const bool wavimg_res = _tdmap_wavimg_parameters->check_clean_run_env();
    std::cout << "  wavimg_res " << wavimg_res << std::endl;
    if( wavimg_res == false ){
      const std::vector<std::string> warnings_list = _tdmap_wavimg_parameters->get_run_env_warnings();
      for(size_t i=0; i < warnings_list.size(); i++){
        test_clean_run_env_warnings.push_back(warnings_list.at(i));
      }
    }
    result &= wavimg_res;
  }
  /*
     if( _run_simgrid_switch ){
     const bool simgrid_res = _tdmap_simgrid_parameters->check_clean_run_env();
     if( wavimg_res == false ){
     const std::vector<std::string> warnings_list = _tdmap_msa_parameters->get_run_env_warnings();
     for(size_t i=0; i < warnings_list.size(); i++){
     test_clean_run_env_warnings.push_back(warnings_list.at(i));
     }
     }
     result &= simgrid_res;
     }*/
  return result;
}


bool TDMap::test_run_config(){
  test_run_config_errors.clear();
  // at least one should be runned. if not return false
  bool result = _run_celslc_switch || _run_msa_switch || _run_wavimg_switch || _run_simgrid_switch;
  if( !result ){
    std::stringstream error_message;
    error_message << "To test run config, at least one runnable option should be marked.";
    test_run_config_errors.push_back( error_message.str() );
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Testing run configuration for steps{ CELSLC: " << std::boolalpha << _run_celslc_switch
      << ", MSA: " << std::boolalpha << _run_msa_switch
      << ", WAVIMG: " << std::boolalpha << _run_wavimg_switch
      << ", SIMGRID: " << std::boolalpha << _run_simgrid_switch << " }";
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  if( _run_celslc_switch ){
    const bool celslc_res = celslc_step_group_options->are_group_vars_setted_up();
    if( celslc_res == false ){
      const std::vector<std::string> celslc_error_list = celslc_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < celslc_error_list.size(); i++){
        test_run_config_errors.push_back(celslc_error_list.at(i));
      }
    }
    result &= celslc_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST CELSLC are_group_vars_setted_up: " << std::boolalpha << celslc_res;
      if( celslc_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _run_msa_switch ){
    const bool msa_res = msa_step_group_options->are_group_vars_setted_up();
    if( msa_res == false ){
      const std::vector<std::string> msa_error_list = msa_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < msa_error_list.size(); i++){
        test_run_config_errors.push_back(msa_error_list.at(i));
      }
    }
    result &= msa_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST MSA are_group_vars_setted_up: " << std::boolalpha << msa_res;
      if( msa_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _run_wavimg_switch ){
    const bool wavimg_res = wavimg_step_group_options->are_group_vars_setted_up();
    if( wavimg_res == false ){
      const std::vector<std::string> wavimg_error_list = wavimg_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < wavimg_error_list.size(); i++){
        test_run_config_errors.push_back(wavimg_error_list.at(i));
      }
    }
    result &= wavimg_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST WAVIMG are_group_vars_setted_up: " << std::boolalpha << wavimg_res;
      if( wavimg_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _run_simgrid_switch ){
    const bool simgrid_res = simgrid_step_group_options->are_group_vars_setted_up();
    if( simgrid_res == false ){
      const std::vector<std::string> simgrid_error_list = simgrid_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < simgrid_error_list.size(); i++){
        test_run_config_errors.push_back(simgrid_error_list.at(i));
      }
    }
    result &= simgrid_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST SIMGRID are_group_vars_setted_up: " << std::boolalpha << simgrid_res;
      if( simgrid_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  return result;
}

bool TDMap::run_tdmap( ){
  bool _simulation_status = false;
  const bool _vars_setted_up = test_run_config();
  if ( _vars_setted_up ){

    bool _celslc_stage_ok = false;
    bool _msa_stage_ok = false;
    bool _wavimg_stage_ok = false;
    bool _simgrid_stage_ok = false;

    const bool _rate_and_size_ok = calculate_simulated_image_sampling_rate_and_size();
    if ( _rate_and_size_ok ){
      const bool _celslc_parameters_ok = prepare_celslc_parameters();
      if( _flag_logger ){
        std::stringstream message;
        message << " prepare celslc parameters status " << std::boolalpha << _celslc_parameters_ok;
        logger->logEvent( ApplicationLog::notification , message.str() );
        message = std::stringstream();
        message << " _run_celslc_switch " << std::boolalpha << _run_celslc_switch;
        logger->logEvent( ApplicationLog::notification , message.str() );
        message = std::stringstream();
        message << " _is_bin_path_defined " << std::boolalpha << _tdmap_celslc_parameters->_is_bin_path_defined();
        logger->logEvent( ApplicationLog::notification , message.str() );
        message = std::stringstream();
      }
      // check that celslc parameters are prepared and paths are defined
      if( _celslc_parameters_ok && _tdmap_celslc_parameters->_is_bin_path_defined() ){
        //if flag run celslc
        if ( _run_celslc_switch ){
          bool _clean_run_env = !_flag_runned_tdmap_celslc;
          if( _flag_runned_tdmap_celslc ){
            _clean_run_env = _tdmap_celslc_parameters->clean_for_re_run();
            _flag_runned_tdmap_celslc = !_clean_run_env;
            if( _flag_logger ){
              std::stringstream message;
              message << "Already runned celslc. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
              if( _clean_run_env ){
                logger->logEvent( ApplicationLog::notification , message.str() );
              }
              else{
                logger->logEvent( ApplicationLog::error , message.str() );
              }
            }
          }
          if( _clean_run_env ){
            emit TDMap_started_celslc( );
            _flag_runned_tdmap_celslc = _tdmap_celslc_parameters->call_boost_bin();
            emit TDMap_ended_celslc( _flag_runned_tdmap_celslc );
          }
          _celslc_stage_ok = _flag_runned_tdmap_celslc;
          if( _flag_logger ){
            std::stringstream message;
            message << "_flag_runned_tdmap_celslc: " << std::boolalpha << _flag_runned_tdmap_celslc;
            if( _flag_runned_tdmap_celslc ){
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
            else{
              logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
        }
        //if run celslc flag is false, the sli and prm files should exist and we should update from them
        else{
          _celslc_stage_ok = _tdmap_celslc_parameters->update_nz_simulated_partitions_from_prm();
          if( _flag_logger ){
            std::stringstream message;
            message << "Run celslc flag is false, the sli and prm files should exist and we should update from them. result: " << std::boolalpha << _celslc_stage_ok;
            logger->logEvent( ApplicationLog::notification , message.str() );
          }
        }
        // compute vars after celslc run
        if( _celslc_stage_ok ){
          const bool _slices_load_ok = set_number_slices_to_load_from_nz_simulated_partitions();
          std::cout << "_slices_load_ok " << std::boolalpha << _slices_load_ok << std::endl;
          const bool _slices_max_thick_ok = set_number_slices_to_max_thickness_from_nz_simulated_partitions();
          std::cout << "_slices_max_thick_ok " << std::boolalpha << _slices_max_thick_ok << std::endl;
          const bool _thick_lower_bound_ok = calculate_thickness_range_lower_bound_slice_from_nm();
          std::cout << "_thick_lower_bound_ok " << std::boolalpha << _thick_lower_bound_ok << std::endl;
          const bool _thick_upper_bound_ok = calculate_thickness_range_upper_bound_slice_from_nm();
          std::cout << "_thick_upper_bound_ok " << std::boolalpha << _thick_upper_bound_ok << std::endl;
          const bool _thick_period_ok = calculate_thickness_range_slice_period();
          std::cout << "_thick_period_ok " << std::boolalpha << _thick_period_ok << std::endl;
          _celslc_stage_ok &= _slices_load_ok;
          _celslc_stage_ok &= _slices_max_thick_ok;
          _celslc_stage_ok &= _thick_lower_bound_ok;
          _celslc_stage_ok &= _thick_upper_bound_ok;
          _celslc_stage_ok &= _thick_period_ok;
          if( _flag_logger ){
            std::stringstream message;
            message << "compute vars after celslc run. result: " << std::boolalpha << _celslc_stage_ok;
            logger->logEvent( ApplicationLog::notification , message.str() );
          }
        }
      }

      //if everything went well until here
      if( _celslc_stage_ok ){
        const bool _msa_parameters_ok =  prepare_msa_parameters();
        if( _flag_logger ){
          std::stringstream message;
          message << "prepare_msa_parameters status " << std::boolalpha << _msa_parameters_ok;
          logger->logEvent( ApplicationLog::notification , message.str() );
          message = std::stringstream();
          message << " _run_msa_switch " << std::boolalpha << _run_msa_switch;
          logger->logEvent( ApplicationLog::notification , message.str() );
          message = std::stringstream();
          message << " _is_bin_path_defined " << std::boolalpha << _tdmap_msa_parameters->_is_bin_path_defined();
          logger->logEvent( ApplicationLog::notification , message.str() );
          message = std::stringstream();
        }
        // check that msa parameters are prepared and paths are defined
        if( _msa_parameters_ok && _tdmap_msa_parameters->_is_bin_path_defined() ){
          if ( _run_msa_switch ){
            bool _clean_run_env = !_flag_runned_tdmap_msa;
            if( _flag_runned_tdmap_msa ){
              _clean_run_env = _tdmap_msa_parameters->clean_for_re_run();
              _flag_runned_tdmap_msa = !_clean_run_env;
              if( _flag_logger ){
                std::stringstream message;
                message << "Already runned msa. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
                if( _clean_run_env ){
                  logger->logEvent( ApplicationLog::notification , message.str() );
                }
                else{
                  logger->logEvent( ApplicationLog::error , message.str() );
                }
              }
            }
            if( _clean_run_env ){
              emit TDMap_started_msa();
              _flag_runned_tdmap_msa = _tdmap_msa_parameters->call_bin();
              emit TDMap_ended_msa( _flag_runned_tdmap_msa );
              if( _flag_logger ){
                std::stringstream message;
                message << "_flag_runned_tdmap_msa: " << std::boolalpha << _flag_runned_tdmap_msa;
                if( _flag_runned_tdmap_msa ){
                  logger->logEvent( ApplicationLog::notification , message.str() );
                }
                else{
                  logger->logEvent( ApplicationLog::error , message.str() );
                }
              }
            }
            _msa_stage_ok = _flag_runned_tdmap_msa;
          }
          //if run msa flag is false, the wav files should exist and we should update from them
          else{
            _msa_stage_ok = _tdmap_msa_parameters->check_produced_waves();
            if( _flag_logger ){
              std::stringstream message;
              message << "Run msa flag was false, the wav files should exist and we should update from them. result: " << std::boolalpha << _msa_stage_ok;
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
          }
        }
      }

      if( _celslc_stage_ok && _msa_stage_ok ){
        bool _clean_run_env = !_flag_runned_tdmap_wavimg;
        if( _run_wavimg_switch && _flag_runned_tdmap_wavimg ){
          _clean_run_env = _tdmap_wavimg_parameters->clean_for_re_run();
          _flag_runned_tdmap_wavimg = !_clean_run_env;
          if( _flag_logger ){
            std::stringstream message;
            message << "Already runned wavimg. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
            if( _clean_run_env ){
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
            else{
              logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
        }
        // check that paths are defined
        if(  _tdmap_wavimg_parameters->get_flag_full_bin_path_execname() ){
          //if run wavimg flag is true, the prm and dat files should be produced
          if ( _run_wavimg_switch ){
            if( _clean_run_env ){
              const bool _wavimg_parameters_ok = prepare_wavimg_parameters();
              emit TDMap_started_wavimg();
              _flag_runned_tdmap_wavimg = _tdmap_wavimg_parameters->call_bin();
              emit TDMap_ended_wavimg( _flag_runned_tdmap_wavimg );
              if( _flag_logger ){
                std::stringstream message;
                message << "_flag_runned_tdmap_wavimg: " << std::boolalpha << _flag_runned_tdmap_wavimg;
                ApplicationLog::severity_level _log_type = _flag_runned_tdmap_wavimg ? ApplicationLog::notification : ApplicationLog::error;
                logger->logEvent( _log_type , message.str() );
              }
            }
            _wavimg_stage_ok = _flag_runned_tdmap_wavimg;
          }
          //if run wavimg flag is false, the dat files should exist and we should update from them
          else{
            _wavimg_stage_ok = _tdmap_wavimg_parameters->check_produced_dat();
            if( _flag_logger ){
              std::stringstream message;
              message << "Run wavimg flag is false, the dat files should exist and we should update from them. result: " << std::boolalpha << _wavimg_stage_ok;
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
          }
        }
      }
      if( _celslc_stage_ok && _msa_stage_ok && _wavimg_stage_ok ){
        const bool _simgrid_parameters_ok = prepare_simgrid_parameters();
        if( _flag_logger ){
          std::stringstream message;
          message << " prepare_simgrid_parameters status " << std::boolalpha << _simgrid_parameters_ok;
          logger->logEvent( ApplicationLog::notification , message.str() );
        }
        if( _simgrid_parameters_ok ){
          if ( _run_simgrid_switch ){
            bool _clean_run_env = !_flag_runned_tdmap_simgrid;
            if( _flag_runned_tdmap_simgrid ){
              _clean_run_env = _td_map_simgrid->clean_for_re_run();
              _flag_runned_tdmap_simgrid = !_clean_run_env;
              if( _flag_logger ){
                std::stringstream message;
                message << "Already runned simgrid. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
                ApplicationLog::severity_level _log_type = _clean_run_env ? ApplicationLog::notification : ApplicationLog::error;
                logger->logEvent( _log_type , message.str() );
              }
            }
            if( _clean_run_env ){
              emit TDMap_started_simgrid();
              const bool _grid_ok = _td_map_simgrid->read_grid_from_dat_files();
              if( _grid_ok ){
                _flag_runned_tdmap_simgrid = _td_map_simgrid->simulate_from_grid();
              }
              else{
                _flag_runned_tdmap_simgrid = _grid_ok;
              }
              emit TDMap_ended_simgrid( _flag_runned_tdmap_simgrid );
              if( _flag_logger ){
                std::stringstream message;
                message << "_flag_runned_tdmap_simgrid: " << std::boolalpha << _flag_runned_tdmap_simgrid;
                ApplicationLog::severity_level _log_type = _flag_runned_tdmap_simgrid ? ApplicationLog::notification : ApplicationLog::error;
                logger->logEvent( _log_type , message.str() );
              }
            }
            _simgrid_stage_ok = _flag_runned_tdmap_simgrid;
          }
        }
      }
      _simulation_status = _celslc_stage_ok & _msa_stage_ok & _wavimg_stage_ok & _simgrid_stage_ok;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "Error while calculation sampling rate and size for TDMap: " << std::boolalpha << _rate_and_size_ok ;
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "TDMap vars are not correcly setted up. _vars_setted_up: " << std::boolalpha << _vars_setted_up ;
      logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return _simulation_status;
}

bool TDMap::_is_simulated_images_grid_defined(){
  return _td_map_simgrid->_is_simulated_images_grid_defined();
}

bool TDMap::set_run_celslc_switch( bool value ){
  _run_celslc_switch = value;
  return true;
}

bool TDMap::set_run_msa_switch( bool value ){
  _run_msa_switch = value;
  return true;
}

bool TDMap::set_run_wavimg_switch( bool value ){
  _run_wavimg_switch = value;
  return true;
}

bool TDMap::set_run_simgrid_switch( bool value ){
  _run_simgrid_switch = value;
  return true;
}

bool  TDMap::prepare_celslc_parameters(){
  _flag_tdmap_celslc_parameters = false;
  if( _core_image_crystal_ptr->_is_unit_cell_cif_path_defined()
      && _core_image_crystal_ptr->_is_perpendicular_dir_defined()
      && _core_image_crystal_ptr->_is_projection_dir_defined()
      && _is_simulated_image_sampling_rate_and_size_defined()
      && _flag_ht_accelaration_voltage
    ){
    const std::string super_cell_cif_file = _core_image_crystal_ptr->get_super_cell_cif_file_path();
    const double perpendicular_dir_u = _core_image_crystal_ptr->get_perpendicular_dir_u();
    const double perpendicular_dir_v = _core_image_crystal_ptr->get_perpendicular_dir_v();
    const double perpendicular_dir_w = _core_image_crystal_ptr->get_perpendicular_dir_w();
    const double projection_dir_h = _core_image_crystal_ptr->get_projection_dir_h();
    const double projection_dir_k = _core_image_crystal_ptr->get_projection_dir_k();
    const double projection_dir_l = _core_image_crystal_ptr->get_projection_dir_l();

    _tdmap_celslc_parameters->set_prp_dir_uvw( perpendicular_dir_u, perpendicular_dir_v, perpendicular_dir_w );
    _tdmap_celslc_parameters->set_prj_dir_hkl( projection_dir_h, projection_dir_k, projection_dir_l );
    _tdmap_celslc_parameters->set_super_cell_size_abc( super_cell_size_a, super_cell_size_b, super_cell_size_c );
    _tdmap_celslc_parameters->set_cif_file( super_cell_cif_file.c_str() );
    _tdmap_celslc_parameters->set_slc_filename_prefix ( slc_file_name_prefix.c_str() );
    _tdmap_celslc_parameters->set_nx_simulated_horizontal_samples( nx_simulated_horizontal_samples );
    _tdmap_celslc_parameters->set_ny_simulated_vertical_samples( ny_simulated_vertical_samples );
    _tdmap_celslc_parameters->set_ht_accelaration_voltage( ht_accelaration_voltage );
    _tdmap_celslc_parameters->set_dwf_switch( dwf_switch );
    _tdmap_celslc_parameters->set_abs_switch( abs_switch );
    _flag_tdmap_celslc_parameters = true;
  }
  return _flag_tdmap_celslc_parameters;
}

bool TDMap::_is_nx_simulated_horizontal_samples_defined(){
  return _tdmap_celslc_parameters->_is_nx_simulated_horizontal_samples_defined();
}

bool TDMap::_is_ny_simulated_vertical_samples_defined(){
  return _tdmap_celslc_parameters->_is_ny_simulated_vertical_samples_defined();
}

int TDMap::get_nx_simulated_horizontal_samples(){
  return _tdmap_celslc_parameters->get_nx_simulated_horizontal_samples();
}

int TDMap::get_ny_simulated_vertical_samples(){
  return _tdmap_celslc_parameters->get_ny_simulated_vertical_samples();
}

bool  TDMap::prepare_msa_parameters(){
  _flag_tdmap_msa_parameters = false;
  if( _is_thickness_range_lower_bound_slice_defined()
      && _is_thickness_range_upper_bound_slice_defined()
      && _is_thickness_period_slice_defined()
      && _is_ht_accelaration_voltage_defined()
      && _is_slc_file_name_prefix_defined()
    ){
    _tdmap_msa_parameters->set_electron_wavelength( ht_accelaration_voltage );
    _tdmap_msa_parameters->set_internal_repeat_factor_of_super_cell_along_x ( 1 );
    _tdmap_msa_parameters->set_internal_repeat_factor_of_super_cell_along_y ( 1 );
    std::stringstream input_prefix_stream;
    _tdmap_msa_parameters->set_slice_filename_prefix ( slc_file_name_prefix );
    _tdmap_msa_parameters->set_number_slices_to_load ( slices_load );
    _tdmap_msa_parameters->set_number_frozen_lattice_variants_considered_per_slice( 1 );
    _tdmap_msa_parameters->set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
    _tdmap_msa_parameters->set_period_readout_or_detection_in_units_of_slices ( 1 ); // bug
    _tdmap_msa_parameters->set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( number_slices_to_max_thickness );
    _tdmap_msa_parameters->set_linear_slices_for_full_object_structure();
    _tdmap_msa_parameters->set_prm_file_name("temporary_msa_im2model.prm");
    _tdmap_msa_parameters->set_wave_function_name ( wave_function_name );
    _tdmap_msa_parameters->produce_prm();

    _flag_tdmap_msa_parameters = true;
  }
  return _flag_tdmap_msa_parameters;
}

bool  TDMap::prepare_wavimg_parameters(){
  _flag_tdmap_wavimg_parameters = false;
  if( _is_wave_function_name_defined()
      && _is_thickness_range_lower_bound_slice_defined()
      && _is_thickness_period_slice_defined()
    ){
    // setters line 2
    _tdmap_wavimg_parameters->set_n_columns_samples_input_wave_function_pixels( ny_simulated_vertical_samples );
    _tdmap_wavimg_parameters->set_n_rows_samples_input_wave_function_pixels( nx_simulated_horizontal_samples );
    // setters line 3
    _tdmap_wavimg_parameters->set_physical_columns_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_x_nm_pixel );
    _tdmap_wavimg_parameters->set_physical_rows_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_y_nm_pixel );
    // setters line 4
    _tdmap_wavimg_parameters->set_primary_electron_energy( ht_accelaration_voltage );
    // setters line 7
    _tdmap_wavimg_parameters->set_n_columns_samples_output_image( nx_simulated_horizontal_samples );
    _tdmap_wavimg_parameters->set_n_rows_samples_output_image( ny_simulated_vertical_samples );

    _tdmap_wavimg_parameters->set_number_parameter_loops( 2 );
    
    _tdmap_wavimg_parameters->add_parameter_loop ( 1 , 1 , 1, defocus_lower_bound, defocus_upper_bound, defocus_samples, "'foc'" );
    _tdmap_wavimg_parameters->add_parameter_loop ( 3 , 1 , 1, slices_lower_bound, slices_upper_bound, slice_samples, "'_sl'" );
    _tdmap_wavimg_parameters->set_prm_file_name("temporary_wavimg_im2model.prm");
    _flag_tdmap_wavimg_parameters = _tdmap_wavimg_parameters->produce_prm();
  }
  return _flag_tdmap_wavimg_parameters;
}


bool  TDMap::prepare_simgrid_parameters(){
  _flag_tdmap_simgrid_parameters = false;
  celslc_accum_nm_slice_vec = _tdmap_celslc_parameters->get_slice_params_accum_nm_slice_vec();
  _flag_celslc_accum_nm_slice_vec = true;
  assert( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness );
  const double _sampling_rate_experimental_x_nm_per_pixel = _core_image_crystal_ptr->get_sampling_rate_experimental_x_nm_per_pixel();
  const double _sampling_rate_experimental_y_nm_per_pixel = _core_image_crystal_ptr->get_sampling_rate_experimental_y_nm_per_pixel();
  cv::Mat _experimental_image_roi = _core_image_crystal_ptr->get_roi_experimental_image_mat();

  _td_map_simgrid->set_sampling_rate_super_cell_x_nm_pixel( _sampling_rate_experimental_x_nm_per_pixel );
  _td_map_simgrid->set_sampling_rate_super_cell_y_nm_pixel( _sampling_rate_experimental_y_nm_per_pixel );
  _td_map_simgrid->set_experimental_image_roi( _experimental_image_roi );

  // defocus setters
  _td_map_simgrid->set_defocus_lower_bound( defocus_lower_bound );
  _td_map_simgrid->set_defocus_upper_bound( defocus_upper_bound );
  _td_map_simgrid->set_defocus_samples( defocus_samples );
  _td_map_simgrid->set_defocus_period( defocus_period );

  // thickness/slice setters

  _td_map_simgrid->set_celslc_accum_nm_slice_vec( celslc_accum_nm_slice_vec );
  _td_map_simgrid->set_slice_samples( slice_samples );
  _td_map_simgrid->set_slice_period( slice_period );
  _td_map_simgrid->set_number_slices_to_max_thickness( number_slices_to_max_thickness );
  _td_map_simgrid->set_slices_lower_bound( slices_lower_bound );
  _td_map_simgrid->set_slices_upper_bound( slices_upper_bound );

  _td_map_simgrid->set_n_rows_simulated_image( nx_simulated_horizontal_samples );
  _td_map_simgrid->set_n_cols_simulated_image( ny_simulated_vertical_samples );

  _td_map_simgrid->set_reshaped_simulated_image_width(reshaped_simulated_image_width);
  _td_map_simgrid->set_reshaped_simulated_image_height(reshaped_simulated_image_height);

  _td_map_simgrid->set_ignore_edge_pixels_rectangle( ignore_edge_pixels_rectangle );
  _td_map_simgrid->set_simulated_image_needs_reshape( simulated_image_needs_reshape );
  _td_map_simgrid->set_reshape_factor_from_supper_cell_to_experimental_x (reshape_factor_from_supper_cell_to_experimental_x);
  _td_map_simgrid->set_reshape_factor_from_supper_cell_to_experimental_y (reshape_factor_from_supper_cell_to_experimental_y);

  _flag_tdmap_simgrid_parameters = true;
  return  _flag_tdmap_simgrid_parameters;
}

bool TDMap::export_sim_grid( std::string sim_grid_file_name_image ){
  return _td_map_simgrid->export_sim_grid( sim_grid_file_name_image );
}

std::string TDMap::get_export_sim_grid_filename_hint(){
  return _td_map_simgrid->get_export_sim_grid_filename_hint();
}

bool TDMap::_is_thickness_range_lower_bound_defined(){
  return _flag_thickness_lower_bound;
}

bool TDMap::_is_thickness_range_lower_bound_slice_defined(){
  return _flag_thickness_lower_bound_slice;
}

bool TDMap::_is_thickness_period_slice_defined(){
  return _flag_thickness_period_slice;
}

bool TDMap::_is_thickness_range_upper_bound_defined(){
  return _flag_thickness_upper_bound;
}

bool TDMap::_is_thickness_range_upper_bound_slice_defined(){
  return _flag_thickness_upper_bound_slice;
}

bool TDMap::_is_defocus_range_lower_bound_defined(){
  return _flag_defocus_lower_bound;
}

bool TDMap::_is_defocus_period_defined(){
  return _flag_defocus_period;
}

bool TDMap::_is_defocus_range_upper_bound_defined(){
  return _flag_defocus_upper_bound;
}

bool TDMap::_is_ht_accelaration_voltage_defined(){
  return _flag_ht_accelaration_voltage;
}

bool TDMap::_is_slc_file_name_prefix_defined(){
  return _flag_slc_file_name_prefix;
}

bool TDMap::_is_super_cell_size_a_defined(){
  return _flag_super_cell_size_a;
}

bool TDMap::_is_super_cell_size_b_defined(){
  return _flag_super_cell_size_b;
}

bool TDMap::_is_celslc_accum_nm_slice_vec_defined(){
  return _flag_celslc_accum_nm_slice_vec;
}

std::vector<double> TDMap::get_celslc_accum_nm_slice_vec(){
  return celslc_accum_nm_slice_vec;
}

bool TDMap::_is_simulated_image_sampling_rate_and_size_defined(){
  return ( _flag_nx_simulated_horizontal_samples
      && _flag_ny_simulated_vertical_samples
      && _flag_sampling_rate_super_cell_x_nm_pixel
      && _flag_sampling_rate_super_cell_y_nm_pixel
      );
}

bool TDMap::_is_wave_function_name_defined(){
  return _flag_wave_function_name;
}

/** getters **/

int TDMap::get_thickness_range_number_samples( ){
  return slice_samples;
}

double TDMap::get_thickness_range_lower_bound( ){
  return nm_lower_bound;
}

int TDMap::get_thickness_range_lower_bound_slice( ){
  return slices_lower_bound;
}

double TDMap::get_thickness_range_upper_bound( ){
  return nm_upper_bound;
}

int TDMap::get_thickness_range_upper_bound_slice( ){
  return slices_upper_bound;
}

int TDMap::get_thickness_range_period_slice() {
  return slice_period;
}

int TDMap::get_defocus_range_number_samples( ){
  return defocus_samples;
}

double TDMap::get_defocus_range_lower_bound( ){
  return defocus_lower_bound;
}

double TDMap::get_defocus_range_upper_bound( ){
  return defocus_upper_bound;
}

double TDMap::get_defocus_range_period( ){
  return defocus_period;
}

// more work here. asserts, etc
std::vector< std::vector<cv::Mat> > TDMap::get_simulated_images_grid(){
  return _td_map_simgrid->get_simulated_images_grid();
}

// more work here. asserts, etc
cv::Point2i TDMap::get_simgrid_best_match_position(){
  return _td_map_simgrid->get_best_match_position();
}

/** setters **/


bool TDMap::set_thickness_range_lower_bound_cast( std::string lower_bound ){
  bool set_result = false;
  try {
    const double casted_value = boost::lexical_cast<double>( lower_bound );
    set_result = set_thickness_range_lower_bound ( casted_value );
  }
  catch(const boost::bad_lexical_cast &)
  {
    set_result = false;
    // send exception
    // more WORK HERE!!!!
  }
  return set_result;
}

bool TDMap::set_thickness_range_lower_bound(  double lower_bound ){
  nm_lower_bound = lower_bound;
  _flag_thickness_lower_bound = true;
  return true;
}

bool TDMap::set_thickness_range_upper_bound_cast( std::string upper_bound ){
  bool set_result = false;
  try {
    const double casted_value = boost::lexical_cast<double>( upper_bound );
    set_result = set_thickness_range_upper_bound ( casted_value );
  }
  catch(const boost::bad_lexical_cast &){
    set_result = false;
    // send exception
    // more WORK HERE!!!!
  }
  return set_result;
}

bool TDMap::set_thickness_range_upper_bound(  double upper_bound ){
  nm_upper_bound = upper_bound;
  _flag_thickness_upper_bound = true;
  return true;
}

bool TDMap::set_thickness_range_nm_interval_dimension_cast( std::string dimension ){
  bool set_result = false;
  try {
    const double casted_value = boost::lexical_cast<double>( dimension );
    set_result = set_thickness_range_nm_interval_dimension ( casted_value );
  }
  catch(const boost::bad_lexical_cast &)
  {
    set_result = false;
    // send exception
    // more WORK HERE!!!!
  }
  return set_result;
}

bool TDMap::set_thickness_range_nm_interval_dimension(  double dimension ){
  thickness_range_nm_interval_dimension = dimension;
  _flag_thickness_range_nm_interval_dimension = true;
  return true;
}

bool TDMap::set_thickness_range_number_samples_cast( std::string number_samples ){
  bool set_result = false;
  try {
    const int casted_value = boost::lexical_cast<int>( number_samples );
    set_result = set_thickness_range_number_samples ( casted_value );
  }
  catch(const boost::bad_lexical_cast &)
  {
    set_result = false;
    // send exception
    // more WORK HERE!!!!
  }
  return set_result;
}

bool TDMap::set_thickness_range_number_samples(  int samples ){
  slice_samples = samples;
  _flag_thickness_samples = true;
  return true;
}

bool TDMap::set_thickness_user_estimated_nm_cast( std::string estimated_nm ){
  bool set_result = false;
  try {
    const double casted_value = boost::lexical_cast<double>( estimated_nm );
    set_result = set_thickness_user_estimated_nm ( casted_value );
  }
  catch(const boost::bad_lexical_cast &)
  {
    set_result = false;
    // send exception
    // more WORK HERE!!!!
  }
  return set_result;
}

bool TDMap::set_thickness_user_estimated_nm( double estimated_nm ){
  user_estimated_thickness_nm = estimated_nm;
  _flag_user_estimated_thickness_nm = true;
  return _flag_user_estimated_thickness_nm;
}

bool TDMap::set_defocus_range_lower_bound( std::string lower_bound ){
  defocus_lower_bound = boost::lexical_cast<double>( lower_bound );
  _flag_defocus_lower_bound = true;
  calculate_simulation_defocus_period();
  return true;
}

bool TDMap::set_defocus_range_upper_bound( std::string upper_bound ){
  defocus_upper_bound = boost::lexical_cast<double>( upper_bound );
  _flag_defocus_upper_bound = true;
  calculate_simulation_defocus_period();
  return true;
}

bool TDMap::set_defocus_range_number_samples( std::string number_samples ){
  defocus_samples = boost::lexical_cast<int>(number_samples);
  _flag_defocus_samples = true;
  calculate_simulation_defocus_period();
  return true;
}

bool TDMap::set_defocus_range_nm_interval_dimension( std::string dimension ){
  defocus_range_nm_interval_dimension = boost::lexical_cast<double>( dimension );
  _flag_defocus_range_nm_interval_dimension = true;
  return true;
}

bool TDMap::set_accelaration_voltage_kv( std::string accelaration_voltage ){
  ht_accelaration_voltage = boost::lexical_cast<double>( accelaration_voltage );
  _flag_ht_accelaration_voltage = true;
  return true;
}

bool TDMap::set_core_image_crystal_ptr( Image_Crystal* image_crystal_ptr ){
  _core_image_crystal_ptr = image_crystal_ptr;
  return true;
}

bool TDMap::set_dr_probe_celslc_execname( std::string celslc_execname ){
  _flag_dr_probe_celslc_execname = _tdmap_celslc_parameters->set_bin_execname(celslc_execname);
  if( _flag_dr_probe_celslc_execname ){
    dr_probe_celslc_execname = celslc_execname;
  }
  if( _flag_logger ){
    std::stringstream message;
    message << " set_dr_probe_celslc_execname result: " << _flag_dr_probe_celslc_execname;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return _flag_dr_probe_celslc_execname;

}

bool TDMap::set_dr_probe_msa_execname( std::string msa_execname ){
  bool result = true;
  result &= _tdmap_msa_parameters->set_bin_execname(msa_execname);
  if( result ){
    dr_probe_msa_execname = msa_execname;
  }
  _flag_dr_probe_msa_execname = result;

  if( _flag_logger ){
    std::stringstream message;
    message << " set_dr_probe_msa_execname result: " << _flag_dr_probe_msa_execname;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }

  return _flag_dr_probe_msa_execname;
}

bool TDMap::set_dr_probe_wavimg_execname( std::string wavimg_execname ){
  bool result = true;
  result &= _tdmap_wavimg_parameters->set_bin_execname( wavimg_execname );
  if( result ){
    dr_probe_wavimg_execname = wavimg_execname;
  }
  _flag_dr_probe_wavimg_execname = result;

  if( _flag_logger ){
    std::stringstream message;
    message << " set_dr_probe_wavimg_execname result: " << _flag_dr_probe_wavimg_execname;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return _flag_dr_probe_wavimg_execname;
}

bool TDMap::set_image_correlation_matching_method( int method ){
  _flag_image_correlation_matching_method = _td_map_simgrid->set_image_correlation_matching_method( method );
  return _flag_image_correlation_matching_method;
}

int TDMap::get_image_correlation_matching_method(){
  return _td_map_simgrid->get_image_correlation_matching_method( );
}

bool TDMap::set_refinement_definition_method ( int method ){
  _refinement_definition_method = method;
  _flag_refinement_definition_method = true;
  return _flag_refinement_definition_method;
}

int TDMap::get_refinement_definition_method(){
  return _refinement_definition_method;
}

bool TDMap::set_defocus_user_estimated_nm( double estimated_nm ){
  user_estimated_defocus_nm = estimated_nm;
  _flag_user_estimated_defocus_nm = true;
  return _flag_user_estimated_defocus_nm;
}

bool TDMap::auto_calculate_thickness_range_lower_upper_nm(){
  bool result = false;
  const double _half_interval = _preset_thickness_range_nm / 2.0f;
  std::cout << " user estimated thick  "  << user_estimated_thickness_nm << std::endl;

  if ( _flag_user_estimated_thickness_nm ){
    const double _temp_nm_thickness_range = _preset_thickness_range_nm;
    const double _temp_nm_lower_bound = user_estimated_thickness_nm - _half_interval;
    const double _temp_nm_upper_bound = user_estimated_thickness_nm + _half_interval;

    std::cout << " user estimated thick  "  << user_estimated_thickness_nm << std::endl;

    result = true;
    result &= set_thickness_range_lower_bound( _temp_nm_lower_bound );
    result &= set_thickness_range_upper_bound( _temp_nm_upper_bound );
    result &= set_thickness_range_nm_interval_dimension( _temp_nm_thickness_range );
  }
  std::cout << " auto auto_calculate_thickness_range_lower_upper_nm result "  << result << std::endl;
  return result;
}

bool TDMap::auto_calculate_thickness_lower_upper_nm(){
  bool result = false;
  const double _half_interval = thickness_range_nm_interval_dimension / 2.0f;
  if ( _flag_user_estimated_thickness_nm ){
    const double _temp_nm_lower_bound = user_estimated_thickness_nm - _half_interval;
    const double _temp_nm_upper_bound = user_estimated_thickness_nm + _half_interval;
    result = true;
  }
  std::cout << " auto auto_calculate_thickness_lower_upper_nm " << std::endl;
  return true;
}

bool TDMap::set_mtf_filename( std::string file_name ){
  bool result = _tdmap_wavimg_parameters->set_mtf_filename( file_name );
  if( result ){
    _tdmap_wavimg_parameters->set_mtf_simulation_switch( true );
  }
  return result;
}

bool TDMap::set_mtf_switch( bool value ){
  return _tdmap_wavimg_parameters->set_mtf_simulation_switch( value );
}
bool TDMap::get_mtf_switch( ){
  return _tdmap_wavimg_parameters->get_mtf_simulation_switch();
}

bool TDMap::set_log_level( int level ){
  _exec_log_level = level;
  return true;
}

int TDMap::get_log_level(){
  return _exec_log_level;
}
