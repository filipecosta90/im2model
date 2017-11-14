#include "cv_image_widget.h"

CVImageWidget::CVImageWidget(QWidget *parent ) : QWidget(parent) , scaleFactor(1), _tmp_original(cv::Mat()), _tmp_current(cv::Mat()) {
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(const QPoint &)));
}


QSize CVImageWidget::sizeHint() const {
  return _qimage.size();
}
QSize CVImageWidget::minimumSizeHint() const {
  return _qimage.size();
}

void CVImageWidget::set_container_window_size( const int width , const int height ){
  _container_window_width = width;
  _container_window_height = height;
}

void CVImageWidget::zoomIn(){
  scaleFactor += 0.10;
  updateImage();
}
void CVImageWidget::zoomOut(){
  scaleFactor -= 0.10;
  if(scaleFactor < 0.10 ){scaleFactor = 0.10;}
  updateImage();
}
void CVImageWidget::normalSize(){
  scaleFactor = 1;
  updateImage();
}

void CVImageWidget::fitToWindow(){
  int window_width = _container_window_width;
  int window_height = _container_window_height;
  // prevent division by zero
  float w_factor = original_size.width > 0 ? ((float) window_width) / ((float) original_size.width ) : 0.0f;
  float h_factor = original_size.height > 0 ? ((float) window_height) / ((float) original_size.height ) : 0.0f;
  scaleFactor = std::min( w_factor , h_factor );
  updateImage();
}

void CVImageWidget::setImage( const cv::Mat& image ){
  // Convert the image to the RGB888 format
  switch (image.type()) {
    case cv::DataType<unsigned char>::type:
    cvtColor(image, _tmp_original, CV_GRAY2RGB);
    break;
    case cv::DataType<unsigned short>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type, 1.0f/255.0f);
      cvtColor(temp, _tmp_original, CV_GRAY2RGB );
      break;
    }
    case CV_8UC3:
    cvtColor(image, _tmp_original, CV_BGR2RGB);
    break;
  }

  // QImage needs the data to be stored continuously in memory
  assert( _tmp_original.isContinuous() );
  original_size = image.size();
  current_size = original_size;
  // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
  // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
  // has three bytes.
  _qimage = QImage(_tmp_original.data, _tmp_original.cols, _tmp_original.rows, _tmp_original.cols*3, QImage::Format_RGB888);
  this->setFixedSize(image.cols, image.rows);
  _image_set = true;

}

void CVImageWidget::updateImage() {
  if( _image_set ){
    // Convert the image to the RGB888 format
    current_size = cv::Size(original_size.width * scaleFactor , original_size.height * scaleFactor);
    cv::resize(_tmp_original,_tmp_current,current_size);//resize image

    // QImage needs the data to be stored continuously in memory
    assert(_tmp_current.isContinuous());
    // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
    // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
    // has three bytes.
    _qimage = QImage(_tmp_current.data, _tmp_current.cols, _tmp_current.rows, _tmp_current.cols*3, QImage::Format_RGB888);
    this->setFixedSize(_tmp_current.cols, _tmp_current.rows);
  }
}

void CVImageWidget::ShowContextMenu(const QPoint &pos){
  // Handle global position
  QPoint globalPos = this->mapToGlobal(pos);

  // Create menu and insert some actions
  QMenu areas_menu;

  for( int list_position = 0; list_position < renderAreas.size() ; list_position++ ){
    QString _path_description =renderAreas_description.at(list_position);
    const bool _path_visible = renderAreas_visible.at(list_position);

    QAction *pathAction = new QAction( _path_description );
    if(_path_visible){
      connect( pathAction, &QAction::triggered, [=](){ this->renderAreas_visible[list_position] = false; this->repaint(); });
    }
    else {
      connect( pathAction, &QAction::triggered, [=](){ this->renderAreas_visible[list_position] = true; this->repaint(); });
    }
    pathAction->setCheckable(true);
    pathAction->setChecked( _path_visible );
    areas_menu.addAction( pathAction );
  }

  // Show context menu at handling position
  areas_menu.exec(globalPos);
}

void CVImageWidget::addRect( cv::Rect _area_rect, int penWidth, QString description ){
  addRect(  _area_rect ,  penWidth, renderAreas_default_color, description );
}

void CVImageWidget::addRect( cv::Rect _area_rect, int penWidth, cv::Vec3b penColor, QString description ){
  renderAreas_description.push_back(description);
  renderAreas_visible.push_back(true);
  renderAreas_top_left.push_back(cv::Point2i( _area_rect.x, _area_rect.y ));
  QPainterPath rectPath;
  rectPath.addRect( 0, 0 , _area_rect.width, _area_rect.height );
  renderAreas.push_back(rectPath);
  renderAreas_penWidth.push_back( penWidth );
  renderAreas_penColor.push_back( QColor(penColor[0], penColor[1], penColor[2]) );
}

void CVImageWidget::addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth , QString description ){
  addShapePolygon( polygon , top_left,  penWidth, renderAreas_default_color , description );
}

void CVImageWidget::addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, cv::Vec3b penColor , QString description ){
  renderAreas_description.push_back(description);
  renderAreas_visible.push_back(true);
  renderAreas_top_left.push_back(top_left);
  QPainterPath polyPath;
  QPolygon poly;

  for( int point_n = 0; point_n < polygon.size(); point_n++ ){
    const cv::Point2i _cv_point  = polygon.at(point_n);
    poly.push_back( QPoint(_cv_point.x, _cv_point.y) );
  }
  polyPath.addPolygon( poly );
  renderAreas.push_back( polyPath );
  renderAreas_penWidth.push_back( penWidth  );
  renderAreas_penColor.push_back( QColor(penColor[0], penColor[1], penColor[2]) );
}


