#include "base_cell.hpp"

BaseCell::BaseCell(){
  orientation_matrix.create(3,3, CV_64FC1);
  chem_database = Chem_Database();
}

bool BaseCell::clear_atom_positions(){
  atom_positions.clear();
  _flag_atom_positions = false;
  to_unit_cell_pos.clear();
  atom_symbol_string.clear();
  atom_site_occupancy.clear();
  atom_debye_waller_factor.clear();
  atom_cpk_rgba_colors.clear();
  atom_empirical_radii.clear();
  /** .cel **/
  atom_fractional_cell_coordinates.clear();
  atoms.clear();
  symetry_atom_positions.clear();
  atom_type_symbols.clear();
  atom_occupancies.clear();
  atom_radii.clear();
  //vertex buffer for colors
  atom_rgba_colors.clear();
  return true;
}

bool BaseCell::set_cif_path( std::string path ){
  cif_path = path;
  _flag_cif_path = true;
  return true;
}

bool BaseCell::set_cel_filename( std::string filename ){
  cel_filename = filename;
  _flag_cel_filename = true;
  return true;
}

bool BaseCell::set_xyz_filename( std::string filename ){
  xyz_filename = filename;
  _flag_xyz_filename = true;
  return true;
}

bool BaseCell::set_cel_margin_nm( double margin ){
  cel_margin_Nanometers = margin;
  cel_margin_Angstroms = margin * 10.0f;
  _flag_cel_margin = true;
  return true;
}

bool BaseCell::set_length_a_Angstroms( double a ){
  length_a_Angstroms = a;
  length_a_Nanometers = a / 10.0f;
  _flag_length_a = true;
  update_length_flag();
  return true;
}

bool BaseCell::set_length_b_Angstroms( double b ){
  length_b_Angstroms = b;
  length_b_Nanometers = b / 10.0f;
  _flag_length_b = true;
  update_length_flag();
  return true;
}

bool BaseCell::set_length_c_Angstroms( double c ){
  length_c_Angstroms = c;
  length_c_Nanometers = c / 10.0f;
  _flag_length_c = true;
  update_length_flag();
  return true;
}

bool BaseCell::set_length_a_Nanometers( double a ){
  length_a_Angstroms = a * 10.0f;
  length_a_Nanometers = a;
  _flag_length_a = true;
  update_length_flag();
  return true;
}

bool BaseCell::set_length_b_Nanometers( double b ){
  length_b_Angstroms = b * 10.0f;
  length_b_Nanometers = b;
  _flag_length_b = true;
  update_length_flag();
  return true;
}

bool BaseCell::set_length_c_Nanometers( double c ){
  length_c_Angstroms = c * 10.0f;
  length_c_Nanometers = c;
  _flag_length_c = true;
  update_length_flag();
  return true;
}

void BaseCell::update_length_flag(){
  _flag_length = _flag_length_a && _flag_length_b && _flag_length_c;
}

bool BaseCell::set_angle_alpha( double alpha ){
  angle_alpha = alpha;
  _flag_angle_alpha = true;
  return true;
}

bool BaseCell::set_angle_beta( double beta ){
  angle_beta = beta;
  _flag_angle_beta = true;
  return true;
}

bool BaseCell::set_angle_gamma( double gamma ){
  angle_gamma = gamma;
  _flag_angle_gamma = true;
  return true;
}

bool BaseCell::set_cell_volume( double volume ){
  cell_volume = volume;
  return true;
}

