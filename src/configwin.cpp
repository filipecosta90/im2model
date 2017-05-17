/*
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>                      // for error

#include <cassert>                                        // for assert
#include <cmath>                                          // for fabs, round
#include <cstdlib>                                        // for div_t, div
#include <exception>                                      // for exception
#include <iostream>                                       // for string, str...
#include <iomanip>                                        // std::setw

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread
*/
// gui includes
#include "configwin.h"
#include "ui_configwin.h"
#include "treemodel.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  /**
   *
   * The “configwin.h” file is generated by the user interface compiler (uic), 
   * using the QT5_WRAP_UI command on cmake
   * The output is directly into the output folder, 
   * so you should add INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) 
   * to your project file — otherwise, the files won’t be found. 
   * In your dialog constructor, you can init the UI now using:
   * **/
  ui->setupUi(this);
  delete ui->mainToolBar; // add this line

  QStringList headers_action;
  headers_action << tr("Field") << tr("Value");

  QStringList project_setup_image_fields;
  project_setup_image_fields << tr("Image path");
  project_setup_image_fields << tr("Sampling (nm/pixel)");
  project_setup_image_fields << tr(" x\t0");
  project_setup_image_fields << tr(" y\t0");
  project_setup_image_fields << tr("ROI");
  project_setup_image_fields << tr(" Center");
  project_setup_image_fields << tr("  x\t0");
  project_setup_image_fields << tr("  y\t0");
  project_setup_image_fields << tr(" Dimensions");
  project_setup_image_fields << tr("  width\t0");
  project_setup_image_fields << tr("  height\t0");
  //project_setup_image_fields << tr("Estimated thickness\t0");
  //project_setup_image_fields << tr("Estimated defocus\t0");

  QStringList project_setup_crystalographic_fields;
  project_setup_crystalographic_fields << tr("Unit-cell file");
  project_setup_crystalographic_fields << tr(" CIF");
  project_setup_crystalographic_fields << tr(" CEL");

  project_setup_crystalographic_fields << tr("Projected y axis");
  project_setup_crystalographic_fields << tr(" u\t0");
  project_setup_crystalographic_fields << tr(" v\t0");
  project_setup_crystalographic_fields << tr(" w\t0");

  project_setup_crystalographic_fields << tr("Projection direction");
  project_setup_crystalographic_fields << tr(" h\t0");
  project_setup_crystalographic_fields << tr(" k\t0");
  project_setup_crystalographic_fields << tr(" l\t0");

  TreeModel *project_setup_image_fields_model = new TreeModel(headers_action, project_setup_image_fields );
  TreeModel *project_setup_crystalographic_fields_model = new TreeModel(headers_action, project_setup_crystalographic_fields );

  ui->qtree_view_project_setup_image->setModel(project_setup_image_fields_model);
  ui->qtree_view_project_setup_crystallography->setModel(project_setup_crystalographic_fields_model);

  QModelIndex exp_path = project_setup_image_fields_model->index(0,1);

  QModelIndex unit_cell_index = project_setup_crystalographic_fields_model->index(0,0);
  QModelIndex cif_path = project_setup_crystalographic_fields_model->index(0,1,unit_cell_index);
  QModelIndex cel_path = project_setup_crystalographic_fields_model->index(1,1,unit_cell_index);

  ui->qtree_view_project_setup_image->setIndexWidget(exp_path,ui->qwidget_load_experimental_image);
  ui->qtree_view_project_setup_image->expandToDepth(0);

  ui->qtree_view_project_setup_crystallography->setIndexWidget(cif_path,ui->qwidget_load_cel);
  ui->qtree_view_project_setup_crystallography->setIndexWidget(cel_path,ui->qwidget_load_cif);
  ui->qtree_view_project_setup_crystallography->expandToDepth(0);

  for (int column = 0; column < project_setup_image_fields_model->columnCount(); ++column){
    ui->qtree_view_project_setup_image->resizeColumnToContents(column);
  }

  for (int column = 0; column < project_setup_crystalographic_fields_model->columnCount(); ++column){
    ui->qtree_view_project_setup_crystallography->resizeColumnToContents(column);
  }

  QStringList headers_simulation_parameters;
  headers_simulation_parameters << tr("Field") << tr("Value");

  QStringList simulation_parameters;
  simulation_parameters << tr("TD map");
  simulation_parameters << tr(" Thickness range");
  simulation_parameters << tr("  Lower Bound\t0");
  simulation_parameters << tr("  Upper Bound\t0");
  simulation_parameters << tr("  Samples\t0");
  simulation_parameters << tr(" Defocus range");
  simulation_parameters << tr("  Lower Bound\t0");
  simulation_parameters << tr("  Upper Bound\t0");
  simulation_parameters << tr("  Samples\t0");
  simulation_parameters << tr(" Incident electron beam");
  simulation_parameters << tr("  Accelaration voltage (kV)\t0");
  simulation_parameters << tr(" [] Import fixed values from refinement?");
  simulation_parameters << tr("Advanced options");
  simulation_parameters << tr(" Multislice phase grating");
  simulation_parameters << tr("  Slice filename prefix\tslc");
  simulation_parameters << tr("  Super-Cell Slicing along c-axis");
  simulation_parameters << tr("   .\t");
  simulation_parameters << tr("   .\t");
  simulation_parameters << tr("   .\t0");
  simulation_parameters << tr("  Optional Parameters");
  simulation_parameters << tr("   Apply random atomic displacements");
  simulation_parameters << tr("   Thermal vibration models");
  simulation_parameters << tr("    .");
  simulation_parameters << tr("    .");
  simulation_parameters << tr(" Electron diffraction patterns");
  simulation_parameters << tr("  Input Wavefunction file name\t");
  simulation_parameters << tr(" Image intensity distribuitions");
  simulation_parameters << tr(" Running configurations");
  simulation_parameters << tr("  Compute multislice phase grating");
  simulation_parameters << tr("  Compute electron diffraction patterns");
  simulation_parameters << tr("  Compute image intensity distribuitions");
  simulation_parameters << tr("  Compute TD map");



  TreeModel *simulation_parameters_model = new TreeModel(headers_simulation_parameters, simulation_parameters );
  ui->qtree_view_simulation_setup->setModel(simulation_parameters_model);

  //tdmap
  QModelIndex tdmap_index = simulation_parameters_model->index(0,0);

  ui->qtree_view_simulation_setup->expand(tdmap_index);

  for (int column = 0; column < simulation_parameters_model->columnCount(); ++column){
    ui->qtree_view_simulation_setup->resizeColumnToContents(column);
  }

  QStringList headers_edge_detection_parameters;
  headers_edge_detection_parameters << tr("Field") << tr("Value");

  QStringList edge_detection_parameters;
  edge_detection_parameters << tr("Edge detection");
  edge_detection_parameters << tr(" Hysteresis Thresholding");
  edge_detection_parameters << tr("Max contour distance (pixels)");

  TreeModel *edge_detection_model = new TreeModel(headers_edge_detection_parameters, edge_detection_parameters);
  ui->qtree_view_supercell_model_edge_detection_setup->setModel(edge_detection_model);

  for (int column = 0; column < edge_detection_model->columnCount(); ++column){
    ui->qtree_view_supercell_model_edge_detection_setup->resizeColumnToContents(column);
  }


  QStringList headers_supercell_model_refinement_parameters;
  headers_supercell_model_refinement_parameters << tr("Field") << tr("Value");

  QStringList supercell_model_refinement_parameters;
  supercell_model_refinement_parameters << tr("Supercell file");
  supercell_model_refinement_parameters << tr(" CIF");
  supercell_model_refinement_parameters << tr(" CEL");
  supercell_model_refinement_parameters << tr("Edge refinement");
  supercell_model_refinement_parameters << tr("Columns Refinement");
  supercell_model_refinement_parameters << tr("Calculation");
  TreeModel *supercell_model_refinement_model = new TreeModel(headers_supercell_model_refinement_parameters, supercell_model_refinement_parameters);

  ui->qtree_view_supercell_model_refinement_setup->setModel(supercell_model_refinement_model);

  for (int column = 0; column < supercell_model_refinement_model->columnCount(); ++column){
    ui->qtree_view_supercell_model_refinement_setup->resizeColumnToContents(column);
  }
}

MainWindow::~MainWindow(){
  delete ui;
}

void MainWindow::on_qpush_load_image_clicked(){
  QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open image"),
      tr("."),
      tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tif)"));
  ui->qline_image_path->setText(fileName);
}

void MainWindow::on_qpush_load_cif_clicked(){

}


void MainWindow::on_actionAbout_triggered(){

}
