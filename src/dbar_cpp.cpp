// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::plugins(cpp11)]]

#include <RcppArmadillo.h>
#include <Rcpp.h>

// [[Rcpp::export]]
arma::mat calc_dbar_c(int nday, int ntupq, arma::mat day_idx, 
                      arma::mat tupq_idx, arma::mat mat_weights_tau, 
                      arma::mat mat_weights_qdatetime,
                      Rcpp::List price_slist, Rcpp::List cf_slist){
  arma::mat dbar(nday * ntupq, 2);
  
  // For each element in time grid
  for(int i = 0; i < nday; ++i){
    arma::rowvec seq_day = day_idx.row(i);
    
    // For each element in time-to-maturity tau grid
    for(int j = 0; j < ntupq; ++j){
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq(2);
      int windowOffset = 0;
      if(tupq_idx.n_cols == 2){
        seq_tupq = tupq_idx.row(j);
      } else {
        seq_tupq = tupq_idx(j, arma::span(2*i, 2*i+1));
        windowOffset = i * ntupq;
      }
      arma::vec num(seq_day[1] - seq_day[0] + 1, arma::fill::zeros), den(seq_day[1] - seq_day[0] + 1, arma::fill::zeros);
      
      // For each element in the window of the current element of ugrid: t=1 to T in the paper
      // The kernel for the rest is 0
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat price_temp =  Rcpp::as<arma::sp_mat>(price_slist[k]);
        arma::sp_mat cf_temp =  Rcpp::as<arma::sp_mat>(cf_slist[k]);
        double ncols = price_temp.n_cols;
        
        // The next two loops are to loop over elements in the cash flow and price matrix in the price_slist and cf_slist
        // For each bond: i=1 to n_t in the paper
        for(unsigned int m = 0; m < price_temp.n_rows; ++m){
          
          // For each element in the window of the current element of xgrid: j=1 to m_it in the paper
          // The kernel for the rest is 0
          for(int n = seq_tupq[0] - 1; n < std::min(seq_tupq[1], ncols); ++n) {
            num(k - seq_day[0] + 1) += price_temp(m, n) * cf_temp(m, n) * mat_weights_tau(n, windowOffset + j) * mat_weights_qdatetime(k, i);
            den(k - seq_day[0] + 1) += pow(cf_temp(m, n), 2) * mat_weights_tau(n, windowOffset + j) * mat_weights_qdatetime(k, i);
          }
        }
      }
      
      double numer = arma::sum(num), denom = arma::sum(den);
      dbar(i*ntupq + j, 0) = numer;
      dbar(i*ntupq + j, 1) = denom;
    }
  }
  return dbar;
}

