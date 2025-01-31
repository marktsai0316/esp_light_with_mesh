#include "user_light_hint.h"
#include "osapi.h"
#include "os_type.h"
#include "user_light.h"
#include "user_light_adj.h"


#if LIGHT_DEVICE
#if ESP_MESH_SUPPORT
	#include "mesh.h"
#endif

os_timer_t light_hint_t;
#define LIGHT_INFO os_printf

struct light_saved_param light_param_pre;
//void light_MeshStoreCurParam(struct light_saved_param* plight_param);

LOCAL void ICACHE_FLASH_ATTR
    light_blink(uint32 color)
{
	static bool blink_flg = true;
	if(blink_flg){
        switch(color){
			case HINT_WHITE:
				user_light_set_duty(0,LIGHT_RED);
				user_light_set_duty(0,LIGHT_GREEN);
				user_light_set_duty(0,LIGHT_BLUE);
				user_light_set_duty(5000,LIGHT_COLD_WHITE);
				user_light_set_duty(5000,LIGHT_WARM_WHITE);
				break;
			case HINT_RED:
				user_light_set_duty(10000,LIGHT_RED);
				user_light_set_duty(0,LIGHT_GREEN);
				user_light_set_duty(0,LIGHT_BLUE);
				user_light_set_duty(2000,LIGHT_COLD_WHITE);
				user_light_set_duty(2000,LIGHT_WARM_WHITE);
				break;
			case HINT_GREEN:
				user_light_set_duty(0,LIGHT_RED);
				user_light_set_duty(10000,LIGHT_GREEN);
				user_light_set_duty(0,LIGHT_BLUE);
				user_light_set_duty(2000,LIGHT_COLD_WHITE);
				user_light_set_duty(2000,LIGHT_WARM_WHITE);
				break;
			case HINT_BLUE:
				user_light_set_duty(0,LIGHT_RED);
				user_light_set_duty(0,LIGHT_GREEN);
				user_light_set_duty(10000,LIGHT_BLUE);
				user_light_set_duty(2000,LIGHT_COLD_WHITE);
				user_light_set_duty(2000,LIGHT_WARM_WHITE);
				break;
			default :
				break;
        }
		blink_flg = false;
	}else{
		user_light_set_duty(0,LIGHT_RED);
		user_light_set_duty(0,LIGHT_GREEN);
		user_light_set_duty(0,LIGHT_BLUE);
		user_light_set_duty(0,LIGHT_COLD_WHITE);
		user_light_set_duty(0,LIGHT_WARM_WHITE);
		blink_flg = true;
	}
	pwm_start();
}

void ICACHE_FLASH_ATTR
	light_blinkStart(uint32 COLOR)
{
    os_timer_disarm(&light_hint_t);
    os_timer_setfn(&light_hint_t,light_blink,COLOR);
    os_timer_arm(&light_hint_t,1000,1);
}

//LOCAL uint8 shade_cnt = 0;
LOCAL void ICACHE_FLASH_ATTR
	light_shade(uint32 color)
{
    static bool color_flg = true;
	#if 0
	static uint32 cnt=0;
	if(shade_cnt != 0){	
    	if(cnt >= (2*shade_cnt)){
    		cnt = 0;
			color_flg = true;
    		os_timer_disarm(&light_hint_t);
			light_set_aim(light_param_pre.pwm_duty[0],light_param_pre.pwm_duty[1],light_param_pre.pwm_duty[2],
						  light_param_pre.pwm_duty[3],light_param_pre.pwm_duty[4],light_param_pre.pwm_period,0);
			os_printf("RECOVER LIGHT PARAM: \r\n");
    	}else if(cnt==0){
			color_flg = true;
    	}
		cnt++;
	}
	#endif
	
    if(color_flg){
        switch(color){
        case HINT_GREEN:
            light_set_aim(0,20000,0,2000,2000,1000,0);
            break;
        case HINT_RED:
            light_set_aim(20000,0,0,2000,2000,1000,0);
            break;
        case HINT_BLUE:
            light_set_aim(0,0,20000,2000,2000,1000,0);
            break;
        case HINT_WHITE:
            light_set_aim(0,0,0,20000,20000,1000,0);
            break;
        }        
        color_flg = false;
    }
    else{
        light_set_aim(0,0,0,1000,1000,1000,0);
        color_flg = true;
    }
}



void ICACHE_FLASH_ATTR
	light_shadeStart(uint32 color,uint32 t)	
	//light_shadeStart(uint32 color,uint32 t,uint32 shadeCnt)
{
    LIGHT_INFO("LIGHT SHADE START");
	//shade_cnt = shadeCnt;
	//light_MeshStoreCurParam(&light_param_pre);
	
    os_timer_disarm(&light_hint_t);
    os_timer_setfn(&light_hint_t,light_shade,color);
    os_timer_arm(&light_hint_t,t,1);
}


