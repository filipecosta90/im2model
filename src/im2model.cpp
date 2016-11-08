#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/opencv_modules.hpp"

# include "opencv2/core/core.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/nonfree/features2d.hpp"



// Include the headers relevant to the boost::program_options
// library
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

// Include std::exception so we can handling any argument errors
// emitted by the command line parser
#include <exception>

#include "msa_prm.h"
#include "wavimg_prm.h"

using namespace cv;

// Global Variables
cv::Mat experimental_image; 
cv::Mat experimental_image_roi;
cv::Mat experimental_working; 

Rect roi_rectangle;
int roi_x_size;
int roi_y_size;

/// Function Headers
void MatchingMethod( int, void* );

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
  if  ( event == EVENT_LBUTTONDOWN )
  {
    std::cout << "New ROI center - position (" << x << ", " << y << ")" << std::endl;
    roi_rectangle.x = x - ( roi_x_size / 2 );
    roi_rectangle.y = y - ( roi_y_size / 2) ;
    roi_rectangle.width = roi_x_size;
    roi_rectangle.height = roi_y_size;
    experimental_working = experimental_image.clone();
    // copy the roi
    cv::Mat roi = experimental_image(roi_rectangle);
    experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
    roi.copyTo(experimental_image_roi);
    //draw the roi
    rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
    imshow("Experimental Window", experimental_working);
  }
  else if  ( event == EVENT_LBUTTONUP )
  {
    //  std::cout << "Left button released" << std::endl;
  }
  else if ( event == EVENT_MOUSEMOVE )
  {
    //  std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
  }
}

