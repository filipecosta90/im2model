
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
#include <vector>
#include <stdio.h>

// Visualization
#include <GL/glut.h>

#include "unit_cell.hpp"
#include "symbcalc.hpp"
#include "chem_database.hpp"
#include "atom_info.hpp"
#include "chem_database.hpp"

Unit_Cell::Unit_Cell(){
  orientation_matrix.create(3,3, CV_64FC1);
  chem_database = Chem_Database();
}

void Unit_Cell::extract_space_group(){

}

void Unit_Cell::set_cell_length_a( double a ){
  _cell_length_a = a;
  _cell_length_a_Nanometers = a / 10.0f;
}

void Unit_Cell::set_cell_length_b( double b ){
  _cell_length_b = b;
  _cell_length_b_Nanometers = b / 10.0f;
}

void Unit_Cell::set_cell_length_c( double c ){
  _cell_length_c = c;
  _cell_length_c_Nanometers = c / 10.0f;
}

void Unit_Cell::set_cell_angle_alpha( double alpha ){
  _cell_angle_alpha = alpha;
}

void Unit_Cell::set_cell_angle_beta( double beta ){
  _cell_angle_beta = beta;
}

void Unit_Cell::set_cell_angle_gamma( double gamma ){
  _cell_angle_gamma = gamma;
}

void Unit_Cell::set_cell_volume( double volume ){
  _cell_volume = volume;
}

void Unit_Cell::set_zone_axis_vector ( cv::Point3d uvw ){
  zone_axis_vector_uvw = uvw;
}

void Unit_Cell::set_upward_vector ( cv::Point3d hkl ){
  upward_vector_hkl = hkl;
}

void Unit_Cell::add_symmetry_equiv_pos_as_xyz(std::string xyz){
  _symmetry_equiv_pos_as_xyz.push_back(xyz);
}

void Unit_Cell::add_symmetry_equiv_pos_as_x(std::string x){
  _symmetry_equiv_pos_as_x.push_back(x);
}

void Unit_Cell::add_symmetry_equiv_pos_as_y(std::string y){
  _symmetry_equiv_pos_as_y.push_back(y);
}

void Unit_Cell::add_symmetry_equiv_pos_as_z(std::string z){
  _symmetry_equiv_pos_as_z.push_back(z);
}

void Unit_Cell::add_atom_site_type_symbol( std::string type_symbol ){
  _atoms_site_type_symbols.push_back( type_symbol );
}

void Unit_Cell::add_atom_site_fract_x( double fract_x ){
  _atoms_site_fract_x.push_back(fract_x);
}

void Unit_Cell::add_atom_site_fract_y( double fract_y ){
  _atoms_site_fract_y.push_back(fract_y);
}

void Unit_Cell::add_atom_site_fract_z( double fract_z ){
  _atoms_site_fract_z.push_back(fract_z);
}

void Unit_Cell::add_atom_site_occupancy( double occupancy ){
  _atoms_site_occupancy.push_back( occupancy );
}

/** getters **/
double Unit_Cell::get_cell_length_a_Angstroms(){
  return _cell_length_a;
}

double Unit_Cell::get_cell_length_b_Angstroms(){
  return _cell_length_b;
}

double Unit_Cell::get_cell_length_c_Angstroms(){
  return _cell_length_c;
}

double Unit_Cell::get_cell_length_a_Nanometers(){
  return _cell_length_a_Nanometers;
}

double Unit_Cell::get_cell_length_b_Nanometers(){
  return _cell_length_b_Nanometers;
}

double Unit_Cell::get_cell_length_c_Nanometers(){
  return _cell_length_c_Nanometers;
}

double Unit_Cell::get_cell_angle_alpha(){
  return _cell_angle_alpha;
}

double Unit_Cell::get_cell_angle_beta(){
  return _cell_angle_beta;
}

double Unit_Cell::get_cell_angle_gamma(){
  return _cell_angle_gamma;
}

double Unit_Cell::get_cell_volume(){
  return _cell_volume;
}

/** Zone Axis / Lattice vector **/
cv::Point3d Unit_Cell::get_zone_axis_vector_uvw(){
  return zone_axis_vector_uvw;
}

/** reciprocal-lattice (Miller) indices  **/
cv::Point3d Unit_Cell::get_upward_vector_hkl(){
  return upward_vector_hkl;
}

