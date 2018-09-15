#include <utils/Log.h>
#include "sensor.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

#define OTP_AUTO_LOAD_LSC_gc5005_gcore

#define RG_TYPICAL_gc5005_gcore    	0x0000
#define BG_TYPICAL_gc5005_gcore		0x0000
#define INFO_START_ADD         0x01
#define INFO_WIDTH       0x07
#define WB_START_ADD           0x0f
#define WB_WIDTH         0x03
#define DD_BASE_ADDR           0x03 
#define CUSTOM_BASE_ADDR           0x16

typedef enum{
	otp_page0=0,
	otp_page1,
}otp_page;

typedef enum{
	otp_close=0,
	otp_open,
}otp_state;


static uint8_t gc5005_read_otp(uint8_t addr)
{
	uint8_t value;

	Sensor_WriteReg(0xfe,0x00);
	Sensor_WriteReg(0xd5,addr);
	Sensor_WriteReg(0xf3,0x20);
	value = Sensor_ReadReg(0xd7);

	return value;
}



static void gc5005_select_page_otp(otp_page otp_select_page)
{
	uint8_t page;
	
	Sensor_WriteReg(0xfe,0x00);
	page = Sensor_ReadReg(0xd4);

	switch(otp_select_page)
	{
	case otp_page0:
		page = page & 0xfb;
		break;
	case otp_page1:
		page = page | 0x04;
		break;
	default:
		break;
	}

	usleep(5 * 1000);
	Sensor_WriteReg(0xd4,page);	

}




