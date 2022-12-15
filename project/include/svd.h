//src: https://www.codeproject.com/Articles/1268576/Singular-Values-Decomposition-SVD-In-Cplusplus11-B

#pragma once

#include "libs.h"

void jordan_gaussian_transform(
	std::vector<std::vector<double>> matrix, std::vector<double>& eigenvector);

void get_inverse_diagonal_matrix(std::vector<std::vector<double>> matrix,
	std::vector<std::vector<double>>& inv_matrix);

void get_reduced_matrix(std::vector<std::vector<double>> matrix,
	std::vector<std::vector<double>>& r_matrix, std::size_t new_size);

void get_hermitian_matrix_inverse(std::vector<double> eigenvector,
	std::vector<std::vector<double>>& ih_matrix);

void matrix_by_matrix(std::vector<std::vector<double>> matrix1,
	std::vector<std::vector<double>>& matrix2, std::vector<std::vector<double>>& matrix3);

void get_hermitian_matrix(std::vector<double> eigenvector,
	std::vector<std::vector<double>>& h_matrix);

void matrix_transpose(std::vector<std::vector<double>> matrix1,
	std::vector<std::vector<double>>& matrix2);

void compute_evd(std::vector<std::vector<double>> matrix,
	std::vector<double>& eigenvalues, std::vector<std::vector<double>>& eigenvectors, std::size_t eig_count);

void SVD(std::vector<std::vector<double>> matrix, std::vector<std::vector<double>>& s,
	std::vector<std::vector<double>>& u, std::vector<std::vector<double>>& v);