// [[Rcpp::export]]
arma::mat calc_dbar_c2(int nday, int ntupq, arma::mat day_idx, 
                       arma::mat tupq_idx, arma::mat mat_weights_tau, 
                       arma::mat mat_weights_qdatetime,
                       Rcpp::List price_slist, Rcpp::List cf_slist){
  arma::mat dbar(nday * ntupq, 2);
  
  // For each element in time grid
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in time-to-maturity tau grid
  for(int j = 0; j < ntupq; ++j){
    Rcpp::checkUserInterrupt();
    arma::rowvec seq_tupq(2);
    seq_tupq = tupq_idx.row(j);
    arma::vec num(seq_day[1] - seq_day[0] + 1, arma::fill::zeros), den(seq_day[1] - seq_day[0] + 1, arma::fill::zeros);
    
    // For each element in the window of the current element of ugrid: t=1 to T in the paper
    // The kernel for the rest is 0
    for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
      arma::sp_mat price_temp =  Rcpp::as<arma::sp_mat>(price_slist[k]);
      arma::sp_mat cf_temp =  Rcpp::as<arma::sp_mat>(cf_slist[k]);
      
      // The next two loops are to loop over elements in the cash flow and price matrix in the price_slist and cf_slist
      // For each bond: i=1 to n_t in the paper
      for(unsigned int m = 0; m < price_temp.n_rows; ++m){
        
        // For each element in the window of the current element of xgrid: j=1 to m_it in the paper
        // The kernel for the rest is 0
        for(int n = seq_tupq[0] - 1; n < seq_tupq[1]; ++n) {
          num(k - seq_day[0] + 1) += price_temp(m, n) * cf_temp(m, n) * mat_weights_tau(n, j) * mat_weights_qdatetime(k, 0);
          den(k - seq_day[0] + 1) += pow(cf_temp(m, n), 2) * mat_weights_tau(n, j) * mat_weights_qdatetime(k, 0);
        }
      }
    }
    
    double numer = arma::sum(num), denom = arma::sum(den);
    dbar(j, 0) = numer;
    dbar(j, 1) = denom;
  }
  return dbar;
}
// [[Rcpp::export]]
arma::mat calc_dbar_c3(int nday, int ntupq, arma::mat day_idx, 
                       arma::mat tupq_idx, arma::mat mat_weights_tau, 
                       arma::mat mat_weights_qdatetime,
                       Rcpp::List price_slist, Rcpp::List cf_slist){
  arma::mat dbar(nday * ntupq, 2);
  
  // For each element in time grid
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in time-to-maturity tau grid
  for(int j = 0; j < ntupq; ++j){
    Rcpp::checkUserInterrupt();
    arma::rowvec seq_tupq(2);
    seq_tupq = tupq_idx.row(j);
    arma::vec num(seq_day[1] - seq_day[0] + 1, arma::fill::zeros), den(seq_day[1] - seq_day[0] + 1, arma::fill::zeros);
    
    // For each element in the window of the current element of ugrid: t=1 to T in the paper
    // The kernel for the rest is 0
    for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
      arma::sp_mat price_temp =  Rcpp::as<arma::sp_mat>(price_slist[k]);
      arma::sp_mat cf_temp =  Rcpp::as<arma::sp_mat>(cf_slist[k]);
      // if(cf_temp.n_nonzero == 0) continue;
      // arma::sp_mat x = cf_temp.cols(seq_tupq[0] - 1, std::min<int>(seq_tupq[1]-  1, cf_temp.n_cols-1));
      arma::sp_mat x = cf_temp.cols(seq_tupq[0] - 1, seq_tupq[1]-1);
      arma::uword q = seq_tupq[0] - 1;
      for(arma::sp_mat::const_iterator c = x.begin();
          c != x.end(); ++c) {
        num(k - seq_day[0] + 1) += price_temp(c.row(), c.col() + q) * *c * mat_weights_tau(c.col() + q, j) * mat_weights_qdatetime(k, 0);
        den(k - seq_day[0] + 1) += pow(*c, 2) * mat_weights_tau(c.col() + q, j) * mat_weights_qdatetime(k, 0);
      };
    };
    
    double numer = arma::sum(num), denom = arma::sum(den);
    dbar(j, 0) = numer;
    dbar(j, 1) = denom;
  }
  return dbar;
}

// [[Rcpp::export]]
arma::sp_mat calc_dbar_m(arma::sp_mat& mat_weights_tau, 
                         arma::sp_mat& mat_weights_qdatetime,
                         const arma::sp_mat& price_smat, 
                         const arma::sp_mat& cf_smat){
  
  arma::uword ns = price_smat.n_rows / mat_weights_qdatetime.n_rows;
  
  arma::umat i(2, mat_weights_qdatetime.n_rows * ns);
  arma::uvec temp_col = arma::conv_to<arma::uvec>::from(arma::vectorise(arma::repmat(arma::regspace(0, mat_weights_qdatetime.n_rows - 1), ns, 1), 1));
  i.row(0) = temp_col.as_row();
  i.row(1) = arma::regspace<arma::uvec>(0, price_smat.n_rows - 1).as_row();
  
  arma::vec x(i.n_cols);
  x.fill(1);
  
  arma::sp_mat tempi(i, x, mat_weights_qdatetime.n_rows, price_smat.n_rows);
  
  arma::sp_mat num = (mat_weights_qdatetime.t() * tempi) * (price_smat % cf_smat) * mat_weights_tau;
  arma::sp_mat den = (mat_weights_qdatetime.t() * tempi) * (cf_smat % cf_smat) * mat_weights_tau;
  
  arma::sp_mat combined = arma::join_cols(num, den);
  arma::sp_mat result = combined.t();
  return result;
}