static uint32_t gc5005_gcore_read_otp_info(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	struct otp_info_t *otp_info=(struct otp_info_t *)param_ptr;
	uint8_t check_info_flag0,check_info_flag1,check_custom_flag;
	uint8_t index,i,j,cnt=0;
	uint8_t info_start_add,wb_start_add;
	uint8_t total_number=0,custom_num=0; 
	uint8_t check_flag,type;
	uint8_t dd0=0,dd1=0,dd2=0;
	uint16_t x,y;


	/*TODO*/
	gc5005_select_page_otp(otp_page0);
	check_info_flag0 = gc5005_read_otp(0x00);
	
	switch(check_info_flag0&0x03)
	{
	case 0x00:
		SENSOR_PRINT("gc5005_gcore_read_otp_info: OTP DD is Empty !!\n");
		otp_info->dd_flag = 0x00;
		break;
	case 0x01:
		
		total_number = gc5005_read_otp(0x01) + gc5005_read_otp(0x02);
		
		SENSOR_PRINT("gc5005_gcore_read_otp_info:total_number = %d\n",total_number);
		for(i=0; i<total_number; i++)
		{
			check_flag = gc5005_read_otp(DD_BASE_ADDR + 4*i + 3);
				
			if(check_flag&0x10)
			{//Read OTP
				type = check_flag&0x0f;
		
				dd0 = gc5005_read_otp(DD_BASE_ADDR + 4*i);
				dd1 = gc5005_read_otp(DD_BASE_ADDR + 4*i + 1); 	
				dd2 = gc5005_read_otp(DD_BASE_ADDR + 4*i + 2);
				x = ((dd1&0x0f)<<8) + dd0;
				y = (dd2<<4) + ((dd1&0xf0)>>4);
				SENSOR_PRINT("gc5005_gcore_read_otp_info : type = %d , x = %d , y = %d \n",type,x,y);
				SENSOR_PRINT("gc5005_gcore_read_otp_info : dd0 = %d , dd1 = %d , dd2 = %d \n",dd0,dd1,dd2);
				if(type == 3)
				{
					for(j=0; j<4; j++)
					{
						otp_info->dd_param_x[cnt] = x;
						otp_info->dd_param_y[cnt] = y + j;
						otp_info->dd_param_type[cnt++] = 2;
					}
				}
				else
				{
					otp_info->dd_param_x[cnt] = x;
					otp_info->dd_param_y[cnt] = y;
					otp_info->dd_param_type[cnt++] = type;
				}
		
			}
			else
			{
				SENSOR_PRINT("gc5005_gcore_read_otp_info:check_id[%d] = %x,checkid error!!\n",i,check_flag);
			}
		}
		otp_info->dd_cnt = cnt;
		otp_info->dd_flag = 0x01;
		break;
	case 0x02:
		SENSOR_PRINT("gc5005_gcore_read_otp_info: OTP DD is Invalid !!\n");
		otp_info->dd_flag = 0x02;
		break;
	default :
		break;
	}



	gc5005_select_page_otp(otp_page1);
	check_info_flag1 = gc5005_read_otp(0x00);


	for(index=0;index<2;index++)
	{
		switch((check_info_flag1>>(4 + 2 * index))&0x03)
		{
		case 0x00:
			SENSOR_PRINT("gc5005_gcore_read_otp_info: OTP info group%d is Empty !!\n", index + 1);
			break;
		case 0x01:
			info_start_add = INFO_START_ADD + index * INFO_WIDTH;
			otp_info->module_id = gc5005_read_otp(info_start_add);
			otp_info->lens_id = gc5005_read_otp(info_start_add + 1); 
			otp_info->vcm_driver_id = gc5005_read_otp(info_start_add + 2);
			otp_info->vcm_id = gc5005_read_otp(info_start_add + 3);
			otp_info->year = gc5005_read_otp(info_start_add + 4);
			otp_info->month = gc5005_read_otp(info_start_add + 5);
			otp_info->day = gc5005_read_otp(info_start_add + 6);
			break;
		case 0x02:
			SENSOR_PRINT("gc5005_gcore_read_otp_info: OTP info group%d is Invalid !!\n", index + 1);
			break;
		default :
			break;
		}
		
		switch((check_info_flag1>>(2 * index))&0x03)
		{
		case 0x00:
			SENSOR_PRINT("gc5005_gcore_read_otp_info: OTP wb group%d is Empty !!\n", index + 1);
			otp_info->wb_flag = otp_info->wb_flag|0x00;
			break;
		case 0x01:
			wb_start_add = WB_START_ADD + index * WB_WIDTH;
			otp_info->r_gain_current = gc5005_read_otp(wb_start_add);
			otp_info->g_gain_current = gc5005_read_otp(wb_start_add + 1); 
			otp_info->b_gain_current = gc5005_read_otp(wb_start_add + 2);
			otp_info->wb_flag = otp_info->wb_flag|0x01;
			break;
		case 0x02:
			SENSOR_PRINT("gc5005_gcore_read_otp_info: OTP wb group%d is Invalid !!\n", index + 1);			
			otp_info->wb_flag = otp_info->wb_flag|0x02;
			break;
		default :
			break;
		}
	}

//custom
	check_custom_flag = gc5005_read_otp(0x15);

	switch(check_custom_flag&0xc0)
	{
	case 0x00:
		SENSOR_PRINT("gc5005_gcore_read_otp_info: SRAM_CUSTOM is Empty !!\n");
		break;
	case 0x40:
		custom_num = check_custom_flag & 0x0f;
		SENSOR_PRINT("gc5005_gcore_read_otp_info:custom_num = %d\n",custom_num);

		for(i=0;i<custom_num;i++)
		{
			otp_info->custom_addr[i] = gc5005_read_otp(CUSTOM_BASE_ADDR + i*2 ) ;
			otp_info->custom_value[i] = gc5005_read_otp(CUSTOM_BASE_ADDR + i*2 + 1 ) ;
			
		SENSOR_PRINT("--travis--gc5005_gcore_read_otp_info:custom_addr[%d] = 0x%x,custom_value[%d] = 0x%x\n",i,otp_info->custom_addr[i],i,otp_info->custom_value[i]);		
		}		

		break;
	case 0x80:
		SENSOR_PRINT("gc5005_gcore_read_otp_info: SRAM_CUSTOM is Invalid !!\n");
		break;
	default :
		break;
	}			

	/*print otp information*/
	SENSOR_PRINT("module_id=0x%x",otp_info->module_id);
	SENSOR_PRINT("lens_id=0x%x",otp_info->lens_id);
	SENSOR_PRINT("vcm_id=0x%x",otp_info->vcm_id);
	SENSOR_PRINT("vcm_driver_id=0x%x",otp_info->vcm_driver_id);
	SENSOR_PRINT("data=%d-%d-%d",otp_info->year,otp_info->month,otp_info->day);
	SENSOR_PRINT("wb_r=0x%x",otp_info->r_gain_current);
	SENSOR_PRINT("wb_g=0x%x",otp_info->g_gain_current);
	SENSOR_PRINT("wb_b=0x%x",otp_info->b_gain_current);

	
	return rtn;
}

