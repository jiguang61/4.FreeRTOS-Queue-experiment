/**
	�����FreeRTOSʵ��
	ʹ�ÿ�����Ұ���STM32F429ϵ�п�����
	ʵ��3-ʹ�ö��н�������������֮���ͨѶ
	ʱ�䣺23/11/17
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp_led.h"
#include "bsp_key.h" 
#include "bsp_debug_usart.h"

/*************************����������********************************/
/*������ƿ�ָ�봴��*/
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t LED_Receive_Task_Handle = NULL;
TaskHandle_t KEY_Send_Task_Handle = NULL;

/*************************�ں˾������********************************/
QueueHandle_t Test_Queue = NULL;


/****************************��������********************************/
static void AppTaskCreate(void);
static void LED_Receive_Task(void);
static void KEY_Send_Task(void);

/******************************�����ڱ������ĺ궨��*******************************/
/*���еĳ��ȣ����ɰ������ٸ���Ϣ*/
#define  QUEUE_LEN    4   
/*������ÿ����Ϣ��С*/
#define  QUEUE_SIZE   4   

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
	BaseType_t xReturn = pdPASS;
	/*LED �˿ڳ�ʼ��*/
	LED_GPIO_Config();
	/*���ڳ�ʼ��*/
	Debug_USART_Config();
	/*��ʼ������*/
	Key_GPIO_Config();
	
	LED_RGBOFF;
	
	printf("FreeRTOSʵ��3,����ʵ�飬���߼���\n");
	
	//���������������������
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate, /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )1,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */
	
	/*�����������*/
	if(xReturn == pdPASS)
	{
		/*�������������*/
		vTaskStartScheduler();
	}
	else
	{
		printf("�������ʧ�ܣ��������\n");
		return -1;
	}
	
	/*������˵����ִ�е�����*/
	while(1);
}

/**************************************************************************************/
static void AppTaskCreate(void)
{
	/*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
	BaseType_t xReturn = pdPASS;
	
	/*�����ٽ�α���*/
	taskENTER_CRITICAL();
	
	/* ����Test_Queue */
  Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
  if(NULL != Test_Queue)
	{
		printf("����Test_Queue��Ϣ���гɹ�!\r\n");
	}
	
	//����LED����
	xReturn = xTaskCreate((TaskFunction_t )LED_Receive_Task, /* ������ں��� */
                        (const char*    )"LED_Receive_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Receive_Task_Handle);/* ������ƿ�ָ�� */
	
	/*�鿴�����Ƿ񴴽��ɹ�*/
	if(xReturn == pdPASS)
	{
		printf("LED���񴴽��ɹ�\n");
	}
	
	//����KEY����
	xReturn = xTaskCreate((TaskFunction_t )KEY_Send_Task, /* ������ں��� */
                        (const char*    )"KEY_Send_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )3,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&KEY_Send_Task_Handle);/* ������ƿ�ָ�� */
							
	/*�鿴�����Ƿ񴴽��ɹ�*/
	if(xReturn == pdPASS)
	{
		printf("KEY���񴴽��ɹ�\n");
	}
	
	/*ɾ������*/
	/*���������ҳ�����ֱ������NULL��ɾ���������������ϾͻῨ����prvTaskExitError��������У��ܹ�*/
	vTaskDelete(AppTaskCreate_Handle);
	
	taskEXIT_CRITICAL();
}

static void LED_Receive_Task(void)
{
	/*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
	BaseType_t xReturn = pdPASS;
	/*����һ��������Ϣ�ı���*/
	uint32_t led_queue = 0;
	
	while(1)
	{
		/*ʹ��led_queue���ܶ���Test_Queue�����ı������ȴ�ʱ��Ϊ���*/
		xReturn = xQueueReceive(Test_Queue,&led_queue,portMAX_DELAY);

//		/*���ݽ��ܼ��*/
		if(xReturn != pdPASS)
		{
			printf("���ݽ��ճ���,�������0x%lx\n",xReturn);
		}
		
		/*���������������1*/
		if(led_queue == 1)
		{
			/*��Ʒ�ת*/
			LED1_TOGGLE;
			printf("��Ʒ�ת\n");
		}
		/*���������������2*/
		if(led_queue == 2)
		{
			/*�����Ʒ�ת*/
			LED2_TOGGLE;
			printf("���Ʒ�ת\n");
		}
	}
}

void KEY_Send_Task(void)
{
	/*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
	BaseType_t xReturn = pdPASS;
	
	uint32_t send_data1 = 1;
  uint32_t send_data2 = 2;
	while(1)
	{
		/* K1 ������ */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
    {
      printf("������Ϣsend_data1��\n");
			/*����������Test_Queue����send_data1ֵ�������еȴ�*/
      xReturn = xQueueSend(Test_Queue,&send_data1,0); 
      if(pdPASS == xReturn)
			{
				printf("��Ϣsend_data1���ͳɹ�!\n\n");
			}
			else
			{
				printf("��Ϣsend_data1����ʧ��\n\n");
			}
    } 
		
		/* K2 ������ */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )
    {
      printf("������Ϣsend_data2��\n");
      /*����������Test_Queue����send_data2ֵ�������еȴ�*/
      xReturn = xQueueSend(Test_Queue,&send_data2,0); 
			
      if(pdPASS == xReturn)
			{
				printf("��Ϣsend_data2���ͳɹ�!\n\n");
			}
			else
			{
				printf("��Ϣsend_data2����ʧ��\n\n");
			}
    }
		
		/*��ʱ20��tick*/
    vTaskDelay(20);
	}
}
 


/*********************************************END OF FILE**********************/