cv::Point3d Unit_Cell::get_vector_t ( ){
  return vector_t;
}

std::vector<cv::Point3d> Unit_Cell::get_atom_positions_vec( ){
  return _atom_positions;
}

std::vector<cv::Point3d> Unit_Cell::get_symetry_atom_positions_vec( ){
  return _symetry_atom_positions;
}

std::vector<glm::vec4> Unit_Cell::get_atom_cpk_rgba_colors_vec( ){
  return _atom_cpk_rgba_colors;
}

std::vector<double> Unit_Cell::get_atom_radii_vec( ){
  return _atom_radii;
}

std::vector<double> Unit_Cell::get_atom_empirical_radii_vec(){
  return _atom_empirical_radii;
}

std::vector<std::string>  Unit_Cell::get_atom_type_symbols_vec(){ 
  return _atom_type_symbols;
}

std::vector<double> Unit_Cell::get_atom_occupancy_vec(){
  return _atom_occupancies;
}

std::vector<double> Unit_Cell::get_atom_debye_waller_factor_vec(){
  return _atoms_debye_waller_factor;
}

cv::Mat Unit_Cell::get_orientation_matrix(){
  return orientation_matrix;
}


bool Unit_Cell::create_atoms_from_site_and_symetry(){
  int distinct = 0;
  for( int atom_site_pos = 0 ; atom_site_pos < _atoms_site_type_symbols.size(); atom_site_pos++ ) {
    distinct++;
    const double fract_x = _atoms_site_fract_x.at(atom_site_pos); 
    const double fract_y = _atoms_site_fract_y.at(atom_site_pos); 
    const double fract_z = _atoms_site_fract_z.at(atom_site_pos); 
    const std::string atom_site_type_symbol = _atoms_site_type_symbols.at(atom_site_pos);
    const double atom_occupancy = _atoms_site_occupancy.at(atom_site_pos);
    const double atom_debye_waller_factor = 0.01f;
    std::cout << "searching for "<<  atom_site_type_symbol << " in chem DB of size " << chem_database.size() << std::endl;
    Atom_Info atom_info = chem_database.get_atom_info( atom_site_type_symbol );
    double atom_radious = atom_info.empiricalRadius_Nanometers(); 
    std::string atom_type_symbol = atom_info.symbol(); 
    std::cout << "Atom symbol " << atom_type_symbol << std::endl;
    std::cout << "Atom occupancy " << atom_occupancy << std::endl;
    std::cout << "Atom radious " << atom_radious << std::endl;
    glm::vec4 cpk_color = atom_info.cpkColor();
    std::cout << "Color CPK RGBA " << cpk_color.r << " , " << cpk_color.g << " , " << cpk_color.b << " , " << cpk_color.a  << std::endl;

    for( int value_list_pos = 0; value_list_pos < _symmetry_equiv_pos_as_x.size(); value_list_pos++ ) {
      std::string symetry_x = _symmetry_equiv_pos_as_x.at(value_list_pos); 
      std::string symetry_y = _symmetry_equiv_pos_as_y.at(value_list_pos); 
      std::string symetry_z = _symmetry_equiv_pos_as_z.at(value_list_pos); 
      double temp_a = symbCalc( symetry_x , fract_x, fract_y, fract_z);
      double temp_b = symbCalc( symetry_y , fract_x, fract_y, fract_z);
      double temp_c = symbCalc( symetry_z , fract_x, fract_y, fract_z);

      if  ( temp_a > 1.0f ) {
        temp_a = temp_a - 1.0f; 
      }
      if  ( temp_a < 0.0f ) {
        temp_a = temp_a + 1.0f;
      } 

      if  ( temp_b > 1.0f ) {
        temp_b = temp_b - 1.0f; 
      }
      if  ( temp_b < 0.0f ) {
        temp_b = temp_b + 1.0f;
      } 

      if  ( temp_c > 1.0f ) {
        temp_c = temp_c - 1.0f; 
      }
      if  ( temp_c < 0.0f ) {
        temp_c = temp_c + 1.0f;
      } 

      const cv::Point3d temporary_point = cv::Point3d(temp_a, temp_b, temp_c );
      std::vector<cv::Point3d>::iterator it ;
      // check if it already exists
      it = std::find(_symetry_atom_positions.begin(), _symetry_atom_positions.end(), temporary_point );
      if(it == _symetry_atom_positions.end() ){
        std::cout << "atom # " << distinct << " ( " << atom_type_symbol << " )" << temp_a << " , " << temp_b << " , " << temp_c << std::endl;
        const double px = temp_a * _cell_length_a_Nanometers; 
        const double py = temp_b * _cell_length_b_Nanometers;
        const double pz = temp_c * _cell_length_c_Nanometers;
        cv::Point3d atom_pos ( px, py, pz );
        _atom_type_symbols.push_back( atom_type_symbol );
        _atom_positions.push_back( atom_pos );
        _atom_occupancies.push_back( atom_occupancy );
        _atoms_debye_waller_factor.push_back( atom_debye_waller_factor );
        _atom_empirical_radii.push_back( atom_radious );
        _symetry_atom_positions.push_back(temporary_point);
        //_atom_cpk_colors
        _atom_cpk_rgba_colors.push_back(cpk_color);
        distinct++;
      }
    }
  }
  return true;
}

