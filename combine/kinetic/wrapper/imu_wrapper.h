//
//  imu_wrapper.h
//  IMU Math
//
//  Created by Matthew Fonken on 12/19/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef imu_wrapper_h
#define imu_wrapper_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <math.h>
    
//#include "sercom.h"
    
#define DEFAULT_INTERFACE SERCOM
    
#define LSM9DS1
    
    typedef enum
    {
        SERCOM = 0,
        I2C,
        SPI
    } INTERFACE;
    
    typedef struct
    {
        INTERFACE interface;
        int       descriptor;
    } channel_t;
    
    typedef struct
    {
        double 	accel_raw[3];
        
        double 	accel[3];
        double 	gyro[3];
        double 	mag[3];
        
        double  accel_res;
        double 	gyro_res;
        double 	mag_res;
        
        double 	accel_bias[3];
        double 	gyro_bias[3];
        double 	mag_bias[3];
        
        double	roll;
        double	pitch;
        double	yaw;
        
        channel_t channel;
    } imu_t;
    
    struct imu_update_ops
    {
        void (*all)(imu_t * imu);
        void (*accel)(imu_t * imu);
        void (*gyro)(imu_t * imu);
        void (*mag)(imu_t * imu);
        
        void (*orientation)(imu_t * imu);
        void (*roll)(imu_t * imu);
        void (*pitch)(imu_t * imu);
        void (*yaw)(imu_t * imu);
    };
    
    struct imu_normalize_ops
    {
        void (*all)(imu_t * imu);
        void (*accel)(imu_t * imu);
        void (*mag)(imu_t * imu);
    };
    
    typedef struct
    {
        struct imu_update_ops update;
        struct imu_normalize_ops normalize;
        int (*init)(imu_t * imu);
    } imu_functions;
    extern const imu_functions IMUFunctions;
    
    void Read_SERCOM_IMU_Packet( imu_t * imu );
    void Read_SERCOM_IMU_Orientation( imu_t * imu );
    
    void IMU_SERCOM_Init(imu_t * imu);
    void IMU_Update_All(imu_t * imu);
    void IMU_Update_Orientation(imu_t * imu);
    void IMU_Normalize_All( imu_t * imu );
    void IMU_Normalize_Accel( imu_t * imu );
    void IMU_Normalize_Mag( imu_t * imu );
    void IMU_Update_Roll( imu_t * imu );
    void IMU_Update_Pitch( imu_t * imu );
    void IMU_Update_Yaw( imu_t * imu );
    double IMU_Roll_Error_Get( imu_t * imu );

#ifdef __cplusplus
}
#endif

#endif /* imu_wrapper_h */
