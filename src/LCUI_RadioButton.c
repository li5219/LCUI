/* ***************************************************************************
 * LCUI_RadioButton.c -- LCUI‘s RadioButton widget
 * 
 * Copyright (C) 2012 by
 * Liu Chao
 * 
 * This file is part of the LCUI project, and may only be used, modified, and
 * distributed under the terms of the GPLv2.
 * 
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 * 
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *  
 * The LCUI project is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for more details.
 * 
 * You should have received a copy of the GPLv2 along with this file. It is 
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/
 
/* ****************************************************************************
 * LCUI_RadioButton.c -- LCUI 的单选框部件
 *
 * 版权所有 (C) 2012 归属于 
 * 刘超
 * 
 * 这个文件是LCUI项目的一部分，并且只可以根据GPLv2许可协议来使用、更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 * 
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 * 
 * LCUI 项目是基于使用目的而加以散布的，但不负任何担保责任，甚至没有适销性或特
 * 定用途的隐含担保，详情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>. 
 * ****************************************************************************/

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_PICBOX_H
#include LC_RADIOBTN_H
#include LC_GRAPHICS_H
#include LC_RES_H 
#include LC_MISC_H 
#include LC_INPUT_H 
#include LC_MEM_H 

static LCUI_Queue	mutex_lib;
static int mutex_lib_init = 0; /* 标志，是否初始化过 */

void RadioButton_Delete_Mutex(LCUI_Widget *widget)
/* 功能：将单选框从互斥关系链中移除 */
{
	int i, total;
	LCUI_Widget *tmp;
	LCUI_RadioButton *rb;
	
	rb = (LCUI_RadioButton *)Get_Widget_Private_Data(widget);
	total = Queue_Get_Total(rb->mutex);
	for(i=0; i<total; ++i)
	{
		tmp = (LCUI_Widget*)Queue_Get(rb->mutex, i);
		if(tmp == widget)
		{
			Queue_Delete_Pointer(rb->mutex, i);
			break;
		}
	}
}

void RadioButton_Create_Mutex(LCUI_Widget *a, LCUI_Widget *b)
/* 功能：为两个单选框建立互斥关系 */
{
	int pos;
	LCUI_Queue *p, queue;
	LCUI_RadioButton *rb_a, *rb_b;
	if(mutex_lib_init == 0)
	{
		Queue_Init(&mutex_lib, sizeof(LCUI_Queue), Destroy_Queue);
		mutex_lib_init = 1;
	}
	
	rb_a = (LCUI_RadioButton *)Get_Widget_Private_Data(a);
	rb_b = (LCUI_RadioButton *)Get_Widget_Private_Data(b);
	
	if(rb_a->mutex == NULL)
	{
		if(rb_b->mutex == NULL)
		{
			Queue_Init(&queue, sizeof(LCUI_Widget*), NULL);
			/* 将子队列添加至父队列，并获取位置 */
			pos = Queue_Add(&mutex_lib, &queue);
			/* 从队列中获取指向子队列的指针 */
			p = (LCUI_Queue*)Queue_Get(&mutex_lib, pos);
			/* 添加指针至队列 */
			Queue_Add_Pointer(p, a);
			Queue_Add_Pointer(p, b);
			/* 保存指向关系队列的指针 */
			rb_a->mutex = p;
			rb_b->mutex = p;
		}
		else
		{
			Queue_Add_Pointer(rb_b->mutex, a);
			rb_a->mutex = rb_b->mutex;
		}
	}
	else
	{
		if(rb_b->mutex == NULL) 
		{
			Queue_Add_Pointer(rb_a->mutex, b);
			rb_b->mutex = rb_a->mutex;
		}
		else 
		{/* 否则，两个都和其它部件有互斥关系，需要将它们拆开，并重新建立互斥关系 */
			RadioButton_Delete_Mutex(a);
			RadioButton_Delete_Mutex(b);
			RadioButton_Create_Mutex(a, b);
		}
	}
}


void Set_RadioButton_On(LCUI_Widget *widget)
/* 功能：设定单选框为选中状态 */
{
	LCUI_RadioButton *radio_button;
	LCUI_Widget *other;
	int i, total;
	radio_button = (LCUI_RadioButton *)
								Get_Widget_Private_Data(widget); 
	if(NULL != radio_button->mutex)
	{/* 如果与其它部件有互斥关系，就将其它单选框部件的状态改为“未选中”状态 */
		total = Queue_Get_Total(radio_button->mutex);
		for(i=0; i<total; ++i)
		{
			other = (LCUI_Widget*)Queue_Get(radio_button->mutex, i);
			Set_RadioButton_Off(other);
		}
	}
	radio_button->on = IS_TRUE;
	Draw_Widget(widget);
}

void Set_RadioButton_Off(LCUI_Widget *widget)
/* 功能：设定单选框为未选中状态 */
{
	LCUI_RadioButton *radio_button = (LCUI_RadioButton *)
								Get_Widget_Private_Data(widget); 
	
	radio_button->on = IS_FALSE;
	Draw_Widget(widget);
}