// [[Rcpp::export]]
arma::sp_mat calc_dbar_m2(arma::sp_mat& mat_weights_tau, 
                          arma::mat mat_weights_qdatetime,
                          const arma::sp_mat& a, 
                          const arma::sp_mat& b){
  
  
  arma::sp_mat num = a * mat_weights_tau;
  arma::sp_mat den = b * mat_weights_tau;
  
  // arma::sp_mat combined = arma::join_cols(num, den);
  // arma::sp_mat result = combined.t();
  return num;
}

arma::mat check_crspid_intersect (Rcpp::List cf_slist, arma::rowvec seq_tupq_x, arma::rowvec seq_tupq_q, arma::rowvec seq_day){
  arma::sp_mat tempMat = Rcpp::as<arma::sp_mat>(cf_slist[seq_day[0]-1]);
  int N = tempMat.n_rows, M = cf_slist.length();
  // Row number is the total number of crspid in the data
  // Column number is the number of qdate
  arma::mat intMatrix (N, M);
  for(int i = seq_day[0] - 1; i < seq_day[1]; ++i){
    tempMat =  Rcpp::as<arma::sp_mat>(cf_slist[i]);
    arma::vec intVec (N, arma::fill::zeros);
    int counter = 0;
    double ncols = tempMat.n_cols;
    for(int k = 0; k < N; ++k){
      double xRowSum = arma::sum(arma::sum(tempMat(k, arma::span(seq_tupq_x[0] - 1, std::min(ncols - 1, seq_tupq_x[1] - 1)))));
      double qRowSum = arma::sum(arma::sum(tempMat(k, arma::span(seq_tupq_q[0] - 1, std::min(ncols - 1, seq_tupq_q[1] - 1)))));
      if((xRowSum > 0) & (qRowSum > 0)){
        intVec(counter) = k;
        counter += 1;
      }
    }
    intMatrix.col(i) = intVec;
  }
  return intMatrix;
}

int find_maxRows(arma::vec qdate_idx, arma::mat crspid_idx) {
  unsigned int maxRows = 0;
  for(int j = qdate_idx(0); j <= qdate_idx(qdate_idx.n_elem - 1); ++j){
    for(unsigned int k = 0; k < crspid_idx.n_rows; ++k){
      if(crspid_idx(k, j) == 0){
        if(k > maxRows){
          maxRows = k;
        }
        break;
      }
    }
  }
  return maxRows;
}

// Reweight weights for tau_p so the weights at where there is a cash flow add up to one
arma::cube weightCF2(arma::vec qdate_idx, Rcpp::List cf_slist, arma::rowvec seq_tupq, arma::mat crspid_idx, arma::mat window, int i){
  unsigned int maxRows = find_maxRows(qdate_idx, crspid_idx);
  // Rprintf("maxRows %i\n", maxRows);
  arma::cube cfTemp(maxRows, seq_tupq[1] - seq_tupq[0] + 1, qdate_idx.n_elem, arma::fill::zeros);
  
  for(unsigned int j = 0; j < qdate_idx.n_elem; ++j){
    arma::sp_mat tempMat = Rcpp::as<arma::sp_mat>(cf_slist[qdate_idx(j)]);
    arma::vec tempVec = crspid_idx.col(qdate_idx(j));
    
    arma::mat Weights (maxRows, seq_tupq[1] - seq_tupq[0] + 1, arma::fill::zeros);
    double ncols = tempMat.n_cols;
    for(unsigned int k = 0; k < maxRows; ++k){
      if(tempVec(k) != 0){
        
        // reweight
        double wsum = 0;
        int counter = 0;
        for(int l = seq_tupq[0] - 1; l < std::min(ncols, seq_tupq[1]); ++l){
          if(tempMat(tempVec(k), l) > 0){
            wsum += window(l, i);
            counter += 1;
          }
        }
        // if there are more than 5 cash flow
        // do not re weight
        if(counter >= 5) {
          wsum = 1;
        }
        
        
        for(int l = seq_tupq[0] - 1; l < std::min(ncols, seq_tupq[1]); ++l){
          Weights(k, l - seq_tupq[0] + 1) = tempMat(tempVec(k), l) * (window(l, i)/wsum);
        }
      } else {
        break;
      }
    }
    cfTemp.slice(j) = Weights;
  }
  return cfTemp;
}

