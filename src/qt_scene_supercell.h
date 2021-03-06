/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SCENEMODIFIER_H
#define SCENEMODIFIER_H

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>
#include <QGoochMaterial>
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
#include <Qt3DExtras/QPhongAlphaMaterial>



#include <QComponent>
#include <QMetaObject>
#include <QMetaProperty>

#include <QtCore/QObject>
#include <QtCore/QDebug>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>
#include <QGoochMaterial>
#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>
#include <QPhongAlphaMaterial>
#include <QNormalDiffuseMapAlphaMaterial>
#include <QDiffuseSpecularMapMaterial>

#include <opencv2/core/hal/interface.h>  // for CV_8UC1, CV_32F, CV_32FC1
#include <opencv2/imgproc/imgproc_c.h>   // for cvGetSpatialMoment
#include <opencv2/imgproc/types_c.h>     // for ::CV_THRESH_BINARY, CvMoments
#include <opencv2/core.hpp>              // for minMaxLoc, normalize, Exception
#include <opencv2/core/base.hpp>         // for NormTypes::NORM_MINMAX, Code...
#include <opencv2/core/cvstd.hpp>        // for Ptr
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/core/ptr.inl.hpp>      // for Ptr::operator->, Ptr::Ptr<T>
#include <opencv2/core/version.hpp>      // for CV_MAJOR_VERSION
#include <opencv2/features2d.hpp>        // for SimpleBlobDetector::Params
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for Canny, boundingRect, drawCon...
#include <opencv2/video/tracking.hpp>    // for findTransformECC, ::MOTION_E...
#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/matx.hpp>     // for Vec4d
#include <opencv2/core/types.hpp>    // for Point3d, Point, Rect, Point2d

#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/matx.hpp>     // for Vec4d
#include <opencv2/core/types.hpp>    // for Point3d

#include <boost/shared_array.hpp>

#include "super_cell.hpp"
#include "vis/components/editorutils.h"
#include "vis/texture_image.h"

static const QString atomEntityName = QStringLiteral("__internal atom");
static const QString imageEntityName = QStringLiteral("__internal image layer");
static const QString atomMeshName = QStringLiteral("__internal atom mesh");

class QtSceneSuperCell : public QObject
{
  Q_OBJECT
  public:
    explicit QtSceneSuperCell( Qt3DCore::QEntity *rootEntity, Qt3DRender::QCamera *cameraEntity );
    ~QtSceneSuperCell();
    void set_super_cell( SuperCell* cell );
    bool add_image_layer( const cv::Mat& layer_image , double width_nm, double height_nm, Qt3DCore::QTransform* transform = nullptr, std::string layer_name = "Image Layer" );
    bool update_image_layer( const cv::Mat& layer_image , double width_nm, double height_nm , Qt3DCore::QTransform* transform = nullptr, std::string layer_name = "Image Layer", int layer_number = 1);
    bool enable_image_layer( std::string layer_name, int layer_number, bool enabled );
    bool updateAtomMeshRadius( int distinct_atom_pos, double radius );
    bool get_enable_atom_type( int distinct_atom_pos );
    bool get_helper_arrows_enable_status();
    bool get_image_layer_enable_status( int layer_number );

    bool enable_atom_type( int distinct_atom_pos, bool enabled );
    bool enable_helper_arrows( bool enabled );
    bool contains_image_layer( std::string layer_name, int layer_number );
    void load_visual_data();

    std::vector<std::string> get_atom_symbols_vec();
    double get_local_atom_empirical_radiis(int distinct_atom_pos);
    Qt3DRender::QLayer* get_xyz_axis_layer(){ return xyz_axis_layer; }
    Qt3DRender::QLayer* get_sphere_layer(){ return sphere_layer; }

    static Qt3DRender::QMesh *createArrowMesh();
    static Qt3DCore::QEntity *createArrowEntity(const QColor &color, Qt3DCore::QEntity *parent, const QMatrix4x4 &matrix, const QString &name);

    public slots:
      void reload_data_from_super_cell();
      void reloadAtomMeshRadiusVisual();

  private:
    Qt3DRender::QCamera *cameraEntity;
    Qt3DCore::QEntity *m_rootEntity;
    // helper arrows
    Qt3DCore::QEntity *m_helperArrows;
    Qt3DCore::QTransform *m_helperArrowsTransform;

    QVector<Qt3DCore::QEntity*> m_plane_entity_vector;
    QVector<Qt3DExtras::QPlaneMesh*> planeMesh_vector;
    QVector<TextureImage*> image_vector;
    QVector<Qt3DExtras::QDiffuseMapMaterial*> planeMaterial_vector;
    QVector<Qt3DCore::QTransform*> planeTransform_vector;

    Qt3DCore::QEntity *m_cylinderEntity_x;
    Qt3DCore::QEntity *m_cylinderEntity_y;
    Qt3DCore::QEntity *m_cylinderEntity_z;
    Qt3DCore::QEntity *m_axisEntity;
    Qt3DRender::QLayer *xyz_axis_layer;
    Qt3DCore::QEntity *m_modelEntity;
    Qt3DRender::QLayer *sphere_layer;

    std::vector<Qt3DCore::QEntity*> sphere_entities;

    // one mesh for every distinct atom type
    SuperCell* super_cell = nullptr;
    bool _flag_super_cell = false;

    // vis
    std::vector<bool> enabled_atom_types;
    std::vector<bool> enabled_image_layers;

};

#endif // SCENEMODIFIER_H
