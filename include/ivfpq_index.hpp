/* ivfpq_index.hpp */

#ifndef IVFPQ_INDEX_HPP
#define IVFPQ_INDEX_HPP

#include <iostream>
#include <vector>
#include <cstdint>

#include "k_means.hpp"

using namespace std;

class ivfpq_index{
    private:
        // for IVF
        int k_clust;                        //πλ΄ήθος cluster για IVF
        vector<vector<double>> ivf_centr;   //κεντροειδή για IVF clustering
        vector<vector<int>> inv_list;       //αντεστραμμένη λίστα [cluster_id -> λίστα point indices] 
        
        //for PQ
        int M, nbits, sub_dim, dim;                 
        vector<vector<vector<double>>> pq_codebook; //PQ codebook [M][2^nbits][sub_dim]
        vector<vector<uint8_t>> pq_code;            //PQ κωδικοί για κάθε point [point_id][M]
        
        // helpful funcs
        vector<double> comp_residual(const vector<double>& point, const vector<double>& centroid) const;    //υπολογισμός υπολοίπου (point - centroid)
        vector<int> find_nearest_centr(const vector<double>& point, int nprobe) const;                      //εύρεση nprobe πλησιέστερων κεντροειδών
        double euclidean_distance(const vector<double>& a, const vector<double>& b) const;                  //υπολογισμός Ευκλείδειας απόστασης
        vector<vector<double>> build_LUT(const vector<double>& query_residual) const;                       //δημιουργία Look-Up Table για γρήγορη απόσταση

    public:
        ivfpq_index(int k_clust, int M, int nbits, int dim);
        ~ivfpq_index();

        // build index
        void build(const vector<vector<double>>& dataset);

        // searches
        vector<pair<double, int>> Query(const vector<double>& query, int nprobe, int N) const;
        vector<pair<double, int>> range_search(const vector<double>& query, int nprobe, double R) const;
        pair<double, int> ANN(const vector<double>& query, int nprobe) const;

        // getters
        vector<vector<double>> get_IVF_centr() const {return ivf_centr;}
        vector<vector<int>> get_inv_list() const {return inv_list;}
        vector<vector<uint8_t>> get_PQ_code() const {return pq_code;}
        int get_k_clust() const {return k_clust;}
        int get_M() const {return M;}
        int get_nbits() const {return nbits;}
        int get_dim() const {return dim;}
};

#endif