arma::cube weightCF(arma::vec qdate_idx, Rcpp::List cf_slist, arma::rowvec seq_tupq, arma::mat crspid_idx, arma::mat window, int i){
  unsigned int maxRows = find_maxRows(qdate_idx, crspid_idx);
  
  arma::cube cfTemp(maxRows, seq_tupq[1] - seq_tupq[0] + 1, qdate_idx.n_elem, arma::fill::zeros);
  
  for(unsigned int j = 0; j < qdate_idx.n_elem; ++j){
    arma::sp_mat tempMat = Rcpp::as<arma::sp_mat>(cf_slist[qdate_idx(j)]);
    arma::vec tempVec = crspid_idx.col(qdate_idx(j));
    
    arma::mat Weights (maxRows, seq_tupq[1] - seq_tupq[0] + 1, arma::fill::zeros);
    double ncols = tempMat.n_cols;
    for(unsigned int k = 0; k < maxRows; ++k){
      if(tempVec(k) != 0){
        for(int l = seq_tupq[0] - 1; l < std::min(ncols, seq_tupq[1]); ++l){
          Weights(k, l - seq_tupq[0] + 1) = tempMat(tempVec(k), l) * window(l, i);
        }
      } else {
        break;
      }
    }
    cfTemp.slice(j) = Weights;
  }
  return cfTemp;
}

arma::cube createSumP (arma::cube cf_temp_q, arma::rowvec seq_tupq_x){
  arma::cube sumP(cf_temp_q.n_rows, seq_tupq_x[1] - seq_tupq_x[0] + 1, cf_temp_q.n_slices);
  for(unsigned int i= 0; i < cf_temp_q.n_slices; ++i){
    for(unsigned int k = 0; k < cf_temp_q.n_rows; ++k){
      sumP.slice(i).row(k).fill(arma::sum(cf_temp_q.slice(i).row(k)));
    }
  }
  return sumP;
}

