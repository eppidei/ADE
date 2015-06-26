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
    int i;
//    ADE_INT32_T ret=0;
//    ADE_FFTSetup_T p_setup_imag;
//    ADE_FFTSetup_T p_setup_real;
//    
//    float in_fft_imag[8]={0,1,2,3,4,5,6,7};//4 immaginari interleaved
//    float in_fft_real[8]={0,1,2,3,4,5,6,7};// 8 reali
//    float out_fft_imag[8]={0,0,0,0,0,0,0,0};//4 immaginari interleaved
//    float out_fft_real[8]={0,0,0,0,0,0,0,0};// 8 reali
//    //float real_part[4]={0,2,4,6};
//    //float imag_part[4]={1,3,5,7};
//    float input_split_imag[8]={0,0,0,0,0,0,0,0};
//    float output_split_imag[8]={0,0,0,0,0,0,0,0};
//    float input_split_real[8]={0,0,0,0,0,0,0,0};
//    float output_split_real[8]={0,0,0,0,0,0,0,0};
//   
//    ADE_SplitComplex_T split_in_imag;
//    ADE_SplitComplex_T split_out_imag;
//    
//    ADE_SplitComplex_T split_in_real;
//    ADE_SplitComplex_T split_out_real;
//    
//    split_in_imag.realp=input_split_imag;
//    split_in_imag.imagp=&(input_split_imag[4]);
//    split_out_imag.realp=output_split_imag;
//    split_out_imag.imagp=&(output_split_imag[4]);
//    
//    split_in_real.realp=input_split_real;
//    split_in_real.imagp=&(input_split_real[4]);
//    split_out_real.realp=output_split_real;
//    split_out_real.imagp=&(output_split_real[4]);
//    
//    ADE_vDSP_DFT_Direction_T dir=ADE_vDSP_DFT_FORWARD;
//    int log2len_imag=2;
//    int len_imag = 1<<log2len_imag;
//    int log2len_real=3;
//    int len_real = 1<<log2len_real;
//    int i=0;
//    float scale;
//    
//    
//    p_setup_imag=vDSP_create_fftsetup ( log2len_imag, ADE_kFFTRadix2);
//    
//    vDSP_ctoz((ADE_DSPComplex *) in_fft_imag, 2, &split_in_imag, 1, len_imag);
//    
//    for (i=0;i<len_imag;i++)
//    {
//        fprintf(stdout,"split_in_imag[%d]=%f+%fi\n ",i,split_in_imag.realp[i],split_in_imag.imagp[i]);
//    }
//    fprintf(stdout,"\n");
//    
//   // vDSP_ctoz((ADE_DSPComplex *) output_split_imag, 2, &split_out_imag, 1, len_imag);
//    
//     vDSP_fft_zop ( p_setup_imag, &(split_in_imag), 1, &(split_out_imag), 1, log2len_imag,dir);
//    
//    vDSP_ztoc (&split_out_imag,1,(ADE_DSPComplex*) out_fft_imag,2,len_imag);
//    
//    for (i=0;i<len_imag;i++)
//    {
//        
//        fprintf(stdout,"fftc2c[%d]=%f+%fi\n ",i,out_fft_imag[2*i],out_fft_imag[2*i+1]);
//    }
//    fprintf(stdout,"\n");
//    
//    p_setup_real=vDSP_create_fftsetup ( log2len_real, ADE_kFFTRadix2);
//    
//    vDSP_ctoz((ADE_DSPComplex *) in_fft_real, 2, &split_in_real, 1, len_real/2);
//    
//    
//    for (i=0;i<len_imag;i++)
//    {
//        fprintf(stdout,"split_in_real[%d]=%f+%fi\n ",i,split_in_real.realp[i],split_in_real.imagp[i]);
//    }
//    fprintf(stdout,"\n");
//    
//    //vDSP_ctoz((ADE_DSPComplex *) output_split_real, 2, &split_out_real, 1, len_real/2);
//    
//    vDSP_fft_zrop ( p_setup_real, &(split_in_real), 1, &(split_out_real), 1, log2len_real,dir);
//    scale = (float) 1.0 / (log2len_imag);
//    
//    vDSP_vsmul(split_out_real.realp, 1, &scale, split_out_real.realp, 1, len_imag);
//    vDSP_vsmul(split_out_real.imagp, 1, &scale, split_out_real.imagp, 1, len_imag);
//    
//    vDSP_ztoc (&split_out_real,1,(ADE_DSPComplex*) out_fft_real,2,len_real);
//    
//    
//    
//
//    
// 
//    for (i=0;i<len_imag;i++)
//    {
//
//     fprintf(stdout,"fftr2c[%d]=%f+(%fi)\n ",i,out_fft_real[2*i],out_fft_real[2*i+1]);
//
    
    ADE_UINT32_T buff_lenghts[10]= {16,32,64,128,256,512,1024,2048,4096,8192};
    for (i=0;i<100;i++)
    {
    fft_test_procedure(ADE_FFT_C2C,buff_lenghts,10,p_debug_out);
        //fft_iosreal_test_procedure(buff_lenghts,10,p_debug_out);
        fprintf(stdout,"iteration n %d \n",i);
    }
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
