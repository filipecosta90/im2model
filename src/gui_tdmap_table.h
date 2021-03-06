/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef TDMap_Table_H
#define TDMap_Table_H

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

#include "gui_tdmap_cell.h"
#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_cell_widget.h"
#include "cv_image_frame.h"
#include "td_map.hpp"
#include "cv_image_delegate.h"
#include "td_map.hpp"
#include "treeitem.h"
#include "cv_tdmap_cell_image_frame_delegate.h"
#include "base_logger.hpp"

class TDMap_Cell;
class TDMap_TableCompare;

class TDMap_Table : public QTableWidget
{
  Q_OBJECT

  public:
    TDMap_Table(QWidget *parent = 0);

    QString currentLocation() const;
    QTableWidgetSelectionRange selectedRange() const;
    void resizeEvent(QResizeEvent* ev);
    void clear();

    void set_tdmap( TDMap* map );
    void set_simulated_images_grid( std::vector< std::vector<cv::Mat> > image_grid );

    /* signal connecting to treeitems with simulation configuration info */
    void connect_thickness_range_number_samples_changes( const TreeItem* item, int item_changes_column );
    void connect_defocus_range_number_samples_changes( const TreeItem* item, int item_changes_column );
    void connect_item_changes_to_invalidate_grid( const TreeItem* item, int item_changes_column );

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    /* masking qfocusevent */
    void connect_focus_event( QFocusEvent *event, int row, int col );
void connect_export_overlay( int row, int col );
void connect_export_simulated_image( int row, int col );

public slots:
    void selectCurrentRow();
    void selectCurrentColumn();
    void update_RowCount_from_thickness_range_number_samples(int signal_item_changed_column );
    void update_ColumnCount_from_defocus_range_number_samples(int signal_item_changed_column );
    void invalidate_grid(int signal_item_changed_column );

signals:
    void modified();
    void tdmap_best_match( int x, int y );
    void export_overlay( int x, int y );
    void export_simulated_image( int x, int y );

    private slots:
      void somethingChanged();

  private:
    enum { MagicNumber = 0x7F51C883 };

    /* class related vars */
    int ColumnCount = 0;
    int ColumnSize = 150;
    //int VerticalHeaderSize = 0;
    //int HorizontalHeaderSize = 0;
    int RowCount = 0;
    int RowSize = 50;
    int _number_drawed_cells = 0;
    int _number_calculated_cells = 0;
    bool _flag_created_cells = false;

    void update_headers_rows();
    void update_headers_columns();
    void update_headers();
    void create_cells();
    void update_cells();
    void update_column_size();
    void update_row_size();

    TDMap_Cell*cell(int row, int column) const;

    /* Make a member pointer to a new MyDelegate instance */
    CvTDMapImageFrameDelegate *image_delegate;
    QWidget *table_parent;

    /* CORE IM2MODEL*/
    TDMap* core_tdmap = nullptr;
    bool _flag_core_tdmap = false;


    /* VISUAL TDMAP vars */
    int _treeitem_thickness_range_number_samples_watch_col;
    int _treeitem_defocus_range_number_samples_watch_col;
    int _treeitem_changes_to_invalidate_grid_watch_col;

    bool _flag_simulated_image_grid = false;
    std::vector< std::vector<cv::Mat> > simulated_image_grid;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

};

#endif