double calc_hhat_once(int j, int k, arma::rowvec seq_day, arma::rowvec seq_tupq_x, arma::rowvec seq_tupq_q,
                      Rcpp::List cf_slist, arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, arma::colvec mat_weights_qdatetime){
  
  // Returns bonds that there is a cashflow in both the x window and q window -> for crossproducts?
  arma::mat crspid_idx = check_crspid_intersect(cf_slist, seq_tupq_x, seq_tupq_q, seq_day);
  //Matches the above to the day sequence for the u window
  arma::vec qdate_idx(seq_day[1] - seq_day[0] + 1);
  int counter = 0;
  for(int m = seq_day[0] - 1; m < seq_day[1]; ++m){
    if(arma::sum(crspid_idx.col(m)) != 0){
      qdate_idx[counter] = m;
      counter += 1;
    }
  }
  // Skip this iteration if there are no payments in q
  if(counter == 0) {
    return 0;
  }
  // Otherwise grab the relevant parts of qdate
  qdate_idx = qdate_idx.head(counter);
  if(find_maxRows(qdate_idx, crspid_idx)==0) {
    // Avoid runtime error in UBSAN check: reference binding to null pointer 
    return 0;
  }
  
  // Weights x and q cash flows by the window function
  arma::cube cf_temp_x = weightCF(qdate_idx, cf_slist, seq_tupq_x, crspid_idx, mat_weights_tau, j);
  arma::cube cf_temp_q = weightCF2(qdate_idx, cf_slist, seq_tupq_q, crspid_idx, mat_weights_tau_p, k);
  // Creates cf_temp_q sized matrices where each element is the sum of that row of cf_temp_q
  arma::cube sumP = createSumP (cf_temp_q, seq_tupq_x);
  // Checks intersections of x and q.
  arma::vec j_idx(seq_tupq_x(1) - seq_tupq_x(0) + 1);
  arma::vec p_idx(seq_tupq_q(1) - seq_tupq_q(0) + 1);
  counter = 0;
  for(int m = 0; m <= seq_tupq_x(1) - seq_tupq_x(0); ++m){
    for(int n = 0; n <= seq_tupq_q(1) - seq_tupq_q(0); ++n){
      if(m + seq_tupq_x(0) == n + seq_tupq_q(0)){
        j_idx(counter) = m;
        p_idx(counter) = n;
        counter += 1;
      }
    }
  }
  if(counter == 1){
    for(unsigned int z = 0; z < qdate_idx.n_elem; ++z){
      sumP.slice(z).col(j_idx[0]) -= sumP.slice(z).col(p_idx[0]);
    }
  } else if(counter > 1){
    for(unsigned int z = 0; z < qdate_idx.n_elem; ++z){
      sumP.slice(z).cols(j_idx[0], j_idx[counter - 1]) -= cf_temp_q.slice(z).cols(p_idx[0], p_idx[counter - 1]);
    }
  }
  // Multiplies the adjusted sum_p matrix by the actual cashflows.
  double hhat_num = 0;
  for(unsigned int m = 0; m < sumP.n_slices; ++m){
    double sumCrossProduct = arma::accu(sumP.slice(m) % cf_temp_x.slice(m));
    hhat_num += sumCrossProduct * mat_weights_qdatetime(qdate_idx(m));
  }
  
  return hhat_num;
}

// [[Rcpp::export]]
arma::cube calc_hhat_num2_c(int nday, int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, arma::mat mat_weights_qdatetime, Rcpp::List cf_slist) {
  
  arma::cube hhat(ntupq_tau, ntupq_tau_p, nday, arma::fill::zeros);
  
  // For each element in xgrid time
  for (int i = 0; i < nday; ++i) {
    arma::rowvec seq_day = day_idx.row(i);
    
    // For each element in tau grid
    for (int j = 0; j < ntupq_tau; ++j) {
      arma::rowvec seq_tupq_x(2);
      int window_lower_x, window_upper_x;
      if(tupq_idx_tau.n_cols == 2){
        seq_tupq_x = tupq_idx_tau.row(j);
        window_lower_x = 0;
        window_upper_x = ntupq_tau - 1;
      } else {
        seq_tupq_x = tupq_idx_tau(j, arma::span(2*i, 2*i+1));
        window_lower_x = ntupq_tau * i;
        window_upper_x = ntupq_tau * (i + 1) - 1;
      }
      
      // For each element in tau_p
      for (int k = 0; k < ntupq_tau_p; ++k) {
        Rcpp::checkUserInterrupt();
        arma::rowvec seq_tupq_q(2);
        int window_lower_q, window_upper_q;
        if(tupq_idx_tau_p.n_cols == 2){
          seq_tupq_q = tupq_idx_tau_p.row(k);
          window_lower_q = 0;
          window_upper_q = ntupq_tau_p - 1;
        } else {
          seq_tupq_q = tupq_idx_tau_p(k, arma::span(2*i, 2*i+1));
          window_lower_q = ntupq_tau_p * i;
          window_upper_q = ntupq_tau_p * (i + 1) - 1;
        }
        hhat(j, k, i) = calc_hhat_once(j, k, seq_day, seq_tupq_x, seq_tupq_q, cf_slist,
             mat_weights_tau.cols(window_lower_x, window_upper_x), mat_weights_tau_p.cols(window_lower_q, window_upper_q), mat_weights_qdatetime.col(i));
        
      }
      
    }
  }
  return hhat;
}

