//
//  imu_wrapper.c
//  IMU Math
//
//  Created by Matthew Fonken on 12/19/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "imu_wrapper.h"

#define DEL  sizeof(char)
#define SEP  sizeof(char)
#define FLSZ (1+1+3)
#define BUFFSIZE(X) (int)(DEL+(int)X*(SEP+FLSZ))

#define MIN_PACKET_LEN_RAW BUFFSIZE(9)
#define MIN_PACKET_LEN_ORI BUFFSIZE(6)

#define BUFFER_LENGTH   (1 << 7)

#define PACKET_DEL '\r'

double mag_cal[] = { -2.396, 38.040, 1.093 };
char line[BUFFER_LENGTH];

static int init( imu_t * imu )
{
    switch(imu->channel.interface)
    {
        default:
        case SERCOM:
            imu->channel.descriptor = Init_SERCOM_Default();
            return (int)SERCOM;
            break;
    }
    return -1;
}

const imu_functions IMUFunctions =
{
    .init = init,
    
    .update.all = IMU_Update_All,
    .update.orientation = IMU_Update_Orientation,
    .update.roll = IMU_Update_Roll,
    .update.pitch = IMU_Update_Pitch,
    .update.yaw = IMU_Update_Yaw,
    
    .normalize.all = IMU_Normalize_All,
    .normalize.accel = IMU_Normalize_Accel,
    .normalize.mag = IMU_Normalize_Mag
};

void IMU_Update_All(imu_t * a)
{
    Read_SERCOM_IMU_Packet(a);
//    IMU_Normalize_All(a);
//    IMU_Update_Roll(a);
//    IMU_Update_Pitch(a);
//    IMU_Update_Yaw(a);
}
void IMU_Update_Orientation(imu_t * a)
{
    Read_SERCOM_IMU_Orientation(a);
}

void IMU_Normalize_All( imu_t * imu )
{
    IMU_Normalize_Accel(imu);
    IMU_Normalize_Mag(imu);
}
void IMU_Normalize_Accel( imu_t * imu )
{
    double x,y,z,n;
    x = imu->accel[0];
    y = imu->accel[1];
    z = imu->accel[2];
    
    n = sqrt( x*x + y*y + z*z );
    imu->accel[0] /= n;
    imu->accel[1] /= n;
    imu->accel[2] /= n;
}
void IMU_Normalize_Mag( imu_t * imu )
{
    double x,y,z,n;
    x = imu->mag[0];
    y = imu->mag[1];
    z = imu->mag[2];
    n = sqrt( x*x + y*y + z*z );
    imu->mag[0] /= n;
    imu->mag[1] /= n;
    imu->mag[2] /= n;
}

void IMU_Update_Roll( imu_t * imu )
{
    /* AN4248: Eq. 13 */
    imu->roll = atan2( imu->accel[1], imu->accel[2] );
    
    /* AN3461: Eq. 37 */
    //    double den = sqrt( ( ( imu->accel[1] * imu->accel[1] ) + ( imu->accel[2] * imu->accel[2] ) ) );
    //    imu->roll = atan2( -imu->accel[0], den );
}

/* Note: Requires updated roll */
void IMU_Update_Pitch( imu_t * imu )
{
    /* AN4248: Eq. 14 */
    double den = ( imu->accel[1] * sin( imu->roll ) ) + ( imu->accel[2] * cos ( imu->roll ) );
    imu->pitch = atan2( -imu->accel[0], den );
    
    /* AN3461: Eq. 38 */
    //    double den = sign( imu->accel[2] ) * sqrt( ( ( imu->accel[2] * imu->accel[2] ) + ( MU * ( imu->accel[0] * imu->accel[0] ) ) ) );
    //    imu->pitch = atan2( imu->accel[1], den );
}

/* Note: Requires updated pitch and roll */
void IMU_Update_Yaw( imu_t * imu )
{
    /* NOTE: Magnetometer X and Y axes are switched between AN4248<>LSM9DS1 */
    
    /* AN4248: Eq. 22 */
    double sin_phi   = sin( imu->roll );
    double sin_theta = sin( imu->pitch );
    double cos_phi   = cos( imu->roll );
    double cos_theta = cos( imu->pitch );
    double num = ( imu->mag[2] * sin_phi ) - ( imu->mag[1] * cos_phi );
    double den = ( imu->mag[0] * cos_theta ) + ( imu->mag[1] * ( sin_theta * sin_phi ) ) + ( imu->mag[2] * ( sin_theta * cos_phi ) );
    imu->yaw = -atan2( -num, den );
}