int main(int argc, char** argv )
{

  // Add descriptive text for display when help argument is
  // supplied
  /*  boost::program_options::options_description desc(
      "\nAn example command using Boost command line "
      "arguments.\n\nAllowed arguments");
      */
  // Define command line arguments using either formats:
  //
  //     (“long-name,short-name”, “Description of argument”)
  //     for flag values or
  //
  //     (“long-name,short-name”, <data-type>, 
  //     “Description of argument”) arguments with values
  //
  // Remember that arguments with values may be multi-values
  // and must be vectors

  if ( argc != 19 )
  {
    printf("usage: im2model <Horizontal Samples> <Vertical Samples> <Simulated Partitions> <Slices Load> <Slices samples> <Slices lower bound> <Slices upper bound> <Slices To max thickness> <Defocus samples> <defocus lower bound> <Defocus upper bound> <experimental_image_path> <Simulated X nm per pixel> <Simulated Y nm per pixel> <ROI pixel size> <ROI center x> <ROI center y> <Ignore edge pixels>\n");
    return -1;
  }

    float super_cell_size_x = 2;
    float super_cell_size_y = 2;
    float super_cell_size_z = 16;
    int nx_simulated_horizontal_samples = atoi(argv[1]);
    int ny_simulated_vertical_samples = atoi(argv[2]);
    int nz_simulated_partitions = atoi(argv[3]);

    float super_cell_x_nm_pixel = super_cell_size_x / nx_simulated_horizontal_samples;
    float super_cell_y_nm_pixel = super_cell_size_y / ny_simulated_vertical_samples;
    float super_cell_z_nm_slice = super_cell_size_z / nz_simulated_partitions;
    
  int rows = nx_simulated_horizontal_samples;
  int cols = ny_simulated_vertical_samples;
  int slices_load = atoi(argv[4]);
  int slice_samples = atoi(argv[5]);
   int slices_lower_bound_int = atoi(argv[6]);
    float slices_upper_bound_int = atoi(argv[7]);
  float fp_slices_upper_bound = atof(argv[7]);
  int number_slices_to_max_thickness = atoi( argv[8] );
  int slice_period =  fp_slices_upper_bound  / slice_samples;
    if ( slices_lower_bound_int < slice_period ){
        slices_lower_bound_int = slice_period;
    }
  int defocus_samples = atoi(argv[9]);
  float defocus_lower_bound = atof(argv[10]);
  float defocus_upper_bound = atof(argv[11]);
  float defocus_period =  ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
  float  simulated_x_nm_per_pixel = atof( argv[13] );
    float  simulated_y_nm_per_pixel = atof( argv[14] );

  int roi_pixel_size = atoi( argv[15] );
  roi_x_size = roi_pixel_size;
  roi_y_size = roi_pixel_size;
  int roi_center_x = atoi( argv[16] );
  int roi_center_y = atoi( argv[17] );
  int ignore_edge_pixels = atoi ( argv[18] );
  Rect ignore_edge_pixels_rectangle;
  ignore_edge_pixels_rectangle.x = ignore_edge_pixels;
  ignore_edge_pixels_rectangle.y = ignore_edge_pixels;
  ignore_edge_pixels_rectangle.width = rows - ( 2 * ignore_edge_pixels );
  ignore_edge_pixels_rectangle.height = cols - ( 2 * ignore_edge_pixels );

  MSA_prm::MSA_prm msa_parameters;
  msa_parameters.set_electron_wavelength( 0.00196875 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
  msa_parameters.set_slice_filename_prefix ( "'test'" );
  msa_parameters.set_number_slices_to_load ( slices_load );
  msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
  msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
  msa_parameters.set_period_readout_or_detection_in_units_of_slices ( slice_period );
  msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( number_slices_to_max_thickness );
  msa_parameters.set_linear_slices_for_full_object_structure();
  msa_parameters.produce_prm("temporary_msa_im2model.prm");

  std::vector<char*> msa_vector;
  msa_vector.push_back((char*) "../bin/drprobe_clt_bin_osx/msa");
  msa_vector.push_back((char*) "-prm");
  msa_vector.push_back((char*) "temporary_msa_im2model.prm");
  msa_vector.push_back((char*) "-out");
  msa_vector.push_back((char*) "wave.wav");
  msa_vector.push_back((char*) "/ctem");
  msa_vector.push_back(0); //end of arguments sentinel is NULL

  pid_t pid;

  std::vector< std::vector<cv::Mat> > simulated_grid;

  if ((pid = fork()) == -1) // system functions also set a variable called "errno"
  {
    perror("fork"); // this function automatically checks "errno"
    // and prints the error plus what you give it
    return EXIT_FAILURE;
  }
  // ---- by when you get here there will be two processes
  if (pid == 0) // child process
  {
    execv(msa_vector[0], &msa_vector.front());
  }
  else {
    int status;
    wait(&status);
  }

  int fd;
  WAVIMG_prm::WAVIMG_prm wavimg_parameters;

  std::string wave_function_name =  "'wave_sl.wav'";
  std::string wavimg_prm_name = "temporary_wavimg_im2model.prm";
  std::string file_name_output_image_wave_function = "'image.dat'";
  // setters line 1
  wavimg_parameters.set_file_name_input_wave_function( wave_function_name );
  // setters line 2
  wavimg_parameters.set_n_columns_samples_input_wave_function_pixels( cols );
  wavimg_parameters.set_n_rows_samples_input_wave_function_pixels( rows );
  // setters line 3
  wavimg_parameters.set_physical_columns_sampling_rate_input_wave_function_nm_pixels( 0.00407f );
  wavimg_parameters.set_physical_rows_sampling_rate_input_wave_function_nm_pixels( 0.00407f );
  // setters line 4
  wavimg_parameters.set_primary_electron_energy( 200.0 );
  // setters line 5
  wavimg_parameters.set_type_of_output( 0 );
  // setters line 6
  wavimg_parameters.set_file_name_output_image_wave_function( file_name_output_image_wave_function );
  // setters line 7
  wavimg_parameters.set_n_columns_samples_output_image( cols );
  wavimg_parameters.set_n_rows_samples_output_image( rows );
  // setters line 8
  wavimg_parameters.set_image_data_type( 0 );
  wavimg_parameters.set_image_vacuum_mean_intensity( 3000.0f );
  wavimg_parameters.set_conversion_rate( 1.0f );
  wavimg_parameters.set_readout_noise_rms_amplitude( 0.0f );
  // setters line 9
  wavimg_parameters.set_switch_option_extract_particular_image_frame( 1 );
  // setters line 10
  wavimg_parameters.set_image_sampling_rate_nm_pixel( 0.00407f );
  // setters line 11
  wavimg_parameters.set_image_frame_offset_x_pixels_input_wave_function( 0.0f );
  wavimg_parameters.set_image_frame_offset_y_pixels_input_wave_function( 0.0f );
  // setters line 12
  wavimg_parameters.set_image_frame_rotation( 0.0f );
  // setters line 13
  wavimg_parameters.set_switch_coherence_model( 1 );
  // setters line 14
  wavimg_parameters.set_partial_temporal_coherence_switch( 1 );
  wavimg_parameters.set_partial_temporal_coherence_focus_spread( 4.0f );
  // setters line 15
  wavimg_parameters.set_partial_spacial_coherence_switch( 1 );
  wavimg_parameters.set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
  // setters line 16
  wavimg_parameters.set_mtf_simulation_switch( 1 );
  wavimg_parameters.set_k_space_scaling( 1.0f );
  wavimg_parameters.set_file_name_simulation_frequency_modulated_detector_transfer_function( "'../simulation/mtf/MTF-US2k-300.mtf'" );
  // setters line 17
  wavimg_parameters.set_simulation_image_spread_envelope_switch( 0 );
  wavimg_parameters.set_isotropic_one_rms_amplitude( 0.03 );
  //  wavimg_parameters.set_anisotropic_second_rms_amplitude( 0.0f );
  // wavimg_parameters.set_azimuth_orientation_angle( 0.0f );
  // setters line 18
  wavimg_parameters.set_number_image_aberrations_set( 2 );
  // setters line 19
  wavimg_parameters.add_aberration_definition ( 1, 0.0f, 0.0f );
  wavimg_parameters.add_aberration_definition ( 5, -12000.0f, 0.0f );
  // setters line 19 + aberration_definition_index_number
  wavimg_parameters.set_objective_aperture_radius( 5500.0f );
  // setters line 20 + aberration_definition_index_number
  wavimg_parameters.set_center_x_of_objective_aperture( 0.0f );
  wavimg_parameters.set_center_y_of_objective_aperture( 0.0f );
  // setters line 21 + aberration_definition_index_number
  wavimg_parameters.set_number_parameter_loops( 2 );
  wavimg_parameters.add_parameter_loop ( 1 , 1 , 1, defocus_lower_bound, defocus_upper_bound, defocus_samples, "'foc'" );
  wavimg_parameters.add_parameter_loop ( 3 , 1 , 1, slices_lower_bound_int, fp_slices_upper_bound, slice_samples, "'_sl'" );
  wavimg_parameters.produce_prm( wavimg_prm_name );

  std::vector<char*> wavimg_vector;
  wavimg_vector.push_back((char*) "../bin/drprobe_clt_bin_osx/wavimg");
  wavimg_vector.push_back((char*) "-prm");
  wavimg_vector.push_back((char*) wavimg_prm_name.c_str());
  wavimg_vector.push_back(0); //end of arguments sentinel is NULL

  if ((pid = fork()) == -1) // system functions also set a variable called "errno"
  {
    perror("fork"); // this function automatically checks "errno"
    // and prints the error plus what you give it
    return EXIT_FAILURE;
  }
  // ---- by when you get here there will be two processes
  if (pid == 0) // child process
  {
    execv(wavimg_vector[0], &wavimg_vector.front());
  }
  else {
    // Read the experimental image from file
    experimental_image = imread(argv[12] , CV_LOAD_IMAGE_GRAYSCALE );

    // initialize your temp images
    experimental_working = experimental_image.clone();

    //if fail to read the image
    if ( experimental_image.empty() ) 
    { 
      std::cout << "Error loading the experimental image" << std::endl;
      return -1; 
    }
    //Create a window
    namedWindow("Experimental Window", WINDOW_AUTOSIZE );
    setMouseCallback("Experimental Window", CallBackFunc, NULL);
    imshow("Experimental Window", experimental_image);
    std::cout << "Width : " << experimental_image.cols << std::endl;
    std::cout << "Heigth : " << experimental_image.rows << std::endl;
    roi_rectangle.x = roi_center_x  - ( roi_x_size / 2 );
    roi_rectangle.y = roi_center_y - ( roi_y_size / 2) ;
    roi_rectangle.width = roi_x_size;
    roi_rectangle.height = roi_y_size;
    experimental_working = experimental_image.clone();
    //copy the roi
    cv::Mat roi = experimental_image(roi_rectangle);
    experimental_image_roi.create( roi_x_size, roi_y_size , CV_32FC1 );
    roi.copyTo(experimental_image_roi);

    //draw the roi 
    rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
    imshow("Experimental Window", experimental_working);

    int status;
    wait(&status);

      int thickness_lower_pos = 1;
      for (int thickness = thickness_lower_pos; (slice_period * thickness) < slices_lower_bound_int; thickness ++ ){
          thickness_lower_pos = thickness;
      }
      
    for (int thickness = thickness_lower_pos; thickness <= slice_samples; thickness ++ ){
        float slice_thickness_nm = super_cell_z_nm_slice * thickness * slice_period;
        
      std::vector<cv::Mat> simulated_thickness_row;
      for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

        std::stringstream output_dat_name_stream;
        output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
        std::string file_name_output_dat = output_dat_name_stream.str();

        // Load image 
        std::cout << "opening " << file_name_output_dat << std::endl;
        fd = open ( file_name_output_dat.c_str() , O_RDONLY );
        if ( fd == -1 ){
          perror("open");
        }

        off_t fsize;
        fsize = lseek(fd, 0, SEEK_END);
        float* p;
        std::cout << "Total file size in bytes " << fsize << std::endl;

        p = (float*) mmap (0, fsize, PROT_READ, MAP_SHARED, fd, 0);

        if (p == MAP_FAILED) {
          perror ("mmap");
        }

        if (close (fd) == -1) {
          perror ("close");
        }

        cv::Mat image ( rows , cols , CV_32FC1);
        double min, max;

        int pos = 0;
        for (int col = 0; col < cols; col++) {
          for (int row = 0; row < rows; row++) {
            image.at<float>(row, col) = (float)  p[pos] ;
            pos++;
          }
        }
        cv::minMaxLoc(image, &min, &max);

        // Create a new matrix to hold the gray image
        Mat draw;
        image.convertTo(draw, CV_8U , 255.0/(max - min), -min * 255.0/(max - min));

        // Create windows
        if ( !draw.data ){
          perror("No image data \n");
          return -1;
        }

        std::stringstream output_file_image;
        output_file_image << "thickness_" << thickness * slice_period <<  "defocus_" << ( (defocus-1) * defocus_period )+ defocus_lower_bound << "_from_dat_" << file_name_output_dat << ".png" ;
        std::string file_name_image = output_file_image.str();

        // remove the ignored edge pixels
        cv::Mat ignore_removed = draw(ignore_edge_pixels_rectangle);
        imwrite( file_name_image, ignore_removed );

        // save the cleaned simulated image
        cv::Mat simulated_image;
        ignore_removed.copyTo(simulated_image);
        simulated_thickness_row.push_back( simulated_image ); 

        /// Do the Matching and Normalize
        cv::Mat simulated_image_float;
        simulated_image.convertTo(simulated_image_float, CV_8UC1 );

        /// Create the result matrix
        cv::Mat result;
        int result_cols =  experimental_image_roi.cols - simulated_image.cols + 1;
        int result_rows = experimental_image_roi.rows  - simulated_image.rows + 1;
        result.create( result_rows, result_cols, CV_8UC1 );

        /// Create the result matrix
        cv::Mat base_result;
        int base_result_cols =  experimental_image_roi.cols - simulated_image.cols + 1;
        int base_result_rows = experimental_image_roi.rows  - simulated_image.rows + 1;
        base_result.create( base_result_rows, base_result_cols, CV_8UC1 );

        // patch size
        int simulated_experimental_divider_margin = 50;
        int simulated_experimental_lower_subtitle_margin = 100;

        // vars for grid creation
        int matchMethod_grid_width  = simulated_image.cols * 6 + simulated_image.cols + simulated_experimental_divider_margin;
        int matchMethod_grid_height = simulated_image.rows + simulated_experimental_lower_subtitle_margin;
        cv::Mat matchMethod_grid;

        matchMethod_grid.create ( matchMethod_grid_height, matchMethod_grid_width , CV_8UC1 );
        matchMethod_grid = cv::Mat::zeros(matchMethod_grid_height, matchMethod_grid_width, CV_8UC1);


        // vars for minMaxLoc
        double base_minVal; double base_maxVal;
        double minVal; double maxVal; Point minLoc; Point maxLoc;
        Point matchLoc;
        double matchVal, base_matchVal;

        // vars for legend positioning
        int legend_position_x = 0;
        int legent_position_y_bottom_left_line_1 = 20;
        int legent_position_y_bottom_left_line_2 = 40;
        int legent_position_y_bottom_left_line_3 = 60;
        int legent_position_y_bottom_left_line_4 = 80;

        /// Apply the template comparison methods
        for( int i = 0; i < 6; i++ )
        {
          int compare_method = i;

          cv::matchTemplate( experimental_image_roi , simulated_image_float, result, compare_method  );
          cv::matchTemplate( experimental_image_roi , experimental_image_roi, base_result, compare_method  );

         // cv::normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

          // Localizing the best match with minMaxLoc
          cv::minMaxLoc( base_result, &base_minVal, &base_maxVal, &minLoc, &maxLoc, Mat() );
          cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

          // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
          if( compare_method  == CV_TM_SQDIFF || compare_method == CV_TM_SQDIFF_NORMED ){
            matchLoc = minLoc;
            matchVal = minVal;
            base_matchVal = base_minVal;
          }
          else{
            matchLoc = maxLoc;
            matchVal = maxVal;
            base_matchVal = base_maxVal;
          }

          const char* method_name = i == 0 ? "CV_TM_SQDIFF"
            : ( i == 1 ? "CV_TM_SQDIFF_NORMED"
                : ( i == 2 ? "CV_TM_CCORR"
                  : ( i== 3 ? ("CV_TM_CCORR_NORMED")
                    : ( i== 4 ? ("CV_TM_CCOEFF")
                      : ("CV_TM_CCOEFF_NORMED") ) ) ) );

          printf( " Method [%s] Perfect, Base-Test(1) : %f, %f \t\t at [%d,%d]\n", method_name, base_matchVal, matchVal, matchLoc.x, matchLoc.y );

          //get the roi rectangle
          Rect match_rectangle;
          match_rectangle.x = matchLoc.x;
          match_rectangle.y = matchLoc.y;
          match_rectangle.width = simulated_image.cols;
          match_rectangle.height = simulated_image.rows;

          //get the roi from the experimental image
          cv::Mat match_roi = experimental_image_roi(match_rectangle);
          // resize(match_roi,match_roi,Size(match_rectangle.width,match_rectangle.height));
          match_roi.copyTo(matchMethod_grid(Rect(match_rectangle.width + simulated_experimental_divider_margin + match_rectangle.width*i,0,match_rectangle.width,match_rectangle.height)));

          //////////////
          // LEGEND
          //////////////
          // line 1
          std::string line1_method_info = method_name;
          // line 2
          std::stringstream output_matchval;
          output_matchval <<  matchVal;
          std::string line2_matchval_info = output_matchval.str();
          // line 3
          std::stringstream base_output_matchval;
          base_output_matchval <<  "of " << base_matchVal;
          std::string line3_base_matchval_info = base_output_matchval.str();
          // line 4
          std::stringstream matchloc_output;
          matchloc_output <<  "[ " << matchLoc.x << " , " << matchLoc.y << " ]";
          std::string line4_matchloc_info = matchloc_output.str();

          // calculate the legend position on the grid
          legend_position_x = match_rectangle.width + simulated_experimental_divider_margin + match_rectangle.width*i + 10;

          putText(matchMethod_grid, line1_method_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 0.7, cvScalar(255,255,255), 1, CV_AA);
          putText(matchMethod_grid, line2_matchval_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 1.2, cvScalar(255,255,255), 1, CV_AA);
          putText(matchMethod_grid, line3_base_matchval_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 0.7, cvScalar(255,255,255), 1, CV_AA);
          putText(matchMethod_grid, line4_matchloc_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_4), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

        }

        // put the simulated image info

        //////////////
        // LEGEND
        //////////////
        // line 1
        std::stringstream output_legend_line1;
        output_legend_line1 <<  "Simulated Image";
        std::string line1_simulated_info = output_legend_line1.str();
        // line 2
        std::stringstream output_legend_line2;
        output_legend_line2 <<  "Thickness: " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) << slice_thickness_nm << "nm, slice " << thickness * slice_period;
        std::string line2_simulated_info = output_legend_line2.str();
        // line 3
        std::stringstream output_legend_line3;
        output_legend_line3 <<  "Defocus: " << ( (defocus-1) * defocus_period )+ defocus_lower_bound ;
        std::string line3_simulated_info = output_legend_line3.str();
        // line 4
        std::stringstream output_legend_line4;
        output_legend_line4 <<  "Size {" << ignore_edge_pixels << "} [" << simulated_image_float.cols << "," << simulated_image_float.rows << "]" << " {" << ignore_edge_pixels << "}";
        std::string line4_simulated_info = output_legend_line4.str();

        legend_position_x = 10;

        putText(matchMethod_grid, line1_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);
        putText(matchMethod_grid, line2_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 0.9, cvScalar(255,255,255), 1, CV_AA);
        putText(matchMethod_grid, line3_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 1.2, cvScalar(255,255,255), 1, CV_AA);
        putText(matchMethod_grid, line4_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_4), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

        simulated_image_float.copyTo(matchMethod_grid(Rect(0,0,simulated_image_float.rows,simulated_image_float.cols)));

          
        //draw the ignore rectangle
        //rectangle(draw, ignore_edge_pixels_rectangle, Scalar(0,255,255), 3);
        // namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
        // imshow( "Display window", simulated_image_float );

        namedWindow( "ROI window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow( "ROI window", matchMethod_grid ); //draw );

          // save the matchMethod grid
          std::stringstream grid_file_image;
          grid_file_image << "GRID_thickness_" << thickness * slice_period <<  "defocus_" << ( (defocus-1) * defocus_period )+ defocus_lower_bound << ".png" ;
          std::string grid_file_name_image = grid_file_image.str();
          
          imwrite( grid_file_name_image, matchMethod_grid );
          
        /// Establish the number of bins
        int histSize = 256;

        /// Set the ranges ( for grayscale or BGR (we are using grayscale) )
        float range[] = { 0, 256 } ;
        const float* histRange = { range };
        bool uniform = true; bool accumulate = false;
        /*
        // Compute the histogram for the experimental matched image:
        Mat match_roi_hist;
        cv::calcHist( &match_roi, 1, 0, Mat(), match_roi_hist, 1, &histSize, &histRange, uniform, accumulate );
        //      Normalize the result to [ 0, match_roi.rows ]
        cv::normalize(match_roi_hist, match_roi_hist, 0, match_roi.rows, NORM_MINMAX, -1, Mat() );

        // Compute the histogram for the simulated image:
        Mat simulated_image_hist;
        cv::calcHist( &simulated_image_float, 1, 0, Mat(), simulated_image_hist, 1, &histSize, &histRange, uniform, accumulate );
        //      Normalize the result to [ 0, match_roi.rows ]
        cv::normalize(simulated_image_hist, simulated_image_hist, 0, simulated_image_hist.rows, NORM_MINMAX, -1, Mat() );

        /// Apply the histogram comparison methods
        for( int i = 0; i < 4; i++ )
        {
        int compare_method = i;
        double base_base = compareHist( match_roi_hist, match_roi_hist, compare_method );
        double base_test1 = compareHist( match_roi_hist, simulated_image_hist, compare_method );

        printf( " Method [%s] Perfect, Base-Test(1) : %f, %f \n", i == 0 ? "Correlation ( CV_COMP_CORREL )" : ( i == 1 ? "Chi-Square ( CV_COMP_CHISQR )" : ( i == 2 ? "Intersection ( method=CV_COMP_INTERSECT )" : ("Bhattacharyya distance ( CV_COMP_BHATTACHARYYA )") ) ), base_base, base_test1 );
        }
        */
        waitKey(0);                                          // Wait for a keystroke in the window
      }
      simulated_grid.push_back(simulated_thickness_row);
    }
    // now that we have the simulated images lets compare them 
  }
  return 0;
}