// [[Rcpp::export]]
arma::mat calc_hhat_num_c(int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, 
                          arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, 
                          arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, 
                          arma::mat mat_weights_qdatetime, Rcpp::List cf_slist) {
  
  arma::mat hhat(ntupq_tau, ntupq_tau_p);
  
  // For each element in xgrid time
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in tau grid
  for (int jj = 0; jj < ntupq_tau; ++jj) {
    arma::rowvec seq_tupq_x(2);
    seq_tupq_x = tupq_idx_tau.row(jj);
    
    // For each element in tau_p
    for (int pp = 0; pp < ntupq_tau_p; ++pp) {
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq_q(2);
      seq_tupq_q = tupq_idx_tau_p.row(pp);
      
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat cf_temp = Rcpp::as<arma::sp_mat>(cf_slist[k]);
        
        arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, seq_tupq_x[1] - 1);
        arma::uword q = seq_tupq_x[0] - 1;
        // Rcpp::Rcout << "The value of x : " << x << "\n";
        // Rcpp::Rcout << "The value of x.row(1) : " << x.row(1) << "\n";
        for(arma::sp_mat::const_iterator j = x.begin(); j != x.end(); ++j) {
          
          double sum_p = 0;
          // arma::sp_mat cf_temp_t = cf_temp.t();
          // arma::sp_mat xx=cf_temp_t.col(j.row());
          arma::sp_mat xx=cf_temp.row(j.row());
        // Rcpp::Rcout << "The value of xx : " << xx << "\n";
        // Rcpp::Rcout << "The value of x.row(j.row()) : " << x.row(j.row()) << "\n";
          for(arma::sp_mat::const_iterator p = xx.begin(); p != xx.end(); ++p){
        // Rcpp::Rcout << "The value of seq_tupq_x : " << seq_tupq_x << "\n";
        // Rcpp::Rcout << "The value of seq_tupq_q : " << seq_tupq_q << "\n";
        // Rcpp::Rcout << "The value of p.col() : " << p.col() << "\n";
        // Rcpp::Rcout << "The value of j.col() : " << j.col() << "\n";
            // if(p.row() != j.col() + q){
            if(p.col() != j.col() + q){
        // Rcpp::Rcout << "The value of p : " << *p << "\n";
              sum_p += *p * mat_weights_tau(p.col(), pp);
            }
          }
        // Rcpp::Rcout << "The value of sum_p : " << sum_p << "\n";
          hhat(jj, pp) += sum_p * *j * mat_weights_tau(j.col() + q, jj) * mat_weights_qdatetime(k, 0);
            
        }
      }
    }
  }
  return hhat;
}

// [[Rcpp::export]]
arma::mat calc_hhat_num_c2(int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, 
                          arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, 
                          arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, 
                          arma::mat mat_weights_qdatetime, Rcpp::List cf_slist) {
  
  arma::mat hhat(ntupq_tau, ntupq_tau_p);
  
  // For each element in xgrid time
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in tau grid
  for (int jj = 0; jj < ntupq_tau; ++jj) {
    arma::rowvec seq_tupq_x(2);
    seq_tupq_x = tupq_idx_tau.row(jj);
    
    // For each element in tau_p
    for (int pp = 0; pp < ntupq_tau_p; ++pp) {
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq_q(2);
      seq_tupq_q = tupq_idx_tau_p.row(pp);
      
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat cf_temp = Rcpp::as<arma::sp_mat>(cf_slist[k]);
        
        arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, seq_tupq_x[1] - 1);
        arma::uword q = seq_tupq_x[0] - 1;
        for(arma::sp_mat::const_iterator j = x.begin(); j != x.end(); ++j) {
          
          double sum_p = 0;
          arma::sp_mat cf_temp_t = cf_temp.t();
          arma::sp_mat xx=cf_temp_t.col(j.row());
          for(arma::sp_mat::const_iterator p = xx.begin(); p != xx.end(); ++p){
            if(p.row() != j.col() + q){
              sum_p += *p * mat_weights_tau(p.row(), pp);
            }
          }
          hhat(jj, pp) += sum_p * *j * mat_weights_tau(j.col() + q, jj) * mat_weights_qdatetime(k, 0);
            
        }
      }
    }
  }
  return hhat;
}


