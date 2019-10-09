#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

typedef vector< vector<int> > Matrix;

// Hardware specification
const int mOrder = 2;
const int BitWidth = 1 << 4;


int DumbFunc() {
    return 0;
}

// Print out the matrix
void PrintMat(const Matrix &m, const int &num_row, const int &num_col) {
    printf("\n");
    for (int i = 0; i < num_row; i++) {
        for (int j = 0; j < num_col; j++) {
            printf("\t%d", m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


// Multiply two matrices
__attribute__((always_inline)) Matrix MulMat(const Matrix &m_A, const Matrix &m_B) {
//Matrix MulMat(const Matrix &m_A, const Matrix &m_B) {
    int num_A_row = m_A.size();
    int num_A_col = m_A[0].size();

    int num_B_row = m_B.size();
    int num_B_col = m_B[0].size();

    vector<int> m_product_row(num_A_row, 0);
    Matrix      m_product    (num_B_col);

    // Handle exception when two matrices are not able to multiply
    if (!(num_A_col == num_B_row)) {
        //printf("\n[ERROR] m_A: (row -- %d, col --%d)", num_A_row, num_A_col);
        //printf("\n        m_B: (row -- %d, col --%d)", num_B_row, num_B_col);
        //printf("\n        The two matrices cannot be multiplied!\n");
        return m_product;
    }

    // Multiplication
    vector<int> empty_vec(num_A_row, 0);  // Used for clear the vector
    for (int i = 0; i < num_A_row; i++) {
        m_product_row = empty_vec;
        for (int j = 0; j < num_B_col; j++) {
            //printf("m_product[%d][%d] = 0", i, j);
            for (int k = 0; k < num_A_col; k++) {
                //printf(" + m_A[%d][%d] * m_B[%d][%d]", i, k, k, j);
                m_product_row[j] += m_A[i][k] * m_B[k][j]; 
            }
            //printf("\n");
        }
        m_product[i] = m_product_row;
    }

    return m_product;
}


// To print out and check the final result for debug propose
void CheckResult(const Matrix &m_a, const Matrix &m_b, const Matrix &m_product, const Matrix &m_product_chk, const int m_order) {
    printf("\n");
    printf("\n======== Original matrices ========\n\n");
    printf("The original matrix A:\n");
    PrintMat(m_a, m_order, m_order);
    printf("The original matrix B:\n");
    PrintMat(m_b, m_order, m_order);

    printf("\n======== Checksum matrices ========\n\n");
    printf("The checksum matrix A:\n");
    PrintMat(m_a, m_order+1, m_order);
    printf("The checksum matrix B:\n");
    PrintMat(m_b, m_order, m_order+1);
    printf("The checksum matrix product:\n");
    PrintMat(m_product_chk, m_order+1, m_order+1);

    // Check if checksum property is correct
    printf("\n======== Check checksum validity ========\n\n");
    bool is_row_correct = true;
    for (int i = 0; i < m_order; i++) {
        int j;
        int tmp_row_chk_sum = 0;
        for (j = 0; j < m_order; j++) {
            tmp_row_chk_sum += m_product_chk[i][j];
        }
        if (tmp_row_chk_sum != m_product_chk[i][j]) {
            printf("[ERROR] Checksum of ROW %d is not correct!\n", i);
            is_row_correct = false;
        }
    }
    if (is_row_correct) {
        printf("All checksums of ROWs are correct!\n");
    }

    printf("\n");
    bool is_col_correct = true;
    for (int i = 0; i < m_order; i++) {
        int j;
        int tmp_col_chk_sum = 0;
        for (j = 0; j < m_order; j++) {
            tmp_col_chk_sum += m_product_chk[j][i];
        }
        if (tmp_col_chk_sum != m_product_chk[j][i]) {
            printf("[ERROR] Checksum of COL %d is not correct!\n", i);
            is_col_correct = false;
        }
    }
    if (is_col_correct) {
        printf("All checksums of COLs are correct!\n");
    }

    // The cornered checksum equals to the sum of all original data
    printf("\n");
    int tmp_all_sum = 0;
    for (int i = 0; i < m_order; i++) {
        for (int j = 0; j < m_order; j++) {
            tmp_all_sum += m_product[i][j];
        }
    }
    if (tmp_all_sum == m_product_chk[m_order][m_order]) {
        printf("Checksum of ALL data is correct!\n");
    }
    else {
        printf("[ERROR] Checksum of ALL data is not correct!\n");
    }


    printf("\n\n============ Product matrices ============\n\n");
    printf("The derived original matrix product:\n");
    PrintMat(m_product, m_order, m_order);

    // For verification purpose, we calculate the matrix again as golden reference
    vector<int> empty_vec(m_order, 0);
    Matrix m_a_tmp(m_order, empty_vec), m_b_tmp(m_order, empty_vec);
    for (int i = 0; i < m_order; i++) {
        for (int j = 0; j < m_order; j++) {
            m_a_tmp[i][j] = m_a[i][j];
            m_b_tmp[i][j] = m_b[i][j];
        }
    }
    Matrix m_product_gd = MulMat(m_a_tmp, m_b_tmp);
    printf("The calculated checksum matrix product:\n");
    PrintMat(m_product, m_order, m_order);

    // Check if multiply result is correct
    printf("\n======== Check product validity ========\n\n");
    bool is_product_correct = true;
    for (int i = 0; i < m_order; i++) {
        for (int j = 0; j < m_order; j++) {
            if (m_product_gd[i][j] != m_product[i][j]) {
                printf("[ERROR] Product of (R:%d, C:%d) is not correct!\n", i, j);
                is_product_correct = false;
            }
        }
    }
    if (is_product_correct) {
        printf("All product results are correct!\n");
    }

    printf("\n");
}


int main() {

    // Randomly initialize the matrices
    srand((unsigned)time(NULL));

    vector<int> m_A_row(mOrder), m_B_row(mOrder+1);
    Matrix      m_A  (mOrder+1), m_B    (mOrder);

    int mat_val = 1;
    for (int i = 0; i < mOrder; i++) {
        for (int j = 0; j < mOrder; j++) {
            // Random signed fix point number
            //m_A_row[j] = ((rand() % BitWidth) - 0.5 * BitWidth);
            //m_B_row[j] = ((rand() % BitWidth) - 0.5 * BitWidth);
            m_A_row[j] = ++mat_val;
            m_B_row[j] = 15 - 2 * mat_val;
        }
        m_A[i] = m_A_row;
        m_B[i] = m_B_row;
    }
    
    //PrintMat(m_A, mOrder, mOrder);
    //PrintMat(m_B, mOrder, mOrder);
    
    // Add redundant checksum bit
    vector<int> m_A_chk_col(mOrder);
    int         m_B_chk_row;

    for (int i = 0; i < mOrder; i++) {
        int j;
        m_A_chk_col[i] = 0;
        for (j = 0; j < mOrder; j++) {
            m_A_chk_col[i] += m_A[j][i];
        }
        m_A[j] = m_A_chk_col;
    }

    for (int i = 0; i < mOrder; i++) {
        int j;
        m_B_chk_row = 0;
        for (j = 0; j < mOrder; j++) {
            m_B_chk_row += m_B[i][j];
        }
        m_B[i][j] = m_B_chk_row;
    }

    //PrintMat(m_A, mOrder+1, mOrder);
    //PrintMat(m_B, mOrder  , mOrder+1);

    Matrix m_product_chk = MulMat(m_A, m_B);
 
    // Get the original result
    vector<int> empty_vec(mOrder, 0);
    Matrix m_product(mOrder, empty_vec);
    for (int i = 0; i < mOrder; i++){
        for (int j = 0; j < mOrder; j++){
            m_product[i][j] = m_product_chk[i][j];
        }
    }

    DumbFunc();

    // Print and check the final result to simplify checking validity
    // Since the hardware do not have this print function, we need to insert hardware error before this function call
    CheckResult(m_A, m_B, m_product, m_product_chk, mOrder);

    return 0;
}
