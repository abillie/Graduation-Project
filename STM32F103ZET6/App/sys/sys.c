#include "sys.h" 
void NVIC_Configuration(void)
{
    //! 设置NVIC中断分组, 2:2;两位枪占优先级;两位响应优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	

}