// [[Rcpp::export]]
arma::mat calc_hhat_num_c3(int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, 
                          arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, 
                          arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, 
                          arma::mat mat_weights_qdatetime, Rcpp::List cf_slist) {
  
  arma::mat hhat(ntupq_tau, ntupq_tau_p);
  
  // For each element in xgrid time
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in tau grid
  for (int jj = 0; jj < ntupq_tau; ++jj) {
    arma::rowvec seq_tupq_x(2);
    seq_tupq_x = tupq_idx_tau.row(jj);
    
    // For each element in tau_p
    for (int pp = 0; pp < ntupq_tau_p; ++pp) {
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq_q(2);
      seq_tupq_q = tupq_idx_tau_p.row(pp);
      
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat cf_temp = Rcpp::as<arma::sp_mat>(cf_slist[k]);
        
        arma::sp_mat cf_temp_p(size(cf_temp));
        
        for(arma::sp_mat::const_iterator p = cf_temp.begin(); p != cf_temp.end(); ++p){
          cf_temp_p(p.row(), p.col()) =  *p * mat_weights_tau(p.col(), pp);
        }
        
        arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, seq_tupq_x[1] - 1);
        arma::uword q = seq_tupq_x[0] - 1;
        for(arma::sp_mat::const_iterator j = x.begin(); j != x.end(); ++j) {
          
          double sum_p = 0;
          arma::sp_mat cf_temp_pt = cf_temp_p.t();
          arma::sp_mat xx=cf_temp_pt.col(j.row());
          for(arma::sp_mat::const_iterator p = xx.begin(); p != xx.end(); ++p){
            if(p.row() != j.col() + q){
              sum_p += *p;
            }
          }
          hhat(jj, pp) += sum_p * *j * mat_weights_tau(j.col() + q, jj) * mat_weights_qdatetime(k, 0);
            
        }
      }
    }
  }
  return hhat;
}

// [[Rcpp::export]]
arma::mat calc_hhat_num_c4(int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, 
                           arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, 
                           arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, 
                           arma::mat mat_weights_qdatetime, Rcpp::List cf_slist) {
  
  arma::mat hhat(ntupq_tau, ntupq_tau_p);
  
  // For each element in xgrid time
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in tau grid
  for (int jj = 0; jj < ntupq_tau; ++jj) {
    arma::rowvec seq_tupq_x(2);
    seq_tupq_x = tupq_idx_tau.row(jj);
    
    // For each element in tau_p
    for (int pp = 0; pp < ntupq_tau_p; ++pp) {
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq_q(2);
      seq_tupq_q = tupq_idx_tau_p.row(pp);
      
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat cf_temp = Rcpp::as<arma::sp_mat>(cf_slist[k]);
        
        arma::sp_mat cf_temp_p(size(cf_temp));
        
        for(arma::sp_mat::const_iterator p = cf_temp.begin(); p != cf_temp.end(); ++p){
          cf_temp_p(p.row(), p.col()) =  *p * mat_weights_tau(p.col(), pp);
        }
        arma::sp_mat row_sum = sum(cf_temp_p.t(), 0);
        for(arma::sp_mat::const_iterator p = cf_temp.begin(); p != cf_temp.end(); ++p){
          cf_temp_p(p.row(), p.col()) =  row_sum(0, p.row());
        }
        
        arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, seq_tupq_x[1] - 1);
        arma::uword q = seq_tupq_x[0] - 1;
        for(arma::sp_mat::const_iterator j = x.begin(); j != x.end(); ++j) {
          hhat(jj, pp) += (cf_temp_p(j.row(), j.col() + q)- *j) * *j * mat_weights_tau(j.col() + q, jj) * mat_weights_qdatetime(k, 0);
        }
      }
    }
  }
  return hhat;
}