static uint32_t gc5005_gcore_update_dd(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	struct otp_info_t *otp_info=(struct otp_info_t *)param_ptr;
	uint16_t i=0,j=0,n=0,m=0,s=0,e=0;
	uint16_t x=0,y=0;
	uint16_t temp_x=0,temp_y=0;
	uint8_t temp_type=0;
	uint8_t temp_val0,temp_val1,temp_val2;
	/*TODO*/

	if(0x01 ==otp_info->dd_flag)
	{
#if defined(IMAGE_NORMAL_MIRROR)
		for(i=0; i<otp_info->dd_cnt; i++)
		{
			if(otp_info->dd_param_type[i]==0)
			{	otp_info->dd_param_x[i]= WINDOW_WIDTH - otp_info->dd_param_x[i] + 1;	}
			else if(otp_info->dd_param_type[i]==1)
			{	otp_info->dd_param_x[i]= WINDOW_WIDTH - otp_info->dd_param_x[i] - 1;	}
			else
			{	otp_info->dd_param_x[i]= WINDOW_WIDTH - otp_info->dd_param_x[i];	}
		}
#elif defined(IMAGE_H_MIRROR)
		//do nothing
#elif defined(IMAGE_V_MIRROR)
		for(i=0; i<otp_info->dd_cnt; i++)
		{
			if(otp_info->dd_param_type[i]==0)
			{	
				otp_info->dd_param_x[i]= WINDOW_WIDTH - otp_info->dd_param_x[i];
				otp_info->dd_param_y[i]= WINDOW_HEIGHT - otp_info->dd_param_y[i];
			}
			else if(otp_info->dd_param_type[i]==1)
			{
				otp_info->dd_param_x[i]= WINDOW_WIDTH - otp_info->dd_param_x[i]-1;
				otp_info->dd_param_y[i]= WINDOW_HEIGHT - otp_info->dd_param_y[i]+1;
			}
			else
			{
				otp_info->dd_param_x[i]= WINDOW_WIDTH - otp_info->dd_param_x[i] ;
				otp_info->dd_param_y[i]= WINDOW_HEIGHT - otp_info->dd_param_y[i] + 1;
			}
		}
#elif defined(IMAGE_HV_MIRROR)
		for(i=0; i<otp_info->dd_cnt; i++)
		{	otp_info->dd_param_y[i]= WINDOW_HEIGHT - otp_info->dd_param_y[i] + 1;	}
#endif

		//for(i=0; i<otp_info->dd_cnt; i++)
		//{
		//	SENSOR_PRINT("gc5005_gcore_update_dd 1111, x = %d , y = %d \n",otp_info->dd_param_x[i],otp_info->dd_param_y[i]);	
		//}


		//y
		for(i=0 ; i< otp_info->dd_cnt-1; i++) 
		{
			for(j = i+1; j < otp_info->dd_cnt; j++) 
			{  
				if(otp_info->dd_param_y[i] > otp_info->dd_param_y[j])  
				{  
					temp_x = otp_info->dd_param_x[i] ; otp_info->dd_param_x[i] = otp_info->dd_param_x[j] ;  otp_info->dd_param_x[j] = temp_x;
					temp_y = otp_info->dd_param_y[i] ; otp_info->dd_param_y[i] = otp_info->dd_param_y[j] ;  otp_info->dd_param_y[j] = temp_y;
					temp_type = otp_info->dd_param_type[i] ; otp_info->dd_param_type[i] = otp_info->dd_param_type[j]; otp_info->dd_param_type[j]= temp_type;
				} 
			}
		
		}
		
		//x
		for(i=0; i<otp_info->dd_cnt; i++)
		{
			if(otp_info->dd_param_y[i]==otp_info->dd_param_y[i+1])
			{
				s=i++;
				while((otp_info->dd_param_y[s] == otp_info->dd_param_y[i+1])&&(i<otp_info->dd_cnt-1))
					i++;
				e=i;

				for(n=s; n<e; n++)
				{
					for(m=n+1; m<e+1; m++)
					{
						if(otp_info->dd_param_x[n] > otp_info->dd_param_x[m])
						{
							temp_x = otp_info->dd_param_x[n] ; otp_info->dd_param_x[n] = otp_info->dd_param_x[m] ;  otp_info->dd_param_x[m] = temp_x;
							temp_y = otp_info->dd_param_y[n] ; otp_info->dd_param_y[n] = otp_info->dd_param_y[m] ;  otp_info->dd_param_y[m] = temp_y;
							temp_type = otp_info->dd_param_type[n] ; otp_info->dd_param_type[n] = otp_info->dd_param_type[m]; otp_info->dd_param_type[m]= temp_type;
						}
					}
				}

			}

		}

		
		//write SRAM
		Sensor_WriteReg(0xfe,0x01);
		Sensor_WriteReg(0xbe,0x00);
		Sensor_WriteReg(0xa9,0x01);

		for(i=0; i<otp_info->dd_cnt; i++)
		{
			temp_val0 = otp_info->dd_param_x[i]& 0x00ff;
			temp_val1 = ((otp_info->dd_param_y[i]<<4)& 0x00f0) + ((otp_info->dd_param_x[i]>>8)&0X000f);
			temp_val2 = (otp_info->dd_param_y[i]>>4) & 0xff;
			Sensor_WriteReg(0xaa,i);
			Sensor_WriteReg(0xac,temp_val0);
			Sensor_WriteReg(0xac,temp_val1);
			Sensor_WriteReg(0xac,temp_val2);
			Sensor_WriteReg(0xac,otp_info->dd_param_type[i]);
		SENSOR_PRINT("gc5005_gcore_update_dd travis, val0 = 0x%x , val1 = 0x%x , val2 = 0x%x \n",temp_val0,temp_val1,temp_val2);
		SENSOR_PRINT("gc5005_gcore_update_dd travis, x = %d , y = %d \n",((temp_val1&0x0f)<<8) + temp_val0,(temp_val2<<4) + ((temp_val1&0xf0)>>4));	
		}

		Sensor_WriteReg(0xbe,0x01);
		Sensor_WriteReg(0xfe,0x00);
	}

	return rtn;
}


