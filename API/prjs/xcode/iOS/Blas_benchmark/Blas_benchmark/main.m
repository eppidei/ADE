//
//  main.m
//  Blas_benchmark
//
//  Created by nuno on 23/02/15.
//  Copyright (c) 2015 nuno. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include "headers/ADE_defines.h"
//#include "headers/ADE_typedefs.h"
//#include "headers/ADE_Bench_Utils.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
//    ADE_BENCH_T test_cases[1]={ADE_BLAS_L3_BM};
//    ADE_BENCH_MAT_T mat_type[2]={ADE_BENCH_REAL,ADE_BENCH_CPLX};
//    FILE* p_debug_out=stdout;
//    ADE_INT32_T ret=0;
//    
//    ADE_INT32_T n_rows_A[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
//    ADE_INT32_T n_cols_A[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
//    ADE_INT32_T n_cols_B[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
//    
//    
//    ret=blas3_test_procedure(test_cases,1,mat_type,2,n_rows_A,n_cols_A,n_cols_B,1,p_debug_out);
}