void Read_SERCOM_IMU_Packet( imu_t * imu )
{
    char buffer[BUFFER_LENGTH];
    int bytes_read = -1, ptr = 0, isnl = 0;
    while( !isnl && ptr < BUFFER_LENGTH-1)
    {
        bytes_read = 0;
        while(bytes_read <= 0) bytes_read = Read_SERCOM_Bytes(imu->channel.descriptor, buffer, (size_t)BUFFER_LENGTH);
        Read_SERCOM_Bytes(imu->channel.descriptor, buffer, (size_t)BUFFER_LENGTH);
        for(int i = ptr, j = 0; i < ptr + bytes_read; i++, j++)
        {
            if(buffer[j] == PACKET_DEL)
            {
                line[i] = PACKET_DEL;
                isnl = 1;
            }
            else if(buffer[j] != '\n' && buffer[j] != '\\') line[i] = buffer[j];
            buffer[j] = 0;
        }
        ptr += bytes_read;
    }
    line[ptr] = ',';
    line[++ptr] = '\0';
    if(ptr < MIN_PACKET_LEN_RAW) return;
    
#ifdef PACKET_DEBUG
    printf("\nR(%d): %s\n", ptr, line);
#endif
    
    double v[9];
    tokenifyPacket( line, ptr, 9, 'r', v);
    
    if( v[0] == 0xffff ) return;
#ifdef PACKET_DEBUG
    for(int i = 0; i < 9; i++)
        printf("(%d)%.2f ", i, v[i]);
    printf("\n");
#endif
    
//    imu->accel_raw[0] = v[0];
//    imu->accel_raw[1] = v[1];
//    imu->accel_raw[2] = v[2];
    
    imu->accel[0] = v[0];
    imu->accel[1] = v[1];
    imu->accel[2] = v[2];
    imu->gyro[0]  = v[3];
    imu->gyro[1]  = v[4];
    imu->gyro[2]  = v[5];
    imu->mag[1]   = v[6] - mag_cal[0];
    imu->mag[0]   = v[7] - mag_cal[1];
    imu->mag[2]   =-(v[8] - mag_cal[2]);
}

void Read_SERCOM_IMU_Orientation( imu_t * imu )
{
    char buffer[BUFFER_LENGTH];
    int bytes_read = -1, ptr = 0, isnl = 0;
    while( !isnl && ptr < BUFFER_LENGTH-1)
    {
        bytes_read = 0;
        while(bytes_read <= 0) bytes_read = Read_SERCOM_Bytes(imu->channel.descriptor, buffer, (size_t)BUFFER_LENGTH);
        Read_SERCOM_Bytes(imu->channel.descriptor, buffer, (size_t)BUFFER_LENGTH);
        for(int i = ptr, j = 0; i < ptr + bytes_read; i++, j++)
        {
            if(buffer[j] == PACKET_DEL)
            {
                line[i] = PACKET_DEL;
                isnl = 1;
            }
            else if(buffer[j] != '\n' && buffer[j] != '\\') line[i] = buffer[j];
            buffer[j] = 0;
        }
        ptr += bytes_read;
    }
    line[ptr] = ',';
    line[++ptr] = '\0';
    if(ptr < MIN_PACKET_LEN_ORI) return;
    
#ifdef PACKET_DEBUG
    printf("\nR(%d): %s\n", ptr, line);
#endif
    
    double v[6];
    tokenifyPacket( line, ptr, 6, 'o', v );
    
    if( v[0] == 0xffff ) return;
#ifdef PACKET_DEBUG
    for(int i = 0; i < 6; i++)
    printf("(%d)%.2f ", i, v[i]);
    printf("\n");
#endif
    
    imu->pitch  =  v[0];
    imu->roll   = -v[1];
    imu->yaw    =  360-v[2];
    
    imu->accel_raw[0] = v[3];
    imu->accel_raw[1] = v[4];
    imu->accel_raw[2] = v[5];
}
