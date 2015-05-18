//
//  AppDelegate.m
//  FFT_Benchmark
//
//  Created by nuno on 28/02/15.
//  Copyright (c) 2015 nuno. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_fft.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_complex.h"
#include "headers/ADE_Bench_Utils.h"
#include <time.h>
#include <errno.h>
#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    FILE* p_debug_out=stdout;
    ADE_INT32_T ret=0;
    ADE_UINT32_T buff_lenghts[10]= {16,32,64,128,256,512,1024,2048,4096,8192};
    
    fft_test_procedure(ADE_FFT_R2C,buff_lenghts,1,p_debug_out);
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
