#include <QtGui>
#include "treeitem.h"
#include "treemodel.h"
#include <iostream>

TreeModel::TreeModel(TreeItem *root,  QObject *parent) : QAbstractItemModel(parent) {
  rootItem = root;
  // to know if data changed in model
  connect(this, SIGNAL( dataChanged(QModelIndex,QModelIndex) ) , this, SLOT( set_model_modified() ) );
}

TreeModel::~TreeModel(){
  delete rootItem;
}

boost::property_tree::ptree* TreeModel::save_data_into_property_tree( ){
  // Create an empty property tree object
  boost::property_tree::ptree* pt_root =  rootItem->save_data_into_property_tree( );
  return pt_root;
}

bool TreeModel::load_data_from_property_tree( boost::property_tree::ptree pt_root ){
  // Load from the property tree object
  rootItem->load_data_from_property_tree( pt_root );
  emit layoutChanged();
  return true;
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const{
  return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const{
  /*  std::cout << " data being called for role: "
      << ( ( role == Qt::DisplayRole) ? "Qt::DisplayRole" : ( role == Qt::EditRole) ? "Qt::EditRole" : "" )  << role << std::endl;

      if ( role != Qt::DisplayRole && role != Qt::CheckStateRole ){
      return QVariant();
      }*/
  if (!index.isValid()){
    return QVariant();
  }
  TreeItem *item = getItem(index);
  return item->data( index.column() , role  );
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const{
  if (!index.isValid()){
    return Qt::NoItemFlags;
  }
  TreeItem *item = getItem(index);
  Qt::ItemFlags flags =  QAbstractItemModel::flags(index);
  if( item->isItemEditable( index.column() ) ){
    flags |= Qt::ItemIsEditable;
  }
  if( item->isItemCheckable(index.column() ) ){
    flags |= Qt::ItemIsUserCheckable;
  }
  return flags;
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const{
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item){
      return item;
    }
  }
  return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
    return rootItem->data(section,role);
  }
  return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid() && parent.column() != 0){
    return QModelIndex();
  }
  TreeItem *parentItem = getItem(parent);
  TreeItem *childItem = parentItem->child(row);
  if (childItem){
    return createIndex(row, column, childItem);
  }
  else{
    return QModelIndex();
  }
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent){
  bool success = false;

  beginInsertColumns(parent, position, position + columns - 1);
  success = rootItem->insertColumns(position, columns);
  endInsertColumns();

  return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent){
  TreeItem *parentItem = getItem(parent);
  bool success = false;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const{
  if (!index.isValid()){
    return QModelIndex();
  }

  TreeItem *childItem = getItem(index);
  TreeItem *parentItem = childItem->parent();

  if (parentItem == rootItem){
    return QModelIndex();
  }
  return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent){
  bool success = false;

  beginRemoveColumns(parent, position, position + columns - 1);
  success = rootItem->removeColumns(position, columns);
  endRemoveColumns();

  if (rootItem->columnCount() == 0){
    removeRows(0, rowCount());
  }

  return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent){
  TreeItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const{
  TreeItem *parentItem = getItem(parent);
  return parentItem->childCount();
}

bool TreeModel::appendData(const QModelIndex &index, const QVariant &value){
  bool result = false;
  TreeItem *item = getItem(index);
  std::cout << "appending data to item in index pos: row " << index.row() << " col "<< index.column()   << std::endl;
  result = item->appendData( index.column(), value);
  if (result){
    emit dataChanged(index, index);
    emit layoutChanged();
  }
  return result;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role ){
  std::cout << "\tsetData being called for role: " << role << std::endl;
  TreeItem *item = getItem(index);

  if ( (role != Qt::EditRole) && ( role != Qt::CheckStateRole ) ){
    return false;
  }

  const bool result = item->setData(index.column(), value, role );
  if (result){
    emit dataChanged(index, index);
  }
  return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role){
  if (role != Qt::EditRole || orientation != Qt::Horizontal){
    return false;
  }
  bool result = rootItem->setData(section, value, role );
  if (result){
    emit headerDataChanged(orientation, section, section);
  }
  return result;
}

bool TreeModel::_was_model_modified(){
  return _flag_model_modified;
}

void TreeModel::set_model_modified(){
  _flag_model_modified = true;
}
