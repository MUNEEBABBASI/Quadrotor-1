/* This file is part of the Razor AHRS Firmware */

// Output angles: yaw, pitch, roll

float maxgx=0,maxgy=0,maxgz=0;
void output_angles()
{
  int yawi=0,pitchi=0,rolli=0;
  char yawi2=0,yawi3=0,pitchi2=0,pitchi3=0,rolli2=0,rolli3=0;
  int gyx=0,gyy=0,gyz=0;
  char gyx2=0,gyx1=0,gyy2=0,gyy1=0,gyz2=0,gyz1=0;
  int SCALE_MULT=32000;
//  int DCM_Matrix00=DCM_Matrix[0][0]*SCALE_MULT;
//  int DCM_Matrix01=DCM_Matrix[0][1]*SCALE_MULT;
//  int DCM_Matrix02=DCM_Matrix[0][2]*SCALE_MULT;
//  int DCM_Matrix10=DCM_Matrix[1][0]*SCALE_MULT;
//  int DCM_Matrix11=DCM_Matrix[1][1]*SCALE_MULT;
//  int DCM_Matrix12=DCM_Matrix[1][2]*SCALE_MULT;
//  int DCM_Matrix20=DCM_Matrix[2][0]*SCALE_MULT;
//  int DCM_Matrix21=DCM_Matrix[2][1]*SCALE_MULT;
//  int DCM_Matrix22=DCM_Matrix[2][2]*SCALE_MULT;
  char tempcharM,tempcharL,i,j;
  int tempint;
  
  if (output_format == OUTPUT__FORMAT_BINARY)
  {
    float ypr[3];  
    ypr[0] = TO_DEG(yaw);
    ypr[1] = TO_DEG(pitch);
    ypr[2] = TO_DEG(roll);
    Serial.write((byte*) ypr, 12);  // No new-line
  }
  else if (output_format == OUTPUT__FORMAT_TEXT)
  {
    for (i=0;i<3;++i)
    {
      for(j=0;j<3;++j)
      {
        tempint=DCM_Matrix[i][j]*SCALE_MULT+.5;
        tempcharL=tempint&0x00FF;
        tempcharM=(tempint>>8)&0x00FF;
        Serial.print(tempcharM);Serial.print(tempcharL); Serial.print(" ");
      }
    }
//    UPPER LIMIT is 100 even though it can exceed.
//    Simulation got me only till 40 on one of the axes, but if 40 can be reached in one axes then it can come in other axes.
//    I have now got it to saturate at 10.
    SCALE_MULT=3200;
    for (i=0;i<3;++i)
    {
      tempint=Gyro_Vector[i]*SCALE_MULT+.5;
      tempcharL=tempint&0x00FF;
      tempcharM=(tempint>>8)&0x00FF;
      Serial.print(tempcharM);Serial.print(tempcharL); Serial.print(" ");
    }
    
    SCALE_MULT=125;
    for (i=0;i<3;++i)
    {
      tempint=accel[i]*SCALE_MULT+.5;
      tempcharL=tempint&0x00FF;
      tempcharM=(tempint>>8)&0x00FF;
      Serial.print(tempcharM);Serial.print(tempcharL); Serial.print(" ");
    }    
    Serial.println();

    Serial.print((char)myaccel[0]); Serial.print((char)myaccel[1]); Serial.print(",");
    Serial.print((char)myaccel[2]); Serial.print((char)myaccel[3]); Serial.print(",");
    Serial.print((char)myaccel[4]); Serial.print((char)myaccel[5]); Serial.println();
    /*
    Serial.print("#YPR=");
    Serial.print(TO_DEG(yaw)); Serial.print(",");
    Serial.print(TO_DEG(pitch)); Serial.print(",");
    Serial.print(TO_DEG(roll)); Serial.println();
    */
    /*
    Serial.print("#");
    Serial.print(TO_DEG(yaw)); Serial.print(",");
    Serial.print(TO_DEG(pitch)); Serial.print(",");
    Serial.print(TO_DEG(roll)); Serial.print("?");
    Serial.print(Gyro_Vector[0]); Serial.print(",");
    Serial.print(Gyro_Vector[1]); Serial.print(",");
    Serial.print(Gyro_Vector[2]); Serial.print("?");Serial.println();
    */
    
    /*
    yawi=TO_DEG(yaw)*160+0.5;
    yawi2=yawi & 0x00FF;
    yawi3=(yawi>>8)& 0x00FF;

    pitchi=TO_DEG(pitch)*320+0.5;
    pitchi2=pitchi & 0x00FF;
    pitchi3=(pitchi>>8)& 0x00FF;
    
    rolli=TO_DEG(roll)*160+0.5;
    rolli2=rolli & 0x00FF;
    rolli3=(rolli>>8)&0x00FF;

    // UPPER LIMIT is 100 even though it can exceed.
    // Simulation got me only till 40 on one of the axes, but if 40 can be reached in one axes then it can come in other axes.
    // I have now got it to saturate at 10.
    gyx=Gyro_Vector[0]*3200+0.5;//yaw
    gyy=Gyro_Vector[1]*3200+0.5;//pitch
    gyz=Gyro_Vector[2]*3200+0.5;//roll
    
    gyx1=gyx & 0x00FF;
    gyx2=(gyx>>8)&0x00FF;

    gyy1=gyy & 0x00FF;
    gyy2=(gyy>>8)&0x00FF;

    gyz1=gyz & 0x00FF;
    gyz2=(gyz>>8)&0x00FF;
    */
    // First the MSByte then LSByte
    /*
    Serial.print("#");
    Serial.print(yawi3);Serial.print(yawi2);Serial.print(",");
    Serial.print(pitchi3);Serial.print(pitchi2);Serial.print(",");
    Serial.print(rolli3);Serial.print(rolli2);Serial.print(",");
    Serial.print(gyx2);Serial.print(gyx1);Serial.print(",");
    Serial.print(gyy2);Serial.print(gyy1);Serial.print(",");
    Serial.print(gyz2);Serial.print(gyz1);Serial.print("?");
    */
    /*
    if (abs(maxgx)<abs(Gyro_Vector[0]))
    {
      maxgx=abs(Gyro_Vector[0]);
    }
    if (abs(maxgy)<abs(Gyro_Vector[1]))
    {
      maxgy=abs(Gyro_Vector[1]);
    }
    if (abs(maxgz)<abs(Gyro_Vector[2]))
    {
      maxgz=abs(Gyro_Vector[2]);
    }
    Serial.print(maxgx);
    Serial.print(",");
    Serial.print(maxgy);
    Serial.print(",");
    Serial.print(maxgz);
    Serial.println();
    */
  }
}