// [[Rcpp::export]]
arma::mat calc_hhat_num_c5(int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, 
                           arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, 
                           arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, 
                           arma::mat mat_weights_qdatetime, Rcpp::List cf_slist) {
  
  arma::mat hhat(ntupq_tau, ntupq_tau_p);
  
  // For each element in xgrid time
  arma::rowvec seq_day = day_idx.row(0);
  
  // For each element in tau grid
  for (int jj = 0; jj < ntupq_tau; ++jj) {
    arma::rowvec seq_tupq_x(2);
    seq_tupq_x = tupq_idx_tau.row(jj);
    
    // For each element in tau_p
    for (int pp = 0; pp < ntupq_tau_p; ++pp) {
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq_q(2);
      seq_tupq_q = tupq_idx_tau_p.row(pp);
      
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat cf_temp = Rcpp::as<arma::sp_mat>(cf_slist[k]);
        
        arma::sp_mat cf_temp_p(size(cf_temp));
        
        for(arma::sp_mat::const_iterator p = cf_temp.begin(); p != cf_temp.end(); ++p){
          cf_temp_p(p.row(), p.col()) =  *p * mat_weights_tau_p(p.col(), pp);
        }
        arma::sp_mat row_sum = sum(cf_temp_p.t(), 0);
        
        arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, seq_tupq_x[1] - 1);
        arma::uword q = seq_tupq_x[0] - 1;
        for(arma::sp_mat::const_iterator j = x.begin(); j != x.end(); ++j) {
          hhat(jj, pp) += (row_sum(0, j.row()) - cf_temp_p(j.row(), j.col() + q)) * *j * 
            mat_weights_tau(j.col() + q, jj) * mat_weights_qdatetime(k, 0);
        }
      }
    }
  }
  return hhat;
}

// [[Rcpp::export]]
arma::mat calc_hhat_num_c6(int ntupq_tau, int ntupq_tau_p, arma::mat day_idx, 
                           arma::mat tupq_idx_tau, arma::mat tupq_idx_tau_p, 
                           arma::mat mat_weights_tau, arma::mat mat_weights_tau_p, 
                           arma::mat mat_weights_qdatetime, Rcpp::List cf_slist, 
                           bool same_tau) {
  
  arma::mat hhat(ntupq_tau, ntupq_tau_p);
  
  // For each element in xgrid time
  arma::rowvec seq_day = day_idx.row(0);
  
  int pp_start = 0;
  
  // For each element in tau grid
  for (int jj = 0; jj < ntupq_tau; ++jj) {
    arma::rowvec seq_tupq_x(2);
    seq_tupq_x = tupq_idx_tau.row(jj);
    
    // For each element in tau_p
    // The numerator of H_hat is symmetric 
    // so we only need to calculate half of it 
    // to cut time
    if(same_tau) pp_start = jj;
    for (int pp = pp_start; pp < ntupq_tau_p; ++pp) {
      Rcpp::checkUserInterrupt();
      arma::rowvec seq_tupq_q(2);
      seq_tupq_q = tupq_idx_tau_p.row(pp);
      
      for(int k = seq_day[0] - 1; k < seq_day[1]; ++k){
        arma::sp_mat cf_temp = Rcpp::as<arma::sp_mat>(cf_slist[k]);
        // if(cf_temp.n_nonzero == 0) continue;
        arma::sp_mat cf_temp_p(size(cf_temp));
        
        for(arma::sp_mat::const_iterator p = cf_temp.begin(); p != cf_temp.end(); ++p){
          cf_temp_p(p.row(), p.col()) =  *p * mat_weights_tau_p(p.col(), pp);
        }
        arma::sp_mat row_sum = sum(cf_temp_p.t(), 0);
        // arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, std::min<int>(seq_tupq_x[1]-  1, cf_temp.n_cols-1));
        
        arma::sp_mat x = cf_temp.cols(seq_tupq_x[0] - 1, seq_tupq_x[1] - 1);
        arma::uword q = seq_tupq_x[0] - 1;
        for(arma::sp_mat::const_iterator j = x.begin(); j != x.end(); ++j) {
          hhat(jj, pp) += (row_sum(0, j.row()) - cf_temp_p(j.row(), j.col() + q)) * *j * 
            mat_weights_tau(j.col() + q, jj) * mat_weights_qdatetime(k, 0);
        }
      }
    }
  }
  return hhat;
}
