#include "HexagonSettingWindow.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


HexagonSettingWindow::HexagonSettingWindow()
{
	init();
}

HexagonSettingWindow::HexagonSettingWindow(const cv::Mat& image, const std::string& window_name)
{
	init();
	Open(image, window_name);
}

HexagonSettingWindow::~HexagonSettingWindow()
{
	Close();
}


void HexagonSettingWindow::init()
{
	_max_window_size = cv::Size(1024, 1024);
	_selected_corner_id = -1;
	_conrner_circle_radius = 4;
	_select_distance_threshold = 5;
}


void HexagonSettingWindow::_GenerateHexagon(std::vector<cv::Point2f>& hexagon, const cv::Point2f& center, float scale, float start_rad)
{
	hexagon.resize(7);
	hexagon[0] = center;
	double step = CV_PI / 3;
	double start = -CV_PI / 2 + start_rad;
	for (int i = 1; i <= 6; i++) {
		float angle = (float)(start + (i-1) * step);
		hexagon[i] = cv::Point2f(scale * std::cos(angle), scale * std::sin(angle));
		hexagon[i] += hexagon[0];
	}
}


void HexagonSettingWindow::_MoveHexagon(std::vector<cv::Point2f>& hexagon, const cv::Point2f& center)
{
	hexagon.resize(7);
	cv::Point2f diff = center - hexagon[0];
	hexagon[0] = center;
	for (int i = 1; i <= 6; i++) {
		hexagon[i] += diff;
	}
}


double HexagonSettingWindow::_CalcRotation(const cv::Point2f& pt1, const cv::Point2f& origin, int corner_id)
{
	double step = CV_PI / 3;
	double base_angle = -CV_PI / 2 + (corner_id - 1) * step;
	cv::Point2f vec1 = pt1 - origin;
	return atan2(vec1.y, vec1.x) - base_angle;
}


void HexagonSettingWindow::Open(const cv::Mat& image, const std::string& window_name)
{
	Close();

	_window_name = window_name;
	_src_image = image;
	float scale = _ComputeDisplayScale(_src_image.size(), _max_window_size);
	_display_scale = (scale < 1.0) ? scale : 1.0;
	
	cv::Mat resize_img;
	if (_display_scale < 1.0) {
		cv::Size disp_size(_display_scale * _src_image.cols, _display_scale * _src_image.rows);
		cv::resize(_src_image, _display_image, disp_size);
	}
	else {
		_display_image = _src_image.clone();
	}

	cv::Point2f hex_center(_display_image.cols / 2, _display_image.rows / 2);
	float hex_scale = _display_image.rows / 6;
	_GenerateHexagon(_display_hexagon, hex_center, hex_scale);

	cv::namedWindow(_window_name);
	cv::setMouseCallback(_window_name, HexagonSettingWindow::on_mouse, this);
	RedrawImage();
}


void HexagonSettingWindow::Close()
{
	if (!_window_name.empty()) {
		cv::destroyWindow(_window_name);
		_window_name = std::string();
	}
}

void HexagonSettingWindow::setMaxWindowSize(const cv::Size& win_size) {
	if (win_size.width <= 0 || win_size.height <= 0)
		return;
	_max_window_size = win_size;
}



int HexagonSettingWindow::GetWindowKey() const{
	return cv::waitKey(0);
}

void HexagonSettingWindow::GetHexagon(std::vector<cv::Point2f>& hexagon) const
{
	hexagon.resize(7);
	for (int i = 0; i < 7; i++) {
		hexagon[i] = _display_hexagon[i] / _display_scale;
	}
}

void HexagonSettingWindow::MouseButtonDown(int x, int y)
{
	cv::Point2f mouse_pt(x, y);
	for (int i = 0; i < _display_hexagon.size(); i++) {
		if (cv::norm(mouse_pt - _display_hexagon[i]) < _select_distance_threshold) {
			_selected_corner_id = i;
			break;
		}
	}
}


void HexagonSettingWindow::MouseDrag(int x, int y)
{
	if (_selected_corner_id < 0)
		return;

	if (_selected_corner_id == 0) {
		_MoveHexagon(_display_hexagon, cv::Point2f(x, y));
	}
	else {
		cv::Point2f center = _display_hexagon[0];
		cv::Point2f mouse_pt(x,y);
		float scale = cv::norm(mouse_pt - center);
		float angle = _CalcRotation(mouse_pt, center, _selected_corner_id);

		_GenerateHexagon(_display_hexagon, center, scale, angle);
	}
	
	RedrawImage();
}


void HexagonSettingWindow::MouseButtonUp()
{
	_selected_corner_id = -1;
}

void HexagonSettingWindow::MouseMove(int x, int y)
{
}


void HexagonSettingWindow::on_mouse(int event, int x, int y, int flag, void * param)
{
	HexagonSettingWindow* viewer = (HexagonSettingWindow*)param;

	// If left mouse button is pressed, record the first coordinate
	if (event == cv::EVENT_LBUTTONDOWN) {
		//printf("Left mouse button pressed\n");
		viewer->MouseButtonDown(x, y);
	}

	// If mouse is moved while left mouse button is still pressed
#ifdef _WINDOWS
	if (event == cv::EVENT_MOUSEMOVE && flag & cv::EVENT_FLAG_LBUTTON) {
#else
	if (event == cv::EVENT_MOUSEMOVE && flag == 33) {
#endif
		viewer->MouseDrag(x, y);
	}
	else if (event == cv::EVENT_MOUSEMOVE) {
		viewer->MouseMove(x, y);
	}

	// If left mouse button is released
	if (event == cv::EVENT_LBUTTONUP)
	{
		viewer->MouseButtonUp();
	}

}


float HexagonSettingWindow::_ComputeDisplayScale(const cv::Size& image_size, const cv::Size& max_image_size)
{
	float scale_w = (float)max_image_size.width / image_size.width;
	float scale_h = (float)max_image_size.height / image_size.height;
	return (scale_w < scale_h) ? scale_w : scale_h;
}


void HexagonSettingWindow::_DrawHex(cv::Mat& img, const std::vector<cv::Point2f>& hex, const cv::Scalar& color, int thickness, int corner_circle_size)
{
	for (int i = 1; i < 6; i++) {
		cv::line(img, hex[i], hex[i+1], color, thickness);
	}
	cv::line(img, hex[6], hex[1], color, thickness);

	if (corner_circle_size <= 0)
		return;

	for (int i = 0; i < hex.size(); i++) {
		cv::circle(img, hex[i], corner_circle_size, color, thickness);
	}
}


void HexagonSettingWindow::RedrawImage() const
{
	cv::Mat draw_img = _display_image.clone();
	_DrawHex(draw_img, _display_hexagon, cv::Scalar(255, 0, 0), 2, _conrner_circle_radius);
	cv::imshow(_window_name, draw_img);
}

