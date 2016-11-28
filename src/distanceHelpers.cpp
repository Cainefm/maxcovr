#include <Rcpp.h>
#include <iostream>
#include <math.h>
using namespace Rcpp;

//' Convert degrees to radians
//'
//' @param deg degrees
//'
//' @return radians
//'
//' @export
// [[Rcpp::export]]
double deg2rad_cpp(double deg) {
    return ((deg*M_PI)/180);
} // End deg2rad


//' Calculate distance using haversines formula
//'
//' @param lat1 latitude from the first location
//' @param long1 longitude from the first location
//' @param lat2 latitude from the second location
//' @param long2 longitude from the second location
//'
//' @return distance in metres between two locations
//'
//' @export
// [[Rcpp::export]]
double spherical_distance_cpp(double lat1,
                              double long1,
                              double lat2,
                              double long2) {

    int radius_earth = 6371;

    // convert angle values into radians
    lat1 = deg2rad_cpp(lat1);
    long1 = deg2rad_cpp(long1);
    lat2 = deg2rad_cpp(lat2);
    long2 = deg2rad_cpp(long2);

    // Determine distance using the haversine formula, assuming a spherical earth
    double a = pow(sin((lat2 - lat1) / 2), 2) + (cos(lat1) * cos(lat2)) * pow(sin((long2 - long1) / 2), 2);

    double d = 2 * atan2(sqrt(a), sqrt(1 - a)) * radius_earth;

    // return distance in metres
    d = d * 1000;

    return d;

}
//' Create a matrix of distances between two areas
//'
//' @param facility a matrix with longitude and latitude in the first two columns
//' @param user a matrix with longitude and latitude in the first two columns
//'
//' @return a matrix of distances in metres between each user and facility,
//' with nrow(user) rows and nrow(facility) columns.
//'
//' @export
//'
// [[Rcpp::export]]
NumericMatrix distance_matrix_cpp(NumericMatrix facility,
                                  NumericMatrix user){

    int n1 = user.nrow();
    int n2 = facility.nrow();

    NumericMatrix dist_mat(n1, n2);

    for(int i = 0; i < n1; i++){
        for(int j = 0; j < n2; j++){
            dist_mat(i,j) = spherical_distance_cpp(user(i, 0), //lat
                                                   user(i, 1), //long
                                                   facility(j, 0), //lat
                                                   facility(j, 1)); //long);
        }
    }

return dist_mat;

}

//' Create a binary matrix TRUE if distance satisfies a condition
//'
//' @param facility a matrix with longitude and latitude in the first two columns
//' @param user a matrix with longitude and latitude in the first two columns
//' @param distance_cutoff
//'
//' @return a logical matrix 1 if distance between element[i,j] is less than or
//' equal to the distance_cutoff, and 0 otherwise
//'
//' @export
//'
// [[Rcpp::export]]
IntegerMatrix binary_matrix_cpp(NumericMatrix facility,
                                NumericMatrix user,
                                double distance_cutoff){


    int n1 = user.nrow();
    int n2 = facility.nrow();

    IntegerMatrix bin_mat(n1,n2);
    NumericMatrix dist_mat(n1, n2);

    dist_mat = distance_matrix_cpp(facility, user);

    for(int i = 0; i < n1; i++){
        for(int j = 0; j < n2; j++){
            if (dist_mat(i,j) <= distance_cutoff){
                bin_mat(i,j) = 1;
            } else if (dist_mat(i,j) > distance_cutoff){
                bin_mat(i,j) = 0;
            }
        }
    }

    return bin_mat;

}

/*** R
dist_cpp <- spherical_distance_cpp(lat1 = 46.19616,
                                   long1 = 8.731278,
                                   lat2 = 46.16850,
                                   long2 = 9.004392)

dist_maxcovr <- maxcovr::spherical_distance(lat1 = 46.19616,
                                                long1 = 8.731278,
                                                lat2 = 46.16850,
                                                long2 = 9.004392)

# dist_cpp
# dist_maxcovr

# dist_cpp == dist_maxcovr

facility_test_cpp <- as.matrix(tibble::tribble(
    ~lat_facility, ~long_facility, ~facility_id, ~key,
    46.19616,      8.731278,           1,     1,
    46.16757,      9.027957,           2,     1
))

user_test_cpp <- as.matrix(tibble::tribble(
            ~lat_user, ~long_user, ~key, ~user_id,
            46.16850,  9.004392,     1,       1,
            46.17690,  8.822994,     1,       2,
            46.17690,  8.822994,     1,       3,
            46.17690,  8.822994,     1,       4,
            46.17690,  8.822994,     1,       5,
            46.01372,  8.963890,     1,       6,
            46.15254,  8.773423,     1,       7,
            45.92970,  8.921419,     1,       8,
            45.92970,  8.921419,     1,       9,
            46.00018,  8.946929,     1,      10
    ))


# spherical_distance_cpp(lat1 = 46.19616, long1 = 9.027957,
#                        lat2 = 46.00018, long2 = 8.946929)
#
# 30368.89

my_dist_cpp <- distance_matrix_cpp(facility_test_cpp, user_test_cpp)

my_dist_cpp

my_dist <- maxcovr::facility_user_dist(facility = ,
                                       user = )

my_indic_cpp <- maxcovr::facility_user_indic()


facility_test_maxcovr <- dplyr::as_data_frame(facility_test_cpp)
facility_test_maxcovr <- dplyr::rename(facility_test_maxcovr,
                                         lat = lat_facility,
                                         long = long_facility)

user_test_maxcovr <- dplyr::as_data_frame(user_test_cpp)
user_test_maxcovr <- dplyr::rename(user_test_maxcovr,
                                         lat = lat_user,
                                         long = long_user)

my_dist_regular <- maxcovr::facility_user_dist(facility_test_maxcovr,
                                                 user_test_maxcovr,
                                                 nearest = "both")
library(tibble)
options(tibble.print_max = 20, tibble.print_min = 10)
as_tibble(dplyr::select(my_dist_regular, distance))

microbenchmark::microbenchmark(
    dist_cpp <- spherical_distance_cpp(lat1 = 46.19616,
                                       long1 = 8.731278,
                                       lat2 = 46.16850,
                                       long2 = 9.004392),

    dist_maxcovr <- maxcovr::spherical_distance(lat1 = 46.19616,
                                                    long1 = 8.731278,
                                                    lat2 = 46.16850,
                                                    long2 = 9.004392),
    unit = "eps"

)
# indicator_matrix_cpp(my_new_dist, indic_dist = 100)

*/
