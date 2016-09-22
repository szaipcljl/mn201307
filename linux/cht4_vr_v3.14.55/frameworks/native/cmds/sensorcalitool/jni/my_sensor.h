
typedef struct _SENSOR_DATA {
    union {
        /*! \struct accelMilliG 
        *  \brief Accelerometer sensor data.
        * */
        struct {
            float accelX; /*!< \brief X axis data */
            float accelY; /*!< \brief Y axis data */
            float accelZ; /*!< \brief Z axis data */
        } accelMilliG;

        struct {
            float gyroX; /*!< \brief X axis data */
            float gyroY; /*!< \brief Y axis data */
            float gyroZ; /*!< \brief Z axis data */
        } gyroMilliDegreesPerSecond;

        struct {
            float magFieldX; /*!< \brief X axis data */
            float magFieldY; /*!< \brief Y axis data */
            float magFieldZ; /*!< \brief Z axis data */
        }magFieldMilliGauss;
	struct{
	    unsigned int illuminance;
	} alsLux;
        struct {
            float x;
            float y;
            float z;
            float n;
        } data;
    }data;
}SENSOR_DATA_T;
