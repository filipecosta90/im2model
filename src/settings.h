#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>



#include <QMainWindow>
#include <QtWidgets>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

#include <boost/process.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>


#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>

#include <boost/process/error.hpp>
#include <boost/process/async.hpp>
#include <boost/process/io.hpp>
#include <boost/process/child.hpp>


#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
#include <boost/process/args.hpp>
#include <boost/process/child.hpp>
#include <boost/process/async_pipe.hpp>
#include <system_error>

#include <boost/filesystem.hpp>

#include <boost/thread.hpp>
#include <future>


#include "image_crystal.hpp"
#include "td_map.hpp"
#include "treemodel.h"
#include "treeitem.h"
#include "cv_image_widget.h"
#include "treeitem_file_delegate.hpp"
#include "gui_sim_updater.hpp"
#include "gui_stream.hpp"
#include "cv_image_table_frame.h"

#include "settings.h"
#include "ui_settings.h"


namespace Ui {
  class Settings;
}

class Settings : public QDialog
{
  Q_OBJECT

  public:
    bool _is_save_preferences();
    bool set_q_settings_fileName( std::string filename );
    bool set_dr_probe_bin_path( std::string path );
    bool set_dr_probe_celslc_bin( std::string bin );
    bool set_dr_probe_msa_bin( std::string bin );
    bool set_dr_probe_wavimg_bin( std::string bin );

    QString get_dr_probe_bin_path( );
    QString get_dr_probe_celslc_bin( );
    QString get_dr_probe_msa_bin( );
    QString get_dr_probe_wavimg_bin( );

    explicit Settings(QWidget *parent = 0);
    void produce_settings_panel();

    ~Settings();

    private slots:
      void on_buttonBox_accepted();

  private:

    Ui::Settings *ui;
    QString _dr_probe_bin_path;
    QString _dr_probe_celslc_bin;
    QString _dr_probe_msa_bin;
    QString _dr_probe_wavimg_bin;
    QString _q_settings_fileName;

    // project preferences model
    TreeModel *project_preferences_model;
    TreeItem* _preferences_dr_probe_celslc;
    TreeItem* _preferences_dr_probe_msa;
    TreeItem* _preferences_dr_probe_wavimg;

    TreeItem* _preferences_dr_probe_path;
    bool _save_preferences = false;

};

#endif // SETTINGS_H