void CVImageWidget::addRenderPoints( std::vector<cv::Point2i> points , int penWidth, cv::Vec3b penColor, QString description ){
  for( auto pt : points ) { 
    renderPoints.push_back( QPoint (pt.x, pt.y) );
  }
}


void CVImageWidget::cleanRenderAreas(){
  renderAreas.clear();
  renderAreas_penWidth.clear();
  renderAreas_penColor.clear();
  renderAreas_top_left.clear();
}

void CVImageWidget::paintEvent(QPaintEvent* event) {
  // Display the image
  QPainter painter(this);
  painter.drawImage(QPoint(0,0), _qimage);

  // Draw the paths
  for( int list_position = 0; list_position < renderAreas.size() ; list_position++ ){
    const bool _area_visible = renderAreas_visible.at( list_position );
    if( _area_visible ){
      cv::Point2i _top_left = renderAreas_top_left.at( list_position );
      _top_left *= scaleFactor;
      QPainterPath _area = renderAreas.at(list_position);
      const int _area_pen_width = renderAreas_penWidth.at(list_position);
      QColor _area_pen_color = renderAreas_penColor.at(list_position);
      painter.save();
      painter.translate( _top_left.x, _top_left.y );
      painter.setPen(QPen(_area_pen_color, _area_pen_width, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
      painter.scale( scaleFactor, scaleFactor );
      painter.drawPath(_area);
      painter.restore();
    }
  }

  // Draw selection rectangle
  if( _started_rectangleSelection ){
    painter.setPen(QPen(QBrush(QColor(0,0,0,180)),1,Qt::DashLine));
    painter.setBrush(QBrush(QColor(255,255,255,120)));
    painter.drawRect(selectionRect);
  }

  // Draw statistical selection rectangle
  if( _started_rectangleSelectionStatistical ){
    painter.setPen(QPen(QBrush(QColor(0,0,0,180)),1,Qt::DashLine));
    painter.setBrush(QBrush(QColor(255,255,255,120)));
    painter.drawRect(selectionStatisticalRect);
  }

  painter.drawPolyline(renderPoints.data(), static_cast<int>(renderPoints.size()));

  painter.end();
}

void CVImageWidget::startRectangleSelection(){
  _enabled_rectangleSelection = true;
  setCursor( Qt::CrossCursor );
}


void CVImageWidget::startStatisticalRectangleSelection(){
  _enabled_rectangleSelectionStatistical = true;
  setCursor( Qt::CrossCursor );
}

void CVImageWidget::mousePressEvent(QMouseEvent *e){
  if ( e->button()==Qt::LeftButton ){
    if( _enabled_rectangleSelection ){
      _started_rectangleSelection = true;
      selectionRect.setTopLeft(e->pos());
      selectionRect.setBottomRight(e->pos());
    }
    if( _enabled_rectangleSelectionStatistical ){
      _started_rectangleSelectionStatistical = true;
      selectionStatisticalRect.setTopLeft(e->pos());
      selectionStatisticalRect.setBottomRight(e->pos());
    }
  }
}

void CVImageWidget::mouseMoveEvent(QMouseEvent *e){
  if ( _started_rectangleSelection ){
    selectionRect.setBottomRight(e->pos());
    repaint();
  }
  if ( _started_rectangleSelectionStatistical ){
    selectionStatisticalRect.setBottomRight(e->pos());
    repaint();
  }
}

QRect CVImageWidget::mapSelectionRectToOriginalSize(){
  QRect original_size_selectionRect;
  original_size_selectionRect.setX( selectionRect.x() / scaleFactor );
  original_size_selectionRect.setY( selectionRect.y() / scaleFactor );
  original_size_selectionRect.setWidth( selectionRect.width() / scaleFactor );
  original_size_selectionRect.setHeight( selectionRect.height() / scaleFactor );
  return original_size_selectionRect;
}

QRect CVImageWidget::mapSelectionStatisticalRectToOriginalSize(){
  QRect original_size_selectionRect;
  original_size_selectionRect.setX( selectionStatisticalRect.x() / scaleFactor );
  original_size_selectionRect.setY( selectionStatisticalRect.y() / scaleFactor );
  original_size_selectionRect.setWidth( selectionStatisticalRect.width() / scaleFactor );
  original_size_selectionRect.setHeight( selectionStatisticalRect.height() / scaleFactor );
  return original_size_selectionRect;
}

void CVImageWidget::mouseReleaseEvent(QMouseEvent *e){
  if( _started_rectangleSelection ){
    _started_rectangleSelection = false;
    _enabled_rectangleSelection = false;
    setCursor( Qt::ArrowCursor );

    //make sure the selection rectangle does not surpases the original image size
    if( selectionRect.bottom() > current_size.height ){
      selectionRect.setBottom( current_size.height );
    }
    if( selectionRect.right() > current_size.width ){
      selectionRect.setRight( current_size.width );
    }
    const QRect original_size_selectionRect = mapSelectionRectToOriginalSize();
    emit selectionRectangleChanged( original_size_selectionRect );
  }

  if( _started_rectangleSelectionStatistical ){
    _started_rectangleSelectionStatistical = false;
    _enabled_rectangleSelectionStatistical = false;
    setCursor( Qt::ArrowCursor );

    //make sure the selection rectangle does not surpases the original image size
    if( selectionStatisticalRect.bottom() > current_size.height ){
      selectionStatisticalRect.setBottom( current_size.height );
    }
    if( selectionStatisticalRect.right() > current_size.width ){
      selectionStatisticalRect.setRight( current_size.width );
    }
    const QRect original_size_selectionStatisticalRect = mapSelectionStatisticalRectToOriginalSize();
    emit selectionStatisticalRectangleChanged( original_size_selectionStatisticalRect );
  }

  repaint();
}