bool BaseCell::set_zone_axis( cv::Point3d uvw ){
  zone_axis = uvw;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector ( cv::Point3d hkl ){
  upward_vector = hkl;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector_u( double u ){
  upward_vector_u = u;
  _flag_upward_vector_u = true;
  _flag_upward_vector = _flag_upward_vector_u & _flag_upward_vector_v & _flag_upward_vector_w;
  upward_vector.x = u;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector_v( double v ){
  upward_vector_v = v;
  _flag_upward_vector_v = true;
  _flag_upward_vector = _flag_upward_vector_u & _flag_upward_vector_v & _flag_upward_vector_w;
  upward_vector.y = v;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector_w( double w ){
  upward_vector_w = w;
  _flag_upward_vector_w = true;
  _flag_upward_vector = _flag_upward_vector_u & _flag_upward_vector_v & _flag_upward_vector_w;
  upward_vector.z = w;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_zone_axis_u( double u ){
  zone_axis_u = u;
  _flag_zone_axis_u = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  zone_axis.x = u;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_zone_axis_v( double v ){
  zone_axis_v = v;
  _flag_zone_axis_v = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  zone_axis.y = v;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_zone_axis_w( double w ){
  zone_axis_w = w;
  _flag_zone_axis_w = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  zone_axis.z = w;
  form_matrix_from_miller_indices();
  return true;
}

void BaseCell::form_matrix_from_miller_indices(){
  if(
      _flag_zone_axis &&
      _flag_upward_vector
    ){
    const double norm_uvw = cv::norm( zone_axis );
    const double norm_hkl = cv::norm( upward_vector );

    // Miller indices [integer representation of direction cosines]
    // can be converted to a unit vector, n, by dividing by the
    // square root of the sum of the squares: {similar for [uvw]}.
    // This is known as normalization.
    cv::Point3d vector_y_axis_projected = zone_axis / norm_uvw;
    cv::Point3d vector_z_axis_projected = upward_vector / norm_hkl;

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
    inverse_orientation_matrix = orientation_matrix.inv();
    _flag_orientation_matrix = true;
    _flag_inverse_orientation_matrix = true;

    /**
     * R is normalized: the squares of the elements in any row or column sum to 1.
     * R is orthogonal: the dot product of any pair of rows or any pair of columns is 0.
     * The rows of R represent the coordinates in the original space of unit vectors along the coordinate axes of the rotated space.
     * The columns of R represent the coordinates in the rotated space of unit vectors along the axes of the original space.
     * **/
  }
}


void BaseCell::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const BaseCell& var) {
  var.output(stream);
  return stream;
}

std::ostream& BaseCell::output(std::ostream& stream) const {
  stream << "BaseCell vars:\n"
    << "\t" << "cif_path : "  << cif_path << "\n"
    << "\t\t" << "_flag_cif_path : " << std::boolalpha << _flag_cif_path << "\n"
    << "\t\t" << "_flag_cif_path : " << std::boolalpha << _flag_cif_path << "\n"
    << "\t" << "cel_path : "  << cel_path << "\n"
    << "\t\t" << "_flag_cel_path : " << std::boolalpha << _flag_cel_path << "\n"
    << "\t\t" << "_flag_cel_format : " << std::boolalpha << _flag_cel_path << "\n"
    << "\t" << "length_a_Angstroms : "  << length_a_Angstroms << "\n"
    << "\t" << "length_b_Angstroms : "  << length_b_Angstroms << "\n"
    << "\t" << "length_c_Angstroms : "  << length_c_Angstroms << "\n"
    << "\t" << "length_a_Nanometers : "  << length_a_Nanometers << "\n"
    << "\t" << "length_b_Nanometers : "  << length_b_Nanometers << "\n"
    << "\t" << "length_c_Nanometers : "  << length_c_Nanometers << "\n"
    << "\t\t" << "_flag_length_a : " << std::boolalpha << _flag_length_a << "\n"
    << "\t\t" << "_flag_length_b : " << std::boolalpha << _flag_length_b << "\n"
    << "\t\t" << "_flag_length_c : " << std::boolalpha << _flag_length_c << "\n"
    << "\t\t" << "_flag_length : " << std::boolalpha << _flag_length << "\n"
    << "\t" << "cell_volume : "  << cell_volume << "\n"
    << "\t" << "angle_alpha : "  << angle_alpha << "\n"
    << "\t" << "angle_beta : "  << angle_beta << "\n"
    << "\t" << "angle_gamma : "  << angle_gamma << "\n"
    /** reciprocal-lattice (Miller) indices  **/
    << "\t" << "vector_t : "  << vector_t << "\n"
    // projected z-axis:
    << "\t" << "upward_vector : "  << upward_vector << "\n"
    << "\t" << "upward_vector_u : "  << upward_vector_u << "\n"
    << "\t" << "upward_vector_v : "  << upward_vector_v << "\n"
    << "\t" << "upward_vector_w : "  << upward_vector_w << "\n"
    << "\t\t" << "_flag_upward_vector_u : " << std::boolalpha << _flag_upward_vector_u << "\n"
    << "\t\t" << "_flag_upward_vector_v : " << std::boolalpha << _flag_upward_vector_v << "\n"
    << "\t\t" << "_flag_upward_vector_w : " << std::boolalpha << _flag_upward_vector_w << "\n"
    << "\t\t" << "_flag_upward_vector : " << std::boolalpha << _flag_upward_vector << "\n"
    /** Zone Axis / Lattice vector **/
    // projected y-axis
    << "\t" << "zone_axis : "  << zone_axis << "\n"
    << "\t" << "zone_axis_u : "  << zone_axis_u << "\n"
    << "\t" << "zone_axis_v : "  << zone_axis_v << "\n"
    << "\t" << "zone_axis_w : "  << zone_axis_w << "\n"
    << "\t\t" << "_flag_zone_axis_u : " << std::boolalpha << _flag_zone_axis_u << "\n"
    << "\t\t" << "_flag_zone_axis_v : " << std::boolalpha << _flag_zone_axis_v << "\n"
    << "\t\t" << "_flag_zone_axis_w : " << std::boolalpha << _flag_zone_axis_w << "\n"
    << "\t\t" << "_flag_zone_axis : " << std::boolalpha << _flag_zone_axis << "\n"
    << "\t" << "#### VECTORS: \n"
    << "\t" << "atom_positions.size() : "  << atom_positions.size() << "\n"
    << "\t\t" << "_flag_atom_positions : " << std::boolalpha << _flag_atom_positions << "\n"
    << "\t" << "to_unit_cell_pos.size() : "  << to_unit_cell_pos.size() << "\n"
    << "\t" << "atom_symbol_string.size() : "  << atom_symbol_string.size() << "\n"
    << "\t" << "atom_site_occupancy.size() : "  << atom_site_occupancy.size() << "\n"
    << "\t" << "atom_debye_waller_factor.size() : "  << atom_debye_waller_factor.size() << "\n"
    << "\t" << "atom_cpk_rgba_colors.size() : "  << atom_cpk_rgba_colors.size() << "\n"
    << "\t" << "atom_empirical_radii.size() : "  << atom_empirical_radii.size() << "\n"
    << "\t" << "atom_fractional_cell_coordinates.size() : "  << atom_fractional_cell_coordinates.size() << "\n"
    << "\t\t" << "_flag_atom_fractional_cell_coordinates : " << std::boolalpha << _flag_atom_fractional_cell_coordinates << "\n"
    /** .cel **/
    << "\t" << "min_a_atom_pos : "  << min_a_atom_pos << "\n"
    << "\t" << "max_a_atom_pos : "  << max_a_atom_pos << "\n"
    << "\t" << "min_b_atom_pos : "  << min_b_atom_pos << "\n"
    << "\t" << "max_b_atom_pos : "  << max_b_atom_pos << "\n"
    << "\t" << "min_c_atom_pos : "  << min_c_atom_pos << "\n"
    << "\t" << "max_c_atom_pos : "  << max_c_atom_pos << "\n"
    << "\t" << "fractional_norm_a_atom_pos : "  << fractional_norm_a_atom_pos << "\n"
    << "\t" << "fractional_norm_b_atom_pos : "  << fractional_norm_b_atom_pos << "\n"
    << "\t" << "fractional_norm_c_atom_pos : "  << fractional_norm_c_atom_pos << "\n"
    << "\t\t" << "_flag_fractional_norm : " << std::boolalpha << _flag_fractional_norm << "\n"
    << "\t" << "cel_margin_nm : "  << cel_margin_Nanometers << "\n"
    << "\t\t" << "_flag_cel_margin : " << std::boolalpha << _flag_cel_margin << "\n"
    << "\t" << "ab_margin : "  << ab_margin << "\n"
    << "\t" << "cel_margin_a_px : "  << cel_margin_a_px << "\n"
    << "\t" << "cel_margin_b_px : "  << cel_margin_b_px << "\n"
    << "\t" << "cel_nx_px : "  << cel_nx_px << "\n"
    << "\t" << "cel_ny_px : "  << cel_ny_px << "\n"
    // used in the simulated super-cell. this is calculated based on _cel_nXY_px - 2 * _cel_margin_AB_px
    << "\t" << "cel_wout_margin_nx_px : "  << cel_wout_margin_nx_px << "\n"
    << "\t" << "cel_wout_margin_ny_px : "  << cel_wout_margin_ny_px << "\n"
    << "\t" << "ignore_cel_margin_rectangle : \n"  << ignore_cel_margin_rectangle << "\n"
    /** Orientation **/
    << "\t" << "orientation_matrix : \n"  << orientation_matrix << "\n"
    << "\t\t" << "_flag_orientation_matrix : " << std::boolalpha << _flag_orientation_matrix << "\n"
    << "\t" << "inverse_orientation_matrix : \n"  << inverse_orientation_matrix << "\n"
    << "\t\t" << "_flag_inverse_orientation_matrix : " << std::boolalpha << _flag_inverse_orientation_matrix << "\n"
    << "\t" << "min_width_px : "  << min_width_px << "\n"
    << "\t" << "min_height_px : "  << min_height_px << "\n"
    << "\t" << "left_padding_w_margin_px : "  << left_padding_w_margin_px << "\n"
    << "\t" << "top_padding_w_margin_px : "  << top_padding_w_margin_px << "\n"
    << "\t" << "left_padding_px : "  << left_padding_px << "\n"
    << "\t" << "top_padding_px : "  << top_padding_px << "\n"
    << "\t" << "width_px : "  << width_px << "\n"
    << "\t" << "height_px : "  << height_px << "\n"
    << "\t" << "#### VECTORS: \n"
    << "\t" << "atoms.size() : "  << atoms.size() << "\n"
    << "\t" << "symetry_atom_positions.size() : "  << symetry_atom_positions.size() << "\n"
    << "\t" << "atom_type_symbols.size() : "  << atom_type_symbols.size() << "\n"
    << "\t" << "atom_occupancies.size() : "  << atom_occupancies.size() << "\n"
    << "\t" << "atom_radii.size() : "  << atom_radii.size() << "\n"
    << "\t" << "atom_rgba_colors.size() : "  << atom_rgba_colors.size() << "\n";
  return stream;
}
