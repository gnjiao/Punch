#pragma once
//#pragma warning(disable:1083)
#include <iostream>
#include <vector>
#include <mutex>
#include <tuple>

//#include "stdfx.h"
#include "Time.h"
#include "../Tools/Time.h"
#include "../Tools/exstring.h"
//#include "MEvent.h"
//#include "exstring.h"
//#include "cv_module\letter_recog.h"
//#include "cv_module\caffe.h"

#ifdef QT_VERSION
#include <QScreen> 
#endif


#ifndef _WIN64
#error not support win32, win64 is OK
#endif


//命令行里加入  /D "USE_HALCON"，并配置好文件就可以了
#ifdef USE_HALCON
#include "HalconCpp.h"
//#include "Halcon.h"
#endif

#define __USE_OPENCV__
#ifdef __USE_OPENCV__
#include "opencv2/opencv.hpp"
#if _MSC_VER<1900
#error MSC_VER is too low
#else
//#pragma comment(lib,"../../third_party/opencv3.4.0/x64/vc14/lib/opencv_world340.lib")
#endif
#endif


#ifndef __USE_OPENCV__
#error need opencv
#else




#ifndef __EXCV___
#define __EXCV___
class excv
{
private:
	std::string m_name;

#ifdef __AFX_H__


	//Convert HBitmap format to cv::Mat format
	static BOOL HBitmapToMat(HBITMAP& _hBmp, cv::Mat& _mat)
	{
		try {
			BITMAP bmp;
			GetObject(_hBmp, sizeof(BITMAP), &bmp);
			int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
			int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
			cv::Mat v_mat;
			v_mat.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
			GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
			_mat = v_mat;
			return TRUE;
		}
		catch (cv::Exception e)
		{
			std::cout << e.what() << std::endl;
			return FALSE;
		}
	}