static uint32_t gc5005_gcore_update_awb(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	struct otp_info_t *otp_info=(struct otp_info_t *)param_ptr;

	/*TODO*/
	if(0x01==(otp_info->wb_flag&0x01))
	{
		Sensor_WriteReg(0xfe,0x00);
		Sensor_WriteReg(0xb8,otp_info->g_gain_current);
		Sensor_WriteReg(0xb9,otp_info->g_gain_current);
		Sensor_WriteReg(0xba,otp_info->r_gain_current);
		Sensor_WriteReg(0xbb,otp_info->r_gain_current);
		Sensor_WriteReg(0xbc,otp_info->b_gain_current);
		Sensor_WriteReg(0xbd,otp_info->b_gain_current);
		Sensor_WriteReg(0xbe,otp_info->g_gain_current);
		Sensor_WriteReg(0xbf,otp_info->g_gain_current);
	}

	return rtn;
}

static uint32_t gc5005_gcore_update_custom(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	uint8_t custom_num=0,i; 
	struct otp_info_t *otp_info=(struct otp_info_t *)param_ptr;

	custom_num = gc5005_read_otp(0x15) & 0x0f;
	SENSOR_PRINT_HIGH("GC5005_MIPI_READ_SRAM_CUSTOM:custom_num = %d\n",custom_num);

	Sensor_WriteReg(0xfe,0x00);

	for(i=0;i<custom_num;i++) 
	{
		Sensor_WriteReg(otp_info->custom_addr[i] ,otp_info->custom_value[i]);
		SENSOR_PRINT_HIGH("--travis--GC5005_MIPI_WRITE_SRAM_CUSTOM,{0x%x,0x%x}!!\n",otp_info->custom_addr[i],otp_info->custom_value[i]);		
	}

	Sensor_WriteReg(0xfe,0x00);	

	return rtn;
}
#ifndef OTP_AUTO_LOAD_LSC_gc5005_gcore