void Unit_Cell::form_matrix_from_miller_indices (){

  const double norm_uvw = cv::norm(zone_axis_vector_uvw);
  const double norm_hkl = cv::norm(upward_vector_hkl);

  // Miller indices [integer representation of direction cosines] 
  // can be converted to a unit vector, n, by dividing by the 
  // square root of the sum of the squares: {similar for [uvw]}.  
  // This is known as normalization.
  cv::Point3d vector_y_axis_projected = zone_axis_vector_uvw / norm_uvw;
  cv::Point3d vector_z_axis_projected = upward_vector_hkl / norm_hkl; 

  // projected x-axis: y cross z
  // projected y-axis: (uvw)
  // projected z-axis: (hkl)
  cv::Mat z_axis_projected_mat ( vector_z_axis_projected , CV_64F);
  cv::Mat y_axis_projected_mat ( vector_y_axis_projected , CV_64F);

  cv::Mat x_axis_projected_mat = y_axis_projected_mat.cross( z_axis_projected_mat ); 
  const double norm_x = cv::norm( x_axis_projected_mat );
  cv::Mat mat_x_axis_projected = x_axis_projected_mat / norm_x; 
  cv::Point3d vector_x_axis_projected; 
  vector_x_axis_projected.x = mat_x_axis_projected.at<double>(0,0);
  vector_x_axis_projected.y = mat_x_axis_projected.at<double>(1,0);
  vector_x_axis_projected.z = mat_x_axis_projected.at<double>(2,0);

  /* insert into matrix */
  std::vector<cv::Point3d> points;
  points.push_back(vector_x_axis_projected);
  points.push_back(vector_y_axis_projected);
  points.push_back(vector_z_axis_projected);
  orientation_matrix = cv::Mat( points , true ); 
  orientation_matrix = orientation_matrix.reshape(1);

  /**
   * R is normalized: the squares of the elements in any row or column sum to 1.
   * R is orthogonal: the dot product of any pair of rows or any pair of columns is 0.
   * The rows of R represent the coordinates in the original space of unit vectors along the coordinate axes of the rotated space. 
   * The columns of R represent the coordinates in the rotated space of unit vectors along the axes of the original space.
   * **/
  //  std::cout << "Orientation Matrix :" << std::endl;
  //  std::cout << orientation_matrix << std::endl;

}
/* deprecated
 *
 void Unit_Cell::orientate_atoms_from_matrix(){
 std::cout << "Orientating atoms from matrix :" << std::endl;
 std::vector<glm::vec3>::iterator it ;
 for ( int pos = 0; pos <  _atom_positions.size(); pos++ ){
 glm::vec3 initial_atom = _atom_positions.at(pos);
 cv::Vec3d V ( initial_atom.x, initial_atom.y, initial_atom.z );
 cv::Mat result = orientation_matrix * cv::Mat(V);
 const glm::vec3 final (result.at<double>(0,0), result.at<double>(1,0), result.at<double>(2,0));
 std::cout << "initial: " << V << " final: " << final.x << " " << final.y << " " <<  final.z  << std::endl;
 result.release();
 _atom_positions.at(pos) =  final; 
 }
 }

 void Unit_Cell::set_chem_database( Chem_Database* chem_db ){

 }

*/


