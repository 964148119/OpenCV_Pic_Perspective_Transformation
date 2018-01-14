#include "stdafx.h"
#include "NewWindows.h"

NewWindow::NewWindow(char* label, char* pic_name)
{
	this->pic_name = pic_name;
	this->label = label;
}
void NewWindow::mouse_callback(int event, int x, int y, int flags, void* param)
{
	p = Point(x, y);
	pp = Point(x + x_offset, y + y_offset);
	if (needScroll)
	{
		switch (event)
		{
		case CV_EVENT_RBUTTONDOWN:
			mx = x, my = y;
			dx = 0, dy = 0;
			// �������ʱ��궨λ��ˮƽ������������  
			if (x >= rect_bar_horiz.x && x <= rect_bar_horiz.x + rect_bar_horiz.width
				&& y >= rect_bar_horiz.y && y <= rect_bar_horiz.y + rect_bar_horiz.height)
			{
				clickHorizBar = true;
			}
			// �������ʱ��궨λ�ڴ�ֱ������������  
			if (x >= rect_bar_verti.x && x <= rect_bar_verti.x + rect_bar_verti.width
				&& y >= rect_bar_verti.y && y <= rect_bar_verti.y + rect_bar_verti.height)
			{
				clickVertiBar = true;
			}
			break;
		case CV_EVENT_MOUSEMOVE:
			if (clickHorizBar)
			{
				dx = fabs(x - mx) > 1 ? (int)(x - mx) : 0;
				dy = 0;
			}
			if (clickVertiBar)
			{
				dx = 0;
				dy = fabs(y - my) > 1 ? (int)(y - my) : 0;
			}
			mx = x, my = y;
			break;
		case CV_EVENT_RBUTTONUP:
			mx = x, my = y;
			dx = 0, dy = 0;
			clickHorizBar = false;
			clickVertiBar = false;
			break;
		case CV_EVENT_LBUTTONDOWN:


			//cvShowImage("jizuhn",dst_img);
			imagePoints.push_back(pp);
			cout << label << ": " << pp << endl;
			//_p1count++;
			//cout << "zhihuan count:" << _p1count << endl;
			flag = 1;
			//dx = 0, dy = 0;
			break;
		default:
			dx = 0, dy = 0;
			break;
		}
	}
}

void NewWindow::myShowImageScroll(char* title, IplImage* src_img, int winWidth, int winHeight) // ��ʾ���ڴ�СĬ��Ϊ 1400��700  
{
	CvRect  rect_dst,   // ��������Ч��ͼ����ʾ����  
		rect_src;   // ����ͼ���Ӧ��Դͼ���е�����  
	int imgWidth = src_img->width,
		imgHeight = src_img->height,
		barWidth = 25;  // �������Ŀ�ȣ����أ�  
	double  scale_w = (double)imgWidth / (double)winWidth,    // Դͼ���봰�ڵĿ�ȱ�ֵ  
		scale_h = (double)imgHeight / (double)winHeight;  // Դͼ���봰�ڵĸ߶ȱ�ֵ  

	if (scale_w<1)
		winWidth = imgWidth + barWidth;
	if (scale_h<1)
		winHeight = imgHeight + barWidth;

	int showWidth = winWidth, showHeight = winHeight; // rect_dst �Ŀ�͸�  
	int src_x = 0, src_y = 0;   // Դͼ���� rect_src �����Ͻ�λ��  
	int horizBar_width = 0, horizBar_height = 0,
		vertiBar_width = 0, vertiBar_height = 0;

	needScroll = scale_w>1.0 || scale_h>1.0 ? TRUE : FALSE;
	// ��ͼ������趨�Ĵ��ڴ�С������ʾ������  
	if (needScroll)
	{
		IplImage* dst_img = cvCreateImage(cvSize(winWidth, winHeight), src_img->depth, src_img->nChannels);
		cvZero(dst_img);
		// Դͼ���ȴ��ڴ��ڿ�ȣ�����ʾˮƽ������  
		if (1)
		{
			showHeight = winHeight - barWidth;
			horizBar_width = (int)((double)winWidth / scale_w);
			horizBar_height = winHeight - showHeight;
			horizBar_x = min(
				max(0, horizBar_x + dx),
				winWidth - horizBar_width);
			rect_bar_horiz = cvRect(
				horizBar_x,
				showHeight + 1,
				horizBar_width,
				horizBar_height);
			// ��ʾˮƽ������  
			cvRectangleR(dst_img, rect_bar_horiz, cvScalarAll(255), -1);
		}

		// Դͼ��߶ȴ��ڴ��ڸ߶ȣ�����ʾ��ֱ������  
		if (scale_h > 1.0)
		{
			// printf("come!\n");
			showWidth = winWidth - barWidth;
			vertiBar_width = winWidth - showWidth;
			vertiBar_height = (int)((double)winHeight / scale_h);
			vertiBar_y = min(
				max(0, vertiBar_y + dy),
				winHeight - vertiBar_height);
			//printf("vertiBar_width:%d vertiBar_height:%d\n", vertiBar_width, vertiBar_height);
			//printf("x:%d y:%d\n", showWidth + 1, vertiBar_y);
			rect_bar_verti = cvRect(
				showWidth + 1,
				vertiBar_y,
				vertiBar_width,
				vertiBar_height);
			// ��ʾ��ֱ������  
			//printf("w:%d h:%d\n", dst_img->width, dst_img->height);
			cvRectangleR(dst_img, rect_bar_verti, cvScalarAll(255), -1);
		}

		showWidth = min(showWidth, imgWidth);
		showHeight = min(showHeight, imgHeight);
		// ���ô�����ʾ���� ROI  
		rect_dst = cvRect(0, 0, showWidth, showHeight);
		cvSetImageROI(dst_img, rect_dst);
		// ����Դͼ��� ROI  
		src_x = (int)((double)horizBar_x*scale_w);
		src_y = (int)((double)vertiBar_y*scale_h);
		src_x = min(src_x, imgWidth - showWidth);
		src_y = min(src_y, imgHeight - showHeight);
		rect_src = cvRect(src_x, src_y, showWidth, showHeight);
		x_offset = src_x;
		y_offset = src_y;
		cvSetImageROI(src_img, rect_src);
		if (flag == 1)
		{
			cvCircle(src_img, p, 3, Scalar(0, 0, 255), -1);
			flag = 0;
		}
		// ��Դͼ�����ݸ��Ƶ�������ʾ��  
		cvCopy(src_img, dst_img);

		cvResetImageROI(dst_img);
		cvResetImageROI(src_img);
		// ��ʾͼ��͹�����  
		cvShowImage(title, dst_img);

		cvReleaseImage(&dst_img);
	}
	// Դͼ��С���趨���ڣ���ֱ����ʾͼ���޹�����  
	else
	{
		cvShowImage(title, src_img);
	}
}


void m_callback(int event, int x, int y, int flags, void* param)
{
	NewWindow* p_win = (NewWindow*)param;
	p_win->mouse_callback(event, x, y, flags, NULL);
}

void NewWindow::CreateWindows()
{
	int width = 1400, height = 700;  //��ʾ��ͼƬ��С

	cvNamedWindow(label, 1);

	cvSetMouseCallback(label, m_callback, this);

	image = cvLoadImage(pic_name, CV_LOAD_IMAGE_COLOR);


	while (1)
	{
		myShowImageScroll(label, image, width, height);
		//Sleep(100);
		int KEY = cvWaitKey(10);
		if ((char)KEY == 27)
			break;
	}

	cvDestroyWindow(label);
}