static uint32_t gc5005_gcore_update_lsc(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	struct otp_info_t *otp_info=(struct otp_info_t *)param_ptr;

	/*TODO*/
	
	return rtn;
}

#endif
static uint32_t gc5005_gcore_update_otp(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	struct otp_info_t *otp_info=(struct otp_info_t *)param_ptr;

	rtn=gc5005_gcore_update_dd(param_ptr);
	if(rtn!=SENSOR_SUCCESS)
	{
		SENSOR_PRINT_ERR("OTP dd appliy error!");
		return rtn;
	}


	rtn=gc5005_gcore_update_awb(param_ptr);
	if(rtn!=SENSOR_SUCCESS)
	{
		SENSOR_PRINT_ERR("OTP awb appliy error!");
		return rtn;
	}

	#ifndef OTP_AUTO_LOAD_LSC_gc5005_gcore
	
	rtn=gc5005_gcore_update_lsc(param_ptr);
	if(rtn!=SENSOR_SUCCESS)
	{
		SENSOR_PRINT_ERR("OTP lsc appliy error!");
		return rtn;
	}
	#endif

	rtn=gc5005_gcore_update_custom(param_ptr);
	if(rtn!=SENSOR_SUCCESS)
	{
		SENSOR_PRINT_ERR("OTP custom appliy error!");
		return rtn;
	}
	
	return rtn;
}


static void gc5005_gcore_enable_otp(otp_state state)
{
	uint8_t otp_clk,otp_en;
	otp_clk = Sensor_ReadReg(0xfa);
	otp_en= Sensor_ReadReg(0xd4);	
	if(state)	
	{ 
		otp_clk = otp_clk | 0x10;
		otp_en = otp_en | 0x80;
		usleep(5 * 1000);
		Sensor_WriteReg(0xfa,otp_clk);	// 0xfa[6]:OTP_CLK_en
		Sensor_WriteReg(0xd4,otp_en);	// 0xd4[7]:OTP_en	
	
		SENSOR_PRINT("Enter GC5005_MIPI_enable_OTP: Enable OTP!\n");		
	}
	else			
	{
		otp_en = otp_en & 0x7f;
		otp_clk = otp_clk & 0xef;
		usleep(5 * 1000);
		Sensor_WriteReg(0xd4,otp_en);
		Sensor_WriteReg(0xfa,otp_clk);

		SENSOR_PRINT("Enter GC5005_MIPI_enable_OTP: Disable OTP!\n");
	}

}


static uint32_t gc5005_gcore_identify_otp(void *param_ptr)
{
	uint32_t rtn = SENSOR_SUCCESS;
	
	/*Sensor_WriteReg(0xfe,0x00);
	Sensor_WriteReg(0xfe,0x00);
	Sensor_WriteReg(0xfe,0x00);
	Sensor_WriteReg(0xf7,0x01);//pll enable
	Sensor_WriteReg(0xf8,0x11);//pll mode
	Sensor_WriteReg(0xf9,0xaa);
	Sensor_WriteReg(0xfa,0x84);
	Sensor_WriteReg(0xfc,0x8a); //[6]wclk
	*/
	gc5005_gcore_enable_otp(otp_open);
	rtn=gc5005_gcore_read_otp_info(param_ptr);
	gc5005_gcore_enable_otp(otp_close);
	SENSOR_PRINT("rtn=%d",rtn);

	return rtn;
}

