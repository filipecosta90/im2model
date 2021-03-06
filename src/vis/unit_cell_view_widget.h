/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_VIS_UNITCELLVIEWERWINDOW_H__
#define SRC_VIS_UNITCELLVIEWERWINDOW_H__


#include <QMainWindow>
#include <QtWidgets>

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtGui/QScreen>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <Qt3DInput/QInputAspect>
#include <QFrameGraphNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>

#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qpointlight.h>

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>
#include <QSceneLoader>
#include <QTechniqueFilter>
#include <QViewport>
#include <QClearBuffers>
#include <QCameraSelector>
#include <QRenderPassFilter>
#include <QSceneLoader>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qforwardrenderer.h>

#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>

/** END BOOST **/
#include "inputcontrols/qtrackballcameracontroller.h"
#include "../qt_scene_supercell.h"
#include "../treemodel.h"
#include "../treeitem.h"
#include "../treeitem_file_delegate.hpp"
#include "../application_log.hpp"

#include <QWidget>
#include <QQuickWidget>
#include <QQuickView>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QQuickItem>

QT_FORWARD_DECLARE_CLASS(QRadioButton)
QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QLayout)

class UnitCellViewerWindow : public QWidget {
  Q_OBJECT
public:
  UnitCellViewerWindow(QWidget *parent = 0, bool vertical = false  );
  void set_super_cell( SuperCell* , bool bind_orientation = true );
  bool update_image_layer( cv::Mat layer_image , double width_nm, double height_nm , Qt3DCore::QTransform* transform = nullptr, std::string layer_name = "Image Layer", int layer_number = 1);
  bool add_image_layer( cv::Mat layer_image, double width_nm, double height_nm, Qt3DCore::QTransform* transform = nullptr );
  void create_standard_atom_options();
        /* Loggers */
  bool set_application_logger( ApplicationLog::ApplicationLog* logger );

  TreeModel* get_atom_info_fields_model(){ return atom_info_fields_model; }

  public slots:
  void init();
  void update_lightEntity_view_vector(const QVector3D &viewVector);
  void update_lightEntity_position(const QVector3D &pos);
  void update_m_cameraEntity_frustum(const QVector3D &pos);
  void update_m_cameraEntity_frustum_to_cam_pos( const int &i);

  void update_cameraEntity_zone_axis();
  void update_cameraEntity_upward_vector();
  void update_m_cameraEntity_centerDistance();
  void view_along_ZA_UV();
  void view_along_a_axis();
  void view_along_b_axis();
  void view_along_c_axis();
  void fit_model();
  void reload_data_from_super_cell();
  void reload_visual_data_from_super_cell();

protected:

      //Layout
  QSplitter *split1;
  QSplitter *split2;
  QWidget *split1_container;
  QWidget *toolbar_and_3d_container;
  QVBoxLayout *split1_container_layout;
  QLayout *m_containerLayout;
  QVBoxLayout *full_layout;
  QWidget *container;
  QBoxLayout *toolsLayout;
  QToolBar *toolbar;
  QTreeView *atom_info_tree_view;
      /* Delegates */
  TreeItemFileDelegate *atom_info_tree_view_delegate;

      //Core vis
  Qt3DCore::QEntity *_m_rootEntity;
  Qt3DExtras::Qt3DWindow *qt_scene_view;
  QtSceneSuperCell* qt_scene_super_cell = nullptr;
  SuperCell* super_cell = nullptr;
  bool _flag_super_cell = false;
  Qt3DRender::QCamera *_m_cameraEntity;
  Qt3DRender::QDirectionalLight* _m_cameraLight;
  Qt3DCore::QTransform *_m_lightTransform;
  double _m_cameraEntity_centerDistance = 10.0f;
  QVector3D q_zone_axis_vector;
      //Atom properties
  TreeModel *atom_info_fields_model = nullptr;
  TreeItem* display_root = nullptr;
  TreeItem* layer_display_root = nullptr;
  TreeItem* model_display_root = nullptr;
  TreeItem* atom_properties_root = nullptr;
  TreeItem* atom_radius_root = nullptr;
  std::vector<TreeItem*> atom_radius_items;
  TreeItem* atom_color_root = nullptr;

  /* Loggers */
  ApplicationLog::ApplicationLog* logger = nullptr;
  bool _flag_logger = false;

private:


};

#endif