	BOOL MatToHBitmap(HBITMAP& _hBmp, cv::Mat& _mat)
	{
		//MAT类的TYPE=（nChannels-1+ CV_8U）<<3
		int nChannels = (_mat.type() >> 3) - CV_8U + 1;
		int iSize = _mat.cols*_mat.rows*nChannels;
		_hBmp = CreateBitmap(_mat.cols, _mat.rows,
			1, nChannels * 8, _mat.data);
		return TRUE;
	}

#endif

#ifdef USE_HALCON
public:
	static bool MatToHImage(cv::Mat &src, Halcon::Hobject &Hobj)
	{
		if (src.empty()) return false;
		using namespace Halcon;
		if (3 == src.channels())
		{
			cv::Mat pImageRed, pImageGreen, pImageBlue;
			std::vector<cv::Mat> sbgr(3);
			cv::split(src, sbgr);

			int length = src.rows * src.cols;
			uchar *dataBlue = new uchar[length];
			uchar *dataGreen = new uchar[length];
			uchar *dataRed = new uchar[length];

			int height = src.rows;
			int width = src.cols;
			for (int row = 0; row < height; row++)
			{
				uchar* ptr = src.ptr<uchar>(row);
				for (int col = 0; col < width; col++)
				{
					dataBlue[row * width + col] = ptr[3 * col];
					dataGreen[row * width + col] = ptr[3 * col + 1];
					dataRed[row * width + col] = ptr[3 * col + 2];
				}
			}

			Halcon::gen_image3(&Hobj, "byte", width, height, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
			delete[] dataRed;
			delete[] dataGreen;
			delete[] dataBlue;
		}
		else if (1 == src.channels())
		{
			int height = src.rows;
			int width = src.cols;
			uchar *dataGray = new uchar[width * height];
			memcpy(dataGray, src.data, width * height);
			Halcon::gen_image1(&Hobj, "byte", width, height, (Hlong)(dataGray));
			delete[] dataGray;
		}
		return true;
	}

	static void h_disp_obj(Halcon::Hobject &obj, Halcon::HTuple &disp_hd) {
		using namespace Halcon;
		try
		{
			Hlong w, h;
			Halcon::get_image_pointer1(obj, NULL, NULL, &w, &h);
			HDevWindowStack::Push(disp_hd);
			HDevWindowStack::SetActive(disp_hd);
			Halcon::set_part(disp_hd, NULL, NULL, h, w);
			disp_obj(obj, disp_hd);
		}
		catch (HException &except)
		{
			TRACE(except.message);
			Halcon::set_tposition(disp_hd, 0, 1);
			Halcon::write_string(disp_hd, except.message);
		}

	}
#endif
	//将小图加到大图中
	static void addImageToBigImage(cv::Mat& big_image, cv::Mat &small_image, cv::Rect pos)
	{
		try
		{
			cv::Mat imageROI = big_image(pos);
			small_image.copyTo(imageROI);
		}
		catch (cv::Exception e)
		{
			std::cout << "add pic error" << std::endl;
		}
	}

	void Init() {
		std::cout << "start init" << std::endl;
	}
public:
	excv(char * name) :m_name(name) {};
	excv() { Init(); };
	~excv() { Init(); };

	static void CvtColor(const cv::Mat &org, cv::Mat &dst, int sel) {
		switch (sel)
		{
		case CV_BGR2GRAY:
			if (CV_8UC1 == org.type())
			{
				dst = org.clone();
			}
			else
			{
				cv::cvtColor(org, dst, CV_BGR2GRAY);
			}
			break;
		case CV_GRAY2BGR:
			if (CV_8UC3 == org.type())
			{
				dst = org.clone();
			}
			else
			{
				cv::cvtColor(org, dst, CV_GRAY2BGR);
			}
		}
	}

	static inline void Contours_Inv(const std::vector<std::vector<cv::Point>> &origin, std::vector<std::vector<cv::Point>> &dst) {
	
	}

	static inline void Contours_Move(const std::vector<std::vector<cv::Point>> &origin, std::vector<std::vector<cv::Point>> &dst, cv::Point direct)
	{

		dst.assign(origin.begin(), origin.end());
		for (size_t i = 0; i < origin.size(); i++)
			for (size_t p = 0; p < origin[i].size(); p++)
			{
				dst[i][p].x = origin[i][p].x + direct.x;
				dst[i][p].y = origin[i][p].y + direct.y;
			}
	}

	static inline void Points_Move(const std::vector<cv::Point> &origin, std::vector<cv::Point> &dst, cv::Point direct)
	{
		dst.clear();
		for (auto pt : origin)
		{
			dst.push_back(cv::Point(pt.x + direct.x, pt.y + direct.y));
		}
	}

	static bool cvt_Color_Gray(cv::Mat src, cv::Mat &dst) {
		try {
			if (CV_8UC1 == src.type())
			{
				dst = src.clone();
			}
			else
			{
				cv::cvtColor(src, dst, CV_BGR2GRAY);
			}
		}
		catch (cv::Exception e)
		{
			return false;
		}
		return true;
	}

//	static excv* GetIns() { static std::mutex mtx; std::lock_guard<std::mutex> lck(mtx); static excv *ec = nullptr; if (nullptr == ec) { ec = new excv(); }return ec; }
	template<typename _T>
	_T distance(cv::Point_<_T> &a, cv::Point_<_T> &b){
		return euclidean_distance(a, b);
	}

	template<typename _T>
	_T euclidean_distance(cv::Point_<_T> &a, cv::Point_<_T> &b){
		return std::sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
	}

	template<typename _T>
	_T mahalanobis_distance(cv::Point_<_T> &a, cv::Point_<_T> &b, int n = 2){
		return 0.0;
	}

 
	//将image图添加到back里
	static void addImageToBigImage(cv::Mat& back, const cv::Mat &image, cv::Rect pos)
	{
		try
		{
			cv::Mat imageROI = back(pos);
			image.copyTo(imageROI);
		}
		catch (cv::Exception e)
		{
			std::cout << "add pic error" << std::endl;
		}
	}

	static void get_contours_image(std::vector<std::vector<cv::Point>> &contours,cv::Mat &image) {
		return;
	};

	static inline int find_max_contours_id(std::vector<std::vector<cv::Point>> &contours){
		try {
			size_t size = contours.at(0).size();
			int max_id = 0; //-1  is invalid
			for (size_t i = 0; i < contours.size(); i++){
				if (size < contours[i].size()){
					size = contours[i].size();
					max_id = static_cast<int>(i);
				}
			}
			return max_id;
		}
		catch (std::out_of_range e)
		{
			std::cout << e.what() << std::endl;
			return -1;
		}
	}


	//保存图片
        static std::string cv_write_image(const cv::Mat & m, char* path, char *file_name)
	{	
		if (0 != strlen(path))
		{
                    system((std::string("mkdir ")+ path).c_str());
		}
		if (m.empty()) return "\0";
                cv::imwrite((cv::String)(std::string(file_name) + ".png"), m);

#ifdef __AFX_H__

		CString Path(path), FileName(file_name), Time_zzz(_Timer::GetIns()->Get_Time_zzz()),fileName;
		if (0 == strlen(path) && 0 == strlen(file_name))
			fileName = Time_zzz;
		else if (0 == strlen(file_name))
			fileName = Path + "/" + Time_zzz;
		else
			fileName = Path + "/" + FileName + "_" + Time_zzz;
		cv::imwrite((cv::String)(CStringA)(fileName+L".bmp"), m);
		return (std::string)(CStringA)(fileName + L".bmp");
#endif
		return "\0";
	}

	//截屏
	static cv::Mat cv_print_screen_ex()
	{
		cv::Mat origin,screen;
		cv_print_screen(origin);
		cv::cvtColor(origin, screen, CV_RGBA2RGB);
		return screen;
	}
	static BOOL cv_print_screen(cv::Mat& dst)
	{
#ifdef __AFX_H__
		HWND hWnd = ::GetDesktopWindow();//获得屏幕的HWND.  
		HDC hScreenDC = ::GetDC(hWnd);   //获得屏幕的HDC.  
		HDC MemDC = ::CreateCompatibleDC(hScreenDC);
		RECT rect;
		::GetWindowRect(hWnd, &rect);
		SIZE screensize;
		screensize.cx = rect.right - rect.left;
		screensize.cy = rect.bottom - rect.top;
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hScreenDC, screensize.cx, screensize.cy);
		HGDIOBJ hOldBMP = ::SelectObject(MemDC, hBitmap);
		::BitBlt(MemDC, 0, 0, screensize.cx, screensize.cy, hScreenDC, rect.left, rect.top, SRCCOPY);
		::SelectObject(MemDC, hOldBMP);
		::DeleteObject(MemDC);
		::ReleaseDC(hWnd, hScreenDC);
		if (FALSE == HBitmapToMat(hBitmap, dst))
		{
			return FALSE;
		};
		return TRUE;
#endif

#ifdef QT_VERSION
		QScreen *screen = QGuiApplication::primaryScreen();
//#error not write qt version print_screen code
#endif
	}

 
	void get_mask_image(const cv::Mat& src, cv::Mat& dst, std::vector<std::vector<cv::Point>>& contours, int ID)
	{
		using namespace cv;
		if (src.empty()) return;
		Mat Mask(src.size(), CV_8U, Scalar(0));
		const int widthStep = static_cast<int>(src.step);
		const int height = static_cast<int>(src.rows);
		uchar *sdata = src.data;
		uchar *ddata = Mask.data;
		drawContours(Mask, contours, ID/*��õ��������*/, Scalar(255), CV_FILLED/*2*/);
#pragma omp parallel
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < widthStep; w++)
			{
				ddata[w] &= sdata[w];
			}
			sdata += widthStep;
			ddata += widthStep;
		}
		dst = Mask.clone();
	}

	//��ϣ���޷�ֱ�ӹ�ϣcv::Point������ת���ַ���
	static std::string Point2str(const cv::Point& pt)
	{
#ifdef __AFX_H__1
#else
		std::ostringstream ostr;
		ostr<< pt.x << "," << pt.y;
		std::string str(ostr.str());
		ostr.clear();
		return str;
#endif
	}

	static cv::Point str2Point(std::string &str)
	{
		std::vector<std::string> pts = tl::exstring::split(str,",");
		try{
			return cv::Point(std::atoi(pts.at(0).c_str()), std::atoi(pts.at(1).c_str()));
		}
		catch (std::out_of_range e)
		{
			std::cout << "str2Point split error" << std::endl;
		}
	}

	//获取旋转角度后得，比较最小外界矩形框，得到一个角度和旋转后的模板
	static cv::RotatedRect get_adjust_model(cv::Mat &model, double &angle, void* _pWnd = nullptr)
	{
		using namespace cv;
		using namespace std;
#ifdef __AFX_H__
		HWND pWnd = (HWND)_pWnd;
#endif
		angle = -0.0;
		int code = 0;
		cv::Mat bin, thres;
		vector<vector<Point>> contours;
		try{
			if (CV_8UC1 == model.type())
			{
				bin = model.clone();
			}
			else if (CV_8UC3 == model.type())
			{
				cv::cvtColor(model, bin, CV_BGR2GRAY);
			}
			else
			{
				code = -2;
				std::cout << "image format error " << code << std::endl;
				return RotatedRect();
			}

			cv::threshold(bin, thres, 128, 255, CV_THRESH_BINARY);
			//寻找最外层轮廓  
			vector<Vec4i> hierarchy;
			findContours(thres, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());
			if (contours.size() != 1)
			{
				puts("没有发现模板，或者发现多个模板\n");
				return RotatedRect();
			}
		}
		catch (cv::Exception e)
		{
			std::cout << "model error code " << code << std::endl;
			return RotatedRect();
		}

		for (auto contour : contours)
		{
			RotatedRect rect = minAreaRect(contour);
			Point2f P[4];
			rect.points(P);
			angle = static_cast<double>(rect.angle);
			return rect;
		}
		return RotatedRect();
	}
};
#endif

#endif