void output_calibration(int calibration_sensor)
{
  if (calibration_sensor == 0)  // Accelerometer
  {
    // Output MIN/MAX values
    Serial.print("accel x,y,z (min/max) = ");
    for (int i = 0; i < 3; i++) {
      if (accel[i] < accel_min[i]) accel_min[i] = accel[i];
      if (accel[i] > accel_max[i]) accel_max[i] = accel[i];
      Serial.print(accel_min[i]);
      Serial.print("/");
      Serial.print(accel_max[i]);
      if (i < 2) Serial.print("  ");
      else Serial.println();
    }
  }
  else if (calibration_sensor == 1)  // Magnetometer
  {
    // Output MIN/MAX values
    Serial.print("magn x,y,z (min/max) = ");
    for (int i = 0; i < 3; i++) {
      if (magnetom[i] < magnetom_min[i]) magnetom_min[i] = magnetom[i];
      if (magnetom[i] > magnetom_max[i]) magnetom_max[i] = magnetom[i];
      Serial.print(magnetom_min[i]);
      Serial.print("/");
      Serial.print(magnetom_max[i]);
      if (i < 2) Serial.print("  ");
      else Serial.println();
    }
  }
  else if (calibration_sensor == 2)  // Gyroscope
  {
    // Average gyro values
    for (int i = 0; i < 3; i++)
      gyro_average[i] += gyro[i];
    gyro_num_samples++;
      
    // Output current and averaged gyroscope values
    Serial.print("gyro x,y,z (current/average) = ");
    for (int i = 0; i < 3; i++) {
      Serial.print(gyro[i]);
      Serial.print("/");
      Serial.print(gyro_average[i] / (float) gyro_num_samples);
      if (i < 2) Serial.print("  ");
      else Serial.println();
    }
  }
}

void output_sensors_text(char raw_or_calibrated)
{
  Serial.print("#A-"); Serial.print(raw_or_calibrated); Serial.print('=');
  Serial.print(accel[0]); Serial.print(",");
  Serial.print(accel[1]); Serial.print(",");
  Serial.print(accel[2]); Serial.println();

  Serial.print("#M-"); Serial.print(raw_or_calibrated); Serial.print('=');
  Serial.print(magnetom[0]); Serial.print(",");
  Serial.print(magnetom[1]); Serial.print(",");
  Serial.print(magnetom[2]); Serial.println();

  Serial.print("#G-"); Serial.print(raw_or_calibrated); Serial.print('=');
  Serial.print(gyro[0]); Serial.print(",");
  Serial.print(gyro[1]); Serial.print(",");
  Serial.print(gyro[2]); Serial.println();
}

void output_sensors_binary()
{
  Serial.write((byte*) accel, 12);
  Serial.write((byte*) magnetom, 12);
  Serial.write((byte*) gyro, 12);
}

void output_sensors()
{
  if (output_mode == OUTPUT__MODE_SENSORS_RAW)
  {
    if (output_format == OUTPUT__FORMAT_BINARY)
      output_sensors_binary();
    else if (output_format == OUTPUT__FORMAT_TEXT)
      output_sensors_text('R');
  }
  else if (output_mode == OUTPUT__MODE_SENSORS_CALIB)
  {
    // Apply sensor calibration
    compensate_sensor_errors();
    
    if (output_format == OUTPUT__FORMAT_BINARY)
      output_sensors_binary();
    else if (output_format == OUTPUT__FORMAT_TEXT)
      output_sensors_text('C');
  }
  else if (output_mode == OUTPUT__MODE_SENSORS_BOTH)
  {
    if (output_format == OUTPUT__FORMAT_BINARY)
    {
      output_sensors_binary();
      compensate_sensor_errors();
      output_sensors_binary();
    }
    else if (output_format == OUTPUT__FORMAT_TEXT)
    {
      output_sensors_text('R');
      compensate_sensor_errors();
      output_sensors_text('C');
    }
  }
}