void ICACHE_FLASH_ATTR
	light_hint_stop(uint32 color)
{
    os_timer_disarm(&light_hint_t);
	switch(color){
        case HINT_RED:
			light_set_aim(2000,0,0,0,0,1000,0);
			break;
		case HINT_GREEN:
			light_set_aim(0,2000,0,0,0,1000,0);
			break;
		case HINT_BLUE:
			light_set_aim(0,0,2000,0,0,1000,0);
			break;
		case HINT_WHITE:
			light_set_aim(0,0,0,0,2000,1000,0);
			break;
		default:
			light_set_aim(0,0,0,0,2000,1000,0);
			break;
	}
}

void ICACHE_FLASH_ATTR
	light_hint_abort()
{
    os_timer_disarm(&light_hint_t);
}


void ICACHE_FLASH_ATTR
	light_MeshStoreCurParam(struct light_saved_param* plight_param)
{
	os_memset(plight_param,0,sizeof(struct light_saved_param));
	
	plight_param->pwm_period = user_light_get_period();
	int i;
	for(i=0;i<PWM_CHANNEL;i++){
		plight_param->pwm_duty[i] = user_light_get_duty(i);
	}
	os_printf("CURRENT LIGHT PARAM:\r\n");
	os_printf("r: %d ; g: %d ; b: %d ;ww: %d ; cw: %d \r\n",plight_param->pwm_duty[0],
		                                                    plight_param->pwm_duty[1],
		                                                    plight_param->pwm_duty[2],
		                                                    plight_param->pwm_duty[3],
		                                                    plight_param->pwm_duty[4]);
		                                                    
}


//LOCAL uint8 MeshLevel;
void ICACHE_FLASH_ATTR
	light_DevShadeLevel(uint32* mlevel)
{
	static uint8 round = 0, color_flg = 0;
	static uint32 level;
	
	level = *((uint32*)mlevel);
	os_printf("round: %d ; level: %d \r\n",round,level);

	//if(level==0) return;
	
	if(round == 0){
		
		light_MeshStoreCurParam(&light_param_pre);
	}

	if(round%2 == 0){
		if(level==0){
			light_set_aim(20000,0,0,0,0,1000,0);
			//user_light_set_duty(20000, LIGHT_RED);
			//user_light_set_duty(0, LIGHT_GREEN);
			//user_light_set_duty(0, LIGHT_BLUE);
			//user_light_set_duty(0, LIGHT_COLD_WHITE);
			//user_light_set_duty(0, LIGHT_WARM_WHITE);
		}else{ 
		    light_set_aim(0,0,0,0,0,1000,0);
		}
	}else if(round < (level*2+1)){  
	    //if(meshStatus==MESH_DISABLE){
#if ESP_MESH_SUPPORT
		sint8 meshStatus=espconn_mesh_get_status();
		switch(meshStatus){
			case MESH_DISABLE:
				os_printf("mesh status: MESH_DISABLE\r\n");
		         light_set_aim(20000,0,0,3000,3000,1000,0);
				 break;
			case MESH_WIFI_CONN:
				os_printf("mesh status: MESH_WIFI_CONN\r\n");
				light_set_aim(0,20000,0,3000,3000,1000,0);
				break;
			case MESH_LOCAL_AVAIL:
				os_printf("mesh status: MESH_LOCAL_AVAIL\r\n");
				light_set_aim(0,0,20000,3000,3000,1000,0);
				break;
			case MESH_ONLINE_AVAIL:
				os_printf("mesh status: MESH_ONLINE_AVAIL\r\n");
				light_set_aim(0,0,0,12000,12000,1000,0);
				break;
		}
#else
		os_printf("NON-MESH DEVICE\r\n");
		light_set_aim(20000,0,0,3000,3000,1000,0);
	
#endif

	}

	round++;
	if(round >= (level*2+1) && round>=2){ //stop,resume color
		os_printf("show level stop ...\r\n");
		light_hint_abort();
		round = 0;
		level = 0;
		light_set_aim(light_param_pre.pwm_duty[0],light_param_pre.pwm_duty[1],light_param_pre.pwm_duty[2],
			          light_param_pre.pwm_duty[3],light_param_pre.pwm_duty[4],light_param_pre.pwm_period,0);
		os_printf("RECOVER LIGHT PARAM: \r\n");
		os_printf("r: %d ; g: %d ; b: %d ;ww: %d ; cw: %d \r\n",light_param_pre.pwm_duty[0],light_param_pre.pwm_duty[1],light_param_pre.pwm_duty[2],
			          light_param_pre.pwm_duty[3],light_param_pre.pwm_duty[4]);
		
	}else{
		os_timer_disarm(&light_hint_t);
		os_timer_setfn(&light_hint_t,light_DevShadeLevel,&level);
		os_timer_arm(&light_hint_t,500,0);
	}

}



void ICACHE_FLASH_ATTR
	light_ShowDevLevel(uint32 mlevel)
{
	light_hint_abort();
	os_printf("%s : level: %d \r\n",__func__,mlevel);
	light_DevShadeLevel(&mlevel);
}

void ICACHE_FLASH_ATTR
	light_Espnow_ShowSyncSuc()
{
	light_ShowDevLevel(1);
}


#endif

