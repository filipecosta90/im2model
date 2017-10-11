#ifndef SRC_INTENSITY_COLUMNS_H__
#define SRC_INTENSITY_COLUMNS_H__

/* BEGIN BOOST */
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
#include <boost/filesystem.hpp>                      // for path, operator==, oper...
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/* END BOOST */

#include <string>                        // for allocator, char_traits, to_s...
#include <vector>                        // for vector, vector<>::iterator
#include <algorithm>                     // for max_element
#include <cassert>                       // for assert
#include <cmath>                         // for round
#include <cstdio>                        // for perror, NULL, SEEK_END
#include <iomanip>                       // for operator<<, setw, setfill
#include <iostream>                      // for operator<<, basic_ostream
#include <iterator>                      // for distance
#include <limits>                        // for numeric_limits
#include <fstream>                    // for ofstream
#include <utility>                     // for pair

#include <opencv2/opencv.hpp>           //
#include <opencv2/core/hal/interface.h>  // for CV_32FC1, CV_8UC1, CV_32F
#include <opencv2/core/types_c.h>        // for CvScalar, cvScalar, CvPoint
#include <opencv2/imgproc/imgproc_c.h>   // for CV_AA
#include <opencv2/imgproc/types_c.h>     // for ::CV_TM_CCOEFF_NORMED
#include <opencv2/core.hpp>              // for minMaxLoc, Exception, Hershe...
#include <opencv2/core/base.hpp>         // for Code::StsNoConv, NormTypes::...
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/highgui.hpp>           // for imshow, namedWindow, waitKey
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for putText, resize, Interpolati...
#include <opencv2/core/mat.hpp>        // for Mat
#include <opencv2/core/mat.inl.hpp>    // for Mat::~Mat
#include <opencv2/core/types.hpp>      // for Point, Point3d, Point2f, Rect
#include <opencv2/video/tracking.hpp>  // for ::MOTION_EUCLIDEAN

#include "wavimg_prm.hpp"              // for WAVIMG_prm
#include "base_crystal.hpp"
#include "base_image.hpp"
#include "application_log.hpp"

class IntensityColumns {

  private:

    // // // // //
    // simulation parameters
    // // // // //

    BaseCrystal* sim_crystal_properties = nullptr;
    bool _flag_sim_crystal_properties = false;
    BaseImage* exp_image_properties = nullptr;
    bool _flag_exp_image_properties = false;
    BaseImage* sim_image_properties = nullptr;
    bool _flag_sim_image_properties = false;

    WAVIMG_prm* wavimg_parameters = nullptr;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  public:

    IntensityColumns();
    bool read_image_from_dat_file();

    /** getters **/

    // flag getters

    bool get_flag_logger(){ return _flag_logger; }
    bool get_flag_sim_crystal_properties(){ return _flag_sim_crystal_properties; }
    bool get_flag_exp_image_properties(){ return _flag_exp_image_properties; }
    bool get_flag_sim_image_properties(){ return _flag_sim_image_properties; }

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    bool produce_png_from_dat_file();

    bool set_sim_crystal_properties ( BaseCrystal* crystal_prop );
    bool set_exp_image_properties ( BaseImage* exp_image_properties );
    bool set_sim_image_properties ( BaseImage* sim_image_properties );
    bool set_wavimg_var( WAVIMG_prm *wavimg_var );

    void print_var_state();

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    virtual std::ostream& output(std::ostream& stream) const;


};

#endif