int Get_RadioButton_Status(LCUI_Widget *widget)
/* 功能：获取单选框的状态 */
{
	LCUI_RadioButton *radio_button = (LCUI_RadioButton *)
								Get_Widget_Private_Data(widget); 
	
	return radio_button->on;
}

int RadioButton_Is_On(LCUI_Widget *widget)
/* 功能：检测单选框是否被选中 */
{
	if(IS_TRUE == Get_RadioButton_Status(widget))
		return 1;
	
	return 0;
}

int RadioButton_Is_Off(LCUI_Widget *widget)
/* 功能：检测单选框是否未选中 */
{
	if(IS_TRUE == Get_RadioButton_Status(widget))
		return 0;
	
	return 1;
}

void Switch_RadioButton_Status(LCUI_Widget *widget, void *arg)
/* 
 * 功能：切换单选框的状态
 * 说明：这个状态，指的是打勾与没打勾的两种状态
 *  */
{ 
	if(RadioButton_Is_Off(widget))
		Set_RadioButton_On(widget); 
}

void RadioButton_Set_ImgBox_Size(LCUI_Widget *widget, LCUI_Size size)
/* 功能：设定单选框中的图像框的尺寸 */
{
	if(size.w <= 0 && size.h <= 0)
		return;
		
	LCUI_Widget *imgbox = Get_RadioButton_ImgBox(widget);
	Resize_Widget(imgbox, size);
	/* 由于没有布局盒子，不能自动调整部件间的间隔，暂时用这个方法 */
	Set_Widget_Align(imgbox->parent, ALIGN_MIDDLE_LEFT, Pos(size.w, 0));
}

static void RadioButton_Init(LCUI_Widget *widget)
/* 功能：初始化单选框部件的数据 */
{
	LCUI_Widget *container[2];
	LCUI_RadioButton *radio_button = (LCUI_RadioButton*)
				Malloc_Widget_Private(widget, sizeof(LCUI_RadioButton));
	
	radio_button->on = IS_FALSE;
	/* 初始化图片数据 */ 
	Graph_Init(&radio_button->img_off_disable);
	Graph_Init(&radio_button->img_off_normal);
	Graph_Init(&radio_button->img_off_focus);
	Graph_Init(&radio_button->img_off_down);
	Graph_Init(&radio_button->img_off_over);
	Graph_Init(&radio_button->img_on_disable);
	Graph_Init(&radio_button->img_on_normal);
	Graph_Init(&radio_button->img_on_focus);
	Graph_Init(&radio_button->img_on_down);
	Graph_Init(&radio_button->img_on_over);
	
	radio_button->mutex = NULL;
	
	radio_button->label = Create_Widget("label");/* 创建label部件 */
	radio_button->imgbox = Create_Widget("picture_box"); /* 创建图像框部件 */
	/* 创建两个容器，用于调整上面两个部件的位置 */
	container[0] = Create_Widget(NULL);
	container[1] = Create_Widget(NULL);
	
	/* 启用这些部件的自动尺寸调整的功能 */
	Enable_Widget_Auto_Size(widget);
	Enable_Widget_Auto_Size(container[0]);
	Enable_Widget_Auto_Size(container[1]);
	
	/* 将按钮部件作为label部件的容器 */
	Widget_Container_Add(container[0], radio_button->imgbox);
	Widget_Container_Add(container[1], radio_button->label);
	Widget_Container_Add(widget, container[0]);
	Widget_Container_Add(widget, container[1]);
	
	/* 调整尺寸 */
	Resize_Widget(radio_button->imgbox, Size(20, 20));
	/* 调整布局 */
	Set_Widget_Align(container[0], ALIGN_MIDDLE_LEFT, Pos(0,0));
	Set_Widget_Align(container[1], ALIGN_MIDDLE_LEFT, Pos(22,0));
	Set_Widget_Align(radio_button->imgbox, ALIGN_MIDDLE_CENTER, Pos(0,0));
	Set_Widget_Align(radio_button->label, ALIGN_MIDDLE_CENTER, Pos(0,0));
	
	Set_PictureBox_Size_Mode(radio_button->imgbox, SIZE_MODE_STRETCH);
	
	/* 显示之 */
	Show_Widget(radio_button->label);
	Show_Widget(radio_button->imgbox);
	Show_Widget(container[0]);
	Show_Widget(container[1]);
	
	Widget_Clicked_Event_Connect(widget, Switch_RadioButton_Status, NULL);
	Response_Status_Change(widget);
}


