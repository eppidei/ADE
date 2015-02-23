//
//  AppDelegate.m
//  Blas_benchmark
//
//  Created by nuno on 23/02/15.
//  Copyright (c) 2015 nuno. All rights reserved.
//

#import "AppDelegate.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Bench_Utils.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
//    // Override point for customization after application launch.
    ADE_BENCH_T test_cases[1]={ADE_BLAS_L3_BM};
    ADE_BENCH_MAT_T mat_type[2]={ADE_BENCH_REAL,ADE_BENCH_CPLX};
    FILE* p_debug_out=stdout;
    ADE_INT32_T ret=0;
    
    ADE_INT32_T n_rows_A[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_A[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_B[10]={2};//{2,10,50,100,500,1000,2000,3000,5000,10000};
    
    
    ret=blas3_test_procedure(test_cases,1,mat_type,2,n_rows_A,n_cols_A,n_cols_B,1,p_debug_out);
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