static void Exec_Update_RadioButton(LCUI_Widget *widget)
/* 功能：更新单选框的图形数据 */
{
	LCUI_Graph *p;
	LCUI_RadioButton *radio_button = (LCUI_RadioButton *)
								Get_Widget_Private_Data(widget); 
								
	if(Strcmp(&widget->style, "custom") == 0)
	{/* 如果为自定义风格，那就使用用户指定的图形 */
		//printf("custom\n"); 
		if(widget->enabled == IS_FALSE) 
			widget->status = WIDGET_STATUS_DISABLE;
			
		switch(widget->status)
		{/* 判断按钮的状态，以选择相应的背景色 */
		case WIDGET_STATUS_NORMAL:
			if(radio_button->on == IS_TRUE)
				p = &radio_button->img_on_normal;
			else
				p = &radio_button->img_off_normal;
			
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p);
			break;
		case WIDGET_STATUS_OVERLAY :
			if(radio_button->on == IS_TRUE)
				p = &radio_button->img_on_over;
			else
				p = &radio_button->img_off_over;
			
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p);
			break;
		case WIDGET_STATUS_CLICKING : 
			if(radio_button->on == IS_TRUE)
				p = &radio_button->img_on_down;
			else
				p = &radio_button->img_off_down;
			
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p);
			break;
		case WIDGET_STATUS_CLICKED : 
			break;
		case WIDGET_STATUS_FOCUS : 
			break;
		case WIDGET_STATUS_DISABLE :
			if(radio_button->on == IS_TRUE)
				p = &radio_button->img_on_disable;
			else
				p = &radio_button->img_off_disable;
			
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p); 
			break;
			default :
			break;
		} 
	}
	else
	{/* 如果按钮的风格为缺省 */
		Strcpy(&widget->style, "default");
		if(widget->enabled == IS_FALSE) 
			widget->status = WIDGET_STATUS_DISABLE;
		
		/* 先释放PictureBox部件中保存的图形数据的指针 */
		p = Get_PictureBox_Graph(radio_button->imgbox);
		Free_Graph(p);
		
		/* 由于本函数在退出后，使用局部变量保存的图形数据会无效，因此，申请内存空间来储存 */
		p = (LCUI_Graph*)calloc(1,sizeof(LCUI_Graph));
		
		switch(widget->status)
		{/* 判断按钮的状态，以选择相应的背景色 */
		case WIDGET_STATUS_NORMAL:
			if(radio_button->on == IS_TRUE)
				Load_Graph_Default_RadioButton_On_Normal(p);
			else
				Load_Graph_Default_RadioButton_Off_Normal(p);
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p);
			break;
		case WIDGET_STATUS_OVERLAY :
			if(radio_button->on == IS_TRUE)
				Load_Graph_Default_RadioButton_On_Selected(p);
			else
				Load_Graph_Default_RadioButton_Off_Selected(p);
			
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p);
			break;
		case WIDGET_STATUS_CLICKING : 
			if(radio_button->on == IS_TRUE)
				Load_Graph_Default_RadioButton_On_Pressed(p);
			else
				Load_Graph_Default_RadioButton_Off_Pressed(p);
			
			Set_PictureBox_Image_From_Graph(radio_button->imgbox, p);
			break;
		case WIDGET_STATUS_CLICKED : 
			break;
		case WIDGET_STATUS_FOCUS : 
			break;
		case WIDGET_STATUS_DISABLE :
			break;
			default :
			break;
		}
	} 
}

LCUI_Widget *Get_RadioButton_Label(LCUI_Widget *widget)
/* 功能：获取单选框部件中的label部件的指针 */
{
	LCUI_RadioButton *radio_button = (LCUI_RadioButton *)
								Get_Widget_Private_Data(widget); 
	if(NULL == radio_button)
		return NULL;
		
	return radio_button->label;
}

LCUI_Widget *Get_RadioButton_ImgBox(LCUI_Widget *widget)
/* 功能：获取单选框部件中的PictureBox部件的指针 */
{
	LCUI_RadioButton *radio_button = (LCUI_RadioButton *)
								Get_Widget_Private_Data(widget); 
	if(NULL == radio_button)
		return NULL;
		
	return radio_button->imgbox;
}

void Set_RadioButton_Text(LCUI_Widget *widget, const char *fmt, ...)
/* 功能：设定与单选框部件关联的文本内容 */
{
	char text[LABEL_TEXT_MAX_SIZE];
	LCUI_Widget *label = Get_RadioButton_Label(widget); 
	
    memset(text, 0, sizeof(text)); 
    
    va_list ap; 
	va_start(ap, fmt);
	vsnprintf(text, LABEL_TEXT_MAX_SIZE, fmt, ap);
	va_end(ap);

	Set_Label_Text(label, text); 
}

LCUI_Widget *Create_RadioButton_With_Text(const char *fmt, ...)
/* 功能：创建一个带文本内容的单选框 */
{
	char text[LABEL_TEXT_MAX_SIZE];
	LCUI_Widget *widget = Create_Widget("radio_button");
	
    memset(text, 0, sizeof(text)); 
    
    va_list ap; 
	va_start(ap, fmt);
	vsnprintf(text, LABEL_TEXT_MAX_SIZE, fmt, ap);
	va_end(ap); 
	
	Set_RadioButton_Text(widget, text);
	return widget;
}


void Register_RadioButton()
/*
 * 功能：注册部件类型-窗口至部件库
 **/
{
	/* 添加几个部件类型 */
	WidgetType_Add("radio_button"); 
	
	/* 为部件类型关联相关函数 */ 
	WidgetFunc_Add("radio_button",	RadioButton_Init, FUNC_TYPE_INIT);
	WidgetFunc_Add("radio_button", Exec_Update_RadioButton, FUNC_TYPE_UPDATE); 
}
